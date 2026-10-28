# validation, phase 2

run: results/builtin_s1.csv

measured with the frozen csv contract, 10 hz samples

| metric | model | literature | source | verdict |
|---|---|---|---|---|
| speed_off_food_mm_s | 0.131 | 0.10..0.40 | yemini 2013 behavioral database, roaming state | match |
| bend_frequency_hz | 0.557 | 0.50..1.50 | wen 2012 proprioceptive coupling study, crawling on agar | match |
| body_length_mm | 1.000 | 0.90..1.20 | model constant, not csv measurable | match |
| reversal_duration_s | 0.600 | 0.50..2.50 | rankin and broster 1992, tap response | match |
| curvature_amplitude_inv_mm | 4.298 | 1.00..5.00 | order of magnitude from bend radius 0.2 to 0.5 mm, approx | match |

diagnostics not in the literature table: net 5.79 mm, tail static bias 0.07 1/mm

5 match, 0 mismatch