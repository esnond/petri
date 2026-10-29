# release plan, v1.2

composed 2026-10-29 from the inline prioritizer, architect,
methodologist and adversary passes. memory pass: agent spawns were
blocked provider side all day, inline review is the established
workaround (HX-4, HX-5); no petri release decisions contradict this
plan. interdisciplinary, blue_team, seam-auditor and schedule-attacker
not spawned: no proteus seam, no MTD schedules, security pass done at
v1.0.1, no analogies needed for a mechanical release.

release definition: every published number reproducible at the tagged
commit. tag v1.2 moves to the final commit; the user pushes.

## phase A, validation channels at v1.2 constants, release blocking

the channel outputs predate the v1.2 calibration, so README, DATASETS
and paper cite dead numbers. rerun everything through nohup with logs
(night-shift-ops), one commit per channel (git-discipline), frozen csv
headers (csv-contract), mean and 95 percent ci with n (statistical-rigor),
phenotype lines per ablation-interpreter.

A1 mutant screen, tools/mutant_screen.sh, 20 mutants x 3 seeds,
   about 15 min cpu. update the mutant table in docs/DATASETS.md.
A2 cross connectome, tools/benchmark_cross_connectome.sh,
   4 connectomes x 10 seeds x 300 s, about 25 min cpu. update the
   table, regenerate figures.
A3 null model, tools/null_model.sh, 20 runs, about 12 min cpu.
A4 predictions, tools/test_predictions.sh, re-score every p-00x at
   the held-out seeds. any status flip goes to docs/DEFECTS.md and
   docs/ERRATA.md v1.2 the same day. adversary: this is the most
   likely place the calibration changed a verdict.

## phase B, h1.4 habituation, registered and untested

protocol: tapped arm with -T 30000 over 600 s, 5 seeds, control arm
without tap, same seeds. metric: reversal duration and speed dip per
tap index, per seed slope, ci across seeds. h1.4 predicts decline,
falsification is flat or rising. the earlier tap train showed
sensitization, so a refutation is the expected outcome and counts as
a result, not a failure. record in docs/hypotheses.md and the
notebook. gates: biological-validation, csv-contract, statistical-rigor.

## phase C, documents

C1 README: replace every stale channel number with the phase A
   outputs.
C2 paper.md: stamp as the v1.0.1 snapshot and point the results
   section at the v1.2 battery, cheaper than a rewrite (editor pass).
C3 the four docx in petri-docs, regenerated from the updated sources
   through the private pipeline, verified by unzipping and grepping
   for the v1.2 numbers, not by trusting the build exit code.
   adversary: silent regeneration failure is the known failure mode.
C4 ERRATA v1.2: channel number changes and any prediction flips.
C5 manifest refresh (reproducibility-manifest) as the last commit
   before the tag.

## phase D, release

D1 move the local tag v1.2 to the final commit, run make test and the
   determinism check one last time.
D2 user only: git push --force origin main and v1.2, zenodo doi,
   validation on real tracks with the petri-docs kit.

## not doing, with reasons

- chasing s5 perimeter 0.31 and the s6/s9 period: the mec_g threshold
  trades amplitude against perimeter, fixing one seed breaks the mean.
- robustness plateau: structural, documented.
- 600 s x 50 seeds for h3.B: the null is already stable at ci 0.02.
- any new feature: suckless default is no.

token economy: batched nohup runs, one output read per channel, no
agent spawns (blocked), inline passes compressed, single plan commit.
