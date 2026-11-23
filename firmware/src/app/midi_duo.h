/* SPDX-License-Identifier: GPL-3.0-only */
/* External MIDI: two sounding drone slots, each with one glide target.
 *
 * The first two keys claim drones I and II. When one owner is released
 * (or a voice is still fading out), the next key reuses that free slot
 * through the original FADE-out / silent retune / FADE-in path.
 *
 * A simultaneous extra key chooses the drone nearest in its CURRENT
 * sounding pitch. Equal distances choose the drone with the most recent
 * accepted Note On. A drone already gliding (including returning toward
 * its old key) ignores another extra key; do NOT divert that note to the
 * other drone. The other drone may glide independently at the same time.
 *
 * If an original owner is released while an extra glides, the glide
 * continues. Only AFTER the scene-time glide completes is the held
 * extra promoted to primary ownership, allowing another successive
 * glide without any third voice. Releasing the extra before completion
 * glides back if its original is held, or fades out when neither is.
 *
 * Physical note-up is separate from CC64 sustain. CC64 >= 64 latches
 * released keys and extra targets, per MIDI channel; pedal-up releases
 * only keys which are no longer physically down. CC120/123 force off.
 *
 * No oscillator allocation, saved parameter mutation, or local key-map
 * changes happen here. The DSP owns the gate envelope and pitch glide.
 */
#ifndef VOLCANO_MIDI_DUO_H
#define VOLCANO_MIDI_DUO_H

#include <stdint.h>
typedef struct {
    int16_t key[2];       /* channel*128+note; -1 = unoccupied base */
    uint8_t down[2];
    int16_t extra[2];     /* each drone has an optional active glide target */
    uint8_t extra_down[2];
    uint16_t pedal;       /* CC64 >=64, one bit per incoming MIDI channel */
    uint8_t last_slot;    /* newest accepted key's drone; tie break */
} midi_duo_t;

static inline void midi_duo_reset(midi_duo_t *d)
{
    int g;
    for (g = 0; g < 2; ++g) {
        d->key[g] = d->extra[g] = -1;
        d->down[g] = d->extra_down[g] = 0u;
    }
    d->pedal = 0u;
    d->last_slot = 0u;
}
static inline int midi_duo_slot_note(const midi_duo_t *d, int g)
{
    if ((unsigned)g >= 2u) return -1;
    if (d->extra[g] >= 0) return d->extra[g] % 128;
    return d->key[g] < 0 ? -1 : d->key[g] % 128;
}
static inline int midi_duo_slot_gliding(const midi_duo_t *d, int g)
{
    return (unsigned)g < 2u && d->extra[g] >= 0;
}
static inline int midi_duo_extra_note(const midi_duo_t *d, int g)
{
    return (unsigned)g < 2u && d->extra[g] >= 0 ? d->extra[g] % 128 : -1;
}
/* busy0/busy1 include a newly queued (not-yet-DSP-acknowledged) glide
 * as well as any active Scene-Time pitch transition. A key whose
 * nearest drone is busy is ignored, even if the other drone is free
 * to accept an extra target. A released slot is always allocated first. */
static inline int midi_duo_on(midi_duo_t *d, unsigned channel,
                              unsigned note, float pitch0, float pitch1,
                              int busy0, int busy1)
{
    int16_t id;
    int g;
    if (channel >= 16u || note >= 128u) return -1;
    id = (int16_t)(channel * 128u + note);
    for (g = 0; g < 2; ++g) {
        if (d->key[g] == id) {
            d->down[g] = 1u; /* re-struck while sustained by CC64 */
            d->last_slot = (uint8_t)g;
            return -1;
        }
        if (d->extra[g] == id) {
            d->extra_down[g] = 1u;
            d->last_slot = (uint8_t)g;
            return -1;
        }
    }
    g = d->key[0] < 0 && d->extra[0] < 0 ? 0 :
        d->key[1] < 0 && d->extra[1] < 0 ? 1 : -1;
    if (g >= 0) {
        d->key[g] = id;
        d->down[g] = 1u;
        d->last_slot = (uint8_t)g;
        return g;
    }
    {
        const float p = (float)note;
        const float a = pitch0 > p ? pitch0 - p : p - pitch0;
        const float b = pitch1 > p ? pitch1 - p : p - pitch1;
        g = a < b ? 0 : b < a ? 1 : (int)d->last_slot;
    }
    if (d->extra[g] >= 0 || (g == 0 ? busy0 : busy1))
        return -1; /* nearest is busy: ignore, never steal/redirect */
    if (d->key[g] < 0) return -1; /* held target not yet promoted */
    d->extra[g] = id;
    d->extra_down[g] = 1u;
    d->last_slot = (uint8_t)g;
    return 2 + g; /* scene-time glide of the existing oscillator */
}
static inline int midi_duo_off(midi_duo_t *d, unsigned channel, unsigned note)
{
    int16_t id;
    int g;
    if (channel >= 16u || note >= 128u) return -1;
    id = (int16_t)(channel * 128u + note);
    for (g = 0; g < 2; ++g) {
        if (d->extra[g] == id) {
            if (!d->extra_down[g]) return -1;
            d->extra_down[g] = 0u;
            if (d->pedal & (uint16_t)(1u << channel)) return -1;
            d->extra[g] = -1;
            return 2 + g;
        }
        if (d->key[g] == id) {
            if (!d->down[g]) return -1;
            d->down[g] = 0u;
            if (d->pedal & (uint16_t)(1u << channel)) return -1;
            d->key[g] = -1;
            return g;
        }
    }
    return -1;
}
/* Called once the DSP has COMPLETED the final queued glide on g.
 * Promotion is legal only if the previous primary key is no longer
 * held or pedal-sustained; otherwise both keys retain ownership.
 * No new Note On is generated: the drone stays at the attained pitch. */
static inline int midi_duo_promote_completed(midi_duo_t *d, int g)
{
    if ((unsigned)g >= 2u || d->key[g] >= 0 || d->extra[g] < 0)
        return 0;
    d->key[g] = d->extra[g];
    d->down[g] = d->extra_down[g];
    d->extra[g] = -1;
    d->extra_down[g] = 0u;
    return 1;
}
static inline unsigned midi_duo_pedal(midi_duo_t *d, unsigned channel, unsigned value)
{
    uint16_t bit;
    unsigned mask = 0u;
    int g;
    if (channel >= 16u) return 0u;
    bit = (uint16_t)(1u << channel);
    if (value >= 64u) {
        d->pedal |= bit;
        return 0u;
    }
    d->pedal &= (uint16_t)~bit;
    for (g = 0; g < 2; ++g) {
        if (d->key[g] >= 0 && !d->down[g] &&
            (unsigned)d->key[g] / 128u == channel) {
            d->key[g] = -1;
            mask |= 1u << g;
        }
        if (d->extra[g] >= 0 && !d->extra_down[g] &&
            (unsigned)d->extra[g] / 128u == channel) {
            d->extra[g] = -1;
            mask |= 1u << g;
        }
    }
    return mask;
}
static inline unsigned midi_duo_channel_off(midi_duo_t *d, unsigned channel)
{
    unsigned mask = 0u;
    int g;
    if (channel >= 16u) return 0u;
    d->pedal &= (uint16_t)~(1u << channel);
    for (g = 0; g < 2; ++g) {
        if (d->key[g] >= 0 && (unsigned)d->key[g] / 128u == channel) {
            d->key[g] = -1;
            d->down[g] = 0u;
            mask |= 1u << g;
        }
        if (d->extra[g] >= 0 && (unsigned)d->extra[g] / 128u == channel) {
            d->extra[g] = -1;
            d->extra_down[g] = 0u;
            mask |= 1u << g;
        }
    }
    return mask;
}
#endif /* VOLCANO_MIDI_DUO_H */
