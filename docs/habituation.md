# s2 tap habituation, measured

protocol: tap every 5 s, 60 s, twelve taps, reversal episodes per tap
window (results/tap_train.csv).

result: reversals per tap window do not decay: first three taps mean
3.3 episodes, last three 5.0. the direction is sensitization, not
habituation, matching the held out refutation of petri-p-003
(docs/predictions.md). rankin and broster 1992 report decaying
responses with recovery; the model does not reproduce the decay at
this protocol.

## hypotheses, none tried

1. the stsp depression sits on the gentle touch afferents (alm, plm),
   but the reversal episodes in a moving animal are dominated by wall
   contacts which refresh their own pulses per edge, so the tap train
   signal drowns.
2. the 5 s interval sits near full recovery of the stsp resource; the
   literature uses 10 s and 60 s trains where depression accumulates.
3. the reversal episode counter counts wall driven episodes as taps.

## fit

no exponential decay fit is reported because the series does not
decay. fitting a rise instead would be overreading three noisy points
per bin.
