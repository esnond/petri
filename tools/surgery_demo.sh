#!/bin/sh
# p4 demo: three scripted operations with the behavioral effect of each,
# plus the session json the interactive dashboard would write
set -e
cd "$(dirname "$0")/.."
make >/dev/null
run() { ./petri --headless -d 120 --seed 5 "$@" 2>/dev/null > /tmp/demo_run.csv
        python3 - /tmp/demo_run.csv <<'PYEOF'
import csv, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
on=[x for x in rows if float(x['serotonin'])<0.3 and float(x['t_s'])>5]
v=[abs(float(x['vel_mm_s'])) for x in on] or [0]
print("%.3f"%(sum(v)/len(v)))
PYEOF
}
echo "op 0: intact"
b=$(run)
echo "op 1: cut AVA"
./petri --headless -d 120 --seed 5 --surgery AVA 2>/dev/null > /tmp/demo_run.csv
python3 - /tmp/demo_run.csv <<'PYEOF'
import csv, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
on=[x for x in rows if float(x['serotonin'])<0.3 and float(x['t_s'])>5]
v=[abs(float(x['vel_mm_s'])) for x in on] or [0]
open('/tmp/demo1.txt','w').write("%.3f"%(sum(v)/len(v)))
PYEOF
a1=$(cat /tmp/demo1.txt)
echo "op 2: also cut AVD"
./petri --headless -d 120 --seed 5 --surgery AVA,AVD 2>/dev/null > /tmp/demo_run.csv
python3 - /tmp/demo_run.csv <<'PYEOF'
import csv, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
on=[x for x in rows if float(x['serotonin'])<0.3 and float(x['t_s'])>5]
v=[abs(float(x['vel_mm_s'])) for x in on] or [0]
open('/tmp/demo2.txt','w').write("%.3f"%(sum(v)/len(v)))
PYEOF
a2=$(cat /tmp/demo2.txt)
echo "op 3: restore AVA, keep AVD cut"
./petri --headless -d 120 --seed 5 --surgery AVD 2>/dev/null > /tmp/demo_run.csv
python3 - /tmp/demo_run.csv <<'PYEOF'
import csv, sys
rows=[x for x in csv.DictReader(open(sys.argv[1]))]
on=[x for x in rows if float(x['serotonin'])<0.3 and float(x['t_s'])>5]
v=[abs(float(x['vel_mm_s'])) for x in on] or [0]
open('/tmp/demo3.txt','w').write("%.3f"%(sum(v)/len(v)))
PYEOF
a3=$(cat /tmp/demo3.txt)
cat > results/surgery_demo_session.json <<JSON
{
 "surgery": [
  {"op": "cut", "neuron": "AVA", "run_speed_mm_s": $a1},
  {"op": "cut", "neuron": "AVD", "run_speed_mm_s": $a2},
  {"op": "restore", "neuron": "AVA", "run_speed_mm_s": $a3}
 ],
 "baseline_run_speed_mm_s": $b,
 "seed": 5,
 "duration_s": 120
}
JSON
echo "demo: intact $b -> +cut AVA $a1 -> +cut AVD $a2 -> restore AVA $a3 (results/surgery_demo_session.json)"
