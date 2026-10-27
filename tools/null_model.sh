#!/bin/sh
# s11: ten seeds wild type versus ten seeds shuffled, one cohort table
set -e
cd "$(dirname "$0")/.."
OUT=results/null_model.csv
[ -f "$OUT" ] || echo "cohort,seed,spd_mm_s,bend_amp,period_s" > "$OUT"
metric() { python3 - "$1" <<'PYEOF'
import csv, math, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
on=[x for x in rows if float(x['serotonin'])<0.3 and float(x['t_s'])>5]
v=[abs(float(x['vel_mm_s'])) for x in on] or [0]
kh=[float(x['k_head']) for x in on] or [0]
tt=[float(x['t_s']) for x in on] or [0,1]
m=lambda a: sum(a)/len(a)
sd=lambda a: math.sqrt(sum((x-m(a))**2 for x in a)/len(a))
c=[x-m(kh) for x in kh]
zc=sum(1 for i in range(1,len(c)) if c[i-1]<=0<c[i])
print("%.4f,%.3f,%.2f"%(m(v),sd(kh),(tt[-1]-tt[0])/zc if zc else 0))
PYEOF
}
for S in 1 2 3 4 5 6 7 8 9 10; do
    ./petri --headless -d 120 --seed $S 2>/dev/null > /tmp/nm_wt.csv
    echo "wt,$S,$(metric /tmp/nm_wt.csv)" >> "$OUT"
    ./petri --headless -d 120 --seed $S --shuffle-connectome 7 2>/dev/null > /tmp/nm_sh.csv
    echo "shuffled,$S,$(metric /tmp/nm_sh.csv)" >> "$OUT"
    echo "null pair $S done"
done
echo "null model complete"
