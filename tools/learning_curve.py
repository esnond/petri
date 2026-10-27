#!/usr/bin/env python3
# s1: chemotaxis preference over time, paired vs control arms
import csv, math, sys
import os as _os
_docs_tools = _os.environ.get("PETRI_DOCS_TOOLS",
    _os.path.join(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))),
                  "..", "petri-docs", "tools"))
sys.path.insert(0, _os.path.abspath(_docs_tools))
import pnglib
from font5x7 import draw_text
for arm in ("paired", "control"):
    try:
        rows = [x for x in csv.DictReader(open(f'results/cond_{arm}_1.csv'))]
    except FileNotFoundError:
        print("missing results/cond_%s_1.csv — run the conditioning arms first" % arm)
        sys.exit(1)
