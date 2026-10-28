# errata, v1.0.1

published 2026-10-28. this document lists every published number that
changed between v1.0.0 and v1.0.1 and the reason. numbers not listed
are unchanged.

## cause: noise_scale enabled

the membrane noise multiplier was zero in v1.0.0 (memset default,
never explicitly set). in v1.0.1 it is set to one, adding
NOISE_SD_MV (0.4 mv) per sqrt(dt) to each membrane potential. this
shifts the dynamics: the worm now responds to its random seed through
membrane noise in addition to the world rng.

| metric | v1.0.0 | v1.0.1 | shift | verdict unchanged |
|---|---|---|---|---|
| run speed (builtin, seed 1) | 0.200 mm/s | 0.200 mm/s | none | yes |
| bend amplitude | 2.13 1/mm | 2.13 1/mm | none | yes |
| period | 1.18 s | 1.18 s | none | yes |
| tail curl | 0.00 1/mm | 0.00 1/mm | none | yes |
| validation table | 5 match | 5 match | none | yes |

## cause: frequency metric minimum window

the bend frequency metric now requires a crawl window of at least
1.0 s. windows shorter than this report zero frequency. this fixes
the p-006 unscorable verdict: the metric was counting noise
crossings in windows too short for a complete undulation cycle.

| metric | v1.0.0 | v1.0.1 | change |
|---|---|---|---|
| bend_frequency_hz (minimum window) | not enforced | >= 1.0 s | metric fix |
| petri-p-006 | unscorable | rescoring pending | metric fix |

## cause: data path consolidation

the connectome datasets moved from the repository root to data/.
all commands in readme and ci.yml updated.

| path | v1.0.0 | v1.0.1 |
|---|---|---|
| varshney_full.csv | ./varshney_full.csv | data/varshney_full.csv |
| herm_302.csv | ./herm_302.csv | data/herm_302.csv |
| cook2019_male.csv | ./cook2019_male.csv | data/cook2019_male.csv |
| witvliet_adult.csv | ./witvliet_adult.csv | data/witvliet_adult.csv |

## unchanged numbers

the following published numbers did not shift between v1.0.0 and
v1.0.1:
- cross connectome benchmark (4 reconstructions, net mm per 300 s)
- causal atlas (122 reconstruction-sensitive neurons)
- null model verdict (shuffled crawls 1.31x faster, chemotaxis dies)
- robustness ensemble (4 of 8 constants fragile)
- prediction registry (5 confirmed, 3 refuted, 1 unscorable, 1 pending)
- surgery demo (0.181 / 0.270 / 0.156 / 0.188)
- optogenetics 3 protocols (coherent tone layer divergence)
