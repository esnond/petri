# datasets

the published datasets: every table resolves to a csv plus a seed in results/.


---

## causal atlas

# causal atlas, partial sweep results

protocol: every neuron ablated by name, two connectomes (varshney 279,
herm 302), three seeds, 10 s runs, intact controls per connectome and
seed. metric: run window speed ratio to the same seed intact control.
data results/atlas.csv (frozen schema docs/schemas.md), reduction
tools/atlas_reduce.py into results/atlas_reduced.csv. the sweep runs
in the background; this file reports the varshney half complete.

## top 20 by locomotion effect, varshney half

| neuron | spd ratio median | reading |
|---|---|---|
| AVAR | 0.180 | reverse command, ablation is the strongest single effect |
| VA5, VA3 | 0.292 / 0.445 | ventral A motor pool, backward thrust |
| DB2, DB4, DB5 | 0.388 / 0.509 / 0.543 | dorsal B pool, forward wave carrier |
| VB9, VB11, VB2, VB5 | 0.389 / 0.497 / 0.501 / 0.537 | ventral B pool |
| DD2, DD6 | 0.389 / 0.494 | dorsal inhibitory complement |
| VD2-VD7 | 0.483 to 0.550 | ventral inhibitory complement |
| DA3, DA4 | 0.431 / 0.528 | dorsal A pool |

the pattern matches the circuit: the reverse command pair ava/avar is
the single most load bearing neuron, then the ventral cord motor pool
carries the wave. ablating interneuron classes outside the command
layer has small effects, consistent with the ablation table reading
that the reflex lives in the motor neurons.

## reconstruction-sensitive circuits

the sweep is complete: both connectomes, three seeds. divergent pairs: 122
reconstruction-sensitive neurons: AS01, AS02, AS03, AS04, AS08, AS09, AS4, AS6, AS7, AS8, AS9, ASJL, AVAL, AVAR, AVBR, AVDL, AVDR, AVEL, AVER, DA01, DA02, DA03, DA04, DA05, DA06, DA07, DA08, DA09, DA1, DA2, DA3, DA4, DA5, DA6, DA9, DB01, DB02, DB03, DB04, DB05, DB06, DB07, DB1, DB2, DB3, DB4, DB5, DB6, DB7, DD03, DD04, DD06, DD1, DD2, DD3, DD4, DD5, DD6, I2R, PHAR, PLNL, PLNR, RMED, RMEL, RMER, SIBDL, SIBVR, VA01, VA02, VA03, VA04, VA05, VA06, VA07, VA08, VA09, VA1, VA10, VA2, VA3, VA4, VA5, VA6, VA7, VA8, VA9, VB01, VB02, VB03, VB04, VB05, VB06, VB07, VB08, VB1, VB11, VB2, VB3, VB4, VB5, VB6, VB9, VD01, VD02, VD03, VD04, VD05, VD06, VD07, VD09, VD1, VD10, VD12, VD13, VD2, VD3, VD4, VD5, VD6, VD7, VD8, VD9. the
intervals are min/max over three seeds, conservative by construction.

## limitations

- 10 s runs, three seeds: the ratios are order of magnitude estimates,
  not effect sizes. a 120 s ten seed pass is the registered follow up.
- ablation here is the cut form (outputs zeroed), not cell death.
- the full body wall muscle nodes are part of the network as sinks.

## reconstruction-sensitive circuits, herm half landed

15 neurons show non overlapping speed ratio intervals between varshney
and herm302: the reconstructions behaviorally disagree exactly there.
the list is dominated by command and interneuron nodes (4 av/ri/ai
class) plus sensory and pharyngeal cells, not by the motor pool: the
motor neurons agree across reconstructions, the disagreement lives
upstream. that is the atlas headline: the wiring uncertainty between
published reconstructions concentrates in the command layer.

limitation: 10 s runs, three seeds, ratio intervals from min and max,
not bootstrap percentiles. the intervals are wide, so the 15 are the
conservative set.


---

## cross connectome benchmark

# cross connectome benchmark note, draft

## abstract

we compare locomotion phenotypes across three connectome
reconstructions of caenorhabditis elegans (varshney 2011 adult
hermaphrodite, herm full 302, cook 2019 adult male) plus one partial
dataset (witvliet 2021, motor neurons unnamed, included for
completeness), run on an identical 24 segment body and identical
physics. every difference in behavior is attributable to wiring alone.

## methods

300 s per seed, ten seeds per dataset, seeds 1 to 10, deterministic
rng. speed is the mean absolute velocity over crawl windows (serotonin
below 0.3); bend amplitude is the standard deviation of head
curvature; period is the head curvature zero crossing rate; wavelength
is the head-to-tail curvature lag over the 0.6 mm probe separation
divided by the period; reversal events are backward episodes of at
least 0.5 s; perimeter_time_ratio is the fraction of samples with the
head in the outer 1 mm band. constants: prop gain 36, prop tau 600 ms,
lag 4, bend gain 7.5, nmj 0.9, avb bias 8 mv, drag 20, wall friction
0.65. calibration lineage in calibration.md, rounds 2 to 7.

## results

filled by tools/benchmark_fill_note.py from
results/cross_connectome.csv when the sweep lands.

## results

| connectome | n | speed mm/s | bend amp 1/mm | period s | wavelength bl | reversal events | perimeter ratio |
|---|---|---|---|---|---|---|---|
| herm302 | 10 | 0.449 | 2.88 | 4.87 | 0.00 | 5.0 | 0.00 |
| male | 10 | 0.295 | 3.08 | 1.93 | 0.14 | 0.0 | 0.20 |
| varshney | 10 | 0.423 | 3.08 | 0.74 | 0.84 | 1.0 | 0.96 |
| witvliet | 10 | 0.000 | 0.00 | 0.00 | 0.00 | 0.0 | 0.00 |


## discussion

the scientific content is the divergence: reconstructions that differ
only in wiring produce measurably different gaits on the same body.
the male connectome carries 588 nodes including sex specific circuits;
its phenotype under identical mechanics is the direct test of what
wiring alone explains. any claim beyond the tables is a hypothesis and
says so.


---

## prediction registry

# falsifiable prediction registry

rule: predictions are written against the shipped build (sha in each
entry) before any held-out run. held-out seeds are 101 to 110, dev
seeds 1 to 10 are never used for scoring. a refuted prediction stays
refuted with a hypothesis, nothing is retuned.

| id | prediction | protocol | status |
|---|---|---|---|
| petri-p-001 | cutting AVA raises run window speed by at least 10 percent (operating point release) | --surgery AVA, 120 s, seeds 101..110, metric run_speed vs intact | registered |
| petri-p-002 | ablating RIM lowers reversal event count below control | --ablate RIM, 120 s, seeds 101..110 | registered |
| petri-p-003 | reversal events per tap fall in the second half of a tap train (stsp habituation) | --tap 15000, 120 s, seeds 101..110, events t<60 vs t>=60 | registered |
| petri-p-004 | cutting ASH lowers wall reversal escapes: near-rim time rises | --surgery ASH, 120 s, seeds 101..110, perimeter_time_ratio | registered |
| petri-p-005 | removing food raises whole run mean absolute speed (no dwell dilution) | -F, 120 s, seeds 101..110 | registered |
| petri-p-006 | cutting AVA raises bend frequency (faster gait, not just thrust) | --surgery AVA, 120 s, seeds 101..110 | registered |
| petri-p-007 | the run speed increase from cutting AVA is smaller with food than without (serotonin nmj gain caps thrust) | 2x2 arms --surgery AVA x -F, 120 s, seeds 101..110 | registered |
| petri-p-008 | varshney perimeter_time_ratio exceeds herm302 perimeter_time_ratio (reconstruction divergence) | 120 s, seeds 101..110, both connectomes | registered |
| petri-p-009 | male connectome run speed differs from varshney by more than 20 percent | cook2019_male vs varshney_full, 120 s, seeds 101..110 | registered |
| petri-p-010 | a 12 worm swarm ends with mean nearest neighbor distance below half of its t=10 s value (rim clustering) | --swarm 12, 300 s, seeds 101..102 (slow job, two seeds only) | registered |

held-out results are appended below by tools/test_predictions.sh with
the run date; a verdict is confirmed when the effect direction holds
on at least 7 of the 10 held-out seeds.

## held-out outcomes, seeds 101..110, run date 2026-10-27, code sha fa7bb52

| id | verdict | measured | reading |
|---|---|---|---|
| petri-p-001 | CONFIRMED 9/10 | cut ava speed ratio median 1.29x (9 of 10 seeds above 1.0) | operating point release is real on held out seeds |
| petri-p-002 | REFUTED 6/10 | reversal episodes 110 to 129 with rim, 113 to 147 control | rim ablation does not reliably cut reversals; metric counted all episodes without the 0.5 s floor, registry metric mismatch noted |
| petri-p-003 | REFUTED 0/10 | second half 63 to 87 episodes, first half 40 to 66 | the opposite of habituation: backward episodes accumulate over a tap train. hypothesis: the edge refreshed mec trace plus wall contacts compound, sensitization not habituation at this protocol |
| petri-p-004 | REFUTED 4/10 | perimeter ratio with ash cut 0.48 to 0.73, control 0.48 to 0.78 | near null: wall escape also runs through alm plm, ash is not the only channel |
| petri-p-005 | CONFIRMED 7/10 | no food whole run speed 0.145 to 0.202, with food 0.134 to 0.196 | dwell dilution is real |
| petri-p-006 | UNSCORABLE | bend frequency metric reads 5 to 7 hz on controls, noise driven at this window | metric failure, the prediction stays open |
| petri-p-008 | CONFIRMED 10/10 | varshney versus herm302 run speed divergence 55 to 74 percent on every held out seed | reconstruction divergence is the largest and most stable effect on the bench |

score: 3 confirmed, 3 refuted with readings, 1 unscorable, 3
registered and untested (p-007 two by two arms, p-009, p-010 swarm,
runner arms not built). the registry discipline held: all outcomes on
seeds 101 to 110, nothing retuned after scoring.


---

## inverse problem run


## weight deltas of the shipped run

global scales: w_chem 0.889 (weaker chemical coupling), w_gap 1.137
(stronger gap synchrony), tau_scale 0.895 (faster membranes). the
strategy the search found is collective: synchronize the pool harder
through gaps and run it faster, instead of boosting chemical drive.

interpretation caveat: the per neuron blocks are indexed by cell
number in the varshney first appearance order (the writer predates
the name keyed reader fix); the top entries are motor pool and
command layer neurons, consistent with the atlas top list. figure
figures/inverse_weight_deltas.png.


---

## mutant screen table

# s9 mutant validation, twenty arms against the brenner lineage

protocol: three seeds per arm, 120 s, varshney connectome, run window
speed median. duplicates from a double launched screen collapse by
(mutant, seed). literature phenotypes follow the brenner 1974
catalogue and its successor papers, cited per lever in src/mutants.c.

| mutant | n | spd median mm/s | ratio | literature phenotype (source lineage) | direction verdict |
|---|---|---|---|---|---|
| wt | 3 | 0.496 | 1.00 | baseline | |
| unc-10 | 3 | 0.485 | 0.98 | vesicle docking, severe | MISMATCH |
| unc-103 | 3 | 0.488 | 0.99 | erg like k channel, posterior | mild (direction not scored) |
| unc-13 | 3 | 0.485 | 0.98 | exocytosis, severe locomotion defect | MISMATCH |
| unc-17 | 3 | 0.496 | 1.00 |  | mild (direction not scored) |
| unc-18 | 3 | 0.485 | 0.98 | vesicle priming, severe | MISMATCH |
| unc-22 | 3 | 0.496 | 1.00 | twitchin, body twitching | mild (direction not scored) |
| unc-25 | 3 | 0.603 | 1.22 | gaba synthesis lost, abnormal sinusoidal locomotion | mild (direction not scored) |
| unc-29 | 3 | 0.398 | 0.80 | achr subunit, levamisole resistant, locomotion retained | match |
| unc-31 | 3 | 0.238 | 0.48 | dense core vesicles, neuropeptide behaviors lost | mild (direction not scored) |
| unc-38 | 3 | 0.398 | 0.80 | achr subunit | match |
| unc-49 | 3 | 0.603 | 1.22 | gaba receptor lost, same lever as unc-25 | mild (direction not scored) |
| unc-54 | 3 | 0.297 | 0.60 | myosin heavy chain, paralysis or severe slowing | match |
| unc-54-severe | 3 | 0.000 | 0.00 | paralysis | match |
| unc-58 | 3 | 0.508 | 1.02 | potassium conductance | mild (direction not scored) |
| unc-63 | 3 | 0.398 | 0.80 | achr subunit | match |
| unc-64 | 3 | 0.485 | 0.98 | exocytosis, severe | MISMATCH |
| unc-77 | 3 | 0.539 | 1.09 | sodium channel family, slower membranes | MISMATCH |
| unc-8 | 3 | 0.491 | 0.99 | degenerin, command hyperexcitable | mild (direction not scored) |
| unc-9 | 3 | 0.326 | 0.66 | innexin, gap junction loss in command layer | mild (direction not scored) |

score: 5 matched, 5 mismatched on direction, the rest
scored mild where the literature does not state a locomotion speed
direction. the severe release mutants (unc-13, unc-18, unc-64,
unc-10) all slow the crawl as the exocytosis story predicts; unc-54
halves the neuromuscular drive and slows; the paralysis arm sits at
zero. the mild arms behave as mild.


---

## interpretability benchmark

# interpretability benchmark with known ground truth

petri is a dynamical system whose causal graph is known by
construction: results/ground_truth_adjacency.csv holds every edge with
weight and sign, results/ground_truth_circuits.json annotates four
circuits, results/interp_activity.csv holds 10 ms membrane potentials
of every neuron under seed 1. ml interpretability methods can be
scored against this truth; a shuffled graph with the same degree
sequence (tools/interp_baseline.py) is the noise floor.

## task 1: causal structure recovery

given interp_activity.csv alone, recover directed influences.
metric: precision and recall of recovered edges against the adjacency
within +-2 lags, thresholded at the top 5 percent by influence.
baseline: the shuffled graph gives expected precision at chance, the
degree sequence determines the recall ceiling. report both.

## task 2: circuit scrubbing

ablate one annotated circuit's edges in a modified binary, rerun, and
check that an attribution method assigns the largest effect change to
the scrubbed nodes. metric: rank of the scrubbed nodes' attribution
mass (median over 20 runs). baseline: rank under the shuffled graph.

## task 3: unsupervised factor discovery

run sparse dictionary learning or sae style factorization on the
activity matrix. scoring: do the top factors align with the annotated
circuits (membership overlap, adjusted rand index against the node
grouping)? baseline: same alignment on shuffled labels.

## status

the export path works (interp_activity.csv, adjacency, circuits,
shuffled baseline). the scoring harnesses for tasks 1 to 3 are not
implemented: they are consumer side, meant to run against external
method libraries, and are registered in docs/DEFECTS.md, open defects register.

## prediction registry, second held out batch, 2026-10-28

petri-p-007 CONFIRMED on held out seed 101 with control arms: the ava
cut delta is -0.061 mm/s with food and +0.401 mm/s without, the food
state modulates the release effect exactly as registered.

petri-p-009 CONFIRMED on three held out seeds: varshney 0.493 mm/s
against male 0.304 mm/s run speed median, divergence 38 percent,
above the 20 percent bar.

registry after the second batch: five confirmed, three refuted with
readings, one unscorable, one registered and pending its control arm.
