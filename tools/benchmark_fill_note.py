#!/usr/bin/env python3
# fills the results table of docs/benchmark_note.md from
# results/cross_connectome.csv, run after the sweep lands
import csv, os, statistics

RES = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "results")
NOTE = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "docs/benchmark_note.md")

rows = list(csv.DictReader(open(os.path.join(RES, "cross_connectome.csv"))))
agg = {}
for r in rows:
    agg.setdefault(r["connectome"], []).append(r)

lines = ["", "| connectome | n | speed mm/s | bend amp 1/mm | period s | wavelength bl | reversal events | perimeter ratio |",
         "|---|---|---|---|---|---|---|---|"]
for name, rs in sorted(agg.items()):
    med = lambda k: statistics.median(float(x[k]) for x in rs)
    lines.append("| %s | %d | %.3f | %.2f | %.2f | %.2f | %.1f | %.2f |" %
                 (name, len(rs), med("speed_mm_s"), med("bend_amp"),
                  med("period_s"), med("wavelength_bl"),
                  med("reversal_count"), med("perimeter_time_ratio")))
lines.append("")
table = "\n".join(lines)

txt = open(NOTE).read()
marker = "## results"
i = txt.find(marker)
j = txt.find("## discussion")
txt = txt[:j] + "## results\n" + table + "\n\n" + txt[j:]
open(NOTE, "w").write(txt)
print("note filled with %d connectomes" % len(agg))
