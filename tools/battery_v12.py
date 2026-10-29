#!/usr/bin/env python3
# v1.2 gait battery: ten seeds, 300 s, control and avb ablated.
# metrics follow tools/validate.py and tools/perimeter_ratio.py so the
# numbers match the rest of the repo. writes results/v12_battery.csv.
import csv, math, os, subprocess, sys, statistics as st

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BIN = os.path.join(ROOT, "petri")
CONN = os.path.join(ROOT, "data", "varshney_full.csv")
OUT = os.path.join(ROOT, "results", "v12_battery.csv")
SEEDS = range(1, 11)
DUR = 300


def run(seed, extra):
    path = "/tmp/v12_%d_%d.csv" % (seed, abs(hash(extra)) % 99999)
    with open(path, "w") as f:
        subprocess.run([BIN, "--headless", "--connectome", CONN,
                        "-d", str(DUR), "--seed", str(seed)] + extra.split(),
                       stdout=f, stderr=subprocess.DEVNULL, check=True)
    return path


def measure(path):
    rows = [x for x in csv.DictReader(open(path))]
    on = [x for x in rows if float(x["serotonin"]) < 0.3 and float(x["t_s"]) > 5]
    if not on:
        return dict(speed=0.0, amp=0.0, period=0.0, perim=1.0, n=0)
    kh = [float(x["k_head"]) for x in on]
    vel = [abs(float(x["vel_mm_s"])) for x in on]
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
    return dict(speed=sum(vel) / len(vel), amp=amp, period=period,
                perim=perim, n=len(on))


def ci(v):
    m = st.mean(v)
    h = 1.96 * st.stdev(v) / (len(v) ** 0.5) if len(v) > 1 else 0.0
    return m, h


def main():
    if not os.path.exists(os.path.dirname(OUT)):
        os.makedirs(os.path.dirname(OUT))
    rows = []
    for seed in SEEDS:
        c = measure(run(seed, ""))
        a = measure(run(seed, "--ablate AVB"))
        ratio = a["speed"] / c["speed"] if c["speed"] else 0.0
        rows.append((seed, c, a, ratio))
        print("s%-2d per %.2f spd %.3f amp %.2f T %.2f | ab %.2f per %.2f"
              % (seed, c["perim"], c["speed"], c["amp"], c["period"],
                 ratio, a["perim"]), flush=True)
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["seed", "speed_mm_s", "bend_amp", "period_s",
                    "perimeter_ratio", "ablate_speed_mm_s",
                    "ablate_perimeter_ratio", "ablation_speed_ratio"])
        for seed, c, a, ratio in rows:
            w.writerow([seed, "%.4f" % c["speed"], "%.4f" % c["amp"],
                        "%.4f" % c["period"], "%.4f" % c["perim"],
                        "%.4f" % a["speed"], "%.4f" % a["perim"],
                        "%.4f" % ratio])
    print("\nmetric        mean +/- 95% ci")
    for name, key in [("speed", "speed"), ("amp", "amp"),
                      ("period", "period"), ("perim", "perim")]:
        m, h = ci([r[1][key] for r in rows])
        print("%-12s %.3f +/- %.3f" % (name, m, h))
    m, h = ci([r[3] for r in rows])
    print("%-12s %.3f +/- %.3f" % ("ablation", m, h))
    ok = sum(1 for r in rows if r[1]["perim"] < 0.3)
    slow = sum(1 for r in rows if r[3] < 1.0)
    print("\nperim<0.3: %d/10   ablation<1.0: %d/10" % (ok, slow))
    print("wrote %s" % OUT)


if __name__ == "__main__":
    sys.exit(main())
