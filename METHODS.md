# methods

## model

### nervous system

each neuron is a graded potential unit, no spikes:

    tau dv/dt = -(v - v_rest) + i_chem + i_gap + i_prop + i_sens + i_bias + noise

integrated with semi implicit euler at dt = 1 ms, membrane potential clamped
to [-90, 0] mV. transmitter release is a sigmoid of v (midpoint -42 mV,
slope 4.5 mV). chemical synapses carry 1 to 3 ms conduction delay and
tsodyks markram short term plasticity: depressing mode on mechanosensory
endings (tap habituation), facilitating mode on interneurons. gap
junctions pass current proportional to the potential difference.
synaptic sign is inferred from the gabaergic presynaptic list, the only
sign information available in electron microscopy data.

connectome sources, all shipped in the repo:

- builtin, 75 neurons / 295 synapses: appetitive chemotaxis, nociception,
  touch and command paths with 58 somatic motor neurons
- varshney_full.csv: wormatlas NeuronConnect sheet, 279 somatic neurons,
  3092 synapses after directional deduplication
- herm_302.csv: openworm herm_full_edgelist, the complete hermaphrodite,
  302 neurons including the pharyngeal nervous system plus 145 muscle
  cells as passive targets, 6036 synapses
- witvliet_adult.csv: witvliet 2021 developmental volume 8, usable for
  load tests, ventral cord motor neurons are not individually identified
  in that export

### body

24 segment hydrostatic skeleton, 1 mm body length. curvature per segment
relaxes toward the dorsal ventral muscle differential (cuticle
viscoelasticity, tau 220 ms, approx). motion comes from a quasi static
3x3 force balance at zero reynolds number with anisotropic friction,
normal to tangential ratio 36 after gray and lissmann 1964. the shape
velocity is measured in the frame carried with the body; the frame
rotation of the previous step is rotated out before differencing.

### proprioceptive pattern generation

no central pattern generator exists in the circuit. the motor neurons
run a stretch reflex: a b class neuron is depolarized by the stretch of
its own side a few segments behind its position, an a class by the
stretch ahead (wen 2012; fieseler, kunert-graf and kutz 2018 model the
same polarity with a multi segment receptive field). the drive is a
signed current, not a rectified one: stretch depolarizes, contraction
hyperpolarizes, so the membrane swings around the sigmoid knee instead
of ratcheting into saturation. dorsal and ventral classes read mirrored
halves of the same curvature field, and the inhibitory d classes get
the inverted sign, which makes the four motor neurons of a segment push
the muscle pair in one direction. the muscle side map follows the
anatomy: dd innervates dorsal muscles, vd ventral, both inhibitory
(white 1986). the loop delay, proprioceptive tau 600 ms over a 4
segment receptive field plus muscle and bend taus, turns the negative
feedback reflex into a traveling wave; no oscillator neuron exists.
the gating of a class proprioception by the ava command is our
assumption, not a literature fact. the drag anisotropy is substrate
dependent, the shipped value 22 sits inside the measured agar band,
gray and lissmann's 36 is the thin film value.

### sensory streams

chemosensation reads the local concentration and its time derivative
against a 3 s adapted baseline, no gradient vector exists in the sensory
code. thermosensation compares to the cultivation temperature. nose
contact feeds ash (nociception), touch feeds alm and plm. mechanoreceptor
currents follow a 800 ms relaxation after a tap.

### neuromodulation and plasticity

serotonin rises on food and slows membrane taus while shrinking
neuromuscular gain; octopamine does the opposite off food; dopamine
shunts mechanosensory synapses on mechanical contact. graded, no mode
switches. tsodyks markram short term depression produces tap
habituation. heterosynaptic aversive learning weakens attractive odor
synapses when ash punishment coincides with odor pathway activity under
octopamine. assumption: this gating rule is circuit design, not a
literature fact. homeostatic scaling of outgoing weights runs on a
600 s timescale.

## protocols

- locomotion: headless run, csv at 10 hz, frozen column contract
- ablation screen: each neuron hyperpolarized to v_rest in turn
- tap train: mechanical tap every 5 s, mechanoreceptor relaxation 800 ms
- cross connectome benchmark: same body, same mechanics, three
  connectomes, summary in results/summary.csv

## deterministic execution

xorshift64* generator seeded from the command line only. same seed, same
flags and same connectome file produce bitwise identical csv output,
verified by tests/run_tests.sh. build is -O2, no fast math, ubsan clean.

## ai assistance statement

code was drafted with large language model assistance. every module was
verified by a human through tests/run_tests.sh, determinism checks and
headless measurement; findings that did not survive measurement were
reverted (see git history: the contrast copy cpg design, the
homeostasis drift hypothesis, the boundary mode hypothesis). citations
were audited; two unverifiable ones were removed.
