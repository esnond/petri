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
