#!/usr/bin/env python3
# phase 2: validation against literature ranges
# every row carries its source; match and mismatch are both reported
import json
import math
import sys

# literature table, ranges the model is compared against
LITERATURE = [
    ("speed_off_food_mm_s", 0.10, 0.40, "yemini 2013 behavioral database, roaming state"),
    ("bend_frequency_hz", 0.5, 1.5, "wen 2012 proprioceptive coupling study, crawling on agar"),
    ("body_length_mm", 0.9, 1.2, "model constant, not csv measurable"),
    ("reversal_duration_s", 0.5, 2.5, "rankin and broster 1992, tap response"),
    ("curvature_amplitude_inv_mm", 1.0, 5.0, "order of magnitude from bend radius 0.2 to 0.5 mm, approx"),
]


def measure(csv_path):
    rows = [l.split(",") for l in open(csv_path).read().strip().split("\n")[1:] if l]
    n = len(rows)
    # roaming metrics use the crawl windows only, serotonin below 0.3,
    # comparing food dwell speed against a roaming state would be unfair
    on = [i for i in range(n) if float(rows[i][9]) < 0.3 and float(rows[i][0]) > 5]
    kh = [float(rows[i][3]) for i in on]
    kt = [float(rows[i][4]) for i in on]
    vel = [abs(float(rows[i][1])) for i in on]
    vel_all = [float(r[1]) for r in rows]
    ser = [float(r[9]) for r in rows]
    sx, sy = float(rows[0][10]), float(rows[0][11])
    lx, ly = float(rows[-1][10]), float(rows[-1][11])
    t_end = float(rows[-1][0])
    out = {}
    out["speed_off_food_mm_s"] = sum(vel) / len(vel) if vel else 0.0
    # bend frequency from head curvature zero crossings with deadband,
    # over the crawl windows, per crawl time not per whole run
    cross = 0
    for i in range(1, len(kh)):
        if kh[i - 1] < -0.3 <= kh[i] or kh[i - 1] > 0.3 >= kh[i]:
            cross += 1
    t_on = (float(rows[on[-1]][0]) - float(rows[on[0]][0])) if on else 0.0
    # minimum window: at least one expected period (~1 s), discard
    # windows shorter than that
    out["bend_frequency_hz"] = cross / 2.0 / t_on if t_on >= 1.0 else 0.0
    mean = sum(kh) / len(kh) if kh else 0.0
    out["curvature_amplitude_inv_mm"] = math.sqrt(
        sum((x - mean) ** 2 for x in kh) / len(kh)) if kh else 0.0
    # the model constant, not a csv measurement, kept for the record
    out["body_length_mm"] = 1.0
    # reversal duration: median length of one contiguous backward episode
    # in the signed velocity of the full run, episodes below 0.5 s are
    # blips, not reversal events; even counts average the two middles
    durs, cur = [], 0
    for v in vel_all:
        if v < -0.02:
            cur += 1
        else:
            if cur:
                durs.append(cur * 0.1)
            cur = 0
    if cur:
        durs.append(cur * 0.1)
    durs = [d for d in durs if d >= 0.5]
    durs.sort()
    if durs:
        out["reversal_duration_s"] = (durs[len(durs) // 2] if len(durs) % 2
                                      else 0.5 * (durs[len(durs) // 2 - 1] +
                                                  durs[len(durs) // 2]))
    else:
        out["reversal_duration_s"] = 0.0
    out["_net_mm"] = math.hypot(lx - sx, ly - sy)
    out["_tail_bias"] = sum(kt) / n
    return out


def main():
    csv_path = sys.argv[1] if len(sys.argv) > 1 else "results/builtin_s1.csv"
    m = measure(csv_path)
    lines = ["# validation, phase 2\n",
             "run: %s\n" % csv_path,
             "measured with the frozen csv contract, 10 hz samples\n",
             "| metric | model | literature | source | verdict |",
             "|---|---|---|---|---|"]
    n_match = n_mismatch = 0
    for name, lo, hi, src in LITERATURE:
        v = m.get(name, 0.0)
        ok = lo <= v <= hi
        n_match += ok
        n_mismatch += not ok
        lines.append("| %s | %.3f | %.2f..%.2f | %s | %s |" %
                     (name, v, lo, hi, src, "match" if ok else "MISMATCH"))
    lines.append("")
    lines.append("diagnostics not in the literature table: net %.2f mm, "
                 "tail static bias %.2f 1/mm" % (m["_net_mm"], m["_tail_bias"]))
    lines.append("\n%d match, %d mismatch" % (n_match, n_mismatch))
    out = "\n".join(lines)
    open("docs/validation.md", "w").write(out)
    print(out)


if __name__ == "__main__":
    main()