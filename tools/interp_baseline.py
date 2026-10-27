#!/usr/bin/env python3
# baseline for the interpretability benchmark: a shuffled graph with the
# same degree sequence. any method that scores as well on this as on the
# real connectome is measuring noise.
import csv, random
rows = list(csv.DictReader(open("results/ground_truth_adjacency.csv")))
edges = [(r["src"], r["dst"], r["kind"]) for r in rows]
names = sorted(set(e[0] for e in edges) | set(e[1] for e in edges))
rnd = random.Random(7)
targets = names[:]
rnd.shuffle(targets)
with open("results/ground_truth_shuffled.csv", "w") as f:
    f.write("src,dst,w,kind\n")
    for (s, d, k), t in zip(edges, targets):
        f.write("%s,%s,%s,%s\n" % (s, t, rows[edges.index((s, d, k))]["w"], k))
print("shuffled baseline: %d edges over %d nodes" % (len(edges), len(names)))
