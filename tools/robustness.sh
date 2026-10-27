#!/bin/sh
# s12: +-10 percent jitter on the eight head constants, two directions,
# two seeds each, against the untuned baseline. restores config.h from
# the backup at the end. long job, nohup it.
set -e
cd "$(dirname "$0")/.."
OUT=results/robustness.csv
[ -f "$OUT" ] || echo "constant,direction,seed,spd_mm_s,bend_amp,period_s" > "$OUT"
cp src/config.h src/config.h.pristine
trap 'cp src/config.h.pristine src/config.h' EXIT
jitter() { # name factor
    sed -i "s/\(static const float $1[ ]*= \)[0-9.]*f/\1$2f/" src/config.h
}
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
for CONST in PROP_GAIN PROP_TAU_MS PROP_BACK_W PROP_SELF_W BEND_GAIN NMJ_GAIN BIAS_AVB_MV DRAG_RATIO; do
    for DIR in 0.9 1.1; do
        for S in 1 2; do
            jitter $CONST $DIR
            make >/dev/null 2>&1
            ./petri --headless -d 60 --seed $S 2>/dev/null > /tmp/rob.csv
            echo "$CONST,$DIR,$S,$(metric /tmp/rob.csv)" >> "$OUT"
        done
    done
    cp src/config.h.pristine src/config.h
done
make >/dev/null 2>&1
echo "robustness complete"
