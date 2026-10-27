#!/usr/bin/env python3
# aggregation figure: final head positions of the swarm on the lawn vs
# without food, plus the mean nearest neighbor distance per arm
import csv, math, sys
import os as _os
_docs_tools = _os.environ.get("PETRI_DOCS_TOOLS",
    _os.path.join(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))),
                  "..", "petri-docs", "tools"))
sys.path.insert(0, _os.path.abspath(_docs_tools))
import pnglib
from font5x7 import draw_text, text_width

RES = "results"
def last_positions(path):
    last_t, pos = None, {}
    for r in csv.DictReader(open(path)):
        last_t = float(r["t_s"])
        pos[int(r["worm"])] = (float(r["x_mm"]), float(r["y_mm"]))
    return last_t, pos

def nn_mean(pos):
    out = []
    keys = sorted(pos)
    for i in keys:
        best = 1e9
        for j in keys:
            if i == j: continue
            dx = pos[i][0]-pos[j][0]; dy = pos[i][1]-pos[j][1]
            d = dx*dx+dy*dy
            if d < best: best = d
        out.append(math.sqrt(best))
    return sum(out)/len(out)

arms = []
for name, path in [("food", "results/swarm_pos_food.csv"),
                   ("no food", "results/swarm_pos_nofood.csv")]:
    try:
        t, pos = last_positions(path)
        arms.append((name, pos, nn_mean(pos), t))
    except FileNotFoundError:
        print("missing", path)

W, H = 560, 260
cv = pnglib.Canvas(W, H)
SC = 10.0   # px per mm, dish 24x16 -> 240x160
OX, OY = 30, 40
# lawn circle at dish center
draw_text(cv, 30, 6, "final swarm positions, 24 worms, 300 s", (40,40,40), 1)
for k, (name, pos, nn, t) in enumerate(arms):
    ox = OX + k*270
    cv.rect(ox, OY, int(24*SC), int(16*SC), (248,248,244))
    # lawn
    for a in range(64):
        import math as m
        ang = a*6.2832/64
        cv.set(ox+int((12+3.5*m.cos(ang))*SC), OY+int((8+3.5*m.sin(ang))*SC), (200,170,90))
    col = (31,78,121) if k == 0 else (150,150,150)
    for w, (x, y) in pos.items():
        cv.rect(ox+int(x*SC)-2, OY+int(y*SC)-2, 4, 4, col)
    draw_text(cv, ox, OY+int(16*SC)+6, "%s: mean nn %.1f mm" % (name, nn), (40,40,40), 1)
cv.save("results/swarm_aggregation.png")
print("wrote results/swarm_aggregation.png")
