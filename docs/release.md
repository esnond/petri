# release checklist, v1.0

1. make test green on the release machine, and on one machine that is
   not the development machine (determinism is bitwise per binary, the
   csv contract is per build).
2. sh run_all.sh from a fresh clone: results/ regenerates end to end,
   summary.csv numbers within a few percent of the committed ones.
3. validation table 5 of 5 or the mismatch rows re-audited against the
   literature with citations.
4. calibration battery: 8 of 10 seeds minimum inside the speed band;
   the two open seeds get a sentence in calibration.md.
5. known open defects re-counted: rim hugging (docs/rim_hugging.md),
   ablation divergence (docs/ablation_divergence.md), witvliet unnamed
   motor neurons, asel/asER and afd asymmetries. limitations in readme
   must reference all of them.
6. CITATION.cff: fill the zenodo doi placeholder, check author order.
7. tag v1.0, push, upload the tag to zenodo, write the real doi back
   into CITATION.cff, commit, re-tag.
8. the three documentation files in petri-docs regenerated against the
   release commit (see petri-docs/README.txt).
