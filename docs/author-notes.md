# author notes, ten reviewer questions with the answers in one place

the point of this file: the author can explain every module without
opening the code, and a reviewer can check each claim at file and line.
read one question, close the file, say the answer out loud.

## 1 why is the cpg a stretch reflex and not a pacemaker circuit?

because wen 2012 showed the wave survives with no internal oscillator:
laser ablations and optogenetics put the pattern generation into the
body wall mechanics looped through the motor neurons. the model
therefore has no pacemaker neuron: body_proprioception (src/body.c)
computes a stretch reflex, a b class neuron depolarizes when its own
side is stretched a few segments behind its position, an a class when
stretched ahead. the loop delay (receptor low pass 600 ms, muscle 180
ms, bend filter 220 ms) turns negative feedback into a traveling wave.
the honest caveat: the offline scan tools/cpg_scan.py explored sign
combinations because the literature polarity alone did not oscillate in
our body; the shipped polarity matches wen 2012 with a signed push pull
current.

## 2 where do the ava vs avb weights come from and how were they calibrated?

structure from white 1986: ava excites the reverse classes (a), avb the
forward classes (b), each inhibits the opposite pool. weights are in
src/connectome.c builtin tables: ava to a classes 2.0, ava to forward
pools 9.0 ("w9" comment: the chloride driving force is only 22 mv, the
inhibition must out-shout the avb bias plus the wave self drive), avb
to b classes 2.0, avb to a pools 5.0. they were hand calibrated against
two constraints: spontaneous reversals exist but are rare, and the
forward wave must win at rest. not fitted by an optimizer; the
evolution module (--evolve) exists to explore this space but the
shipped config is hand set.

## 3 how are gap junctions symmetric in the csr?

the loader (src/connectome.c conn_load_csv) deduplicates gap rows by
unordered pair: when a second row lists the same pair reversed, it is
dropped before staging. in the csr the gap current is computed
diffusively (src/sim.c synaptic_currents): i_gap += g * (pre->v -
post->v) * i_scale, applied to the post cell of each stored direction;
since every pair is stored once, the current is added to one side only
and the reciprocal side receives it through the same row read from the
other cell? no: each pair is staged once, so the row walk adds the
current to one direction. the symmetry is guaranteed by the dedup plus
the symmetric conductance form; a direction would only appear if a csv
listed a pair twice, which the dedup removes.

## 4 what does tsodyks-markram do in src/sim.c?

short term plasticity per synapse, two state variables: u (utilization,
facilitates with activity) and x (vesicle resource, depletes). in
synaptic_currents (src/sim.c) each chemical synapse updates u and x per
step from the delayed presynaptic activation, release p = u * x scales
the synaptic current. assignment at stage time (src/connectome.c):
sensory afferents are depressing (tap habituation: the tap train
response decays), interneurons facilitating (command bursts sharpen).
constants in src/config.h (STSP_U_DEP, STSP_TAU_REC_*). this is what
makes the tap habituation curve in results/tap_train.csv.

## 5 how is the a class gating on ava connected to the literature?

it is not, and it says so: the comment in body_proprioception marks it
"assumption: a class proprioception gated by the ava command is circuit
design, not a literature fact". the mechanism: the a class stretch
reflex is multiplied by ava activity squared, so the reverse reflex is
active only while the reverse command is on. rationale: without the
gate the reverse reflex fights the forward wave in antiphase. the
methods section carries the same declaration. a reviewer's follow up:
is gate squared justified? it sharpens the on/off transition beyond the
linear act of ava; it is a design choice, not a measurement.

## 6 why is nmj gain 0.9?

nmj_gain (src/config.h) scales motor neuron activation into muscle
activation. history: it was 2.4, which slammed the muscle drive into
saturation at every peak (bang bang, the body froze in a clamped
standing wave, kymograph proved segments pinned at the bend ceiling).
0.9 keeps the muscle pair inside its graded range so differential
control survives; the describing function argument is that the loop
self limits where the sigmoid compresses. it is a calibration constant,
not a measurement; the honest label is "tuned for graded operation".

## 7 why did rim hugging emerge from receptor adaptation?

it did not, directly. receptor adaptation (ash tau 2.5 s, edge
refreshed pulses) fixed the corner freeze by ending the perpetual
aversive command; the remaining wall attraction is geometric: the wall
is a position clamp that converts normal push into tangential slide,
and the heading random walk is absorbed at the rim. probes of three
constant level fixes (nociceptor sustained firing, higher wall friction
cost, softer boundary) all failed to get under 30 percent perimeter
time (docs/rim_hugging.md, numbers inside). the suspected real fix is a
segment level wall reaction torque, pure mechanics, not yet
implemented.

## 8 what does the live ablation divergence mean?

see docs/ablation_divergence.md for the table. short form: the model
agrees with fouad 2018 that the gait survives command interneuron
ablation, but predicts avb and pvc removal speed the crawl up 1.3 to
2.3 times where life slows down. the model explanation: the command
layer sets the motor neuron operating point; removing it moves the pool
off sigmoid saturation and the differential reflex gain rises. it is a
falsifiable prediction of the tone-only architecture, documented, not
tuned away.

## 9 how does --ablate-screen count rev_count?

the screen ablates each neuron in turn (sim_ablate_screen, src/sim.c),
resets dynamics per neuron, runs the configured duration, and reads the
metrics counters. reversal counting lives in metrics_step (src/metrics.c):
a reversal episode starts when signed speed stays below -0.02 mm/s,
ends when it rises back, and counts only if it lasted at least 0.5 s.
the screen's rev_count is that counter per ablated neuron against the
control run. caveat: single seed, single 2 s to 60 s window, noisy for
rare events like reversals; treat ratios, not absolutes.

## 10 on which seed and machine is determinism verified?

make test runs two fresh 10 s processes with seed 7 and compares the
full csv bitwise (cmp). it passes on this machine (linux 6.12, gcc,
x86_64) and in ci on github runners (ubuntu latest). determinism is
per binary: same source, same compiler, same flags, same seed gives
identical bytes; cross compiler identity is not claimed. the pool
initializer consumes no randomness (a past bug: capacity changes
silently reseeded runs), the box muller cache lives inside rng_t and
clears on reseed.
