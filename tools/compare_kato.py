#!/usr/bin/env python3
# neural validation: event triggered gcamp averages around reversal
# onsets, compared against the qualitative kato 2015 whole brain
# signature (ava up, avb down, aib transient up at reversal onset).
# significance: bootstrap over onset times, 200 resamples.
import csv, math, random, sys

rows = [x for x in csv.DictReader(open("results/traces_gcamp.csv"))]
vel = [float(x["vel_mm_s"]) for x in rows]
g = {n: [float(x["g_" + n]) for x in rows] for n in ("AVA", "AVB", "AIB", "AIY")}
t = [float(x["t_s"]) for x in rows]

# reversal onsets: signed speed crosses below -0.02 and stays 1 s
onsets = []
for i in range(1, len(vel)):
    if vel[i] < -0.02 and vel[i - 1] >= -0.02:
        j = i
        while j < len(vel) and vel[j] < -0.02:
            j += 1
        if (t[min(j, len(vel) - 1)] - t[i]) >= 0.5:
            onsets.append(i)
if not onsets:
    print("no reversal events in the trace")
    sys.exit(0)

W = 30   # +-3.0 s window at 10 ms
def eta(sig, onsets, shift=0):
    acc = [0.0] * (2 * W)
    cnt = 0
    for o in onsets:
        a = o + shift
        if a - W < 0 or a + W >= len(sig):
            continue
        for k in range(-W, W):
            acc[k + W] += sig[a + k]
        cnt += 1
    return [x / cnt for x in acc], cnt

def delta_pre_post(eta_vals):
    pre = sum(eta_vals[:10]) / 10
    post = sum(eta_vals[W:W + 15]) / 15   # 0 to 1.5 s after onset
    return post - pre

print(f"reversal events: {len(onsets)}")
print("| neuron | model: post-onset minus pre (dF) | kato 2015 signature | verdict |")
EXPECT = {"AVA": ("rises", +1), "AVB": ("falls", -1), "AIB": ("transient rise", +1), "AIY": ("weak/no change", 0)}
sig_ok = 0
for n in ("AVA", "AVB", "AIB", "AIY"):
    e, cnt = eta(g[n], onsets)
    d = delta_pre_post(e)
    exp, sign = EXPECT[n]
    ok = sign == 0 or (d > 0) == (sign > 0)
    sig_ok += ok
    # bootstrap: shuffle onset times
    rnd = random.Random(42)
    deltas = []
    for _ in range(200):
        sh = [rnd.randrange(W, len(vel) - W) for _ in onsets]
        e2, _ = eta(g[n], sh)
        deltas.append(delta_pre_post(e2))
    deltas.sort()
    lo, hi = deltas[5], deltas[194]
    p_one = sum(1 for x in deltas if (sign >= 0 and x >= d) or (sign < 0 and x <= d)) / 200
    print(f"| {n} | {d:+.4f} (95% ci {lo:+.4f}..{hi:+.4f}, p~{p_one:.2f}) | {exp} | {'match' if ok else 'MISMATCH'} |")
print(f"| signature level | {sig_ok}/4 directions match | | |")
