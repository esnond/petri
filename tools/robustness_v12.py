#!/usr/bin/env python3
# v1.2 robustness scan: +-10 percent on each calibration constant, three
# seeds, 120 s each, against the committed battery baseline. behavior
# change is the largest relative shift among speed, amplitude and period,
# plus the absolute shift in perimeter ratio. target is < 0.05.
import csv, math, os, re, subprocess, sys, statistics as st

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BIN = os.path.join(ROOT, "petri")
CONN = os.path.join(ROOT, "data", "varshney_full.csv")
CFG = os.path.join(ROOT, "src", "config.h")
OUT = os.path.join(ROOT, "results", "robustness_v12.csv")
SEEDS = (1, 4, 7)
DUR = 120
CONSTANTS = ["PROP_GAIN", "PROP_TAU_MS", "PROP_BACK_W", "PROP_SELF_W",
             "BEND_GAIN", "NMJ_GAIN", "BIAS_AVB_MV", "DRAG_RATIO",
             "DRAG_T", "AVB_MOTOR_FLOOR"]


def get(name):
    for line in open(CFG):
        m = re.match(r"static const float " + re.escape(name) + r"\s*=\s*([0-9.]+)f;", line)
        if m:
            return float(m.group(1))
    raise KeyError(name)


def setc(name, val):
    txt = open(CFG).read()
    pat = re.compile(r"(static const float " + re.escape(name) + r"\s*=\s*)[0-9.]+f;")
    txt, n = pat.subn(lambda m: m.group(1) + ("%.5f" % val) + "f;", txt)
    assert n == 1, "%s matched %d" % (name, n)
    open(CFG, "w").write(txt)


def build():
    r = subprocess.run(["make", "-C", ROOT], capture_output=True)
    if r.returncode:
        print(r.stderr.decode()[-500:])
        sys.exit(1)


def measure(seed):
    path = "/tmp/robv12_%d.csv" % seed
    with open(path, "w") as f:
        subprocess.run([BIN, "--headless", "--connectome", CONN, "-d", str(DUR),
                        "--seed", str(seed)], stdout=f, stderr=subprocess.DEVNULL,
                       check=True)
    rows = [x for x in csv.DictReader(open(path))]
    on = [x for x in rows if float(x["serotonin"]) < 0.3 and float(x["t_s"]) > 5]
    kh = [float(x["k_head"]) for x in on]
    v = [abs(float(x["vel_mm_s"])) for x in on]
    mean = sum(kh) / len(kh)
    amp = math.sqrt(sum((x - mean) ** 2 for x in kh) / len(kh))
    cross = 0
    for i in range(1, len(kh)):
        if kh[i - 1] < -0.3 <= kh[i] or kh[i - 1] > 0.3 >= kh[i]:
            cross += 1
    t_on = float(on[-1]["t_s"]) - float(on[0]["t_s"])
    period = 2.0 * t_on / cross if cross else 0.0
    perim = sum(1 for x in on
                if not (1.0 <= float(x["head_x_mm"]) <= 23.0 and
                        1.0 <= float(x["head_y_mm"]) <= 15.0)) / len(on)
    return dict(speed=sum(v) / len(v), amp=amp, period=period, perim=perim)


def mean_metrics():
    ms = [measure(s) for s in SEEDS]
    return {k: st.mean(m[k] for m in ms) for k in ms[0]}


def main():
    base_vals = {c: get(c) for c in CONSTANTS}
    print("base " + " ".join("%s=%.3f" % (c, base_vals[c]) for c in CONSTANTS), flush=True)
    build()
    base = mean_metrics()
    print("baseline speed %.3f amp %.2f period %.2f perim %.2f"
          % (base["speed"], base["amp"], base["period"], base["perim"]),
          flush=True)
    rows = []
    try:
        for c in CONSTANTS:
            for factor in (0.9, 1.1):
                setc(c, base_vals[c] * factor)
                build()
                m = mean_metrics()
                ds = abs(m["speed"] - base["speed"]) / max(base["speed"], 1e-6)
                da = abs(m["amp"] - base["amp"]) / max(base["amp"], 1e-6)
                dp = abs(m["period"] - base["period"]) / max(base["period"], 1e-6)
                dper = abs(m["perim"] - base["perim"])
                change = max(ds, da, dp, dper)
                rows.append((c, factor, m["speed"], m["amp"], m["period"],
                             m["perim"], change))
                print("%-16s %.1f  spd %.3f amp %.2f T %.2f per %.2f  change %.3f"
                      % (c, factor, m["speed"], m["amp"], m["period"],
                         m["perim"], change), flush=True)
                setc(c, base_vals[c])
    finally:
        for c in CONSTANTS:
            setc(c, base_vals[c])
        build()
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["constant", "factor", "speed_mm_s", "bend_amp",
                    "period_s", "perimeter_ratio", "behavior_change"])
        for r in rows:
            w.writerow([r[0], "%.1f" % r[1]] + ["%.4f" % x for x in r[2:]])
    worst = max(rows, key=lambda r: r[6])
    robust = sum(1 for r in rows if r[6] < 0.05)
    print("\nworst: %s at %.1fx, change %.3f" % (worst[0], worst[1], worst[6]))
    print("robust perturbations: %d/%d" % (robust, len(rows)))
    print("wrote %s" % OUT)


if __name__ == "__main__":
    sys.exit(main())
