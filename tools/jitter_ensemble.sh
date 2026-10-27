#!/bin/sh
# s12: +-10 percent jitter ensemble on the eight head constants,
# ten seeds per direction, against the untuned control
set -e
cd "$(dirname "$0")/.."
OUT=results/jitter_ensemble.csv
[ -f "$OUT" ] || echo "constant,direction,seed,spd_mm_s,bend_amp,period_s" > "$OUT"
cp src/config.h src/config.h.pristine
trap 'cp src/config.h.pristine src/config.h' EXIT
jitter() { sed -i "s/\(static const float $1[ ]*= \)[0-9.]*f/\1$2f/" src/config.h; }
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
        for S in 1 2 3 4 5 6 7 8 9 10; do
            jitter $CONST $DIR
            make >/dev/null 2>&1
            ./petri --headless -d 60 --seed $S 2>/dev/null > /tmp/je.csv
            echo "$CONST,$DIR,$S,$(metric /tmp/je.csv)" >> "$OUT"
        done
        cp src/config.h.pristine src/config.h
        echo "$CONST $DIR done"
    done
done
make >/dev/null 2>&1
echo "jitter ensemble complete"
