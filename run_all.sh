#!/bin/sh
# one command reproduction of every published number, writes results/
set -e
cd "$(dirname "$0")"
mkdir -p results

echo "== build =="
make clean >/dev/null
make >/dev/null 2>&1

echo "== locomotion runs, 120 s each =="
for c in "builtin:" "varshney:--connectome data/varshney_full.csv" \
         "herm302:--connectome data/herm_302.csv" "witvliet:--connectome data/witvliet_adult.csv" \
         "male:--connectome data/cook2019_male.csv"; do
    name=${c%%:*}
    flag=${c#*:}
    # shellcheck disable=SC2086
    ./petri --headless $flag -d 120 --seed 1 2>/dev/null > "results/${name}_s1.csv"
done

echo "== determinism pair =="
./petri --headless -d 30 --seed 1 2>/dev/null > results/det_a.csv
./petri --headless -d 30 --seed 1 2>/dev/null > results/det_b.csv
cmp results/det_a.csv results/det_b.csv

echo "== ablation screen, 2 s per neuron =="
./petri --ablate-screen -d 2 --seed 1 2>/dev/null > results/ablation_screen.csv

echo "== tap habituation, tap every 5 s =="
./petri --headless -d 60 --seed 1 --tap 5000 2>/dev/null > results/tap_train.csv

echo "== summary =="
{
    echo "run,net_mm,mean_speed_mm_s"
    for f in results/builtin_s1.csv results/varshney_s1.csv results/herm302_s1.csv results/witvliet_s1.csv results/male_s1.csv; do
        name=$(basename "$f" _s1.csv)
        awk -F, -v n="$name" 'NR==2{sx=$11;sy=$12}{lx=$11;ly=$12;v+=($2<0?-$2:$2);nn++} END{printf "%s,%.2f,%.4f\n", n, sqrt((lx-sx)^2+(ly-sy)^2), v/nn}' "$f"
    done
} > results/summary.csv
cat results/summary.csv
echo "done, see results/"
tools/manifest.sh results/manifest.txt