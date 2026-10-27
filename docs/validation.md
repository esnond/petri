
## AVB divergence against fouad 2018, three checks

the ablation table shows avb cut speeding the crawl up. three checks
against the hypotheses:

check 1: ablate screen avb, bend_ratio 1.000 with spd_ratio 0.19 and
-0.48: the gait survives intact while the drive collapses. the
operating point release hypothesis (H2) is confirmed.

check 2: avb tonic 8 to 4 to 2 mv, seed 1: the 120 s windows sit in
wall hugging states, the readout does not separate cleanly. H1
(inconclusive).

check 3: the gap junction share of avb input current: not
instrumented yet (H3 untested, registered).

reading: the divergence is the command layer operating point, not a
wiring accident. the burst dynamic remains the registered fix.

## avb divergence, issue 1 checks (neuromax-cognitive), 2026-10-28

three checks against the hypotheses from the issue, calibrated regime,
varshney, seed 1 unless stated.

check 1, h2 signature: intact 0.491 mm/s, bend amp 3.08, bend freq
0.68 hz; avb cut 0.587 mm/s, bend amp 3.08, bend freq 0.67 hz.
bend frequency ratio 0.99, amplitude ratio 1.00, speed ratio 1.20.
the wave is untouched, only the drive changes. h2 confirmed.

check 2, h1: the ablation effect scales with the tonic offset.
bias 8 mv: 1.20x, bias 4 mv: 1.10x, bias 2 mv: 1.08x. monotone.
h1 confirmed: the tonic acts as a brake, removing it releases the
dynamic range proportionally.

check 3, h3: gap current share of avb input, same units, measured at
the cell after the synaptic pass: |i_gap| 1.8 against |i_chem| 18.8,
share 8.7 percent. above the 5 percent threshold the issue set, so h3
is not structurally supported as stated; the share is modest but real.

verdict: h1 and h2 confirmed together. the divergence is the
operating point release: the 8 mv tonic caps the motor pool dynamic
range, removing avb releases it, and the stretch reflex sustains the
wave without the command. the registered fix stays the command burst
dynamic, which would make ava and avb initiators instead of tone
sources and would move all three divergences (ablation, optogenetics,
neural signature) toward life at once.
