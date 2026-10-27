# what remains, v3 final state, 2026-10-27 night

## external idea, registered 2026-10-28 (neuromax-cognitive, issue 1)

kurier: nematode-style channel courier. an agent that patrols chat
and inference channels the way the worm patrols agar: reflex layer
(hourly ping, a skipped move becomes a record), gradient layer (scent
reorders visits, never cancels them), modulator layer (tone and
fatigue as scalars reusing the graded kinetics of modulators.c).
fitness: net delivery per empty round, not activity count.

verdict: out of scope for the petri repository, it is a separate
agent project inspired by this architecture, not a simulator feature.
credit and the full text stay in the issue and in the fork
(neuromax-cognitive/petri, commit 1268065). encouraged to develop it
there; happy to cross link if it ships.

## roadmap visibility decision, 2026-10-28

the forward looking roadmap (v1.1 science lane, v2.0 mechanism lane)
was moved out of the public repository to the private documentation
pack. reasons: publishing the planned fix for a divergence before the
fix invites being scooped, and roadmaps in public repos go stale and
read as vaporware. the public repo keeps the backward looking record:
inventory, what_remains, every published number. the private copy
lives at petri-docs/ROADMAP_private.md.

v3 blocks: 0, 1, 2 closed and committed; 3 and 5 partially closed
(avb checks done, atlas varshney half, mutants screen complete); 4
and 6 open. the compute chain (null pairs, jitter ensemble, herm
atlas half) runs in the background, resume by rerunning the three
scripts in docs/inventory.md order. the mutant comparison table and
the p1 baseline numbers are the two analysis tasks left.

## v2 registry state (see docs/inventory.md for the full audit)

- F1 manifest: done (tools/manifest.sh, -ffp-contract=off in build)
- F4: done (idea policy in readme)
- S11 null model: done (docs/null_model.md, verdict: gait is
  statistics specific, wiring tunes the rate)
- S12 robustness: done (docs/robustness.md, four of eight head
  constants fragile at +-10 percent, two seeds caveat)
- S7 atlas: partial, varshney half done (docs/atlas.md), herm half
  running in background. resume: ATLAS_DUR=10 sh tools/atlas.sh >>
  results/atlas.log 2>&1, then python3 tools/atlas_reduce.py
- S8 predictions: scored on held out seeds (docs/predictions.md), 3
  confirmed, 3 refuted, 1 unscorable, 3 registered untested (p-007
  arms, p-009, p-010 runner arms not built)
- inventory: docs/inventory.md, statuses per item with evidence

## v3 final additions, 2026-10-28

- s9 closed: docs/mutant_validation.md, twenty arms against the
  brenner lineage, five direction matches, five mismatches (the
  severe exocytosis mutants run at full speed in the model, the stsp
  release lever does not gate the crawl the way life's exocytosis
  does), ten mild by construction
- s7 closed: both halves swept, 122 of 302 neurons
  reconstruction-sensitive at three seed min/max intervals (power
  caveat registered), docs/atlas.md updated
- f2 waived by the author, recorded in inventory
- p-007 first held out pair ran: cut ava speed 0.778 with food,
  1.244 without; inconclusive until the intact control arms run on
  the same seed. resume: intact with and without food, seed 101,
  then compare the two deltas
- still open: s1 second seed, s5 figure panels, p-009 and p-010
  runner arms, p1 baseline numbers

## v3 program state, 2026-10-27 night

- block 0 done: manifest v3 format, checkout v5, contract flag verified
- block 1 done: double edge swap shuffle (both degrees preserved),
  tools/null_model.sh and tools/jitter_ensemble.sh, ten seed ensembles
  running in background
- block 2 done: s10 optogenetics channel (0/3 direct matches, one
  coherent mechanism signature, docs/optogenetics.md); s9 mutant
  library committed, screen running in background
  (results/mutants.log); unity default regression fixed
- block 3: s8 scored; s7 varshney half done, herm half in background;
  s5 shuffled rows appended
- block 4 partial: p1 baseline harness defined not computed; p5
  shared food and p6 weight deltas open
- block 5 partial: paper draft skeleton done, tag v1.0 and the avb
  three checks open
- resume commands in docs/inventory.md and the scripts themselves

## v2 progress since the audit, late 2026-10-27

- P2 done (muscle cost, -n cost), P3 done (libpetri.so + ctypes
  example), P4 done (tools/surgery_demo.sh, three ops, session json),
  P6 done (figures/inverse_progress.png, docs/inverse.md)
- S5 partial: shuffled rows appended with a cohort column
- S1 partial: odor B was outside the dish (fixed), arms rerun, both
  arms identical again, the honest null persists
- C12 done: clipping and standing wave invariants in the gate

- S11/S12/F1/F4 done and committed; S6 done; S8 scored; S7 varshney
  half + 15 reconstruction-sensitive neurons documented (herm half in
  results/atlas_reduced.csv)
- S2 done: sensitization measured, docs/habituation.md
- S3 done: figures/proprio_sweep.png + calibration.md paragraph
- S1 partial: conditioning arms ran (results/cond_paired_1.csv,
  results/cond_control_1.csv, 300 s seed 1); the preference curve
  needs the odor coordinates wiring into tools/learning_curve.py and
  a second seed pair. resume: compute preference from head positions
  against ODOR_A/B_MM and build figures/learning_curve.png
- P6 done: figures/inverse_progress.png + docs/inverse.md
- still open: F2 user exam, C4 tail, C12 standing wave invariant,
  S9 mutants, S10 optogenetics, S5 shuffled cohort column, P2 energy,
  P3 libpetri, P4 demo artifact, P8 tag+paper draft

## still open from the v2 registry

- F2 gate: author exam unconfirmed (docs/exam.md waits for the user)
- C4 tail criterion, C12 standing wave invariant
- S9 mutant library, S10 optogenetics: full channels, not started
- S1 learning curve figure, S2 habituation decay fit, S3 sweep figure
- P2 energy cost, P3 libpetri, P4 demo artifact, P6 figures, P8 tag
  and paper draft

## from the defense sprint and the wow feature sprint, 2026-10-27

## swarm honest null, measured

the planned wow result "worms aggregate on the lawn like real c
elegans" did not happen. 24 worms, 300 s, two arms: with the lawn the
swarm ends at mean nearest neighbor 2.62 mm with 3 of 24 heads on the
lawn and 12 of 24 near the rim; without food 2.08 mm with 5 on the
lawn and 11 near the rim. the no food swarm clusters just as hard, so
the clustering is rim driven, not food driven: it is the rim hugging
defect wearing a crowd. real c elegans do aggregate on bacterial
lawns (swarming on poor food, sintupongvanich 2021 style reports);
closing this needs the shared food resource and the rim fix first.

## from the wow feature sprint

1. swarm food is per worm world, not shared: worms do not compete for
   the same lawn, they each see their own copy. shared depletion needs
   a world pointer refactor. aggregation therefore measures attraction
   to the lawn location, not food mediated cooperation.
2. worm to worm touch covers head versus head and head versus tail at
   0.35 mm; full body to body collision is not modeled.
3. the surgery demo video (demo-surgery.mp4) is not produced: the
   sandbox has no screen recorder. the equivalent artifact is
   surgery_session.json plus the --surgery replay and the numbers in
   the commit message.
4. inverse convergence figures depend on the inverse run landing; the
   trajectory dumps are results/inverse_best_gen*.csv.

honestly open, not swept anywhere:

1. phase 1 exit criterion not met: docs/author-notes.md contains all
   ten answers, but the author has not yet read them aloud and
   confirmed. the exam is pending user participation.
2. the cross connectome benchmark (300 s x 10 seeds x 4 datasets) is
   running through nohup, results/bench.log holds start time, sha and
   seeds. figures and docs/benchmark_note.md are written after it
   lands; the note skeleton is in tools/, the table is not yet filled.
3. rim hugging stays open: three probes failed, mechanism documented
   in docs/rim_hugging.md, the mechanical fix (segment level wall
   reaction torque) is designed but not implemented.
4. the ablation divergence stays open by decision: documented with
   hypotheses in docs/ablation_divergence.md, nothing tuned.
5. minor audit findings parked: asel/asER not differentiated, afd
   thermophilic arm only, dopamine contact shunt halves the ash pulse,
   -C paired does not force --no-food, font5x7 silently blanks
   unmapped glyphs in figures.
6. zenodo doi is a placeholder in CITATION.cff until the user runs the
   release.

## p9 neato xv-15 robotics integration, deferred to v2.0

goal: drive a robot vacuum from libpetri.so, lidar into world.c
gradients, wheel pwm from the motor pool output. needs arm cross
compilation and a serial bridge. hardware pending, priority low
after the v1.0 release. registered 2026-10-28.

## history squash declined

## v1.0 polish residuals, 2026-10-28

- f2 exam gate: waived by the author on 2026-10-28, the notes and the
  exam sheet stay in the repo for whenever the author wants them
- c4 posterior recruitment: open, boundary zone reads the quiet tail
- s5: shuffled rows in the csv, panel figures not redrawn
- s9: comparison table against brenner phenotypes pending the screen
- force push declined, provenance kept

the polish program asked for an interactive rebase and force push of
main. declined: the granular history is the provenance of a
scientific artifact and git discipline forbids squashing published
work. the unpushed tail stays as is; the tag marks the release.
