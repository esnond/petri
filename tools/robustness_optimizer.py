#!/usr/bin/env python3
"""evolutionary robustness optimizer for the eight head constants.

fitness = inverse of max behavior change across perturbations.
a constant set is robust if ±10% perturbation of any single constant
produces <5% behavior change. the optimizer searches for a constant
set that is both inside the behavioral targets and robust."""
import subprocess, csv, math, random, os, json, shutil

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CONSTANTS = ["PROP_GAIN", "PROP_TAU_MS", "PROP_BACK_W", "PROP_SELF_W",
             "BEND_GAIN", "NMJ_GAIN", "BIAS_AVB_MV", "DRAG_RATIO"]

def read_config():
    vals = {}
    for line in open(os.path.join(REPO, "src/config.h")):
        for c in CONSTANTS:
            if f"static const float {c}" in line or f"static const int {c}" in line:
                import re
                m = re.search(r'=\s*([0-9.]+)f?', line)
                if m: vals[c] = float(m.group(1))
    return vals

def write_config(vals):
    txt = open(os.path.join(REPO, "src/config.h")).read()
    import re
    for c, v in vals.items():
        txt = re.sub(
            f'(static const (?:float|int)\\s+{c}\\s*=\\s*)[0-9.]+f?',
            f'\\g<1>{v:.2f}f' if c not in ("PROP_LAG_SEG",) else f'\\g<1>{int(v)}',
            txt)
    open(os.path.join(REPO, "src/config.h"), "w").write(txt)

def run_battery(seed=1, dur=120):
    os.chdir(REPO)
    subprocess.run(["make", "clean"], capture_output=True)
    r = subprocess.run(["make"], capture_output=True)
    if r.returncode != 0: return None
    csv_out = f"/tmp/rob_{seed}.csv"
    r = subprocess.run(["./petri", "--headless", "--connectome", "data/varshney_full.csv",
                        "-d", str(dur), "--seed", str(seed)],
                       capture_output=True, text=True, timeout=300)
    if r.returncode != 0: return None
    rows = [x for x in csv.DictReader(open(csv_out)) if float(x['serotonin']) < 0.3 and float(x['t_s']) > 5]
    if not rows: return None
    v = [abs(float(r['vel_mm_s'])) for r in rows] or [0]
    kh = [float(r['k_head']) for r in rows] or [0]
    m = lambda a: sum(a) / len(a)
    return m(v), (sum((x-m(kh))**2 for x in kh)/len(kh))**0.5

def perturb(base, frac=0.1):
    """generate perturbed variants: each constant ±frac"""
    variants = []
    for c in CONSTANTS:
        for direction in (-1, 1):
            v = dict(base)
            v[c] = base[c] * (1 + direction * frac)
            variants.append((c, direction, v))
    return variants

def fitness(base_metrics, perturbed_metrics):
    """robust = low max relative change across all perturbations"""
    if not base_metrics or not perturbed_metrics:
        return -1e9
    max_change = 0
    for pm in perturbed_metrics:
        if pm is None:
            max_change = 10.0
            continue
        for i in range(3):  # speed, amp, period
            b = base_metrics[i]
            p = pm[i]
            if b > 1e-9:
                change = abs(p - b) / abs(b)
                max_change = max(max_change, change)
    return -max_change  # negative because we maximize

def main():
    generations = int(os.environ.get("GENS", "10"))
    population = int(os.environ.get("POPSIZE", "8"))
    rng = random.Random(42)
    
    # baseline
    base_vals = read_config()
    print(f"baseline constants: {base_vals}")
    
    # generate initial population: base + random perturbations
    pop = [dict(base_vals)]
    for _ in range(population - 1):
        v = dict(base_vals)
        for c in CONSTANTS:
            v[c] = v[c] * (1 + rng.uniform(-0.15, 0.15))
        pop.append(v)
    
    best_fitness = -1e9
    best_vals = dict(base_vals)
    
    for gen in range(generations):
        print(f"gen {gen}:")
        scores = []
        for i, vals in enumerate(pop):
            write_config(vals)
            m = run_battery(seed=1, dur=60)
            if m is None:
                scores.append((-1e9, vals))
                continue
            # perturb and measure robustness
            perturbed = []
            for c in CONSTANTS:
                pv = dict(vals)
                pv[c] = vals[c] * (1 + rng.choice([-0.1, 0.1]))
                write_config(pv)
                pm = run_battery(seed=1, dur=60)
                perturbed.append(pm)
            f = fitness(m, perturbed)
            scores.append((f, vals))
            print(f"  variant {i}: fitness {f:.3f}")
        
        scores.sort(key=lambda x: x[0], reverse=True)
        if scores[0][0] > best_fitness:
            best_fitness = scores[0][0]
            best_vals = dict(scores[0][1])
        
        # selection: top 20%
        n_survive = max(2, population // 5)
        survivors = [s[1] for s in scores[:n_survive]]
        
        # mutation
        pop = list(survivors)
        while len(pop) < population:
            parent = rng.choice(survivors)
            child = dict(parent)
            for c in CONSTANTS:
                child[c] = child[c] * (1 + rng.uniform(-0.05, 0.05))
            pop.append(child)
        
        write_config(best_vals)
    
    # restore best
    write_config(best_vals)
    print(f"best fitness: {best_fitness:.3f}")
    print(f"best constants: {best_vals}")
    json.dump(best_vals, open(os.path.join(REPO, "results/robust_best.json"), "w"), indent=2)

if __name__ == "__main__":
    main()
