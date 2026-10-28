#!/usr/bin/env python3
# aligns petri metrics against any behavioral csv (timestamp, x, y, speed)
# produces: aligned csv, distribution comparison, summary statistics
# usage: python3 tools/align_metrics.py <reference.csv> <petri.csv> [--plot]
import csv, math, sys, os

def load_petri(path):
    rows = list(csv.DictReader(open(path)))
    t = [float(r["t_s"]) for r in rows]
    v = [abs(float(r["vel_mm_s"])) for r in rows]
    kh = [float(r["k_head"]) for r in rows]
    kt = [float(r["k_tail"]) for r in rows]
    x = [float(r["head_x_mm"]) for r in rows]
    y = [float(r["head_y_mm"]) for r in rows]
    ser = [float(r["serotonin"]) for r in rows]
    return t, v, kh, kt, x, y, ser

def load_reference(path):
    """reference csv: timestamp_s,x_mm,y_mm,speed_mm_s[,curvature_1_mm]"""
    rows = list(csv.DictReader(open(path)))
    t = [float(r.get("timestamp_s", r.get("t_s", 0))) for r in rows]
    x = [float(r.get("x_mm", r.get("x", 0))) for r in rows]
    y = [float(r.get("y_mm", r.get("y", 0))) for r in rows]
    v = [abs(float(r.get("speed_mm_s", r.get("speed", 0)))) for r in rows]
    kh = [float(r.get("curvature_1_mm", r.get("curvature", 0))) for r in rows]
    return t, v, kh, x, y

def stats(v):
    m = sum(v) / len(v) if v else 0
    sd = math.sqrt(sum((x - m) ** 2 for x in v) / len(v)) if v else 0
    return m, sd

def ks_statistic(a, b):
    """two sample ks statistic"""
    import bisect
    sa, sb = sorted(a), sorted(b)
    na, nb = len(sa), len(sb)
    i = j = 0
    dmax = 0.0
    cdf_a = cdf_b = 0.0
    while i < na and j < nb:
        if sa[i] <= sb[j]:
            cdf_a = (i + 1) / na; i += 1
        else:
            cdf_b = (j + 1) / nb; j += 1
        dmax = max(dmax, abs(cdf_a - cdf_b))
    while i < na:
        cdf_a = (i + 1) / na; i += 1
        dmax = max(dmax, abs(cdf_a - cdf_b))
    while j < nb:
        cdf_b = (j + 1) / nb; j += 1
        dmax = max(dmax, abs(cdf_a - cdf_b))
    return dmax

def pearson(a, b):
    n = min(len(a), len(b))
    if n < 3: return 0.0
    ma = sum(a[:n]) / n; mb = sum(b[:n]) / n
    num = sum((a[i] - ma) * (b[i] - mb) for i in range(n))
    da = math.sqrt(sum((x - ma) ** 2 for x in a[:n]))
    db = math.sqrt(sum((x - mb) ** 2 for x in b[:n]))
    if da < 1e-9 or db < 1e-9: return 0.0
    return num / (da * db)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("usage: align_metrics.py <reference.csv> <petri.csv>")
        sys.exit(1)
    ref_t, ref_v, ref_k, ref_x, ref_y = load_reference(sys.argv[1])
    pt_t, pt_v, pt_kh, pt_kt, pt_x, pt_y, pt_ser = load_petri(sys.argv[2])
    rm, rsd = stats(ref_v)
    pm, psd = stats(pt_v)
    rk, rkd = stats(ref_k)
    pk, pkd = stats(pt_kh)
    ks_speed = ks_statistic(ref_v, pt_v)
    ks_curv = ks_statistic(ref_k, pt_kh)
    corr = pearson(ref_v, pt_v)
    print("metric | reference | petri | comparison")
    print(f"mean_speed | {rm:.3f} mm/s | {pm:.3f} mm/s | ratio {pm/rm if rm else 0:.2f}x")
    print(f"speed_sd | {rsd:.3f} | {psd:.3f} | ks {ks_speed:.3f}")
    print(f"mean_curvature | {rk:.3f} 1/mm | {pk:.3f} 1/mm | ks {ks_curv:.3f}")
    print(f"speed_correlation | - | r={corr:.3f} | {'>' if abs(corr) > 0.5 else '<'} 0.5")
    print(f"n_samples | {len(ref_v)} | {len(pt_v)} | |")
