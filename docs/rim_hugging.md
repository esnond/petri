# rim hugging, open question

## the phenomenon

in a 600 s run the worm spends about 95 percent of its time on the dish
perimeter (corner 48 percent, wall 47 percent) and did not visit the
food lawn. over ten 120 s seeds the mean perimeter_time_ratio is 0.568
(tools/perimeter_ratio.py: fraction of csv rows with the head outside
the inner 23x14 mm rectangle of the 24x16 mm dish).

## what was tested, with numbers

baseline: drag ratio 20, wall friction 0.65, boundary damp 3.0, ash tau
2.5 s. three seeds (1, 5, 6), 120 s each, mean perimeter ratio:

| probe | constant change | perimeter ratio | verdict |
|---|---|---|---|
| baseline | none | 0.568 | reference |
| a: nociceptor stops adapting under sustained press | ash tau 2500 to 25000 ms | 0.595 | no improvement |
| b: wall grinding costs more | wall friction 0.65 to 0.40 | 0.572 | no improvement |
| c: softer boundary zone | boundary damp 3.0 to 1.0 | 0.658 | worse |

none reaches the 30 percent target. hypothesis c actively hurts: the
softer boundary lets the wave reach the rim and the worm lingers.

## the mechanism we believe is responsible

the wall reaction is a position clamp on the head frame. when the worm
pushes into the rim, the clamp removes the normal velocity component
and keeps the tangential one, so every contact converts into sliding
along the wall. the heading changes only through the wave's own omega,
whose random walk is absorbed at the wall: each touch re-enters the
contact band, fires one ash pulse, the reversal moves the body along
its own axis, and the axis is parallel to the wall. nothing in the
circuit or the mechanics rotates the heading inward, so the rim is an
absorbing boundary for the heading process.

this is a mechanics gap, not a sensory one: real walls produce a
normal-reaction torque distributed over the pressed body segments,
which both stops the slide and pitches the nose away.

## candidate fixes, not yet tried

1. segment level wall reaction: for each body segment inside the
   contact band, project its velocity against the wall normal and add
   the resulting torque to the frame solve. pure mechanics, no rules.
2. curvature dependent wall escape: the clamp currently stores no
   information about which side was pressed; feeding the contact side
   into the existing anterior stretch pathway is circuit level but
   borders on a rule, needs a literature anchor first.

## status

open. documented 2026-10-27, probe data reproducible with
tools/perimeter_ratio.py and the constants table above.
