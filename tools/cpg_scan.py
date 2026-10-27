#!/usr/bin/env python3
# offline scan of the proprioceptive wave generator
# segment loop: dk = ( -k + BEND * mdmv ) / tau_bend
# mdmv follows rectified drive with tau_muscle
# drive reads filtered curvature k_read with lag tau_prop
# success = limit cycle with linear phase gradient along the body
import math
import itertools

N = 24
DT = 0.001
TSIM = 30.0
STEPS = int(TSIM / DT)

def run(sign, w_self, w_back, lag_seg, tau_prop, tau_mus, tau_bend, bend_gain,
        nmj, noise, seed, p=4.0):
    # sign: +1 own-side positive feedback, -1 stretch reflex (contralateral),
    # 0 contrast copy (k[src] - k[own])
    k = [0.25 * math.cos(i * 0.5) for i in range(N)]
    kr = [0.0] * N
    m = [0.0] * (2 * N)
    rnd = seed
    def u01():
        nonlocal rnd
        rnd = (rnd * 6364136223846793005 + 1442695040888963407) & 0xFFFFFFFFFFFFFFFF
        return ((rnd >> 40) & 0xFFFFFF) / 16777216.0
    hist = []
    for it in range(STEPS):
        # filter the proprioceptive copy
        kread = [kr[i] + DT / tau_prop * (k[i] - kr[i]) for i in range(N)]
        for i in range(N):
            src = max(0, min(N - 1, i - lag_seg))
            own = kread[i]
            nb = kread[src]
            if sign == 2:
                # stretch reflex self plus positive delayed copy: the
                # segment oscillates against its own lag and repeats the
                # headward neighbor bend later, making the wave travel
                d = -w_self * own + w_back * nb
            elif sign == 0:
                d = nb - own
            else:
                d = sign * (w_self * own + w_back * nb)
            dpos = d * p if d > 0 else 0.0
            dneg = -d * p if d < 0 else 0.0
            # push pull muscle pair, no co contraction
            mp = m[2 * i] + DT / tau_mus * (nmj * dpos - m[2 * i])
            mn = m[2 * i + 1] + DT / tau_mus * (nmj * dneg - m[2 * i + 1])
            mp = 0.0 if mp < 0.0 else (1.0 if mp > 1.0 else mp)
            mn = 0.0 if mn < 0.0 else (1.0 if mn > 1.0 else mn)
            m[2 * i], m[2 * i + 1] = mp, mn
            mdmv = m[2 * i] - m[2 * i + 1]
            k[i] += DT / tau_bend * (-k[i] + bend_gain * mdmv)
            if k[i] > 9: k[i] = 9
            if k[i] < -9: k[i] = -9
            k[i] += noise * (u01() - 0.5)
        kr = kread[:]
        if it % 10 == 0:
            hist.append(k[:])
    return hist

def score(hist):
    # measure the dominant period from the reference autocorrelation, then
    # extract per segment phase against the reference and its quadrature
    n = len(hist)
    ref = [hist[t][6] for t in range(n)]
    mr = sum(ref) / n
    vr = sum((x - mr) ** 2 for x in ref)
    if vr == 0:
        return None
    ac = []
    for lag in range(10, n // 2):
        num = sum((ref[t] - mr) * (ref[t + lag] - mr) for t in range(n - lag))
        ac.append((lag, num / vr))
    peak = max(ac, key=lambda p: p[1])
    period = peak[0]  # in 10 step units
    quad = max(1, period // 4)
    mr = sum(ref) / n
    phases = []
    amps = []
    for j in range(N):
        sig = [hist[t][j] for t in range(n)]
        ms = sum(sig) / n
        num = sum((ref[t] - mr) * (sig[t] - ms) for t in range(n))
        den = math.sqrt(sum((x - mr) ** 2 for x in ref) * sum((x - ms) ** 2 for x in sig))
        if den == 0:
            return None
        c = num / den
        # phase via hilbert-ish: use quadrature by 90 degree shifted reference
        q = [ref[(t + quad) % n] for t in range(n)]
        mq = sum(q) / n
        numq = sum((q[t] - mq) * (sig[t] - ms) for t in range(n))
        denq = den
        ph = math.atan2(numq / denq if denq else 0, c)
        phases.append(ph)
        amps.append(math.sqrt(sum((x - ms) ** 2 for x in sig) / n))
    # unwrap phases, fit line vs segment index
    unw = [phases[0]]
    for i in range(1, N):
        d = phases[i] - phases[i - 1]
        while d > math.pi: d -= 2 * math.pi
        while d < -math.pi: d += 2 * math.pi
        unw.append(unw[-1] + d)
    sx = sum(range(N)); sy = sum(unw)
    sxx = sum(i * i for i in range(N)); sxy = sum(i * unw[i] for i in range(N))
    slope = (N * sxy - sx * sy) / (N * sxx - sx * sx)
    mean_amp = sum(amps) / N
    tail = [hist[t][j] for t in range(n - 1000, n) for j in range(18, 24)]
    tail_bias = sum(tail) / len(tail)
    return slope, mean_amp, min(amps), max(amps), period * 10 * 0.001, tail_bias

configs = []
for p in [2.5, 5.0, 10.0]:
    for w_back in [1.0, 1.2, 1.4]:
        for tau_prop in [0.4, 0.6, 0.8, 1.0]:
            configs.append((2, "mixed", 1.0, w_back, 3, tau_prop, p))

print("%-9s %5s %5s %4s %5s | %7s %7s %7s" %
      ("design", "wself", "wback", "lag", "tp", "slope", "amp", "minamp"))
for cfg in configs:
    sign, name, ws, wb, lag, tp, p = cfg
    h = run(sign, ws, wb, lag, tp, 0.09, 0.11, 9.0, 0.9, 0.0, 12345, p)
    r = score(h)
    if r is None:
        print("%-9s %5.2f %5.2f %4d %5.2f | dead" % (name, ws, wb, lag, tp))
        continue
    slope, amp, amin, amax, period, tb = r
    thrust = amp * amp / period if period > 0 else 0
    ok = 2.0 <= amp <= 4.0 and 0.7 <= period <= 1.5 and 0.26 <= abs(slope) <= 0.52 and abs(tb) < 0.5
    print("%-9s p%4.1f w%4.2f w%4.2f tp%4.2f | slope %6.3f amp %5.2f T %4.1fs tb %5.2f thrust %6.1f %s" %
          (name, p, ws, wb, tp, slope, amp, period, tb, thrust, "PASS" if ok else ""))