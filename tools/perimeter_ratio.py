#!/usr/bin/env python3
# perimeter_time_ratio from the frozen csv contract: fraction of rows with
# the head inside the outer two cell band (1 mm) of the 24x16 mm dish
import csv, sys, math

def ratio(path):
    rows = [x for x in csv.DictReader(open(path))]
    n = wall = 0
    for r in rows:
        x, y = float(r['head_x_mm']), float(r['head_y_mm'])
        if 1.0 <= x <= 23.0 and 1.0 <= y <= 15.0:
            pass
        else:
            wall += 1
        n += 1
    return wall / n if n else 0.0

if __name__ == '__main__':
    for p in sys.argv[1:]:
        print("%s %.3f" % (p, ratio(p)))
