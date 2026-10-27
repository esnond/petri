# s12 robustness, +-10 percent jitter on the head constants

protocol: eight constants, both directions, seeds 1 and 2, 60 s runs,
against an untuned 60 s control (0.201 mm/s, amp 3.72, period 1.62).
sweep data results/robustness.csv, generator tools/robustness.sh.

| constant | dir | speed rel | verdict |
|---|---|---|---|
| BEND_GAIN | 0.9 / 1.1 | 0.00x / 0.00x | FRAGILE, both directions kill the crawl |
| PROP_GAIN | 0.9 / 1.1 | 0.01x / 0.00x | FRAGILE, razor band |
| PROP_TAU_MS | 0.9 / 1.1 | 0.32x / 0.30x | FRAGILE |
| DRAG_RATIO | 0.9 / 1.1 | 0.34x / 0.39x | FRAGILE |
| BIAS_AVB_MV | 0.9 / 1.1 | 0.77x / 0.84x | stable, degrades gracefully |
| NMJ_GAIN | 0.9 / 1.1 | 1.00x / 1.14x | stable |
| PROP_BACK_W | 0.9 / 1.1 | 1.11x / 1.04x | stable |
| PROP_SELF_W | 0.9 / 1.1 | 0.93x / 1.06x | stable |

## the honest headline

four of the eight head constants are fragile at +-10 percent with two
seeds: the calibrated point sits on a ridge, not on a plateau. bend
amplitude stays near 3.4 to 4.0 in every surviving configuration, the
wave shape is robust, the rate is not. the sharpest sensitivity is
prop gain: the operating point argument (docs/calibration.md round 2)
predicts exactly this, the loop needs the motor pool inside the
sigmoid knee and ten percent moves it out.

## caveats

- two seeds, 60 s windows: the fragility classes are solid, the exact
  ratios are not. a ten seed 120 s pass is registered in
  what_remains.
- the null model result (docs/null_model.md) reads differently: a
  degree preserved random wiring crawls at 0.8x. a rewired connectome
  survives, a ten percent gain shift does not. the calibrated point is
  more fragile than the wiring, which means the constants, not the
  connectome, carry the fine tuning.


## ten seed update

the ten seed ensemble confirms the four fragile constants (prop gain,
prop tau, bend gain, drag ratio, all 0.00x to 0.39x of control) and
the four stable ones (0.83x to 1.25x). the fragility classes from the
two seed pass hold.
