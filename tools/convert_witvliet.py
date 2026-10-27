#!/usr/bin/env python3
# converts a witvliet 2021 nemanode json into the simulator csv format
# usage: python3 tools/convert_witvliet.py witvliet_2020_8.json witvliet_adult.csv
# nemanode typ 0 is chemical, typ 2 is a gap junction. gap pairs are listed
# from both sides, one direction is kept.
import json
import sys


def main():
    if len(sys.argv) != 3:
        print("usage: convert_witvliet.py <in.json> <out.csv>")
        return 1
    edges = json.load(open(sys.argv[1]))
    chem, gaps = [], []
    seen_gap = set()
    for e in edges:
        w = sum(e["syn"])
        if w <= 0:
            continue
        pre, post = e["pre"], e["post"]
        if e["typ"] == 0:
            chem.append((pre, post, w))
        else:
            key = tuple(sorted((pre, post)))
            if key not in seen_gap:
                seen_gap.add(key)
                gaps.append((pre, post, w))
    with open(sys.argv[2], "w") as f:
        f.write("Source,Target,Type,Weight\n")
        for pre, post, w in chem:
            f.write("%s,%s,S,%d\n" % (pre, post, w))
        for pre, post, w in gaps:
            f.write("%s,%s,EJ,%d\n" % (pre, post, w))
    print("chemical %d, gap %d, wrote %s" % (len(chem), len(gaps), sys.argv[2]))
    return 0


if __name__ == "__main__":
    sys.exit(main())