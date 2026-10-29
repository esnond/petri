# petri

whole animal c. elegans locomotion simulator. behavior emerges exclusively from
connectome dynamics, biomechanics and sensory streams. no behavioral scripts,
no state machines.

c99, libc + libm only, posix termios for the interactive mode. no ncurses.

## build

    make            # -std=c99 -Wall -Wextra -pedantic -O2 -ffp-contract=off
    make debug      # -O0 -fsanitize=address,undefined
    make clean

## modes

interactive (requires a real tty):

    ./petri

keys: q quit, p pause, t tap, +/- simulation speed. the render loop is ansi
only, termios raw mode, non blocking buffered stdin drain.

headless batch, csv (10 Hz) to stdout:

    ./petri --headless --duration 600 --seed 7

genome evaluation (applies mutations, prints fitness):

    ./petri --eval best_genome.json --duration 1000

evolutionary search, population genetics over connectome weight scales:

    ./petri --evolve 50 --population 20 --duration 300 \
        --evolve-output best_genome.json --seed 42

fitness is the head path length (odometer, mm) over the evaluation run, all
individuals of one generation are scored on the same noise realization.
per generation log on stderr shows fitness and net displacement per
individual; the net column separates real crawling from undulation in place.

single neuron ablation screen, csv to stdout:

    ./petri --ablate-screen --duration 60

columns: neuron, type, spd_ratio, bend_ratio, rev_count, ci. ratios are
relative to the intact baseline run.

duration defaults when -d is not given: 180 s plain, 300 s per eval in
--evolve, 60 s per neuron in --ablate-screen, 1000 s semantics via explicit
-d for --eval.

## connectome

three connectomes ship with the repo, switch with one flag:

    ./petri                                   builtin, 75 neurons / 295 synapses
    ./petri --connectome data/varshney_full.csv    wormatlas sheet, 279 somatic neurons
    ./petri --connectome data/herm_302.csv         full hermaphrodite, 302 neurons

data/herm_302.csv comes from the openworm herm_full_edgelist (openworm/c302,
data/herm_full_edgelist.csv) and covers the complete animal: all 302 neurons
including the pharyngeal nervous system, plus 145 muscle and pharyngeal
muscle cells carried as passive chemical targets. 6036 synapses after
deduplication, 58 somatic motor neurons in the motor map. regenerate with
tools/convert_herm_full.py. note the openworm naming uses db01 style motor
names and dbwml01 style muscle names, the loader upper cases everything so
the motor map requires a digit right after the class letters to tell them
apart.

the builtin 75 neuron net covers the appetitive chemotaxis path
(awa, awc -> aia, aiy), nociception (ash -> ava), touch (alm, plm -> avd, pvc),
command pools (ava, avb with endogenous avb bias) and 36 somatic motor neurons
(db, vb, da, va, dd, vd x 6 positions) with electrotonic chains.

external connectomes via csv, one edge per line, optional header:

    Source,Target,Type,Weight[,Sign]
    ADAL,ADAR,S,3
    ADAL,ADAR,EJ,2

type: S / Sp / Rp / R chemical, EJ electrical, Nmj skipped (junctions are
rebuilt from the motor map). synaptic sign comes from the optional 5th column
(-1) or from the gabaergic presynaptic list (dd, vd, ris, rme, avl, dvb, rig).

a ready converted file is included: data/varshney_full.csv, 279 somatic neurons,
3092 synapses, 58 somatic motor neurons, sourced from the canonical wormatlas
NeuronConnect sheet (openworm/c302, data/NeuronConnectFormatted.xlsx). to
regenerate: convert that xlsx to Source,Target,Type,Weight rows, one edge per
line, types kept as is. pharyngeal neurons are not in the wormatlas sheet,
use data/herm_302.csv for the complete animal.

the witvliet 2021 developmental connectomes (8 em volumes, dwitvliet/nature2021,
data/nemanode) convert with tools/convert_witvliet.py:

    python3 tools/convert_witvliet.py witvliet_2020_8.json data/witvliet_adult.csv
    ./petri --headless --connectome data/witvliet_adult.csv

typ 0 is chemical, typ 2 is a gap junction, one json entry aggregates the
synapse sizes of one directed pair. limitation: in that export the ventral
cord motor neurons are not individually identified, so the motor map finds
almost nothing and the animal does not crawl on that file. the dataset is
kept for future work, the wormatlas sheet stays the primary full connectome.

wormatlas lists every chemical synapse twice, once per partner: s/sp rows run
sender to receiver, r/rp rows run receiver to sender. the loader flips r/rp
rows and drops flipped copies of synapses already covered by a send row; gap
junctions are deduplicated to one edge per pair. weights are anatomical
synapse counts from the white 1986 em series, not functional conductances:
no dataset records release probability or receptor density, so the mapping
from anatomy to effective strength is closed by evolution (--evolve) or hand
tuning. synaptic sign is inferred from the gabaergic presynaptic list, the
only sign information available in em data.

## genome json

    {
      "w_chem": 1.0, "w_gap": 1.0, "tau_scale": 1.0,
      "neuron": {
        "N0": {"tau": 1.2, "w_out": 0.8}
      }
    }

flat scalars scale all chemical / electrical conductances and membrane taus.
per neuron entries use builtin connectome indices (N<index>); with a csv
connectome the index mapping follows load order, check the neuron list before
reusing a genome across connectomes.

## architecture

    src/main.c        getopt, termios, ansi render, modes
    src/sim.c         orchestrator: sensory, synaptic currents, stsp, genome
    src/connectome.c  csr graph, builtin wiring, csv import
    src/neuron.c      graded membrane, semi implicit euler, noise
    src/body.c        24 segment hydrostatic skeleton, proprioceptive wave,
                      quasi static force balance with anisotropic drag
    src/world.c       agar plate, odor diffusion, lawn, temperature
    src/modulators.c  5-ht, octopamine, dopamine kinetics
    src/plasticity.c  homeostatic and heterosynaptic plasticity
    src/evolution.c   ga over weight scales, fitness = odometer
    src/metrics.c     speed, reversals, omega turns, chemotaxis index, csv
    src/rng.c         xorshift64*, deterministic per seed

## biophysics

graded potentials, no spikes: tau dv/dt = -(v - v_rest) + i_syn + i_gap +
i_prop + i_sens + i_mod + noise, semi implicit euler, dt 1 ms.

chemical synapses: delayed presynaptic activation ring (1 to 3 ms), sigmoid
transmitter release, tsodyks markram short term plasticity, depressing on
mechanosensory endings (tap habituation), facilitating on interneurons.
gap junctions: current proportional to potential difference.

proprioceptive pattern generation: b class motor neurons sense the bend
contrast (k[src] - k[own]) a few segments toward the head, a class toward the
tail. the spatial mean cancels, the traveling contrast regenerates itself at
the right phase, a wave emerges with no cpg neuron. d class cross inhibition
prevents co contraction.

body: 24 segments, curvature from the dorsal ventral muscle imbalance with
viscoelastic cuticle relaxation, motion from a 3x3 quasi static force balance
at zero reynolds number with anisotropic drag (c_n/c_t = 36, gray and
lissmann). the shape velocity is measured in the frame carried with the body,
the frame rotation of the previous step is rotated out before differencing.

sensory: chemo response is the offset from the adapted baseline plus dc/dt,
no gradient vector anywhere. thermo compares to the cultivation temperature.
nose contact feeds ash (reversal), touch feeds alm/plm -> avd/pvc.

neuromodulation: serotonin rises on food and slows taus plus nmj gain,
octopamine rises off food and does the opposite, dopamine shunts
mechanosensory synapses on contact. graded, no mode switches.

## limitations

- fitness = path length rewards undulation in place; watch the net column or
  switch fitness to net displacement if forward locomotion is the goal
- genome json keys are connectome load order indices, not neuron names
- chemo adaptation is a single shared baseline, no bilateral receptor pair
- interactive mode expects a terminal at least ~90 columns wide
- tap and nose touch circuits respond (measurable shift in command pool
  activity) but the sustained reversal capture does not reliably trigger on
  the builtin parameter set: the running wave self-drives the b pools at
  about the same level the touch path drives the a pools. candidates for
  closing the gap: evolution over the command weights (--evolve), or a
  longer mechanoreceptor relaxation constant
- interactive terminal dashboard, hand rolled ansi, no ncurses: world
  view with the worm colored by local bend, state panel with neural and
  modulator bars, and a live curvature kymograph that shows the
  traveling wave. keys: q quit, p pause, t tap, f food, plus and minus
  simulation speed. compact fallback for small terminals
- the cpg is a stretch reflex, not a copy chain. b class motor neurons
  read the stretch of their own side a few segments behind, a class
  reads ahead (wen 2012, fieseler kunert-graf kutz 2018), the drive is a
  signed push pull current so the membrane swings around the sigmoid
  knee, and the inhibitory d classes get the inverted sign so all four
  motor neurons of a segment push the muscle pair one way (dd innervates
  dorsal, vd ventral, white 1986). the loop lag turns the reflex into a
  traveling wave. the ava gating of a class proprioception is our
  assumption, not literature.
  measured phenotype at 120 s, drag ratio 22, 10 seeds (calibration.md):
  run speed 0.14 to 0.24 mm/s (9 of 10 seeds inside the 0.15 to 0.30
  literature band), bend amplitude 2.1 to 2.2 1/mm, per burst period
  1.2 to 1.4 s, tail curl 0.00 1/mm in every seed. locomotion comes in
  foraging runs separated by dwells: nose on food raises serotonin,
  serotonin cuts nmj gain, the worm stops on the patch and leaves when
  the trace decays. the static curl mode of the previous design is
  gone: three sign errors were found and fixed on the way (dd/vd muscle
  sides inverted, rectified proprioceptive drive ratcheting the
  operating point into saturation, same side class rectification hiding
  the anti reflex). remaining gaps: wavelength near 0.4 body length
  against the 0.5 to 1.0 target, one seed under the speed floor

## quick start

    git clone https://github.com/esnond/petri
    cd petri
    make
    ./petri --help
    ./petri --headless -d 60 --seed 1 > run.csv

## project status

**v1.2 experiment** (2026-10-29, branch v1.2-fix-branch). the default
connectome is now the full varshney wiring shipped in data/. the
locomotion battery sits inside the literature targets on ten seeds,
300 s: speed 0.246 +/- 0.005 mm/s, bend amplitude 2.71 +/- 0.02 1/mm,
period 1.31 +/- 0.20 s, perimeter ratio 0.12 +/- 0.05, avb ablation
0.79 +/- 0.03x. nine of ten seeds clear the perimeter target (s5 at
0.31), eight of ten the period box (s6 and s9 about 1.9 s). reproduce
with tools/battery_v12.py. robustness and the neural signature control
are in docs/NOTEBOOK.md, defects and resolutions in docs/DEFECTS.md.
v1.0.1 remains the last tagged release. 30 of 36 registry items done,
the rest documented as partial with reasons in docs/DEFECTS.md.

## reproducibility

one command regenerates every number in this readme:

    sh run_all.sh

expect roughly fifteen minutes on a typical laptop: four 120 s
connectome runs, a determinism pair, the ablation screen and the tap
train, all into results/. the six gate suite is `make test` (about two
minutes, includes bitwise determinism and ubsan). the cross connectome
benchmark (300 s x 10 seeds x 4 datasets, about an hour) is
`tools/benchmark_cross_connectome.sh`, results land in
results/cross_connectome.csv with three figures. every published
number is a csv plus a seed; ci runs the gates on every push.

## idea policy

new ideas and feature proposals go to docs/DEFECTS.md with a
date, nowhere else. the registry is audited in docs/DEFECTS.md.

## positioning

not "another worm simulator". petri is a falsifiable test bench for
connectome to behavior hypotheses: one binary, c99, zero dependencies,
deterministic, every claim is a script plus csv plus seed. the flagship
experiment nobody else ships: the same body and mechanics run against
four published connectome reconstructions, hermaphrodite adult and l4
plus the full male (cook 2019, 588 nodes), and the behavioral
divergence of the reconstructions is measured. current table, 120 s
seed 1, net mm: builtin 8.4, varshney 8.7, herm302 7.7, male 6.9,
witvliet 0.0 (motor neurons unnamed in that reconstruction). a
difference between datasets is a result, so is their agreement.

the sharpest current divergence against reality sits in the ablation
table: the model agrees with fouad 2018 that the gait survives command
interneuron ablation, but predicts the crawl speeds up 1.3 to 2.3
times while life slows down. details in validation.md.

development is ai assisted and declared: code drafted with llm
assistance, every change gated by tests/run_tests.sh and human review,
citations audited against the literature, limitations kept public.

## roadmap

| phase | content | exit criterion | status |
|---|---|---|---|
| 0 | ci, run_all.sh, frozen csv contract, results/ | make test green, one command reproduction | done |
| 1 | calibration sweep to the target phenotype vector | t1 gates pass on 8 of 10 seeds, calibration.md | done: 10 of 10 seeds on the bounded dish, see calibration.md |
| 2 | literature validation table (yemini 2013, wen 2012, kunert 2017) | validate.py emits match and mismatch rows | done: 5 match 0 mismatch, plus the ablation table with a live divergence |
| 3 | flagship: ablation matrix, proprioceptive gain sweep, tap habituation fit, cross connectome a/b/c | figures in results/, experiments.md | done: cross connectome table live, ablation divergence documented |
| 4 | associative conditioning protocol | paired vs control preference curve | protocol done, honest null: ci 0.000 all arms |
| 5 | methods draft, benchmark note, zenodo doi, CITATION.cff | citable v1.0 | drafts done, tagged v0.1.0, zenodo upload is manual |

non goals, deliberately: 3d mechanics, spikes, python bindings, gui,
openworm integration, new neuron classes, wasm. they would kill the
lightness and determinism that make the bench worth having.

## csv schema

frozen header, see tests/run_tests.sh:
t_s,vel_mm_s,curv_inv_mm,k_head,k_tail,ava,avb,aib,aiy,serotonin,head_x_mm,head_y_mm
units: t s, vel mm/s, curv 1/mm, k 1/mm, activations 0..1, serotonin 0..1,
position mm.
