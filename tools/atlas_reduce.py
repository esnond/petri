#!/usr/bin/env python3
# atlas reducer: per neuron per connectome, median metrics over seeds,
# ratio to the intact control, reconstruction divergence flag when the
# two connectomes disagree beyond both of their spreads
import csv, statistics, sys

# the shipped atlas.csv has a header row column drift and some null byte
# rows from a concurrent rebuild crash; parse positionally from the end,
# the last five fields are always spd,amp,period,rev,perim
ctl = {}
recs = []
for line in open("results/atlas.csv", errors="replace"):
    if "\x00" in line or line.startswith("neuron,"):
        continue
    f = line.rstrip("\n").split(",")
    if len(f) < 8:
        continue
    try:
        spd, amp, per, rev, rim = (float(x) for x in f[-5:])
    except ValueError:
        continue
    conn = f[1]
    seed = f[2]
    if f[0] == "intact":
        ctl.setdefault(conn, {})[seed] = spd
    else:
        recs.append((f[0], conn, seed, spd))
out = []
by = {}
for neuron, conn, seed, spd in recs:
    c = ctl.get(conn, {}).get(seed)
    if c:
        by.setdefault((neuron, conn), []).append(spd / c if c else 0.0)
for (neuron, conn), ratios in sorted(by.items()):
    if not ratios:
        continue
    med = statistics.median(ratios)
    out.append((neuron, conn, med, min(ratios), max(ratios)))
# reconstruction divergence: same neuron, non overlapping ratio intervals
seen = {}
for neuron, conn, med, lo, hi in out:
    seen.setdefault(neuron, {})[conn] = (med, lo, hi)
with open("results/atlas_reduced.csv", "w") as f:
    f.write("neuron,connectome,spd_ratio_median,spd_ratio_lo,spd_ratio_hi,divergent\n")
    for neuron, per in sorted(seen.items()):
        vs = list(per.values())
        if len(vs) == 2:
            (_, l1, h1), (_, l2, h2) = vs
            div = "yes" if (h1 < l2 or h2 < l1) else "no"
        else:
            div = "na"
        for conn, (med, lo, hi) in per.items():
            f.write("%s,%s,%.3f,%.3f,%.3f,%s\n" % (neuron, conn, med, lo, hi, div))
top = sorted(out, key=lambda x: x[2])[:20]
print("top 20 slowest-down (neuron, connectome, spd ratio median):")
for t in top:
    print("  %s %s %.3f" % (t[0], t[1], t[2]))
print("divergent pairs:", sum(1 for n, p in seen.items() if len(p) == 2 and
      (p.get('varshney_full') and p.get('herm_302') and
       (p['varshney_full'][2] < p['herm_302'][1] or p['herm_302'][2] < p['varshney_full'][1]))))
