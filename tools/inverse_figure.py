#!/usr/bin/env python3
# inverse convergence figure: target path plus the best trajectories
# at generations 0, 10, 20 and the final one
import sys, os, glob, math, csv
import os as _os
_docs_tools = _os.environ.get("PETRI_DOCS_TOOLS",
    _os.path.join(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))),
                  "..", "petri-docs", "tools"))
sys.path.insert(0, _os.path.abspath(_docs_tools))
import pnglib
from font5x7 import draw_text, text_width

RES = "results"
files = sorted(glob.glob(os.path.join(RES, "inverse_best_gen*.csv")))
if not files:
    print("no inverse trajectory dumps"); sys.exit(1)
target = [(-3.0, 4.0), (21.0, 4.0), (21.0, 12.0), (3.0, 12.0)]
W, H = 560, 240
SC, OX, OY = 11.0, 30, 30
cv = pnglib.Canvas(W, H)
cv.rect(OX, OY, int(24*SC), int(16*SC), (246,246,242))
# target polyline
for k in range(len(target)):
    a = target[k]; b = target[(k+1) % len(target)]
    steps = 60
    for s in range(steps):
        x = a[0] + (b[0]-a[0])*s/steps
        y = a[1] + (b[1]-a[1])*s/steps
        cv.rect(OX+int(x*SC)-1, OY+int(y*SC)-1, 3, 3, (200,170,90))
draw_text(cv, OX, 10, "inverse problem: best trajectory by generation", (40,40,40), 1)
draw_text(cv, OX, 20, "yellow dots = target path", (150,150,150), 1)
cols = [(31,78,121), (60,122,82), (164,60,60), (40,40,40)]
show = [files[0], files[len(files)//2], files[-2] if len(files) > 1 else files[-1], files[-1]]
shown = []
for k, f in enumerate(dict.fromkeys(show)):
    gen = os.path.basename(f).replace("inverse_best_gen", "").replace(".csv", "")
    pts = [(float(r["x_mm"]), float(r["y_mm"])) for r in csv.DictReader(open(f))]
    for x, y in pts:
        cv.rect(OX+int(x*SC), OY+int(y*SC), 2, 2, cols[k % 4])
    shown.append("gen%s" % gen)
xx = OX
for k, s in enumerate(shown):
    draw_text(cv, xx, OY+int(16*SC)+8, s, cols[k % 4], 1)
    xx += text_width(s) + 10
cv.save("figures/inverse_progress.png")
print("wrote figures/inverse_progress.png")
