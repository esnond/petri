#!/bin/sh
# flagship benchmark: three to four connectome reconstructions on the
# same body and physics, 300 s per seed, ten seeds each. results land
# in results/cross_connectome.csv, figures via the python script.
# long job: run through nohup, see docs/release.md
set -e
cd "$(dirname "$0")/.."
make >/dev/null
OUT=results/cross_connectome.csv
echo "connectome,seed,speed_mm_s,bend_amp,period_s,wavelength_bl,reversal_count,perimeter_time_ratio" > "$OUT"
for c in "varshney:varshney_full.csv" "herm302:herm_302.csv" "male:cook2019_male.csv" "witvliet:witvliet_adult.csv"; do
    name=${c%%:*}; file=${c#*:}
    for s in 1 2 3 4 5 6 7 8 9 10; do
        ./petri --headless --connectome "$file" -d 300 --seed "$s" 2>/dev/null > "results/bench_${name}_s${s}.csv"
    done
    echo "$name done"
done
python3 tools/benchmark_cross_connectome.py
echo "benchmark complete, see $OUT"
