# ablation divergence against published laser ablations

## the protocol

three seeds per arm, 120 s, median run window speed (serotonin below
0.3, food dwells excluded), prefix ablation hits both bilateral
partners. published phenotypes are directional claims, the verdict
compares directions.

## the table

| ablated | model speed | ratio | published phenotype | source | verdict |
|---|---|---|---|---|---|
| none | 0.139 mm/s | 1.00 | baseline | | |
| AVA | 0.183 mm/s | 1.32 | reversals gone, forward crawling largely spared | gray hill bargmann 2005 pnas; fouad 2018 elife | forward spared: matches |
| AVB | 0.314 mm/s | 2.25 | forward strongly impaired | fouad 2018 elife | contradicts: our gait speeds up |
| PVC | 0.192 mm/s | 1.38 | forward impaired | gray 2005; liu 2012 plos one | mild mismatch: forward speed holds |
| AVD | 0.200 mm/s | 1.43 | backward impaired, forward spared | liu 2012 plos one | forward spared: matches |

## the divergence, stated precisely

in the real animal, removing the forward command interneurons (AVB,
PVC) slows or abolishes forward locomotion. in the model the same
ablations speed the crawl up by 1.3 to 2.3 times. the model agrees with
fouad 2018 that the gait itself survives (the proprioceptive stretch
reflex lives in the motor neurons, not in the command layer), it
disagrees about the sign of the rate change.

## why we believe the model does this

the command layer in the model holds the motor neuron operating point
through tonic chemical drive. AVB depolarizes the forward pools
(bias 8 mv plus 2.0 weight excitation); removing it moves the pool away
from sigmoid saturation, the differential stretch reflex gain rises,
and the gait frequency and thrust increase. in life the command
interneurons appear to pace and initiate the wave rather than merely
set a tone; our architecture gives them tone only.

## hypotheses that would close the gap, none tried

1. the missing piece is wave initiation, not pacing: in life AVB/PVC
   bursts start each run. adding a burst dynamic to the command layer
   is a circuit change and needs its own literature anchor.
2. the real motor circuit contains electrical coupling from the command
   layer that our weights underrepresent; an ablated animal then loses
   more drive than our operating point argument predicts.
3. our graded synapse model may overestimate the residual drive of
   partially saturated pools after ablation.

## what we did not do

no constant was tuned to reduce this divergence. the numbers above are
the shipped configuration (commit 5e6a1de lineage).
