/* SPDX-License-Identifier: GPL-3.0-only */
/* Browser-only spectral GHOST experiment, included by drone.c (one DSP unit).
 *
 * Unlike the polynomial Original, select a few strong Fourier components
 * from EACH audible parent and synthesize only specified sum/difference
 * combination frequencies. Full spectral convolution would merely reproduce
 * multiplication; this sparse selection is the experimental difference.
 *
 * STFT: 4096 samples, 1024-sample hop, periodic Hann analysis/synthesis,
 * 4x overlap, 2/3 overlap normalization, 4096-sample synthesis latency.
 * Large static buffers are deliberate and NOT hardware RAM approved.
 */
#define GHFFT_N 4096u
#define GHFFT_HOP 1024u
#define GHFFT_RING 8192u
#define GHFFT_PEAKS 8u
#define GHFFT_IN_MIN 4u
#define GHFFT_IN_MAX 260u       /* 43 .. 2799 Hz; includes bass and mids */
#define GHFFT_DIFF_MAX 112u     /* <=1206 Hz */
#define GHFFT_SUM_MAX 325u      /* <=3499 Hz */

typedef struct { float r, i; } ghost_bin_t;
typedef struct { unsigned k; float p; } ghost_peak_t;

static float ghost_fft_input[2][GHFFT_N];
static float ghost_fft_output[GHFFT_RING];
static float ghost_fft_window[GHFFT_N];
static ghost_bin_t ghost_fft_a[GHFFT_N], ghost_fft_b[GHFFT_N], ghost_fft_s[GHFFT_N];
static uint32_t ghost_fft_clock, ghost_fft_write;
static uint8_t ghost_fft_window_ready, ghost_fft_active;

static void ghost_fft_reset(void)
{
    if (!ghost_fft_active)
        return;
    dr_memset(ghost_fft_input, 0, sizeof ghost_fft_input);
    dr_memset(ghost_fft_output, 0, sizeof ghost_fft_output);
    ghost_fft_clock = ghost_fft_write = 0u;
    ghost_fft_active = 0u;
}
static void ghost_fft_prepare(void)
{
    unsigned i;
    if (ghost_fft_active)
        return;
    if (!ghost_fft_window_ready) {
        for (i = 0; i < GHFFT_N; ++i)
            ghost_fft_window[i] = 0.5f - 0.5f *
                fm_cosf(FM_TWO_PI * (float)i / (float)GHFFT_N);
        ghost_fft_window_ready = 1;
    }
    ghost_fft_active = 1u;
}
/* Radix-2 DIT complex transform. The inverse includes 1/N normalization.
 * Twiddles are evaluated once per radix stage, not per bin. */
static void ghost_fft_transform(ghost_bin_t *v, int inverse)
{
    unsigned i, j, len;
    for (i = 1u, j = 0u; i < GHFFT_N; ++i) {
        unsigned bit = GHFFT_N >> 1u;
        while (j & bit) { j ^= bit; bit >>= 1u; }
        j ^= bit;
        if (i < j) {
            const ghost_bin_t tmp = v[i];
            v[i] = v[j]; v[j] = tmp;
        }
    }
    for (len = 2u; len <= GHFFT_N; len <<= 1u) {
        const float theta = (inverse ? 1.0f : -1.0f) * FM_TWO_PI / (float)len;
        const float wr0 = fm_cosf(theta), wi0 = fm_sin_turns(theta / FM_TWO_PI);
        unsigned base;
        for (base = 0; base < GHFFT_N; base += len) {
            float wr = 1.0f, wi = 0.0f;
            for (j = 0u; j < len / 2u; ++j) {
                const unsigned a = base + j, b = a + len / 2u;
                const float tr = wr * v[b].r - wi * v[b].i;
                const float ti = wr * v[b].i + wi * v[b].r;
                const float ar = v[a].r, ai = v[a].i;
                const float next_wr = wr * wr0 - wi * wi0;
                wi = wr * wi0 + wi * wr0;
                wr = next_wr;
                v[a].r = ar + tr; v[a].i = ai + ti;
                v[b].r = ar - tr; v[b].i = ai - ti;
            }
        }
    }
    if (inverse) {
        for (i = 0u; i < GHFFT_N; ++i) {
            v[i].r *= 1.0f / (float)GHFFT_N;
            v[i].i *= 1.0f / (float)GHFFT_N;
        }
    }
}
/* Keep up to eight LOCAL spectral maxima, sorted by power. A relative
 * floor avoids interpreting low-level leakage/noise as genuine parents. */
static unsigned ghost_fft_peaks(const ghost_bin_t *spec, ghost_peak_t *peaks)
{
    float strongest = 0.0f;
    unsigned i, n = 0u;
    for (i = GHFFT_IN_MIN; i <= GHFFT_IN_MAX; ++i) {
        const float p = spec[i].r * spec[i].r + spec[i].i * spec[i].i;
        if (p > strongest) strongest = p;
    }
    if (strongest < 1e-5f)
        return 0u;
    for (i = GHFFT_IN_MIN; i <= GHFFT_IN_MAX; ++i) {
        const float p = spec[i].r * spec[i].r + spec[i].i * spec[i].i;
        const float p0 = spec[i-1u].r * spec[i-1u].r + spec[i-1u].i * spec[i-1u].i;
        const float p1 = spec[i+1u].r * spec[i+1u].r + spec[i+1u].i * spec[i+1u].i;
        unsigned pos;
        if (p < 0.012f * strongest || p <= p0 || p < p1)
            continue;
        for (pos = 0u; pos < n && peaks[pos].p >= p; ++pos) {}
        if (pos >= GHFFT_PEAKS)
            continue;
        if (n < GHFFT_PEAKS) ++n;
        for (unsigned q = n - 1u; q > pos; --q)
            peaks[q] = peaks[q-1u];
        peaks[pos] = (ghost_peak_t){ i, p };
    }
    return n;
}
static void ghost_fft_put(unsigned k, float r, float im)
{
    if (k == 0u || k >= GHFFT_N / 2u)
        return;
    ghost_fft_s[k].r += r;
    ghost_fft_s[k].i += im;
    ghost_fft_s[GHFFT_N-k].r += r;
    ghost_fft_s[GHFFT_N-k].i -= im;
}
static void ghost_fft_frame(void)
{
    ghost_peak_t xp[GHFFT_PEAKS], yp[GHFFT_PEAKS];
    unsigned i, a, b, nx, ny;
    const float scale = 2.0f / (float)GHFFT_N; /* Hann coherent-gain normalization */
    for (i = 0u; i < GHFFT_N; ++i) {
        const unsigned k = (ghost_fft_write + i) & (GHFFT_N - 1u);
        const float w = ghost_fft_window[i];
        ghost_fft_a[i] = (ghost_bin_t){ ghost_fft_input[0][k] * w, 0.0f };
        ghost_fft_b[i] = (ghost_bin_t){ ghost_fft_input[1][k] * w, 0.0f };
    }
    ghost_fft_transform(ghost_fft_a, 0);
    ghost_fft_transform(ghost_fft_b, 0);
    nx = ghost_fft_peaks(ghost_fft_a, xp);
    ny = ghost_fft_peaks(ghost_fft_b, yp);
    dr_memset(ghost_fft_s, 0, sizeof ghost_fft_s);
    for (a = 0u; a < nx; ++a) for (b = 0u; b < ny; ++b) {
        const unsigned ka = xp[a].k, kb = yp[b].k;
        const unsigned sum = ka + kb, diff = ka > kb ? ka-kb : kb-ka;
        const ghost_bin_t x = ghost_fft_a[ka], y = ghost_fft_b[kb];
        /* x*y sum phase and x*conj(y) difference phase. These
         * are the selected terms of spectral convolution, NOT bin-wise
         * multiplication of the two FFTs. */
        if (sum <= GHFFT_SUM_MAX) {
            ghost_fft_put(sum, 0.50f * scale * (x.r*y.r-x.i*y.i),
                                    0.50f * scale * (x.r*y.i+x.i*y.r));
        }
        if (diff >= 3u && diff <= GHFFT_DIFF_MAX) {
            const float dr = x.r*y.r + x.i*y.i;
            const float di = x.i*y.r - x.r*y.i;
            ghost_fft_put(diff, 1.50f * scale * dr,
                                1.50f * scale * (ka > kb ? di : -di));
        }
    }
    ghost_fft_transform(ghost_fft_s, 1);
    for (i = 0u; i < GHFFT_N; ++i) {
        /* End-of-frame processing writes future delayed samples. Four
         * Hann*Hann frames overlap; their summed power is exactly 3/2. */
        const unsigned dst = (ghost_fft_clock + i) & (GHFFT_RING - 1u);
        ghost_fft_output[dst] += (2.0f / 3.0f) * ghost_fft_s[i].r * ghost_fft_window[i];
    }
}
static float ghost_fft_sample(float u, float v)
{
    float out;
    const unsigned read = ghost_fft_clock & (GHFFT_RING - 1u);
    ghost_fft_prepare();
    /* Read before writing a new frame: output is always strictly delayed. */
    out = ghost_fft_output[read];
    ghost_fft_output[read] = 0.0f;
    ghost_fft_input[0][ghost_fft_write] = u;
    ghost_fft_input[1][ghost_fft_write] = v;
    ghost_fft_write = (ghost_fft_write + 1u) & (GHFFT_N - 1u);
    ++ghost_fft_clock;
    if (!(ghost_fft_clock & (GHFFT_HOP - 1u)))
        ghost_fft_frame();
    /* Allow sparse low-frequency pairs to be audible under the same
     * GHOST-only compressor, without adding independent generators. */
    return fm_clampf(6.0f * out, -0.85f, 0.85f);
}
