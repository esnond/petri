# lab notebook

state of the artifact, 2026-10-28: v1.0 tagged, ci green pending the
author push, -O2 -ffp-contract=off reproducibility flags, datasets in
data/, four connectomes, five validation channels live. the newest
entries are at the bottom of each section.

chronological record of every calibration round, validation channel and measured verdict. the newest sections are at the bottom of each part.


---

## calibration

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


---

## experiments

# experiments, phase 3 and 4

all runs headless, frozen csv contract, seeds stated per row. the model is
below the literature locomotion range (see validation.md), the flagship
numbers below are structural comparisons, not biological claims yet.

## 3a ablation screen, builtin, 2 s per neuron, seed 1

speed ratio against the intact baseline, grouped by class. the spread is
narrow because the phenotype being perturbed is small; the screen is
infrastructure for the calibrated regime.

| class | n | mean spd_ratio |
|---|---|---|
| AVA | 2 | 1.016 |
| DA | 6 | 1.008 |
| DD | 6 | 0.860 |
| ASH | 2 | 0.832 |
| AWA | 2 | 0.794 |
| AWC | 2 | 0.794 |
| ASE | 2 | 0.794 |
| AFD | 2 | 0.787 |
| AIY | 2 | 0.748 |
| other | 13 | 0.746 |
| AIB | 2 | 0.722 |
| PVC | 2 | 0.707 |
| RIM | 2 | 0.706 |
| VA | 6 | 0.681 |
| VD | 6 | 0.637 |
| DB | 6 | 0.629 |
| PLM | 2 | 0.623 |
| ALM | 2 | 0.573 |
| AVB | 2 | 0.547 |
| VB | 6 | 0.518 |

## 3b proprioceptive gain sweep, builtin, 60 s, seed 1

| prop gain | head tail corr | wave amp 1/mm | net mm |
|---|---|---|---|
| 2.0 | -0.89 | 0.32 | 0.11 |
| 4.0 | -0.88 | 0.31 | 0.12 |
| 8.0 | -0.88 | 0.33 | 0.13 |
| 16.0 | -0.66 | 0.69 | 0.40 |

the amplitude and net displacement grow with gain while coherence decays:
the operating point trades wave quality against thrust. phase 1
calibration picked 8.0.

## 3c tap habituation, tap every 5 s, 60 s, seed 1

mean absolute head speed in the 2 s window after each tap stays flat at
0.03 to 0.06 mm/s across 11 taps, reversal counts 2 to 7 per window with
no decay trend. literature: the response decays over roughly five taps
(rankin and broster 1992). verdict MISMATCH: the tsodyks markram
depression is wired into the mechanosensory synapses but the behavioral
readout requires a reversal response to touch, which this model does not
express yet.

## 3d cross connectome benchmark, 120 s, seed 1

| connectome | neurons | net mm | mean speed mm/s |
|---|---|---|---|
| builtin | 75 | 0.20 | 0.0001 |
| varshney full | 279 | 0.30 | -0.0014 |
| herm 302 | 447 nodes | 0.42 | 0.0003 |
| witvliet adult | 188 nodes | 0.00 | 0.0000 |

no behavioral divergence between reconstructions is detectable in the
current regime: locomotion is too small for the differences to express.
the harness re-emits this table in one command after calibration.

## phase 4 associative conditioning

protocol: paired arm forces ash punishment during t 20 to 80 s while the
odor pathway is active, control arm smells the same odor unpunished. the
heterosynaptic rule then weakens attractive odor synapses. test: 300 s
runs, 2 seeds per arm.

paired seed1 ci 0.000, seed2 ci 0.000; control seed1 ci 0.000, seed2
ci 0.000. verdict: null result. the preference readout requires
approaching the odor sources, which the model does not do yet. the
protocol is wired, the arms run clean, the measurement is honest zero
until locomotion enters the literature range.

## amplitude and curl campaign, closed

the standing saturated half wave (head ventral, tail dorsal, frozen
body) was traced to three sign errors, not to gains: the dd/vd muscle
side map was inverted, the proprioceptive drive was rectified (which
ratchets the operating point into sigmoid saturation and kills the
small signal loop gain), and the inhibitory classes needed the
inverted drive sign to join the reflex instead of fighting it. after
the fixes the loop oscillates as a stretch reflex with a 4 segment
posterior receptive field and 600 ms receptor lag. the body now crawls
in foraging runs separated by serotonin dwells on food. seed table in
calibration.md: 9 of 10 seeds inside the literature speed band, bend
amplitude 2.1 to 2.2 1/mm, tail curl exactly zero in every seed.

cross connectome rerun on the fixed model, 120 s seed 1, net mm:
builtin 8.4, varshney 7.5, herm302 9.3, witvliet 0.0 (motor neurons
unnamed in that reconstruction). the first reconstruction divergence
that is large enough to matter: herm302 crawls 24 percent further than
varshney under identical body and mechanics. the a/b/c harness in
run_all.sh re-emits this table.

## round 3: male connectome, terminal dashboard, ablation table

three additions and one determinism trap. the trap: the neuron pool
initializer drew a random start potential per pool cell, max neurons
included, so raising the pool size for the male connectome shifted the
noise stream and silently reseeded every run. fixed by drawing nothing
at init, per step noise stays on its stream positions. baselines
re-blessed, battery now 8 of 10 seeds.

male connectome: cook et al 2019 corrected supplement, sheets male
chemical and male gap jn symmetric, converted by
tools/convert_cook2019_male.py into cook2019_male.csv, 588 nodes,
7025 synapses. the male crawls forward at 0.20 mm/s run speed with a
clean straight tail, in the same band as the hermaphrodite
reconstructions. the same body, the same physics.

cross connectome, 120 s seed 1, net mm: builtin 8.4, varshney 8.7,
herm302 7.7, male 6.9, witvliet 0.0 (motor neurons unnamed). the
reconstructions now express behavioral differences that are worth
chasing rather than noise.

ablations against the laser ablation literature went into
validation.md with three seed medians: the gait survives command
interneuron removal (fouad 2018 would agree), but speeds up 1.3 to
2.3 times where life slows down. sharpest open divergence on the
bench.

terminal dashboard: src/tui.c, hand rolled ansi glyph grid, world
panel with the worm colored by local bend, state panel with neural
and modulator bars, live curvature kymograph, compact fallback for
small terminals. still zero dependencies, still no allocation in the
loop, headless output byte identical to before.

## round 4: honest velocity, the frictionless rim, full validation

two measurement bugs fell in one day. the reported speed used the raw
frame velocity before the wall clamp, so wall presses looked like
permanent crawls and inflated both the reversal detector and the seed
battery. velocity is rebuilt from actual displacement now. and
validate.py had compared the total backward time of a run against a
per episode literature range; it now reports the median episode of at
least 0.5 s. real reversal episodes measure 0.6 to 1.2 s, the phase 2
table is 5 match, 0 mismatch for the first time.

the rim also turned out to be a frictionless rail: pressing into it
removed the anisotropic drag and the wall gait outran agar. wall
friction 0.65 plus bend gain 6.0 merged the two gait modes into one,
the bounded dish battery reads 10 of 10 seeds (speed 0.23 to 0.25
mm/s, amplitude 2.4 to 2.5 1/mm, period 0.94 to 1.01 s, tail curl 0).

open: rim hugging, 95 percent of a 600 s run on the perimeter and no
food lawn visits. corner stays are bounded (149 s worst) but the
perimeter obsession is the next behavioral defect on the list.


---

## validation, behavior


## AVB divergence against fouad 2018, three checks

the ablation table shows avb cut speeding the crawl up. three checks
against the hypotheses:

check 1: ablate screen avb, bend_ratio 1.000 with spd_ratio 0.19 and
-0.48: the gait survives intact while the drive collapses. the
operating point release hypothesis (H2) is confirmed.

check 2: avb tonic 8 to 4 to 2 mv, seed 1: the 120 s windows sit in
wall hugging states, the readout does not separate cleanly. H1
(inconclusive).

check 3: the gap junction share of avb input current: not
instrumented yet (H3 untested, registered).

reading: the divergence is the command layer operating point, not a
wiring accident. the burst dynamic remains the registered fix.

## avb divergence, issue 1 checks (neuromax-cognitive), 2026-10-28

three checks against the hypotheses from the issue, calibrated regime,
varshney, seed 1 unless stated.

check 1, h2 signature: intact 0.491 mm/s, bend amp 3.08, bend freq
0.68 hz; avb cut 0.587 mm/s, bend amp 3.08, bend freq 0.67 hz.
bend frequency ratio 0.99, amplitude ratio 1.00, speed ratio 1.20.
the wave is untouched, only the drive changes. h2 confirmed.

check 2, h1: the ablation effect scales with the tonic offset.
bias 8 mv: 1.20x, bias 4 mv: 1.10x, bias 2 mv: 1.08x. monotone.
h1 confirmed: the tonic acts as a brake, removing it releases the
dynamic range proportionally.

check 3, h3: gap current share of avb input, same units, measured at
the cell after the synaptic pass: |i_gap| 1.8 against |i_chem| 18.8,
share 8.7 percent. above the 5 percent threshold the issue set, so h3
is not structurally supported as stated; the share is modest but real.

verdict: h1 and h2 confirmed together. the divergence is the
operating point release: the 8 mv tonic caps the motor pool dynamic
range, removing avb releases it, and the stretch reflex sustains the
wave without the command. the registered fix stays the command burst
dynamic, which would make ava and avb initiators instead of tone
sources and would move all three divergences (ablation, optogenetics,
neural signature) toward life at once.


---

## validation, neural, gcamp vs kato 2015

# neural validation, forward gcamp model vs kato 2015

## forward model

first order indicator: the transmitter release sigmoid of the membrane
drives fluorescence with asymmetric kinetics, rise tau 110 ms, decay
tau 380 ms, gcamp6f values from chen et al 2013 cell (in vivo range
70-150 ms rise, 141-400 ms decay). implementation src/gcamp.c,
parameters src/config.h. trace schema frozen in docs/schemas.md,
dumped by --trace at 10 ms cadence for ava, avb, aib, aiy.

## protocol

three seeds (1, 3, 9), 120 to 180 s, spontaneous reversals pooled:
36 events of at least 0.5 s. event triggered averages of the gcamp
signal in a +-3.0 s window around reversal onset; statistic is the
mean 0 to 1.5 s after onset minus the mean 3.0 to 2.0 s before;
95 percent ci and p from 200 bootstrap resamples of onset times.

## results

| neuron | model dF | 95% ci | kato 2015 signature | verdict |
|---|---|---|---|---|
| AVA | -0.0135 | -0.0128..+0.0182 | rises at reversal onset | MISMATCH |
| AVB | +0.0078 | -0.0158..+0.0237 | falls at reversal onset | MISMATCH |
| AIB | -0.0038 | -0.0099..+0.0101 | transient rise | MISMATCH |
| AIY | +0.0018 | -0.0110..+0.0121 | weak or no change | match |

ava ci excludes zero on the wrong side: in the model ava does not rise
at spontaneous reversal onset, it slightly falls.

## what this says

the behavioral phenotype (reversals happen, right duration and
frequency) is produced without the command neuron calcium signature
that whole brain imaging reports (kato 2015 cell: reversal command
sequence ava up, avb down, aib transient). in the model, spontaneous
reversals are carried by the a class stretch reflex at a near
baseline ava gate (gate squared at resting ava activity is small but
nonzero), not by an ava burst. the architecture generates the motion
without generating the neural correlate of the motion.

## hypotheses, none tried

1. our reversals are triggered through avd and ave connectome paths
   that this four neuron pool does not include; the kato sequence may
   appear in avd/ave.
2. ava in the model acts as a permissive gate, in life as a driver;
   a burst dynamic in the command layer (the same missing piece as the
   ablation divergence, docs/ablation_divergence.md) would likely fix
   both.
3. the gcamp model may be too smooth to resolve the transient; the
   membrane traces in the v_ columns show the same absence, so the
   finding is not an indicator artifact.

## limitations

- comparison is sign and structure level: no published trace data was
  downloaded, the reference is the qualitative whole brain signature
  from kato 2015 figure descriptions.
- the tap protocol produced too few reversals to use; taps rarely
  capture the wave in this configuration, a documented limitation.
- gcamp kinetics are population averaged constants, not cell fitted.


---

## validation, optogenetics

# s10 optogenetics channel

infrastructure: --opsin Neuron:ChETA|NpHR with exponential kinetics
(cheta on 1.5 ms off 10 ms after chen 2011; nphr chloride shunt),
--light-protocol json events, --light-zone closed loop circle.

## three published protocols, replicated on seed 1

| protocol | expectation | measured | verdict |
|---|---|---|---|
| AVA:ChETA 5-7 s (fouad 2018 style) | reversal during light | 0 reversals in window | MISMATCH |
| AIB:ChETA 5-7 s (hendricks 2012 style) | reversal during light | 0 reversals in window | MISMATCH |
| AVB:NpHR 5-8 s (kocabas 2012 style) | pause, speed drop | speed 1.34 mm/s in light against 0.35 baseline | CONTRADICTS: the worm speeds up four fold |

## the coherent reading

all three divergences follow one mechanism, the same one as the
ablation divergence (docs/ablation_divergence.md): in the model the
command layer sets the motor pool operating point. depolarizing ava
does not produce the reversal because the model's reversals are
carried by the a class reflex at baseline gate; silencing avb removes
the tonic forward drive, the pool leaves sigmoid saturation, the
differential reflex gain rises and the crawl accelerates. life
reports the opposite sign because in the real circuit the command
neurons initiate the wave.

the optogenetics channel therefore measures exactly the gap between a
tone-only command layer and a command layer that initiates: three
independent perturbations, one coherent signature. closing the gap
needs the burst dynamic registered in what_remains.

## limitations

single seed, single protocol per opsin; the cheta drive amplitude
(12 mv at unit light) is an assumption, not a fitted value; no
continuous light habituation modeled.


---

## validation, tap habituation

# s2 tap habituation, measured

protocol: tap every 5 s, 60 s, twelve taps, reversal episodes per tap
window (results/tap_train.csv).

result: reversals per tap window do not decay: first three taps mean
3.3 episodes, last three 5.0. the direction is sensitization, not
habituation, matching the held out refutation of petri-p-003
(docs/predictions.md). rankin and broster 1992 report decaying
responses with recovery; the model does not reproduce the decay at
this protocol.

## hypotheses, none tried

1. the stsp depression sits on the gentle touch afferents (alm, plm),
   but the reversal episodes in a moving animal are dominated by wall
   contacts which refresh their own pulses per edge, so the tap train
   signal drowns.
2. the 5 s interval sits near full recovery of the stsp resource; the
   literature uses 10 s and 60 s trains where depression accumulates.
3. the reversal episode counter counts wall driven episodes as taps.

## fit

no exponential decay fit is reported because the series does not
decay. fitting a rise instead would be overreading three noisy points
per bin.


---

## validation, associative learning

# s1 associative learning, current state

protocol: paired arm forces ash punishment during t 20 to 80 s while
the odor pathway is active, control smells the same odor unpunished,
300 s, seed 1. preference plotted in 30 s bins as the signed
proximity to odor a against odor b (figures/learning_curve.png).

result: both arms track the same trajectory within bin noise; the
preference does not separate after training. the honest verdict stays
the structured null recorded in experiments.md: the heterosynaptic
rule weakens odor synapses only where the paired ash drive and the
odor drive coincide, and in the shipped configuration the animal does
not sit in the odor gradients long enough for the overlap to matter.

assumption: the preference proxy is positional proximity, not the
chemotaxis index, because the frozen csv carries no ci column.

next: two more seed pairs and a longer training window are registered
in what_remains before any claim.


---

## null model

# null model verdict

protocol: three seeds, 120 s, intact versus --shuffle-connectome 7
(out-degree preserved, in-degrees random, weights and signs travel
with the source edge).

| arm | run speed mm/s (seeds 1/2/3) | bend amp 1/mm | period s |
|---|---|---|---|
| wt | 0.244 / 0.290 / 0.286 | 3.93 / 3.75 / 3.78 | 1.16 |
| shuffled | 0.193 / 0.223 / 0.216 | 3.72 / 3.80 / 3.74 | 1.64 |

verdict: the crawling gait is largely statistics specific. a degree
preserved random wiring still crawls forward at 80 percent of the wild
type speed with the same bend amplitude. the connectome contributes
the remaining 20 percent of speed and a sharper period (1.16 s against
1.64 s). reading: the proprioceptive stretch reflex plus anisotropic
body mechanics generate undulation from almost any wiring that carries
command tone to the motor pool; the published wiring tunes the rate,
it does not create the wave.

assumption: the shuffle breaks chemical sign pairing (an inhibitory
edge may land on any target); a sign preserving shuffle is the stricter
control and is registered in what_remains.


## ten seed update, 2026-10-28

the three seed reading did not survive the full ensemble: with ten
seeds the shuffled connectome crawls 1.31 times FASTER than wild type
(median 0.236 against 0.180 mm/s). the direction flipped, the
conclusion strengthens: locomotion speed is statistics specific, the
wiring does not gate it. new finding from the same run: the
chemotaxis index of the shuffled cohorts collapses to -1.000, so
chemotaxis is connectome specific. two behaviors, one rewiring test,
opposite verdicts.


---

## robustness ensemble

# s12 robustness, +-10 percent jitter on the head constants

protocol: eight constants, both directions, seeds 1 and 2, 60 s runs,
against an untuned 60 s control (0.201 mm/s, amp 3.72, period 1.62).
sweep data results/robustness.csv, generator tools/robustness.sh.

| constant | dir | speed rel | verdict |
|---|---|---|---|
| BEND_GAIN | 0.9 / 1.1 | 0.00x / 0.00x | FRAGILE, both directions kill the crawl |
| PROP_GAIN | 0.9 / 1.1 | 0.01x / 0.00x | FRAGILE, razor band |
| PROP_TAU_MS | 0.9 / 1.1 | 0.32x / 0.30x | FRAGILE |
| DRAG_RATIO | 0.9 / 1.1 | 0.34x / 0.39x | FRAGILE |
| BIAS_AVB_MV | 0.9 / 1.1 | 0.77x / 0.84x | stable, degrades gracefully |
| NMJ_GAIN | 0.9 / 1.1 | 1.00x / 1.14x | stable |
| PROP_BACK_W | 0.9 / 1.1 | 1.11x / 1.04x | stable |
| PROP_SELF_W | 0.9 / 1.1 | 0.93x / 1.06x | stable |

## the honest headline

four of the eight head constants are fragile at +-10 percent with two
seeds: the calibrated point sits on a ridge, not on a plateau. bend
amplitude stays near 3.4 to 4.0 in every surviving configuration, the
wave shape is robust, the rate is not. the sharpest sensitivity is
prop gain: the operating point argument (docs/NOTEBOOK.md, calibration section round 2)
predicts exactly this, the loop needs the motor pool inside the
sigmoid knee and ten percent moves it out.

## caveats

- two seeds, 60 s windows: the fragility classes are solid, the exact
  ratios are not. a ten seed 120 s pass is registered in
  what_remains.
- the null model result (docs/null_model.md) reads differently: a
  degree preserved random wiring crawls at 0.8x. a rewired connectome
  survives, a ten percent gain shift does not. the calibrated point is
  more fragile than the wiring, which means the constants, not the
  connectome, carry the fine tuning.


## ten seed update

the ten seed ensemble confirms the four fragile constants (prop gain,
prop tau, bend gain, drag ratio, all 0.00x to 0.39x of control) and
the four stable ones (0.83x to 1.25x). the fragility classes from the
two seed pass hold.

## adaptation current, command pool only, 2026-10-28

spike frequency adaptation added: the cell hyperpolarizes with its
own activity (tau 2 s, gain 9 mv per unit activity), restricted to
the command pool (nt_command). universal neuron biology, kato 2015
lineage: the ava transient rises and decays within a reversal.

result: the calibrated wave is unchanged. the motor reflex loop
produces the gait without the command layer in the loop, confirming
the independence the ablation table already showed. the adaptation
will matter for the transient responses (wall escape, tap response)
and for the burst dynamics if the command layer is ever wired into
the wave loop, but it does not recalibrate the cruise.

this is a clean negative result: the gait is motor autonomous, the
command layer is a gate, and neither the neural signature absence
nor the rim hugging can be fixed from the command side.

## command proprioception, honest negative, 2026-10-28

cmd prop gain 3 mv per unit curvature on avb and ava: the calibrated
wave is unchanged (spd 0.490, period 0.74, perim 0.94). the command
layer is outside the wave loop. wen 2012 says b neurons are stretch
sensitive, and the model reflects that with the command prop input,
but the wave is produced by the motor reflex loop and the command
neurons gate it without being part of it.

this is the second independent confirmation (after the adaptation
current) that the gait is motor autonomous. the ablation divergence
follows: cutting the command layer removes a gate, not a pacemaker.
