# calibration log, phase 1

## round 7, the agent audit: 58 motor neurons and the serotonin arm

four independent audits ran over the codebase. the c bug hunt proved by
execution: the genome json writer stores per neuron blocks under the
cell index (n0 n1 ...) while the reader looked names up, so every
evolved genome was silently inert except its three global scalars; the
box muller cache lived in a function static and leaked one noise sample
across reseeds, so ablation screen rows were not reproducible from the
seed alone; recv_flag in the csv loader was never cleared, a second
dataset loaded in one process lost 43 percent of synapses; the name
fields truncated at 7 characters merging muscle identities, and the
male whole animal table injected 24 gap junction rows from body wall
muscles into real neurons. all fixed.

the biological sign audit found two more. the builtin wiring built only
six neurons per class (36 total) while positions normalize by the real
class counts, so the posterior ventral body from segment 12 on had no
motor neurons at all; the pool now builds all 58 (white 1986 counts)
and the kymograph wave span grew from 0.4 to 0.67 body lengths. and the
blanket serotonin gain boost applied to the aversive sensory arm, where
the literature says feeding suppresses it (chao 2004); the sensory
output now gets its own polarity (suppressed on food, restored by
octopamine when starved). bend gain recalibrated 6.0 to 7.5 for the
full pool.

battery on the bounded dish: 8 of 10 seeds (speed 0.148 to 0.193,
amplitude 3.84 to 4.02 1/mm, period 1.35 to 1.45 s, tail curl 0.05 to
0.07), validation table 5 match 0 mismatch. assumptions the audit
confirmed as documented design, not bugs: ava gating, octopamine gated
heterosynaptic rule (note: -C paired does not force --no-food), standing
avb bias. open minors from the audits: asel and aser not differentiated,
afd has only the thermophilic arm, dopamine contact shunt also halves
the ash pulse at the moment of contact.

## round 6, two metric bugs and a frictionless rail

three finds, two of them tooling. first, the reported worm speed came
from the raw frame velocity before the wall clamp, so a worm pressed
against the rim reported a permanent crawl into the wall and the
reversal detector read wall presses as giant reversals. the velocity is
now rebuilt from the actual displacement after the clamp. real
reversal episodes turned out to be 0.6 to 1.2 s, inside the rankin and
broster range all along. second, validate.py summed all backward time
in a run and compared it against a per episode range, the metric now
reports the median episode of at least 0.5 s. the phase 2 table reads
5 match, 0 mismatch. third, the rim acted as a frictionless rail: a
worm pressing into it lost the anisotropic drag and its wall gait
outran agar, which fed the fast gait mode. wall friction 0.65 plus
bend gain 6.0 merged the two gaits into one at 0.24 mm/s.

battery on the bounded dish, release build, 10 of 10 seeds pass:
speed 0.231 to 0.248 mm/s, amplitude 2.40 to 2.47 1/mm, period 0.94
to 1.01 s, tail curl 0.00 in every seed. cross connectome: builtin
7.1, varshney 9.1, herm302 7.6, male 6.3 mm net per 120 s.

still open: the worm hugs the rim 95 percent of the time in a 600 s
run (corner 48 percent, wall 47 percent) and reached the food lawn
0 percent in that run. corner stays are bounded now (longest 149 s,
was 313 s) but the perimeter obsession is real. suspected root: the
clamp constrains every touch into tangential motion and nothing in the
circuit turns the heading inward. next candidate: make the reversal
re-entry angle depend on the wave phase, which is circuit and not a
rule.

## round 5, the corners stop holding the worm

the wall touch fed ash with a step that lasted as long as the contact,
and the gentle touch trace refreshed the same way, so a worm wedged
into a corner carried the aversive command forever. both receptor
envelopes now refresh on the contact edge only and decay under
sustained pressure (ash tau 2.5 s, mec tau 0.8 s unchanged), the
aversive pulse got stronger (mec gain 7 to 10) so a single touch can
capture the wave. a 600 s run went from 313 s longest corner stay to
143 s, corner time 56 to 24 percent. what remains: the worm still
hugs the walls 61 percent of the time and reaches the food lawn only
4 percent, wall following is real c elegans behavior but this much is
an artifact of the clamp geometry, open defect.

## round 4, the dish got walls

the worm could walk through the assay plate rim and off the arena,
where the wall contact flag stayed true forever and the constant
aversive drive froze it. the rim is now a hard wall, the frame cannot
penetrate it and the contact band feeds ash one cell before the stop.
this is correct physics and it changed the phenotype: wall bounces
flip seeds between a slow gait near the speed floor and a fast gait
near the ceiling, drag was re-tuned to 19 which keeps the fast mode
at the 0.30 ceiling and the battery reads 4 of 10. phase 1 criterion
is re-opened on the bounded dish. the actual blocker is the known
reversal duration defect, tens of seconds instead of about one: long
reversals at walls are what throw the gait across modes. fixing the
reversal release is the next calibration target and should fix both
the validation mismatch and this battery.

## round 3, after the rng stream fix

the pool initializer drew a random start potential for every cell of
the capacity sized pool, max neurons included. raising the capacity for
the male connectome shifted the noise stream for the whole run and
silently reseeded everything. the initializer now draws nothing, the
first per step noise lands on the same stream positions at any pool
size. seed table re-measured after the fix, this re-blesses the csv
baselines: battery 8 of 10, seeds 2 and 3 under the speed floor
(0.139, 0.134 against 0.15), the rest inside every band.

## round 2, after the sign error fixes

the round 1 sweep at the bottom optimized inside a broken loop: three
sign errors made the proprioceptive loop an anti reflex, and the body
always collapsed into a saturated standing half wave. fixing the signs
changed the problem. the three errors, in the order they were found:

1. dd and vd muscle sides were swapped. dd is the inhibitory dorsal
   neuron, it must inhibit dorsal muscle, not ventral (white 1986).
   with the wrong side any d class tone pushed the body into a bend
   instead of against it.
2. the proprioceptive drive was rectified per class (forward classes
   took the positive half, backward the negative). rectified drive
   ratchets the membrane potential up, the operating point slides into
   the saturated tail of the sigmoid and the small signal loop gain
   dies. replaced by a signed push pull current: stretch depolarizes,
   contraction hyperpolarizes.
3. with signed drive, the inhibitory classes need the inverted sign,
   otherwise the dd/vd path cancels the db/vb reflex. after inversion
   all four motor neurons of a segment push the muscle pair one way.

measured with the release build, 120 s, drag ratio 22, seed sweep.
metrics use run windows only (serotonin below 0.3, that is the crawl
phases between food dwells). target vector from the phase 1 plan:
speed 0.15 to 0.30 mm/s, bend amplitude 2 to 4 1/mm, period 0.7 to
1.5 s, static tail curl near zero.

| seed | run speed mm/s | amplitude 1/mm | period s | tail curl 1/mm | net mm | verdict |
|---|---|---|---|---|---|---|
| 1 | 0.200 | 2.13 | 1.18 | 0.00 | 8.4 | pass |
| 2 | 0.191 | 2.16 | 1.36 | 0.00 | 8.6 | pass |
| 3 | 0.199 | 2.17 | 1.20 | 0.00 | 8.8 | pass |
| 4 | 0.200 | 2.16 | 1.28 | 0.00 | 8.8 | pass |
| 5 | 0.201 | 2.12 | 1.28 | 0.00 | 10.1 | pass |
| 6 | 0.144 | 2.15 | 1.18 | 0.00 | 5.1 | fail, speed 0.006 under floor |
| 7 | 0.203 | 2.13 | 1.23 | 0.00 | 9.0 | pass |
| 8 | 0.154 | 2.19 | 1.36 | 0.00 | 6.5 | pass |
| 9 | 0.203 | 2.17 | 1.17 | 0.00 | 9.0 | pass |
| 10 | 0.193 | 2.12 | 1.30 | 0.00 | 8.4 | pass |

9 of 10 seeds pass, criterion was 8 of 10. period measured per burst
(median over run windows); the whole log zero crossing number reads
high because it mixes bursts and dwells.

final constants of this round: prop gain 36, prop tau 600 ms,
prop lag 4 segments, prop back w 1.2, prop self w 1.0, bend gain 5.8,
nmj gain 0.9, avb bias 8 mv, drag ratio 22, boundary damp 3.0,
boundary seg 3. note: an attempt to recruit the tail by softening the
boundary damping (1.5) passed the builtin battery but broke the
varshney and herm connectomes into a backward drift attractor, the
softer boundary was reverted. the posterior third stays quiet, the
wave span sits near 0.4 body lengths.

known residual gaps:
- wavelength near 0.4 body length, target 0.5 to 1.0. the wave span
  in the kymograph runs head to mid body; the posterior third reads
  the boundary damped zone and stays quiet.
- the drag ratio was calibrated inside the measured agar band rather
  than taken from one paper, because the gray and lissmann 36 value is
  a thin film measurement and the crawl speed is sensitive to it.
- seed 6 speed 0.144 against the 0.15 floor, within noise of the gate.

## round 1, superseded

the sweep below ran against the broken loop, kept for the record. the
score could not break past the standing mode no matter the constants,
which is exactly why the signs, not the gains, were the problem.

objective: net displacement + 40 x wave coherence - 8 x static curl, 30 s runs, seed 1, builtin connectome

| step | score | net | corr | amp | curl |
|---|---|---|---|---|---|
| baseline | 15.04 | 0.12 | -0.93 | 0.42 | 2.81 |
| PROP_GAIN=4.0 | 17.60 | 0.11 | -0.93 | 0.39 | 2.44 |
| PROP_BACK_W=0.8 | 20.73 | 0.08 | -0.89 | 0.38 | 1.88 |
| PROP_TAU_MS=250.0 | 21.16 | 0.08 | -0.9 | 0.37 | 1.88 |
| NMJ_GAIN=0.5 | 27.22 | 0.05 | -0.86 | 0.18 | 0.91 |
| BIAS_AVB_MV=2.0 | 27.27 | 0.05 | -0.86 | 0.19 | 0.9 |
| NOISE_SD_MV=0.2 | 31.13 | 0.05 | -0.95 | 0.17 | 0.89 |
| PROP_GAIN=8.0 | 31.13 | 0.05 | -0.95 | 0.17 | 0.89 |
| PROP_BACK_W=1.2 | 31.13 | 0.05 | -0.95 | 0.17 | 0.89 |
| PROP_TAU_MS=500.0 | 31.13 | 0.05 | -0.95 | 0.17 | 0.89 |
| NMJ_GAIN=0.9 | 31.13 | 0.05 | -0.95 | 0.17 | 0.89 |
| BIAS_AVB_MV=4.0 | 31.13 | 0.05 | -0.95 | 0.17 | 0.89 |
| NOISE_SD_MV=0.4 | 31.13 | 0.05 | -0.95 | 0.17 | 0.89 |
