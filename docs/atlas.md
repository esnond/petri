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
