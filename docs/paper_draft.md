# paper draft, petri v1.0 skeleton

title: petri, a falsifiable test bench for connectome to behavior
hypotheses in caenorhabditis elegans

## methods (drafted, see METHODS.md for the full text)

graded potential neurons, semi implicit euler at 1 ms; connectome
driven chemical and electrical synapses with tsodyks markram short
term plasticity; proprioceptive stretch reflex pattern generation
(wen 2012); 24 segment body under quasi static anisotropic drag;
serotonin, octopamine and dopamine as gain and tau modulators; a
gcamp6f forward model (chen 2013) for neural level comparison. build
is c99, deterministic, gated by tests/run_tests.sh.

## results, by figure

1. calibration battery, 10 seeds on the bounded dish: 8 of 10 inside
   the literature speed band (docs/calibration.md).
2. cross connectome, 300 s x 10 seeds x 4 reconstructions: four
   phenotypes from one body (results/cross_connectome.csv, three
   figures). varshney 0.43 mm/s rim hugging, herm302 0.44 with 5.3 s
   waves, male 0.30, witvliet 0.
3. causal atlas, 302 neurons: avar is the strongest single ablation
   effect (0.18x), 15 reconstruction-sensitive neurons concentrated
   in the command layer (results/atlas_reduced.csv).
4. neural validation: the kato 2015 command sequence is absent at
   spontaneous reversals (docs/neural_validation.md), the sharpest
   falsifiable divergence.
5. prediction registry: three confirmed, three refuted on held out
   seeds (docs/predictions.md).
6. null model: degree preserved rewiring keeps 80 percent of the
   crawl speed (docs/null_model.md).

## limitations

rim hugging; fragile gain constants; neural command sequence absent;
swarm food not shared; witvliet non crawling; two seed caveats in
robustness and atlas intervals. full register docs/what_remains.md.

## status

skeleton for the external reader; every number above resolves to a
csv plus a seed in the repository.
