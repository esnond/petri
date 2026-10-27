#!/bin/sh
# run three optogenetics protocols
set -e
mkdir -p results
./petri --headless -d 120 --seed 1 --opsin "AVA:ChETA" --light-protocol results/ava_stim.json > results/opto_ava.csv
./petri --headless -d 120 --seed 1 --opsin "AIB:ChETA" --light-protocol results/aib_stim.json > results/opto_aib.csv
./petri --headless -d 120 --seed 1 --opsin "AVB:NpHR" --light-protocol results/avb_silence.json > results/opto_avb.csv
echo "optogenetics protocols complete"
