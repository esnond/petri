# frozen csv schemas, added 2026-10-27

## results/atlas.csv (workstream b)

neuron,connectome,seed,spd_mm_s,bend_amp,period_s,rev_count,perimeter_ratio
ci_chemo was dropped before the first sweep: the frozen metrics contract
carries no chemotaxis column, so the atlas cannot measure it honestly.

one row per (neuron, connectome, seed) ablated run; control rows use
neuron = "intact". a row is a full headless run; metrics identical in
definition to the calibration battery (run windows for speed).

## results/traces_gcamp.csv (workstream a)

t_s,vel_mm_s,g_AVA,g_AVB,g_AIB,g_AIY,v_AVA,v_AVB,v_AIB,v_AIY

10 ms cadence; g_ columns are the gcamp forward model output of the
v_ columns, model in src/gcamp.c, kinetics chen 2013.

## results/predictions_heldout.csv (workstream c)

prediction_id,seed,metric,value

one row per held-out seed (101..110) per tested prediction; verdicts
aggregate in docs/predictions.md only.

## results/interp_activity.csv (workstream d)

t_s,<neuron name>... one column per neuron, membrane potentials, 10 ms
cadence; ground truth adjacency in results/ground_truth_adjacency.csv
(src,dst,w,kind) and circuits in results/ground_truth_circuits.json.
