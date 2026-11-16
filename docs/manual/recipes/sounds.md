---
title: Sounds
intro: Explore Volcano and find the sounds you want to make
description: Recipes for sounds on Volcano: ticks, a singing filter, hard sync, metallic cross-FM bells, stepped figures, warbling echoes, a frozen chord.
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

Each recipe is a sound that takes more than one knob: a key and a knob together, or settings on two or three pages.
Each sound example starts from a different Preset.

## Tone and texture

### Ticks and clicks {#ticks}

*A sub so low it breaks into a few clicks a second*

Take drone I down to its lowest notes and its sub four octaves under that, and the sub stops being a tone: it becomes
a slow tick, a few clicks a second under drone II, a pulse to play against.

1. Tap [[HOME]] until **Pitch** appears, and turn KNOB 1 down to drone I's lowest notes: **A0** (27.5 Hz), about 29 clicks
   from where most Presets start.
2. Press [[EDIT]] twice for **Sub · Drive**, and tap [[G#3]], drone I's sub, until the screen says **−4 oct**. It
   goes round −1, −2, ⅓, ⅕, −3, −4.
3. Turn KNOB 1, Osc/Sb, all the way to the sub: **100**.
4. Turn KNOB 2, Drive, up for a harder click.

> **Good to know.** The ticks come at drone I's pitch ÷ 16, two to a cycle (each edge of the sub's square): lower
> drone I, slower ticks. Higher, or at −3 oct, they blur into a flutter. The clicks reach the speaker, though the hum
> under them doesn't. The **Tick** gesture ([[G4]]) does this on its own, faster as you press harder. Or take drone I
> itself under 20 Hz (Pitch, KNOB 1): a click each cycle, as in **Sonar** (P7).

> **Screenshot needed — Sub · Drive, drone I's sub at -4 oct, Osc/Sb 100.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Sub · Drive, Drive 50.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

### A singing filter {#sing}

*Resonance up, and the LFO moving it: a ring that swells and fades*

Turn a filter's resonance up and it rings on the drone's notes; let the LFO move the resonance instead of the cutoff,
and the ring swells and fades on its own.

1. Tap [[HOME]] until **Pitch** appears, and turn KNOB 2 and KNOB 4, the resonances, up: the filters ring. Their last dot is
   red: there they ring on their own.
2. Press [[HOME]] again for **Drones**, and turn the cutoffs (KNOB 2 and KNOB 4) up: the ring brighter.
3. Tap [[LFO]] until **LFO Sends** appears, and tap [[C#4]] and [[D#4]]: on the LFO pages they switch each drone's cutoff
   send to **resonance**.
4. Turn KNOB 2 and KNOB 4, the resonance sends, up: the ring swells and fades as the LFO goes round.

> **Good to know.** **Abyss** (P5) swells its resonances this way. With the drones a fifth or an octave apart the two
> rings meet; a semitone apart, they beat. Weight ([[D5]]) lets the resonance give way while you hold it.

> **Screenshot needed — LFO sends, both cutoff sends on resonance.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Drones, the filters ringing.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

### Hard sync sweep {#sync}

*Drone II locked to drone I, its pitch swept by hand: a sweep of tone, not of note*

With hard sync, drone II restarts its wave on every cycle of drone I: turning drone II's pitch then changes its tone,
not its note, a tearing, vocal, laser-like sweep.

1. Press [[HOME]] for **Drones**: turn KNOB 4, drone II's cutoff, well up, and bring drone II forward (KNOB 3 up,
   KNOB 1 down).
2. Tap black key 10, [[D#5]]: **hard sync**, drone II locked to drone I.
3. Tap [[HOME]] until **Pitch** appears, and turn KNOB 3, drone II's pitch, slowly up two octaves and back: the tone sweeps.

> **Good to know.** It's strongest with drone II above drone I and a saw or square in its wave (Osc page). To keep
> it moving without a hand, send the LFO to drone II's pitch (LFO sends, KNOB 3) with a deep send: the LFO moves
> only a few semitones, so the sweep is gentler. Here Abyss's own resonance LFO was turned off first (LFO sends,
> KNOB 2 and 4 to 0), so its slow ringing doesn't cover the sweep.

> **Screenshot needed — Pitch, hard sync, drone II on B2.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Pitch, drone II two octaves up on B4.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

## Cross-mod

### Metallic bells {#bells}

*Cross-FM: each drone bending the other's pitch, high and pure, into bells*

Each drone's sound bends the other's pitch, FM synthesis between the two. On pure, high tones a tritone apart it rings
into a metallic, bell-like sound; turned higher it clangs and runs wild.

1. Press [[EDIT]] for **Osc**, and turn KNOB 1 and KNOB 3, the X-Waves, all the way down: **sine**.
2. Tap [[HOME]] until **Pitch** appears, and turn both drones high, a tritone apart (here C#6 and G6: the top of their
   range is about 1.6 kHz).
3. Tap black key 11, [[F#5]], until it winks: **bend each other** (twice from off).
4. Tap [[LFO]] until **Mods** appears, and turn KNOB 4, X-mod, up: about 70 rings, 85 clangs.

> **Good to know.** Simple intervals (an octave, a fifth) lock into harmonies instead of bells; awkward ones (a
> tritone, a seventh) ring metallic. Lower down, on rich waves, cross-FM only thickens and roughens. If the Swell is
> on, switch it off ([[ARP]], then [[ARP]] again on its page) so the bells ring steadily.

> **Screenshot needed — Mods, bend each other, X-mod 70.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Mods, X-mod 85.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

### Drone I shaping drone II {#xfilter}

*Drone I's sound moving drone II's filter*

At key 11's first setting, drone I's sound moves drone II's filter, as fast as drone I sounds: drone II growls,
buzzes and takes on drone I's pitch as a grain.

1. Tap black key 11, [[F#5]], once: it lights, **I moves II's cutoff**.
2. Tap [[LFO]] until **Mods** appears, and turn KNOB 4, X-mod, up: the growl deepens.
3. Tap [[HOME]] until **Pitch** appears, and move drone I's pitch (KNOB 1): the growl changes its grain.

> **Good to know.** It's strongest with drone II's filter resonant and band-pass ([[A#4]]), as in **Vespers** (P3)
> here. **Seance** (P6) uses it gently.

> **Screenshot needed — Mods, I moves II's cutoff.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Pitch, drone I moved to G3.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

### Snarl {#snarl}

*The finished sound, echoes and all, bending both drones*

At key 11's last setting, the finished sound, the delay's echoes included, bends both drones' pitch: a growl that
drags and tears the pitch, and the echoes with it. It's clearest on a steady sound with plenty of echoes.

1. Press [[FX]] for **Space**, and turn Repeat (KNOB 1) and Mix (KNOB 2) up: more echoes for Snarl to drag.
2. Tap black key 11, [[F#5]], three times from off: **Snarl** (it winks).
3. Tap [[LFO]] until **Mods** appears, and turn KNOB 4, X-mod, up: 40 a growl, 85 a snarl.

> **Good to know.** The **Snarl** gesture ([[B4]]) brings it in by pressure, with the drive and Feedback on top, and
> leaves without a trace. **Clean** ([[F5]]) takes it out while you hold it.

> **Screenshot needed — Mods, Snarl.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Mods, X-mod 85.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

## Movement and rhythm

### A stepped figure that moves {#figure}

*OR on drone II's pitch: four notes round and round, and Wander moving the figure*

LFO II's **OR** shape adds two square waves, one five times as fast as the other: on a drone's pitch it steps round
four notes, a figure that repeats. Wander then moves the whole figure to a new note now and then.

1. Make drone II bright, so the figure stands out: press [[EDIT]] for **Osc** and turn KNOB 3, its wave, to a
   **saw**; then on **Drones** ([[HOME]]) turn KNOB 4, its cutoff, all the way up, KNOB 3, its level, up, and KNOB 1,
   drone I's level, down.
2. Tap [[LFO]] until **LFO Sends** appears, and tap [[OCT+]], LFO II's wave, until the screen says **OR 5**.
3. Turn KNOB 3, drone II's pitch send, up: 40 is about ±4 semitones.
4. Hold [[SEL]] and turn SELECT to adjust the LFO's rate, up for a quicker figure.
5. Tap [[SEQ]] until **Pitch moves** appears, and turn KNOB 3, Wander, up: every so often the figure starts on another
   note.

> **Good to know.** LFO I's OR ([[OCT−]]) is three times as fast: another figure. **AND** (AND 3 on LFO I, AND 5 on
> LFO II) steps between two notes instead, as in **Piston** (P11). **Telegraph** (P13) is built this way, on LFO I's OR 3.

> **Screenshot needed — LFO sends, LFO II on OR 5, drone II's pitch send +40.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Pitch moves, Wander 2.4-6 s.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

### A filter rhythm in time with Swell {#swell}

*A square LFO on both cutoffs, locked to the Swell's breath*

The Swell makes both drones breathe; lock the LFO to it and a square wave on the filters jumps on the same beat, the
two filters in turn.

1. Tap [[ARP]] until the **Swell** page appears (hold ARP for 400 ms if Swell is off), and turn KNOB 1 for about a breath a
   second.
2. Tap [[LFO]] until **LFO Sends** appears. Tap [[OCT−]] until LFO I is **Square**, and tap key 8, [[A#4]], until LFO I moves
   drone II's cutoff too.
3. Turn KNOB 2 and KNOB 4, the cutoff sends, one up and one down: the filters jump in turn. Hold [[SEL]] and turn SELECT to about the
   Swell's speed.
4. Tap [[C#5]] to lock the LFO to Swell. Hold [[C#5]] for 500 ms if you want to reset the LFO phase without changing the lock state.

> **Good to know.** Echoes blur the rhythm: if your sound has them, turn the delay's Mix down first ([[FX]],
> KNOB 2). The Swell's speed sets the rhythm: the locked LFO restarts on each of its breaths. Hold [[D#4]]
> again to free the LFO. (With a MIDI clock, the Swell and the locked LFO follow its beat too.)

> **Screenshot needed — LFO sends, LFO I Square on both cutoffs, on Swell.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Swell, rate 1.2 s.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

### LFO chasing itself {#chase}

*Speed mod: the LFO rushing and dragging on its own wave*

Speed mod lets the LFO speed up and slow down on one of its own waves, so every wobble rushes, then drags. It's
easiest to hear on a vibrato.

1. Tap [[LFO]] until **LFO Sends** appears. Tap key 6, [[F#4]], until LFO I moves drone II's pitch, and turn KNOB 3, its pitch
   send, up: a slow vibrato.
2. Press [[LFO]] again for **Mods**, and turn KNOB 1, Speed, all the way up: the LFO speeds up and slows down by up to
   three octaves.
3. Turn KNOB 2, Source, to **LFO I**: a regular rush and drag (LFO II's random glide makes it uneven).

> **Good to know.** Speed mod moves the rate of everything the LFO moves at once: the filters, the pitches, the
> delay. Speed at 0 steadies it again.

> **Screenshot needed — LFO sends, LFO I on drone II's pitch.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Mods, Speed +100, Source LFO I.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

## Echoes and space

### Echoes that warble {#warble}

*The LFO moving the delay's time: echoes bending in pitch*

When the delay's time moves, its echoes bend in pitch, as tape does when it speeds up and slows down. Let an LFO move
it, and every echo warbles; let the Swell dip the drones, and the bending echoes ring on their own in the gaps.

1. Press [[FX]] for **Space**, and turn Repeat (KNOB 1) and Mix (KNOB 2) up: clear echoes to bend.
2. Tap [[ARP]] until **Swell** appears (hold ARP for 400 ms if Swell is off), and turn KNOB 1 up for about a breath a second: the
   echoes sound in the gaps.
3. Tap [[LFO]] until **Mods** appears, and turn KNOB 3, Delay, all the way up: the LFO wobbles the delay's time by a
   quarter, and every echo bends.
4. Tap [[FX]] until **Details** appears, and turn Time (KNOB 2) up: deeper, slower bends.

> **Good to know.** On the LFO pages, black key 10, [[D#5]], picks which LFO moves the delay's time: LFO I's
> regular waves warble evenly, LFO II's random glide makes them wander, as here. Turning Time by hand bends the
> echoes too. Movement (ALGORITHM) scales the warble with the other LFO sends.

> **Screenshot needed — Mods, Delay +100.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Details, a longer Time.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

### Crackling echoes {#crackle}

*The Self delay and Drive: echoes that sag, distort and crackle*

The **Self** delay stretches its time as its echoes get louder, so loud echoes sag and wobble; **Drive** after it
distorts the drones and their echoes together: ghost notes and crackle.

1. Press [[FX]] for **Space**, and turn Repeat (KNOB 1) up: the echoes pile up.
2. Press [[FX]] again for **Details**. Hold [[SEL]] and turn KNOB 2 to **Self** (or **Grain**, whose own echoes shake
   its time).
3. Turn KNOB 1, Drive, up.

> **Good to know.** Past Repeat 85 the echoes run away into a ringing tone that holds itself ("runaway" in red); Self
> makes the runaway lurch. PLAY lets it ring out.

> **Screenshot needed — Details, the Self delay.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Details, Drive 45.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

### Freeze a chord, play over it {#freeze}

*Reverb at Inf holds the chord; new notes play over it*

At its top, the reverb's Length is **Inf**: it never dies away, a freeze of what's in it. Move the drones and they
play over the frozen chord.

1. Press [[FX]] for **Space**. Turn KNOB 4, the reverb's Mix, up, then KNOB 3, Length, all the way: **Inf**, "freeze"
   in blue.
2. Tap [[HOME]] until **Pitch** appears, and turn KNOB 1 and KNOB 3 to new notes: the drones play over the frozen chord.
3. Turn Length back down to let it go, or stop with [[PLAY]].

> **Good to know.** On Details, [[SEL]] + KNOB 4 picks what goes into the reverb: **Drones** freezes the drones and
> keeps the echoes dry on top. Damp is out at Inf, so the freeze holds what's in it.

> **Screenshot needed — Space, Length Inf, freeze.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Pitch, new notes over the freeze.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

### Shimmer on echoes {#shimmer}

*Only the echoes rise an octave into the halo*

The **Shimmer** reverb lifts what goes into it an octave, in two voices just apart. Send it only the echoes, and each
echo rises into a bright halo while the drones stay dry. It shines on voices in the middle and up; on a low drone the
octave stays low.

1. Press [[FX]] for **Space**, and turn Repeat (KNOB 1) and Mix (KNOB 2) up: more echoes.
2. Press [[FX]] again for **Details**, and turn KNOB 4, the reverb's Type, to **Shimmer**.
3. Hold [[SEL]] and turn KNOB 4: Into **Echoes**.
4. Press [[FX]] for Space again, and turn the reverb's Mix (KNOB 4) up.

> **Good to know.** The Shimmer washes in over about 8 seconds and out over 6. **Cistern** (P10) uses it on
> everything. If your drones are low, raise them an octave or two (Pitch page) and the halo sparkles.

> **Screenshot needed — Space, more echoes, the reverb's Mix up.** Regenerate this view from the Volcano v1.5 firmware.
> **Screenshot needed — Details, Shimmer, Into Echoes.** Regenerate this view from the Volcano v1.5 firmware.

> **Audio preview pending:** Re-record this recipe from the Volcano v1.5 source after updating its host scenario.

