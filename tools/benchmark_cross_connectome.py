#!/usr/bin/env python3
# aggregates results/bench_<name>_s<seed>.csv into cross_connectome.csv
# columns: speed (run windows), bend amplitude, period, wavelength in
# body lengths (probe separation 0.6 mm, head seg 6 vs tail seg 18),
# reversal count, perimeter time ratio
import csv, glob, math, os, re

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RES = os.path.join(REPO, "results")
PROBE_MM = 12 * (1.0 / 24.0)   # seg 6 to seg 18 on a 1.2 mm body

def episode_median(vel_all):
    durs, cur = [], 0
    for v in vel_all:
        if v < -0.02:
            cur += 1
        else:
            if cur:
                durs.append(cur * 0.1)
            cur = 0
    durs = [d for d in durs if d >= 0.5]
    durs.sort()
    if not durs:
        return 0.0, 0
    med = (durs[len(durs)//2] if len(durs) % 2
           else 0.5 * (durs[len(durs)//2 - 1] + durs[len(durs)//2]))
    return med, len(durs)

def measure(path):
    rows = [x for x in csv.DictReader(open(path))]
    on = [x for x in rows if float(x['serotonin']) < 0.3 and float(x['t_s']) > 5]
    if not on:
        return dict(speed=0.0, amp=0.0, period=0.0, wl=0.0, rev=0, perim=1.0)
    v = [abs(float(x['vel_mm_s'])) for x in on]
    kh = [float(x['k_head']) for x in on]
    kt = [float(x['k_tail']) for x in on]
    tt = [float(x['t_s']) for x in on]
    mean = lambda a: sum(a)/len(a)
    sd = lambda a: math.sqrt(sum((x-mean(a))**2 for x in a)/len(a))
    mkh, mkt = mean(kh), mean(kt)
    skh, skt = sd(kh) or 1e-9, sd(kt) or 1e-9
    best, blag = -2, 0
    for lag in range(0, min(120, len(kh)//2)):
        c = sum(kh[i]*kt[(i+lag)%len(kh)] for i in range(len(kh)))/len(kh) - mkh*mkt
        c /= skh*skt
        if c > best: best, blag = c, lag
    dt = tt[1]-tt[0] if len(tt) > 1 else 0.1
    period = 0.0
    c = [x - mean(kh) for x in kh]
    zc = sum(1 for i in range(1, len(c)) if c[i-1] <= 0 < c[i])
    if zc:
        period = (tt[-1]-tt[0])/zc
    # wave speed: probe separation 0.6 mm divided by the measured lag,
    # wavelength: wave speed divided by the crawl period
    wave_v = (PROBE_MM / (blag * dt)) if best > 0.2 and blag > 0 and dt > 0 else 0.0
    wl = wave_v / period if period else 0.0
    _, nrev = episode_median([float(x['vel_mm_s']) for x in rows])
    per = sum(1 for x in rows if not (1.0 <= float(x['head_x_mm']) <= 23.0
                                      and 1.0 <= float(x['head_y_mm']) <= 15.0))
    return dict(speed=mean(v), amp=sd(kh), period=period, wl=wl,
                rev=nrev, perim=per/len(rows))

out = []
for p in sorted(glob.glob(os.path.join(RES, "bench_*_s*.csv"))):
    m = re.match(r"bench_(\w+)_s(\d+)\.csv", os.path.basename(p))
    d = measure(p)
    out.append((m.group(1), int(m.group(2)), d))
with open(os.path.join(RES, "cross_connectome.csv"), "w") as f:
    f.write("connectome,seed,speed_mm_s,bend_amp,period_s,wavelength_bl,reversal_count,perimeter_time_ratio\n")
    for name, seed, d in out:
        f.write("%s,%d,%.3f,%.2f,%.2f,%.2f,%d,%.3f\n" %
                (name, seed, d['speed'], d['amp'], d['period'], d['wl'],
                 d['rev'], d['perim']))
# per connectome summary to stderr for the log
agg = {}
for name, seed, d in out:
    agg.setdefault(name, []).append(d)
for name, ds in sorted(agg.items()):
    mean = lambda k: sum(d[k] for d in ds)/len(ds)
    print("%s: speed %.3f amp %.2f period %.2f wl %.2f rev %.1f perim %.2f (n=%d)"
          % (name, mean('speed'), mean('amp'), mean('period'), mean('wl'),
             mean('rev'), mean('perim'), len(ds)), file=__import__('sys').stderr)
