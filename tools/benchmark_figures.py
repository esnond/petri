#!/usr/bin/env python3
# three figures from results/cross_connectome.csv, pure python png
import csv, os, sys
import os as _os
_docs_tools = _os.environ.get("PETRI_DOCS_TOOLS",
    _os.path.join(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))),
                  "..", "petri-docs", "tools"))
sys.path.insert(0, _os.path.abspath(_docs_tools))
import pnglib
from font5x7 import draw_text, text_width

RES = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "results")
OUT = os.path.join(RES)
INK, GRAY, AXIS = (40,40,40), (150,150,150), (90,90,90)
COLS = {0: (31,78,121), 1: (60,122,82), 2: (164,60,60), 3: (150,150,150)}

def barfig(rows, key, label, vmax, fname, band=None):
    names = sorted(set(r["connectome"] for r in rows))
    agg = {}
    for r in rows:
        agg.setdefault(r["connectome"], []).append(float(r[key]))
    med = {n: sorted(v)[len(v)//2] for n, v in agg.items()}
    ml, mt, mr, mb = 40, 24, 10, 30
    W, H = 420, 170
    cv = pnglib.Canvas(W, H)
    x0, y0, x1, y1 = ml, mt, W-mr, H-mb
    cv.vline(x0, y0, y1, AXIS); cv.hline(x0, x1, y1, AXIS)
    if band:
        b0 = y1 - (y1-y0)*band[1]/vmax; b1 = y1 - (y1-y0)*band[0]/vmax
        cv.rect(x0+1, b0, (x1-x0)-2, b1-b0, (232,240,232))
    bw = (x1-x0)//len(names)
    for i, n in enumerate(names):
        bh = int((y1-y0)*min(1.0, med[n]/vmax))
        cv.rect(x0 + i*bw + bw//4, y1-bh, bw//2, bh, COLS.get(i, (31,78,121)))
        s = "%.2f" % med[n]
        draw_text(cv, x0 + i*bw + bw//2 - text_width(s)//2, y1-bh-10, s, INK)
        draw_text(cv, x0 + i*bw + bw//2 - text_width(n)//2, y1+7, n, INK)
    title = "%s by connectome, median of 10 seeds" % label
    draw_text(cv, x0, 6, title, INK, 1)
    for i in range(5):
        v = vmax*i/4
        yy = y1 - (y1-y0)*i/4
        cv.hline(x0-3, x0, yy, AXIS)
        s = "%.1f" % v
        draw_text(cv, x0-text_width(s)-6, yy-2, s, INK)
    cv.save(os.path.join(OUT, fname))
    print("wrote", fname)

rows = list(csv.DictReader(open(os.path.join(RES, "cross_connectome.csv"))))
barfig(rows, "speed_mm_s", "crawl speed", 0.6, "cross_connectome_speed.png", band=(0.15, 0.30))
barfig(rows, "wavelength_bl", "wavelength, body lengths", 1.2, "cross_connectome_wavelength.png")
barfig(rows, "perimeter_time_ratio", "time on the rim", 1.0, "cross_connectome_perimeter.png")
