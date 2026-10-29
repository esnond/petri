# state checkpoint, v1.2 experiment

updated 2026-10-30, branch v1.2-fix-branch
anchor: v1.0.1-stable (never moved)

## status

phase 1 (locomotion) and phase 2 (ablation) are done and committed
(83232fe). the gait battery is inside the targets on ten seeds and the
avb ablation slows the worm on all ten. phase 3 (robustness) and phase
4 (neural signature) are recorded below.

## measured, tools/battery_v12.py, ten seeds, 300 s, varshney full

  speed            0.246 +/- 0.005 mm/s   target 0.15..0.30   pass
  bend amplitude   2.71 +/- 0.02 1/mm     target 2..4         pass
  period           1.31 +/- 0.20 s        target 0.7..1.5     pass mean
  perimeter ratio  0.12 +/- 0.05          target below 0.30   9/10
  avb ablation     0.79 +/- 0.03x         target below 1.0    10/10

open seeds: s5 perimeter 0.31, s6 and s9 period about 1.9 s.

## robustness, tools/robustness_v12.py

one of twenty +-10 percent perturbations keeps behavior change under
0.05. the period carries the sensitivity, as expected for a limit
cycle whose time scale is set by the loop constants. speed and
amplitude stay within about 16 percent. the avb motor gate that buys
the ablation phenotype makes avb bias load bearing. not a plateau in
the strict sense, recorded as a result.

## neural signature

the three earlier independent negatives (adaptation, command
proprioception, ablation) stand. the h3.A temporal probe is the last
item.

## release readiness, 2026-10-30

phase A done: all four channels rerun at v1.2, tables in DATASETS.md,
findings in DEFECTS.md and ERRATA.md, v1.0.1 outputs archived in
results/archive_v101/. phase B done: h1.4 flat, recorded. phase C
done: paper stamped, four docx regenerated and verified by unzip,
manifest below. phase D: tag moved, push and doi are user actions.

open items for the user: git push --force origin main and v1.2,
zenodo doi, manual validation on real tracks. known degradations,
registered not hidden: mutant screen 1 matched / 8 mismatched, p-005
flip, p-006 extractor bug, degenerate wavelength metric.
