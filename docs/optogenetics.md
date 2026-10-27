# s10 optogenetics channel

infrastructure: --opsin Neuron:ChETA|NpHR with exponential kinetics
(cheta on 1.5 ms off 10 ms after chen 2011; nphr chloride shunt),
--light-protocol json events, --light-zone closed loop circle.

## three published protocols, replicated on seed 1

| protocol | expectation | measured | verdict |
|---|---|---|---|
| AVA:ChETA 5-7 s (fouad 2018 style) | reversal during light | 0 reversals in window | MISMATCH |
| AIB:ChETA 5-7 s (hendricks 2012 style) | reversal during light | 0 reversals in window | MISMATCH |
| AVB:NpHR 5-8 s (kocabas 2012 style) | pause, speed drop | speed 1.34 mm/s in light against 0.35 baseline | CONTRADICTS: the worm speeds up four fold |

## the coherent reading

all three divergences follow one mechanism, the same one as the
ablation divergence (docs/ablation_divergence.md): in the model the
command layer sets the motor pool operating point. depolarizing ava
does not produce the reversal because the model's reversals are
carried by the a class reflex at baseline gate; silencing avb removes
the tonic forward drive, the pool leaves sigmoid saturation, the
differential reflex gain rises and the crawl accelerates. life
reports the opposite sign because in the real circuit the command
neurons initiate the wave.

the optogenetics channel therefore measures exactly the gap between a
tone-only command layer and a command layer that initiates: three
independent perturbations, one coherent signature. closing the gap
needs the burst dynamic registered in what_remains.

## limitations

single seed, single protocol per opsin; the cheta drive amplitude
(12 mv at unit light) is an assumption, not a fitted value; no
continuous light habituation modeled.
