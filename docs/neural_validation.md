# neural validation, forward gcamp model vs kato 2015

## forward model

first order indicator: the transmitter release sigmoid of the membrane
drives fluorescence with asymmetric kinetics, rise tau 110 ms, decay
tau 380 ms, gcamp6f values from chen et al 2013 cell (in vivo range
70-150 ms rise, 141-400 ms decay). implementation src/gcamp.c,
parameters src/config.h. trace schema frozen in docs/schemas.md,
dumped by --trace at 10 ms cadence for ava, avb, aib, aiy.

## protocol

three seeds (1, 3, 9), 120 to 180 s, spontaneous reversals pooled:
36 events of at least 0.5 s. event triggered averages of the gcamp
signal in a +-3.0 s window around reversal onset; statistic is the
mean 0 to 1.5 s after onset minus the mean 3.0 to 2.0 s before;
95 percent ci and p from 200 bootstrap resamples of onset times.

## results

| neuron | model dF | 95% ci | kato 2015 signature | verdict |
|---|---|---|---|---|
| AVA | -0.0135 | -0.0128..+0.0182 | rises at reversal onset | MISMATCH |
| AVB | +0.0078 | -0.0158..+0.0237 | falls at reversal onset | MISMATCH |
| AIB | -0.0038 | -0.0099..+0.0101 | transient rise | MISMATCH |
| AIY | +0.0018 | -0.0110..+0.0121 | weak or no change | match |

ava ci excludes zero on the wrong side: in the model ava does not rise
at spontaneous reversal onset, it slightly falls.

## what this says

the behavioral phenotype (reversals happen, right duration and
frequency) is produced without the command neuron calcium signature
that whole brain imaging reports (kato 2015 cell: reversal command
sequence ava up, avb down, aib transient). in the model, spontaneous
reversals are carried by the a class stretch reflex at a near
baseline ava gate (gate squared at resting ava activity is small but
nonzero), not by an ava burst. the architecture generates the motion
without generating the neural correlate of the motion.

## hypotheses, none tried

1. our reversals are triggered through avd and ave connectome paths
   that this four neuron pool does not include; the kato sequence may
   appear in avd/ave.
2. ava in the model acts as a permissive gate, in life as a driver;
   a burst dynamic in the command layer (the same missing piece as the
   ablation divergence, docs/ablation_divergence.md) would likely fix
   both.
3. the gcamp model may be too smooth to resolve the transient; the
   membrane traces in the v_ columns show the same absence, so the
   finding is not an indicator artifact.

## limitations

- comparison is sign and structure level: no published trace data was
  downloaded, the reference is the qualitative whole brain signature
  from kato 2015 figure descriptions.
- the tap protocol produced too few reversals to use; taps rarely
  capture the wave in this configuration, a documented limitation.
- gcamp kinetics are population averaged constants, not cell fitted.
