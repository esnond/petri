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
