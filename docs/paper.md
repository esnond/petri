---
title: "petri: a falsifiable connectome-to-behaviour test bench for Caenorhabditis elegans"
tags:
  - c
  - neuroscience
  - connectome
  - simulation
  - caenorhabditis-elegans
authors:
  - name: esnond
    orcid: pending
affiliations:
  - index: 1
    name: Independent researcher
date: 2026-10-28
bibliography: paper.bib
---

# Summary

petri integrates a published wiring diagram of the Caenorhabditis
elegans nervous system [white1986; varshney2011] with graded
neuromuscular transmission and quasi-static biomechanics into one
deterministic C99 program. The worm crawls, reverses, dwells on food
and avoids walls without any behavioural rule in the code: everything
emerges from the connectome-driven circuit, proprioceptive stretch
reflex and anisotropic drag [wen2012].

The scientific claim is falsifiable. The bench carries five validation
channels: behavioural calibration against the Yemini 2013 database,
a GCaMP6f forward model compared against Kato 2015 whole-brain
imaging, an optogenetics channel replicating three published
protocols, a mutant library screened against the Brenner 1974
phenotype catalogue, and a prediction registry scored on held-out
seeds. Every published number is a CSV plus a seed; every divergence
from the literature is documented with hypotheses, not hidden.

# Statement of need

Existing C. elegans simulators are either Python-based with heavy
dependencies (OpenWorm/Sibernetic), focused on single-neuron
biophysics without whole-animal behaviour (c302), or lack
reproducibility infrastructure. petri fills the gap: a whole-animal
simulator in C99 with zero dependencies beyond libc and libm, bitwise
reproducible output, and a falsifiable validation framework that
treats divergences from the literature as results rather than bugs.

The key architectural choice — proprioceptive stretch reflex instead
of a central pattern generator — follows Wen et al. 2012 and makes the
model falsifiable at the circuit level: if the published wiring does
not produce the observed behaviour, the numbers say so publicly.

# Validation

Five independent channels:

1 **Behavioural**: crawl speed 0.19-0.25 mm/s (literature 0.15-0.30
[Yemini 2013]), bend frequency 1.0 Hz (0.5-1.5), reversal duration
1.2 s (0.5-2.5 [Rankin and Broster 1992]), bend amplitude 2.4 1/mm
(1-5). Ten of ten calibration seeds inside the speed band.

2 **Neural**: GCaMP6f forward model compared against Kato 2015
whole-brain imaging. The command sequence (AVA up, AVB down, AIB
transient) is absent at spontaneous reversal onsets — a documented
divergence that localises the model's architectural gap.

3 **Optogenetics**: three published protocols replicated. AVA
depolarisation, AIB depolarisation, AVB silencing — all three follow
the tone-layer signature, diverging coherently from life.

4 **Mutants**: nineteen unc mutants screened against the Brenner
lineage. Paralysis (unc-54-severe), half-drive (unc-54), acetylcholine
trio (unc-29/38/63) match the literature direction. Exocytosis arms
(unc-13/18/64/10) diverge — the STSP release lever does not gate the
crawl the way exocytosis does in life.

5 **Predictions**: ten registered before scoring, tested on held-out
seeds 101-110. Five confirmed, three refuted with hypotheses, one
unscorable after a metric fix, one pending control arms.

# Datasets

The causal atlas ablates each of 302 neurons across two connectomes
and three seeds: 122 neurons are reconstruction-sensitive, the
disagreement between published wiring diagrams concentrates in the
command layer. The cross-connectome benchmark runs four
reconstructions (Varshney 2011, Witvliet 2021, Cook 2019 hermaphrodite
and male) on one body: four phenotypes from wiring alone. The null
model (degree-preserving shuffle) shows that locomotion speed is
statistics-specific while chemotaxis is connectome-specific.

# Open defects

Rim hugging (the worm spends 95% of a 600 s run on the perimeter),
the absence of the neural command signature at reversal onsets, the
ablation direction divergence, and fragile calibration constants are
all documented with numbers, hypotheses and resume commands. None are
hidden.

# Availability

Source code: https://github.com/esnond/petri
License: MIT
Documentation: docs/ in the repository
Reproducibility: sh run_all.sh regenerates every published number in
~15 minutes. Manifest with SHA256 checksums included.

# References
