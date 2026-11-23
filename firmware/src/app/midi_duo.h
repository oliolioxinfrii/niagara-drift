/* SPDX-License-Identifier: GPL-3.0-only */
/* External MIDI controller: exactly two sounding drone slots.
 *
 * Each key is identified by (MIDI channel, note). The first two keys
 * occupy drones I/II. If a slot is released (including during its FADE),
 * the next key reuses THAT slot via fade-out/retune/fade-in.
 *
 * While both drones have owners, one extra held key can instead modulate
 * the nearest SOUNDING drone's pitch over Scene Time. Its original note
 * retains ownership; releasing that original while the extra stays held
 * must NOT interrupt the glide. If the extra is released first, glide
 * back to the still-held original; otherwise release the drone.
 * A fourth overlapping key is ignored (never creates a new oscillator).
 *
 * CC64 is per MIDI channel. Note Off while pedal is held only records
 * physical key release; pedal-up finally frees released notes. Thus the
 * pedal latches both ordinary notes and the extra glide target.
 *
 * This file owns note allocation ONLY. Audio fades and glides run on
 * the DSP core. No saved pitch, parameter or FM-1 key mapping changes.
 */
#ifndef VOLCANO_MIDI_DUO_H
#define VOLCANO_MIDI_DUO_H

#include <stdint.h>
typedef struct {
    int16_t key[2];        /* channel * 128 + note, -1 unassigned */
    uint8_t down[2];       /* physical key still held; if false, CC64 may sustain */
    int16_t extra;         /* third held note; -1 if none */
    int8_t extra_slot;     /* 0 or 1: which sounding drone the extra glides */
    uint8_t extra_down;
    uint16_t pedal;        /* bit for each MIDI channel, CC64 >=64 */
} midi_duo_t;

static inline void midi_duo_reset(midi_duo_t *d)
{
    d->key[0] = d->key[1] = -1;
    d->down[0] = d->down[1] = 0u;
    d->extra = -1;
    d->extra_slot = -1;
    d->extra_down = 0u;
    d->pedal = 0u;
}
static inline int midi_duo_slot_note(const midi_duo_t *d, int slot)
{
    if ((unsigned)slot >= 2u) return -1;
    if (d->extra >= 0 && d->extra_slot == slot)
        return d->extra % 128;
    return d->key[slot] < 0 ? -1 : d->key[slot] % 128;
}
static inline int midi_duo_slot_gliding(const midi_duo_t *d, int slot)
{
    return d->extra >= 0 && d->extra_slot == slot;
}
static inline int midi_duo_on(midi_duo_t *d, unsigned channel,
                              unsigned note, float pitch0, float pitch1)
{
    int16_t id;
    int slot;
    if (channel >= 16u || note >= 128u) return -1;
    id = (int16_t)(channel * 128u + note);
    for (slot = 0; slot < 2; ++slot)
        if (d->key[slot] == id) {
            d->down[slot] = 1u; /* re-struck while sustained by pedal */
            return -1;
        }
    if (d->extra == id) {
        d->extra_down = 1u;
        return -1;
    }
    /* Do not claim the slot already sounding the third-note glide just
     * because its original key was released. It remains occupied. */
    slot = d->key[0] < 0 && d->extra_slot != 0 ? 0 :
           d->key[1] < 0 && d->extra_slot != 1 ? 1 : -1;
    if (slot >= 0) {
        d->key[slot] = id;
        d->down[slot] = 1u;
        return slot; /* standard fade-separated trigger */
    }
    if (d->extra >= 0) return -1; /* a fourth key cannot steal a voice */
    {
        const float p = (float)note;
        const float a = pitch0 > p ? pitch0 - p : p - pitch0;
        const float b = pitch1 > p ? pitch1 - p : p - pitch1;
        slot = a <= b ? 0 : 1; /* equal distance: stable Drone I tie break */
    }
    d->extra = id;
    d->extra_slot = (int8_t)slot;
    d->extra_down = 1u;
    return 2 + slot; /* scene-timed glide of an already sounding drone */
}
static inline int midi_duo_off(midi_duo_t *d, unsigned channel, unsigned note)
{
    int16_t id;
    int slot;
    if (channel >= 16u || note >= 128u) return -1;
    id = (int16_t)(channel * 128u + note);
    if (d->extra == id) {
        if (!d->extra_down) return -1;
        d->extra_down = 0u;
        if (d->pedal & (uint16_t)(1u << channel)) return -1;
        slot = d->extra_slot;
        d->extra = -1;
        d->extra_slot = -1;
        return 2 + slot;
    }
    for (slot = 0; slot < 2; ++slot)
        if (d->key[slot] == id) {
            if (!d->down[slot]) return -1;
            d->down[slot] = 0u;
            if (d->pedal & (uint16_t)(1u << channel)) return -1;
            d->key[slot] = -1;
            return slot;
        }
    return -1;
}
/* Return bitmask of affected drone slots after a CC64 change.
 * Pedal-up removes only notes whose physical keys are released. */
static inline unsigned midi_duo_pedal(midi_duo_t *d, unsigned channel, unsigned value)
{
    uint16_t bit;
    unsigned affected = 0u;
    int slot;
    if (channel >= 16u) return 0u;
    bit = (uint16_t)(1u << channel);
    if (value >= 64u) {
        d->pedal |= bit;
        return 0u;
    }
    d->pedal &= (uint16_t)~bit;
    for (slot = 0; slot < 2; ++slot)
        if (d->key[slot] >= 0 && !d->down[slot] &&
            (unsigned)d->key[slot] / 128u == channel) {
            d->key[slot] = -1;
            affected |= 1u << slot;
        }
    if (d->extra >= 0 && !d->extra_down &&
        (unsigned)d->extra / 128u == channel) {
        affected |= 1u << d->extra_slot;
        d->extra = -1;
        d->extra_slot = -1;
    }
    return affected;
}
/* CC120/123 and panic: explicitly release assigned notes on that
 * channel even when CC64 is held. */
static inline unsigned midi_duo_channel_off(midi_duo_t *d, unsigned channel)
{
    unsigned mask = 0u;
    int slot;
    if (channel >= 16u) return 0u;
    d->pedal &= (uint16_t)~(1u << channel);
    for (slot = 0; slot < 2; ++slot)
        if (d->key[slot] >= 0 && (unsigned)d->key[slot] / 128u == channel) {
            d->key[slot] = -1;
            d->down[slot] = 0u;
            mask |= 1u << slot;
        }
    if (d->extra >= 0 && (unsigned)d->extra / 128u == channel) {
        mask |= 1u << d->extra_slot;
        d->extra = -1;
        d->extra_slot = -1;
        d->extra_down = 0u;
    }
    return mask;
}
#endif /* VOLCANO_MIDI_DUO_H */
