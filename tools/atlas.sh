#!/bin/sh
# causal atlas: every neuron ablated on two connectomes x three seeds.
# metrics land in results/atlas.csv with the frozen schema from
# docs/schemas.md. long job, nohup it. tune scale with ATLAS_DUR.
set -e
cd "$(dirname "$0")/.."
make >/dev/null
OUT=results/atlas.csv
[ -f "$OUT" ] || echo "neuron,connectome,seed,spd_mm_s,bend_amp,period_s,rev_count,perimeter_ratio,ci_chemo" > "$OUT"
DUR=${ATLAS_DUR:-10}
row() { # csv_file seed connectome_name
    python3 - "$1" "$2" "$3" <<'PYEOF'
import csv, math, sys
p, seed, cn = sys.argv[1], sys.argv[2], sys.argv[3]
rows = [x for x in csv.DictReader(open(p))]
on = [x for x in rows if float(x["serotonin"]) < 0.3 and float(x["t_s"]) > 1]
v = [abs(float(x["vel_mm_s"])) for x in on] or [0.0]
kh = [float(x["k_head"]) for x in on] or [0.0]
tt = [float(x["t_s"]) for x in on] or [0.0]
m = lambda a: sum(a)/len(a)
sd = lambda a: math.sqrt(sum((x-m(a))**2 for x in a)/len(a))
c = [x-m(kh) for x in kh]
zc = sum(1 for i in range(1, len(c)) if c[i-1] <= 0 < c[i])
per = sum(1 for x in rows if not (1.0 <= float(x["head_x_mm"]) <= 23.0
                                  and 1.0 <= float(x["head_y_mm"]) <= 15.0))/len(rows)
vel = [float(x["vel_mm_s"]) for x in rows]
durs, cur = [], 0
for q in vel:
    if q < -0.02: cur += 1
    else:
        if cur: durs.append(cur)
        cur = 0
print(",,,%.4f,%.3f,%.2f,%d,%.3f" % (m(v), sd(kh),
      (tt[-1]-tt[0])/zc if zc else 0.0, len(durs), per))
PYEOF
}
for conn in varshney_full.csv herm_302.csv; do
    cn=${conn%.csv}
    for s in ${ATLAS_SEEDS:-1 2 3}; do
        ./petri --headless --connectome "$conn" -d "$DUR" --seed "$s" 2>/dev/null > /tmp/atlas_ctrl.csv
        echo "intact,$cn,$s$(row /tmp/atlas_ctrl.csv "$s" "$cn")" >> "$OUT"
    done
    while read -r neuron; do
        [ -n "$neuron" ] || continue
        for s in ${ATLAS_SEEDS:-1 2 3}; do
            ./petri --headless --connectome "$conn" -d "$DUR" --seed "$s" -a "$neuron" 2>/dev/null > /tmp/atlas_run.csv
            echo "$neuron,$cn,$s$(row /tmp/atlas_run.csv "$s" "$cn")" >> "$OUT"
        done
    done < data/neuron_list.txt
    echo "atlas: $cn done"
done
echo "atlas complete"
