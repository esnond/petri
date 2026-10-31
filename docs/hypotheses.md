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

## results, 2026-10-29, filled after the battery

h1.1 rim_hugging: REFUTED as stated. mean perimeter ratio 0.12, nine
of ten seeds below 0.30, s5 at 0.31. mechanism found (sustained
anterior and posterior touch drive), but the all-ten criterion failed
by one seed. see the notebook entry for the sharp mec_g transition.

h1.2 speed: REFUTED as stated. drag ratio 20 to 28 was not the lever;
raising drag ratio broke the wave. speed landed instead via drag t,
nmj gain and the avb motor gate. the target was met, the stated
mechanism was wrong. post_hoc.

h2.A phasic_initiator: not supported. avb ablation now slows the worm
but without any periodic burst trigger; the effect is a permissive
gate on motor output, not wave onset.

h2.B pacemaker_in_loop: falsified. after ablation the undulation does
not collapse, earlier probes showed it can even speed up. avb does not
set the wave frequency.

h2.C gain_modulator_in_loop: falsified as a robust mechanism. the
reflex gain version was implemented and gave a sign that flipped with
the operating point (ratio 0.46x at one setting, 2.3x at another).
not monotonic, therefore not usable as a phenotype.

h2.E post_hoc gating: CONFIRMED. avb gates the nmj output, smoothed
over 2 s. ablation removes drive and can only slow the worm. ten of
ten seeds, ratio 0.79 +/- 0.03x. registered after the mechanism was
found, so it is post_hoc and does not retroactively support h2.A-D.

h1.3 constants: NOT ACHIEVED as stated. the scan (three seeds, 120 s,
plus minus ten percent on ten constants) gives behavior_change under
0.05 for one of twenty perturbations, prop back weight at 0.9x
(0.042). the worst is avb bias at 0.9x (3.07, period 5.6 s), then prop
self weight (0.85 and 1.63), bend gain at 0.9x (1.26) and the avb gate
floor (1.16). the failure is structural, not a tuning miss: the period
scales with the constants that set the loop time scale, so a ten
percent constant shift moves the period about ten percent by
construction. the speed and amplitude are far more stable (amplitude
within 16 percent across the whole scan). the 0.05 criterion conflates
fragility with a definitional response of a limit cycle's period.
recorded as a result, not worked around.

h1.4 habituation: NOT CONFIRMED. protocol: tap every 30 s for 600 s,
five held-out seeds (101..105), control arms without tap, response is
seconds of backward velocity in the 10 s window after each tap onset.
the tapped arm is flat: late minus early 4.55 minus 4.23 s, delta
+0.32 +/- 1.86 (n 5, two seeds decline, three rise). the no tap
control rises +1.65 +/- 1.47, so the v1.2 worm generally reverses more
late in a run and the tap does not add a declining or rising component
on top. neither habituation nor the v1.0.1 sensitization reproduces at
this protocol. power caveat: n 5 with a 1.86 s ci cannot resolve a
small decline; the falsification bar of the registry is met in the
weak sense only. hypothesis for the null: the stsp depression sits on
sensory synapses, but the behavioral reversal readout is saturated by
the high baseline reversal rate (121 episodes per 300 s, see the
defect register).

h3.A/B/C: see the neural signature note in the notebook.

## neural signature results, 2026-10-29

h3.A metric_wrong: a temporal alternative was run, pearson r between
command neuron activity and head curvature, ten seeds, 300 s. at the
shipped constants avb tracks k_head at r +0.853 +/- 0.019 and ava at
-0.665 +/- 0.019, stable across seeds. that looks like a signature.
control: set cmd prop gain to zero, which removes the curvature input
wired into avb and ava. avb collapses to -0.01, -0.01, +0.00 on seeds
1, 4, 7; ava to +0.11..+0.29; aib and aiy stay near zero.
the control was rerun at the committed constants after a harness
leak, the numbers above are the committed configuration. the
correlation was the imposed input, not an emergent property of the
connectome. h3.A: no emergent temporal signature.

h3.B data_short: not run at 600 s x 50. the null above is already
stable (95 percent ci +/- 0.02 across ten seeds), so more data is
unlikely to overturn a zero that is zero to three decimals. recorded
as a resource bound, not a claim of exhaustiveness.

h3.C genuine_absence: supported. four independent probes now agree:
adaptation current does not change the cruise, command proprioception
does not drive the wave, avb ablation does not kill the rhythm, and
the only apparent temporal signature traces to a model input. the
gait is motor autonomous and the command layer gates it. this is the
standing conclusion of the v1.2 experiment.

## campaign hypotheses, registered 2026-10-30 before any code change

h-m1 extractor: fixing the p-006 extractor (metric is bend frequency
from the deadband crossings of the ava cut arm against its control)
yields a scorable verdict. falsification: the value still has no
usable control arm.

h-m2 reversal metric: counting only episodes with at least 0.1 mm net
backward displacement removes the wall slide artifacts and lowers the
varshney reversal count toward the literature order (units per
minute). falsification: the count stays above 20 per 300 s.

h-m3 wavelength: the head to tail lag metric degenerates at v1.2
because the probe separation assumed the v1.0.1 wave. recomputing the
lag at the v1.2 wavelength (0.6 to 0.9 body length) restores nonzero
wavelengths. falsification: still 0.00 on every connectome.

h-m4 exocytosis: mapping unc-13, unc-18, unc-64, unc-10 to half
effective transmission at the neuromuscular junction (coarse richmond
1999) slows the crawl to at most 0.8 of wild type. falsification:
ratio stays above 0.9.

h-m5 achr: unc-29, unc-38, unc-63 are nicotinic receptor subunits on
the body wall muscle (lewis 1980, levamisole receptor), so the honest
lever is half muscle response (nmj gain scale 0.5), not half motor
output weights. prediction: the arms slow to 0.5 to 0.8, direction
match. falsification: they speed up or sit at 1.0.

h-m6 p-005: the no food speedup dies because the avb motor gate
absorbs the serotonin gain change. test: log serotonin and the gate
along a food and no food pair; if the gate term moves opposite to the
serotonin term by the same fraction, the hypothesis holds and the fix
is to let serotonin act on the gate as well. falsification: serotonin
is flat or the gate does not move.

h-m7 smooth touch: replacing the step touch drive with receptor
kinetics (rise under press, decay after) widens the escape basin:
s5 perimeter drops below 0.30 and the mec_g transition softens.
falsification: s5 stays at or above 0.31 or the battery amplitude
falls under 2.0.

h-m8 habituation protocol: at 10 s inter tap interval (inside the
stsp recovery range) the per tap response declines across the train
on most of ten seeds. falsification: flat or rising again.

h-m9 population signature: the v1.2 neural state space has a low
dimensional rotating trajectory with a participation ratio near the
kato 2015 four dimensions. falsification: the participation ratio
exceeds 10 or the trajectory does not rotate.

h-m10 robustness criterion: scoring each constant against its
predicted first order response (time scales move the period
proportionally, drag moves speed inversely) reclassifies most of the
scan as first order; true fragility remains only where the measured
change exceeds the linear prediction by more than 50 percent.
falsification: more than half the perturbations stay above the bar.
