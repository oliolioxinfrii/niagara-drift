/* SPDX-License-Identifier: GPL-3.0-only */
/* External MIDI keyboard -> two monophonic drone triggers.
 * Only incoming MIDI is dispatched here; the FM-1's physical keyboard
 * keeps its ordinary gesture, sequencer and GHOST performance functions.
 *
 * The first held note uses drone I, a second simultaneous note uses
 * drone II. Replacing a released note reuses EXACTLY its vacant slot:
 * release drone I and the next note takes drone I; release drone II
 * and the next note takes drone II. If a third key is pressed while BOTH
 * notes are held, it waits silently: its pitch enters whichever drone is
 * released first, following that drone's FADE-out. It never steals a held
 * note or adds a third voice. At most one waiting key is remembered; a
 * newer press supersedes the older unassigned key.
 *
 * Only the key allocator lives here. Pitch and envelope transitions are
 * performed in the DSP audio queue, without changing saved parameters. */
#ifndef VOLCANO_MIDI_DUO_H
#define VOLCANO_MIDI_DUO_H

#include <stdint.h>
typedef struct {
    int16_t key[2];  /* MIDI channel * 128 + note; -1 = no assigned key */
    int16_t waiting; /* latest still-held key waiting for either slot */
} midi_duo_t;

static inline void midi_duo_reset(midi_duo_t *d)
{
    d->key[0] = d->key[1] = -1;
    d->waiting = -1;
}
static inline int midi_duo_on(midi_duo_t *d, unsigned channel, unsigned note)
{
    int16_t id;
    int slot;
    if (channel >= 16u || note >= 128u) return -1;
    id = (int16_t)(channel * 128u + note);
    if (d->key[0] == id || d->key[1] == id ||
        d->waiting == id) return -1; /* duplicate key-down */
    slot = d->key[0] < 0 ? 0 : d->key[1] < 0 ? 1 : -1;
    if (slot < 0) {
        d->waiting = id; /* third key is silent until a held key releases */
        return -1;
    }
    d->key[slot] = id;
    return slot;
}
static inline int midi_duo_off(midi_duo_t *d, unsigned channel, unsigned note)
{
    int16_t id;
    int slot;
    if (channel >= 16u || note >= 128u) return -1;
    id = (int16_t)(channel * 128u + note);
    if (d->waiting == id) {
        d->waiting = -1;
        return -1; /* released before assignment: never sounds */
    }
    for (slot = 0; slot < 2; ++slot)
        if (d->key[slot] == id) {
            d->key[slot] = -1;
            return slot;
        }
    return -1; /* stale/unassigned note-off */
}
/* After releasing an occupied slot, promote the latest third key (if any).
 * Caller posts Note Off first, then this Note On; the DSP performs the
 * actual fade-out before retuning. Returns -1 if no pending key. */
static inline int midi_duo_promote(midi_duo_t *d, int slot)
{
    int note;
    if ((unsigned)slot >= 2u || d->key[slot] >= 0 || d->waiting < 0)
        return -1;
    note = d->waiting % 128;
    d->key[slot] = d->waiting;
    d->waiting = -1;
    return note;
}
static inline unsigned midi_duo_channel_off(midi_duo_t *d, unsigned channel)
{
    unsigned mask = 0u;
    int slot;
    if (channel >= 16u) return 0u;
    if (d->waiting >= 0 && (unsigned)d->waiting / 128u == channel)
        d->waiting = -1;
    for (slot = 0; slot < 2; ++slot)
        if (d->key[slot] >= 0 && (unsigned)d->key[slot] / 128u == channel) {
            d->key[slot] = -1;
            mask |= 1u << slot;
        }
    return mask;
}
#endif /* VOLCANO_MIDI_DUO_H */
