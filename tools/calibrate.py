#!/usr/bin/env python3
# phase 1 calibration: coordinate descent over the cpg and command constants
# objective: net displacement first, wave coherence and static curl penalties
# every eval is a rebuild plus a 30 s headless run, results go to calibration.md
import json
import math
import os
import re
import subprocess

CONFIG = "src/config.h"

# name, default, allowed values during the descent
# grids centered on the current tuned values, running the phase 1 sweep
# grids would silently clobber config.h with stale constants
PARAMS = [
    ("PROP_GAIN", 36.0, [24.0, 36.0, 48.0]),
    ("PROP_BACK_W", 1.2, [0.8, 1.2, 1.8]),
    ("PROP_TAU_MS", 600.0, [450.0, 600.0, 800.0]),
    ("NMJ_GAIN", 0.9, [0.6, 0.9, 1.4]),
    ("BIAS_AVB_MV", 8.0, [6.0, 8.0, 10.0]),
    ("NOISE_SD_MV", 0.4, [0.2, 0.4, 0.8]),
]


def set_param(name, value):
    if not os.path.exists(CONFIG + ".bak"):
        open(CONFIG + ".bak", "w").write(open(CONFIG).read())
    src = open(CONFIG).read()
    pat = r"(static const float %s\s+=\s+)[0-9.]+f" % name
    src = re.sub(pat, r"\g<1>%.2ff" % value, src)
    open(CONFIG, "w").write(src)


def evaluate():
    subprocess.run(["make", "CFLAGS=-std=c99 -Wall -Wextra -pedantic -O2"],
                   capture_output=True)
    subprocess.run(["./petri", "--headless", "-d", "30", "--seed", "1",
                    "--quiet"], capture_output=True)
    r = subprocess.run(["./petri", "--headless", "-d", "30", "--seed", "1"],
                       capture_output=True, text=True)
    rows = [l.split(",") for l in r.stdout.strip().split("\n")[1:] if l]
    n = len(rows)
    if n < 100:
        return -999.0, {}
    kh = [float(x[3]) for x in rows]
    kt = [float(x[4]) for x in rows]
    sx, sy = float(rows[0][10]), float(rows[0][11])
    lx, ly = float(rows[-1][10]), float(rows[-1][11])
    net = math.hypot(lx - sx, ly - sy)
    head_mean = sum(kh) / n
    tail_mean = sum(kt) / n
    head_var = sum((x - head_mean) ** 2 for x in kh) / n
    amp = math.sqrt(head_var)
    # head tail lag correlation, negative and lagging means traveling
    best = 0.0
    for lag in range(-30, 31):
        if lag >= 0:
            a, b = kh[lag:], kt[:n - lag]
        else:
            a, b = kh[:lag], kt[-lag:]
        m = min(len(a), len(b))
        if m < 50:
            continue
        ma, mb = sum(a) / m, sum(b) / m
        num = sum((a[i] - ma) * (b[i] - mb) for i in range(m))
        den = math.sqrt(sum((x - ma) ** 2 for x in a[:m]) *
                        sum((x - mb) ** 2 for x in b[:m]))
        c = num / den if den else 0.0
        if abs(c) > abs(best):
            best = c
    curl = (abs(head_mean) + abs(tail_mean)) / 2.0
    score = net + 40.0 * abs(best) - 8.0 * curl
    return score, {"net": round(net, 2), "corr": round(best, 2),
                   "amp": round(amp, 2), "curl": round(curl, 2)}


log = []
base = {}
for name, dflt, _ in PARAMS:
    set_param(name, dflt)
score, metrics = evaluate()
log.append(("baseline", score, metrics, {}))
print("baseline %.2f %s" % (score, metrics))

for round_no in (1, 2):
    for name, dflt, values in PARAMS:
        best_val, best_score, best_metrics = None, log[-1][1], log[-1][2]
        for v in values:
            set_param(name, v)
            s, m = evaluate()
            print("r%d %s=%.1f -> %.2f %s" % (round_no, name, v, s, m))
            if s > best_score:
                best_val, best_score, best_metrics = v, s, m
        if best_val is None:
            best_val = dflt
        set_param(name, best_val)
        log.append(("%s=%.1f" % (name, best_val)), )
        log[-1] = ("%s=%.1f" % (name, best_val), best_score, best_metrics,
                   {name: best_val})
        print("keep %s=%.1f (%.2f)" % (name, best_val, best_score))

set_param(log[-1][0].split("=")[0], float(log[-1][0].split("=")[1]))
subprocess.run(["make", "CFLAGS=-std=c99 -Wall -Wextra -pedantic -O2"],
               capture_output=True)

with open("docs/calibration.md", "w") as f:
    f.write("# calibration log, phase 1\n\n")
    f.write("objective: net displacement + 40 x wave coherence - 8 x static "
            "curl, 30 s runs, seed 1, builtin connectome\n\n")
    f.write("| step | score | net | corr | amp | curl |\n|---|---|---|---|---|---|\n")
    for step in log:
        f.write("| %s | %.2f | %s |\n" % (
            step[0], step[1],
            "%s | %s | %s | %s" % (step[2].get("net", ""), step[2].get("corr", ""),
                                   step[2].get("amp", ""), step[2].get("curl", ""))
            if len(step) > 2 and isinstance(step[2], dict) else " - "))
print("done, best %.2f" % log[-1][1])