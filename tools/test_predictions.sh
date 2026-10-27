#!/bin/sh
# runs the held-out seeds for every registered prediction and appends
# rows to results/predictions_heldout.csv. one metric extractor per id.
set -e
cd "$(dirname "$0")/.."
make >/dev/null
mkdir -p results
OUT=results/predictions_heldout.csv
[ -f "$OUT" ] || echo "prediction_id,seed,metric,value" > "$OUT"
runspeed() { python3 - "$1" <<'PYEOF'
import csv, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
on=[x for x in rows if float(x['serotonin'])<0.3 and float(x['t_s'])>5]
v=[abs(float(x['vel_mm_s'])) for x in on] or [0]
print("%.4f"%(sum(v)/len(v)))
PYEOF
}
perim() { python3 tools/perimeter_ratio.py "$1" | awk '{print $2}'; }
freq() { python3 - "$1" <<'PYEOF'
import csv, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
on=[x for x in rows if float(x['serotonin'])<0.3 and float(x['t_s'])>5]
kh=[float(x['k_head']) for x in on]; tt=[float(x['t_s']) for x in on]
m=sum(kh)/len(kh); c=[x-m for x in kh]
zc=sum(1 for i in range(1,len(c)) if c[i-1]<=0<c[i])
print("%.4f"%((tt[-1]-tt[0])/zc if zc else 0))
PYEOF
}
revent() { python3 - "$1" <<'PYEOF'
import csv, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
vel=[float(x['vel_mm_s']) for x in rows]
n=cur=0
for q in vel:
    if q<-0.02: cur+=1
    else:
        if cur: n+=1
        cur=0
if cur: n+=1
print("%d"%n)
PYEOF
}
for S in 101 102 103 104 105 106 107 108 109 110; do
    ./petri --headless -d 120 --seed $S 2>/dev/null > /tmp/p_ctl.csv
    echo "petri-p-005,$S,whole_run_abs_speed,$(python3 - /tmp/p_ctl.csv <<'PYEOF'
import csv, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
print("%.4f"%(sum(abs(float(x['vel_mm_s'])) for x in rows)/len(rows)))
PYEOF
)" >> "$OUT"
    echo "petri-p-008-ctl,$S,run_speed,$(runspeed /tmp/p_ctl.csv)" >> "$OUT"
    ./petri --headless -d 120 --seed $S -F 2>/dev/null > /tmp/p_nof.csv
    echo "petri-p-005-nf,$S,whole_run_abs_speed,$(python3 - /tmp/p_nof.csv <<'PYEOF'
import csv, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
print("%.4f"%(sum(abs(float(x['vel_mm_s'])) for x in rows)/len(rows)))
PYEOF
)" >> "$OUT"
    ./petri --headless -d 120 --seed $S --surgery AVA 2>/dev/null > /tmp/p_ava.csv
    echo "petri-p-001,$S,run_speed,$(runspeed /tmp/p_ava.csv)" >> "$OUT"
    echo "petri-p-006,$S,period_s,$(freq /tmp/p_ava.csv)" >> "$OUT"
    echo "petri-p-001-ctl,$S,run_speed,$(runspeed /tmp/p_ctl.csv)" >> "$OUT"
    ./petri --headless -d 120 --seed $S --ablate RIM 2>/dev/null > /tmp/p_rim.csv
    echo "petri-p-002,$S,rev_count,$(revent /tmp/p_rim.csv)" >> "$OUT"
    echo "petri-p-002-ctl,$S,rev_count,$(revent /tmp/p_ctl.csv)" >> "$OUT"
    ./petri --headless -d 120 --seed $S --surgery ASH 2>/dev/null > /tmp/p_ash.csv
    echo "petri-p-004,$S,perimeter_ratio,$(perim /tmp/p_ash.csv)" >> "$OUT"
    echo "petri-p-004-ctl,$S,perimeter_ratio,$(perim /tmp/p_ctl.csv)" >> "$OUT"
    ./petri --headless -d 120 --seed $S --tap 15000 2>/dev/null > /tmp/p_tap.csv
    echo "petri-p-003,$S,tap_reversals_2nd_half,$(python3 - /tmp/p_tap.csv <<'PYEOF'
import csv, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
vel=[float(x['vel_mm_s']) for x in rows if float(x['t_s'])>=60]
n=cur=0
for q in vel:
    if q<-0.02: cur+=1
    else:
        if cur: n+=1
        cur=0
if cur: n+=1
print("%d"%n)
PYEOF
)" >> "$OUT"
    echo "petri-p-003-ctl,$S,tap_reversals_1st_half,$(python3 - /tmp/p_tap.csv <<'PYEOF'
import csv, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
vel=[float(x['vel_mm_s']) for x in rows if float(x['t_s'])<60]
n=cur=0
for q in vel:
    if q<-0.02: cur+=1
    else:
        if cur: n+=1
        cur=0
if cur: n+=1
print("%d"%n)
PYEOF
)" >> "$OUT"
    ./petri --headless --connectome herm_302.csv -d 120 --seed $S 2>/dev/null > /tmp/p_h.csv
    echo "petri-p-008-herm,$S,run_speed,$(runspeed /tmp/p_h.csv)" >> "$OUT"
    echo "prediction batch seed $S done"
done
echo "held out batch complete"
