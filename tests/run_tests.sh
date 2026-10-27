#!/bin/sh
# ci gate, phase 0 exit criterion: everything green or the commit does not go
set -e
cd "$(dirname "$0")/.."

echo "== build strict =="
make clean >/dev/null
make > /tmp/build.log 2>&1
make_status=$?
if [ "$make_status" != 0 ]; then
    tail -30 /tmp/build.log
    echo "FAIL: make exited $make_status" ; exit 1
fi
if grep -qE "warning|error" /tmp/build.log; then
    echo "FAIL: build has warnings" ; grep -E "warning|error" /tmp/build.log | head -5 ; exit 1
fi
echo "PASS: clean build"

echo "== csv contract freeze =="
CONTRACT="t_s,vel_mm_s,curv_inv_mm,k_head,k_tail,ava,avb,aib,aiy,serotonin,head_x_mm,head_y_mm"
./petri --headless -d 1 --seed 1 2>/dev/null | head -1 > /tmp/hdr.csv
if [ "$(cat /tmp/hdr.csv)" != "$CONTRACT" ]; then
    echo "FAIL: csv header drifted from the frozen contract" ; exit 1
fi
echo "PASS: csv header frozen"

echo "== determinism =="
# no --quiet here: the gate must compare real csv rows, two empty files
# would pass regardless of determinism
./petri --headless -d 10 --seed 7 2>/dev/null >/tmp/det_a.csv
./petri --headless -d 10 --seed 7 2>/dev/null >/tmp/det_b.csv
[ -s /tmp/det_a.csv ] || { echo "FAIL: determinism run produced no rows"; exit 1; }
cmp /tmp/det_a.csv /tmp/det_b.csv
echo "PASS: bitwise identical runs"

echo "== connectome smoke =="
for c in "" "--connectome data/varshney_full.csv" "--connectome data/herm_302.csv"; do
    # shellcheck disable=SC2086
    ./petri --headless $c -d 5 --seed 1 --quiet >/dev/null 2>&1 || { echo "FAIL: $c"; exit 1; }
done
echo "PASS: all three connectomes run"

echo "== nan guard =="
./petri --headless -d 20 --seed 3 2>/dev/null > /tmp/nan.csv
if grep -qi "nan\|inf" /tmp/nan.csv; then
    echo "FAIL: non finite values in csv" ; exit 1
fi
echo "PASS: finite values"

echo "== wave invariants =="
# clipping: curvature must never exceed the bend ceiling the config
# declares (muscle saturation at BEND_GAIN means |k| stays under it)
if grep -qE ",[+-]9\.[0-9]{2}," /tmp/nan.csv; then
    echo "FAIL: curvature clipped at the ceiling, the loop is saturated" ; exit 1
fi
# standing wave: head and tail must not freeze at nonzero means while
# the animal is nominally awake; a frozen bent body is the old defect
python3 - /tmp/nan.csv <<'PYEOF' || exit 1
import csv, sys
rows = [x for x in csv.DictReader(open(sys.argv[1]))]
late = [x for x in rows if float(x['t_s']) > 15]
kh = [float(x['k_head']) for x in late]
kt = [float(x['k_tail']) for x in late]
m = lambda a: sum(a)/len(a)
sd = lambda a: (sum((x-m(a))**2 for x in a)/len(a))**0.5
if abs(m(kh)) > 1.5 and sd(kh) < 0.3:
    print("FAIL: standing wave, head frozen at a bent mean")
    sys.exit(1)
if abs(m(kt)) > 1.5 and sd(kt) < 0.3:
    print("FAIL: standing wave, tail frozen at a bent mean")
    sys.exit(1)
print("no clipping, no frozen bend")
PYEOF
echo "PASS: wave invariants"

echo "== ubsan =="
cc -std=c99 -Wall -Wextra -pedantic -O1 -g -fsanitize=undefined -D_DEFAULT_SOURCE \
   -o /tmp/petri_ubsan src/*.c -lm
/tmp/petri_ubsan --headless -d 5 --seed 1 --quiet 2>/tmp/ub.log >/dev/null
if grep -qE "runtime error|warning" /tmp/ub.log; then
    echo "FAIL: ubsan flagged undefined behavior" ; cat /tmp/ub.log ; exit 1
fi
echo "PASS: ubsan clean"

echo ""
echo "ALL TESTS GREEN"