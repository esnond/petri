# petri v1.0 post release summary

## what was built, 2026-10-26 to 2026-10-28

core: modular c99, zero dependencies beyond libc libm posix, 302
neurons across four connectome datasets, proprioceptive traveling
wave without a central pattern generator, tsodyks markram plasticity,
homeostatic scaling, heterosynaptic learning, optogenetics channel,
mutant library, gcamp6f forward model.

## validation channels

1 behavior: five of five literature metrics matched after calibration
2 neural: gcamp comparison found the command sequence absent, the
  sharpest falsifiable divergence
3 optogenetics: three protocols, one coherent tone layer signature
4 mutants: nineteen named mutants screened, comparison table pending
5 predictions: ten registered pre hoc, scored on held out seeds

## datasets

causal atlas 302 neurons x two connectomes x three seeds, cross
connectome benchmark 300 s x ten seeds x four reconstructions, null
model, robustness ensemble, prediction registry.

## open defects, public

rim hugging, neural command sequence absent, ablation divergence,
fragile gain constants, sensitization instead of habituation, swarm
food not shared.

## lessons

llm assisted development worked where architectural decisions were
written down, skills enforced standards, the registry tracked state,
and null models plus robustness checks prevented tuning to the pet
metrics. the history stays granular on purpose.
