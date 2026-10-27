# interpretability benchmark with known ground truth

petri is a dynamical system whose causal graph is known by
construction: results/ground_truth_adjacency.csv holds every edge with
weight and sign, results/ground_truth_circuits.json annotates four
circuits, results/interp_activity.csv holds 10 ms membrane potentials
of every neuron under seed 1. ml interpretability methods can be
scored against this truth; a shuffled graph with the same degree
sequence (tools/interp_baseline.py) is the noise floor.

## task 1: causal structure recovery

given interp_activity.csv alone, recover directed influences.
metric: precision and recall of recovered edges against the adjacency
within +-2 lags, thresholded at the top 5 percent by influence.
baseline: the shuffled graph gives expected precision at chance, the
degree sequence determines the recall ceiling. report both.

## task 2: circuit scrubbing

ablate one annotated circuit's edges in a modified binary, rerun, and
check that an attribution method assigns the largest effect change to
the scrubbed nodes. metric: rank of the scrubbed nodes' attribution
mass (median over 20 runs). baseline: rank under the shuffled graph.

## task 3: unsupervised factor discovery

run sparse dictionary learning or sae style factorization on the
activity matrix. scoring: do the top factors align with the annotated
circuits (membership overlap, adjusted rand index against the node
grouping)? baseline: same alignment on shuffled labels.

## status

the export path works (interp_activity.csv, adjacency, circuits,
shuffled baseline). the scoring harnesses for tasks 1 to 3 are not
implemented: they are consumer side, meant to run against external
method libraries, and are registered in docs/what_remains.md.
