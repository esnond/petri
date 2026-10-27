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
