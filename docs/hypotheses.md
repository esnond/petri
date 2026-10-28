# registered hypotheses, v1.2 experiment, 2026-10-29

registered BEFORE any code change. each carries success criterion and
falsification. post-hoc explanations are labeled post_hoc.

## phase 1, cheap fixes

h1.1 rim_hugging: wall force perturbation plus stochastic escape drives
perim below 0.30 on all ten dev seeds. falsification: any seed above
0.30 after the sweep.

h1.2 speed: raising DRAG_RATIO from 20 to 28-32 brings run speed to
0.15-0.30 mm/s without breaking curvature or reversal frequency.
falsification: speed drops but curvature or reversals regress.

h1.3 constants: a plateau set exists with behavior_change under 0.05
at plus minus ten percent on all eight constants. falsification: no
plateau in the searched grid.

h1.4 habituation: stsp depression tuning yields declining response to
repeated nose touch. falsification: direction stays rising.

## phase 2, ablation experiment (the architectural bet)

literature: fouad 2018 elife (command neurons pace the rhythm), gray
2005 pnas (ava avb antagonism), korta 2007 (proprioceptive stretch
reflex carries the wave once initiated).

h2.A phasic_initiator: avb emits periodic bursts triggering wave onset
in motor pools. ablate avb, no trigger, gait slows, ratio below 1.0.
falsification: avb ablation still speeds up. status: untested.

h2.B pacemaker_in_loop: avb sets wave frequency via coupling into the
reflex loop. ablate, frequency collapses, slows. falsification:
frequency unchanged after ablation. status: untested.

h2.C gain_modulator_in_loop: avb modulates stretch reflex gain through
recurrent coupling. ablate, gain drops, slows. prior partial result:
perim 0.94 to 0.56, ratio still above 1.0. falsification: ratio stays
at or above 1.0. status: partially implemented, completion pending.

h2.D antagonistic_pushpull: ava avb form a push pull pair inside the
loop. removing one unbalances toward the other, forward slows.
falsification: removal does not bias direction. status: untested.

## phase 3, neural signature

h3.A metric_wrong: alternative metrics (temporal, spatial, functional
connectivity) may reveal a signature the gcamp comparison missed.
falsification: all alternatives null.

h3.B data_short: 600 s x 50 seeds may expose what 300 s x 10 hides.
falsification: longer data still null.

h3.C genuine_absence: the model produces behavior via a different
route than life, no single neuron signature exists by construction.
falsification: a signature appears under h3.A or h3.B.
