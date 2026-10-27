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
