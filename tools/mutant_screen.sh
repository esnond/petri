#!/bin/sh
# s9: twenty mutants x three seeds on varshney, frozen schema
set -e
cd "$(dirname "$0")/.."
OUT=results/mutants.csv
[ -f "$OUT" ] || echo "mutant,seed,spd_mm_s,bend_amp,rev_count,perim" > "$OUT"
MUT="unc-8 unc-9 unc-54 unc-54-severe unc-22 unc-29 unc-38 unc-63 unc-13 unc-18 unc-31 unc-64 unc-10 unc-17 unc-25 unc-49 unc-77 unc-58 unc-103 wt"
metric() { python3 - "$1" <<'PYEOF'
import csv, math, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
on=[x for x in rows if float(x['serotonin'])<0.3 and float(x['t_s'])>5]
v=[abs(float(x['vel_mm_s'])) for x in on] or [0]
kh=[float(x['k_head']) for x in on] or [0]
per=sum(1 for x in rows if not (1.0<=float(x['head_x_mm'])<=23.0 and 1.0<=float(x['head_y_mm'])<=15.0))/len(rows)
m=lambda a: sum(a)/len(a)
vel=[float(x['vel_mm_s']) for x in rows]
n=cur=0
for q in vel:
    if q<-0.02: cur+=1
    else:
        if cur: n+=1
        cur=0
if cur: n+=1
print("%.4f,%.3f,%d,%.3f"%(m(v),(sum((x-m(kh))**2 for x in kh)/len(kh))**0.5,n,per))
PYEOF
}
for M in $MUT; do
    for S in 1 2 3; do
        ./petri --headless --connectome data/varshney_full.csv -d 120 --seed $S --mutant "$M" 2>/dev/null > /tmp/mut.csv
        echo "$M,$S,$(metric /tmp/mut.csv)" >> "$OUT"
    done
    echo "$M done"
done
echo "mutant screen complete"
