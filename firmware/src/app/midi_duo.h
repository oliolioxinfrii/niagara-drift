/* SPDX-License-Identifier: GPL-3.0-only */
/* External MIDI keyboard -> two monophonic drone triggers.
 * Only incoming MIDI is dispatched here; the FM-1's physical keyboard
 * keeps its ordinary gesture, sequencer and GHOST performance functions.
 *
 * The first held note uses drone I, a second simultaneous note uses
 * drone II. Replacing a released note reuses its vacant slot. If a third
 * key is pressed while both are held, it steals drone II (last-note
 * priority) without creating a third voice. Matching note-off messages
 * are required: a late note-off for a stolen key cannot stop its replacement.
 *
 * Only the key allocator lives here. Pitch and envelope transitions are
 * performed in the DSP audio queue, without changing saved parameters. */
#ifndef VOLCANO_MIDI_DUO_H
#define VOLCANO_MIDI_DUO_H

#include <stdint.h>
typedef struct {
    int16_t key[2]; /* MIDI channel * 128 + note; -1 = no physical key held */
} midi_duo_t;

static inline void midi_duo_reset(midi_duo_t *d)
{
    d->key[0] = d->key[1] = -1;
}
static inline int midi_duo_on(midi_duo_t *d, unsigned channel, unsigned note)
{
    int16_t id;
    int slot;
    if (channel >= 16u || note >= 128u) return -1;
    id = (int16_t)(channel * 128u + note);
    if (d->key[0] == id || d->key[1] == id) return -1; /* duplicate key-down */
    slot = d->key[0] < 0 ? 0 : d->key[1] < 0 ? 1 : 1;
    d->key[slot] = id;
    return slot;
}
static inline int midi_duo_off(midi_duo_t *d, unsigned channel, unsigned note)
{
    int16_t id;
    int slot;
    if (channel >= 16u || note >= 128u) return -1;
    id = (int16_t)(channel * 128u + note);
    for (slot = 0; slot < 2; ++slot)
        if (d->key[slot] == id) {
            d->key[slot] = -1;
            return slot;
        }
    return -1; /* stale note-off, including a stolen key */
}
static inline unsigned midi_duo_channel_off(midi_duo_t *d, unsigned channel)
{
    unsigned mask = 0u;
    int slot;
    if (channel >= 16u) return 0u;
    for (slot = 0; slot < 2; ++slot)
        if (d->key[slot] >= 0 && (unsigned)d->key[slot] / 128u == channel) {
            d->key[slot] = -1;
            mask |= 1u << slot;
        }
    return mask;
}
#endif /* VOLCANO_MIDI_DUO_H */
