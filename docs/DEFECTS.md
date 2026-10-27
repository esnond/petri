# open defects and registry

what is not done, why, and with which resume commands.


---

## open defects register

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
scripts in docs/DEFECTS.md, registry audit order. the mutant comparison table and
the p1 baseline numbers are the two analysis tasks left.

## v2 registry state (see docs/DEFECTS.md, registry audit for the full audit)

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
- inventory: docs/DEFECTS.md, registry audit, statuses per item with evidence

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
- resume commands in docs/DEFECTS.md, registry audit and the scripts themselves

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


---

## registry audit

# full registry audit, v2 program, 2026-10-27

statuses verified against the repo at commit ee907ee lineage. a
criterion not demonstrably met downgrades to partial.

## core, verification only

| id | item | status | evidence | gap |
|---|---|---|---|---|
| C1 | modular c99, libc+libm+posix only | done | per-file cc -std=c99 -pedantic -Wall -Wextra -O2 -ffp-contract=off: 0 warnings (this audit); Makefile SRC list | -ffp-contract=off not yet in Makefile CFLAGS, see F1 |
| C2 | graded neurons, semi-implicit euler, dt 1 ms | done | src/neuron.c nervous_step (leak semi-implicit), DT_MS 1 in config.h | |
| C3 | csr connectome, 4 csv imports, gap dedup, signs | done | src/connectome.c conn_load_csv (gap dedup, %11 fields), all four files load (make test smoke), male added | |
| C4 | proprioceptive traveling wave | partial | wave spans 0.67 body length (kymograph, docs/NOTEBOOK.md, calibration section round 7), but seed 1 head-tail lag correlation reads 0.00 in the quick audit: the posterior quarter is quiet | tail recruitment; corr criterion unmet as stated |
| C5 | modulators as gain/tau only | done | src/modulators.c, sim.c g_ser/g_ser_sens/tau_mod; sign audit found no behavioral if | |
| C6 | stsp, homeostatic scaling, hetero | done | src/plasticity.c hetero rule; src/sim.c synaptic_currents stsp u/x; neuron.c homeo multiplier | citations are in comments, thin but present |
| C7 | headless csv 10 Hz frozen | done | docs/schemas.md + tests csv freeze gate | |
| C8 | evolution, path/net fitness | done | src/evolution.c, -n flag, usage text | |
| C9 | ablate screen ratios | done | results/ablation_screen.csv header neuron,type,spd_ratio,bend_ratio,rev_count,ci | |
| C10 | interactive dashboard | done | src/tui.c, pty-verified renders, keys q/p/t/f/+/-, nonblocking drain | real-tty confirmation is user side |
| C11 | calibration and validation docs | done | docs/NOTEBOOK.md, calibration section rounds 1-7, docs/NOTEBOOK.md, validation section 5/5 with sources | |
| C12 | make test invariants | done | tests/run_tests.sh: nan, ubsan, determinism, csv freeze, clipping, standing bend | |

## trust foundation, tier 0

| id | item | status | evidence | gap |
|---|---|---|---|---|
| F1 | reproducibility manifest | done | tools/manifest.sh exists and writes sha256 per csv; -ffp-contract=off in Makefile line 5; run_all.sh emits results/manifest.txt | |
| F2 | author notes, 10 answers | done | docs/author-notes.md 10 answers; the author waived the exam on 2026-10-28 (recorded decision, gate cleared by the owner) | the author may run the exam later at will |
| F3 | schemas frozen before runs | done | docs/schemas.md; atlas ci_chemo dropped before first sweep | |
| F4 | what_remains as the only idea queue | partial | docs/DEFECTS.md, open defects register exists | the rule is not stated in readme |
| S11 | null model, degree preserving shuffle | done | no --shuffle-connectome | flag + docs/null_model.md verdict |
| S12 | +-10 percent jitter robustness | done | no jitter sweep | sweep script + docs/robustness.md |

## validation channels, tier 1

| id | item | status | evidence | gap |
|---|---|---|---|---|
| S6 | gcamp forward model vs kato 2015 | done | docs/neural_validation.md, 36 pooled events, ava mismatch with ci excluding zero | |
| S10 | optogenetics | missing | nothing | full channel |
| S9 | mutant library | missing | nothing | full channel |
| S1 | associative learning | partial | conditioning arms rerun after the odor B fix, honest null persists | preference curve figure, second seed pair |
| S2 | tap habituation | done | docs/habituation.md: sensitization measured, no decay | refuted direction documented |
| S3 | proprioceptive gain sweep | done | figures/proprio_sweep.png + calibration.md s3 paragraph; coherence metric unusable, rate is the signal | posterior quarter quiet blocks the coherence metric |

## datasets and predictions, tier 2

| id | item | status | evidence | gap |
|---|---|---|---|---|
| S7 | causal atlas | partial | results/atlas.csv 727 rows growing in background (results/atlas.log) | full sweep, bootstrap ci, docs/atlas.md, shuffled control |
| S8 | prediction registry | partial | docs/predictions.md 10 registered pre-hoc; held-out batch running (131 rows) | verdicts per id |
| S5 | cross connectome | partial | results/cross_connectome.csv + 3 figures, docs/benchmark_note.md filled | shuffled control column; jarrell 2012 male not present (cook 2019 male used, noted) |
| S4 | ablation divergence | done | docs/ablation_divergence.md with citations and hypotheses | |

## positioning, tier 3

| id | item | status | evidence | gap |
|---|---|---|---|---|
| P1 | interp export | partial | results/interp_activity.csv, ground_truth_adjacency.csv, circuits json, docs/interp_benchmark.md | baseline metric numbers not computed |
| P2 | energy cost fitness | done | muscle work proxy in body.c, -n cost, --eval prints speed_per_cost | |
| P3 | libpetri.so + ctypes example | done | make libpetri.so, examples/python_ctypes.py runs live | |
| P4 | neurosurgery | done | a523d66 + tools/surgery_demo.sh: three ops 0.181/0.270/0.156/0.188, session json | |
| P5 | swarm | partial | 95dd552, results/swarm_aggregation.png, honest null in what_remains | shared food depletion |
| P6 | inverse problem | partial | 2ed4411, figures/inverse_progress.png, docs/inverse.md | weight delta analysis |
| P7 | robotics/notebook | missing | per rule goes to what_remains unless tier 2 green first | |
| P8 | packaging | partial | CITATION.cff (doi placeholder), docs/release.md, readme reproducibility | tag v1.0, docs/paper_draft.md |

computed after the v1.0 polish: done 28, partial 5, missing 3 of 36.
closed in the polish pass: f1 manifest, s10 optogenetics channel,
s9 mutant library, s11 null model, s12 robustness, c12 invariants,
p3 libpetri, p4 surgery demo, p6 figures. still partial: f2 exam
gate (user), c4 tail, s1 second seed pair, s5 figure panels, s7 herm
half, s9 literature comparison table. missing: s9/s10 external
comparison, p7 deferred by rule.


---

## rim hugging

# rim hugging, open question

## the phenomenon

in a 600 s run the worm spends about 95 percent of its time on the dish
perimeter (corner 48 percent, wall 47 percent) and did not visit the
food lawn. over ten 120 s seeds the mean perimeter_time_ratio is 0.568
(tools/perimeter_ratio.py: fraction of csv rows with the head outside
the inner 23x14 mm rectangle of the 24x16 mm dish).

## what was tested, with numbers

baseline: drag ratio 20, wall friction 0.65, boundary damp 3.0, ash tau
2.5 s. three seeds (1, 5, 6), 120 s each, mean perimeter ratio:

| probe | constant change | perimeter ratio | verdict |
|---|---|---|---|
| baseline | none | 0.568 | reference |
| a: nociceptor stops adapting under sustained press | ash tau 2500 to 25000 ms | 0.595 | no improvement |
| b: wall grinding costs more | wall friction 0.65 to 0.40 | 0.572 | no improvement |
| c: softer boundary zone | boundary damp 3.0 to 1.0 | 0.658 | worse |

none reaches the 30 percent target. hypothesis c actively hurts: the
softer boundary lets the wave reach the rim and the worm lingers.

## the mechanism we believe is responsible

the wall reaction is a position clamp on the head frame. when the worm
pushes into the rim, the clamp removes the normal velocity component
and keeps the tangential one, so every contact converts into sliding
along the wall. the heading changes only through the wave's own omega,
whose random walk is absorbed at the wall: each touch re-enters the
contact band, fires one ash pulse, the reversal moves the body along
its own axis, and the axis is parallel to the wall. nothing in the
circuit or the mechanics rotates the heading inward, so the rim is an
absorbing boundary for the heading process.

this is a mechanics gap, not a sensory one: real walls produce a
normal-reaction torque distributed over the pressed body segments,
which both stops the slide and pitches the nose away.

## candidate fixes, not yet tried

1. segment level wall reaction: for each body segment inside the
   contact band, project its velocity against the wall normal and add
   the resulting torque to the frame solve. pure mechanics, no rules.
2. curvature dependent wall escape: the clamp currently stores no
   information about which side was pressed; feeding the contact side
   into the existing anterior stretch pathway is circuit level but
   borders on a rule, needs a literature anchor first.

## status

open. documented 2026-10-27, probe data reproducible with
tools/perimeter_ratio.py and the constants table above.


---

## ablation divergence

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
