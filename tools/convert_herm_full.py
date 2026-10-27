#!/usr/bin/env python3
# converts the openworm herm_full_edgelist.csv into the simulator csv format
# the source file lists the complete hermaphrodite: 302 neurons plus muscle
# cells, chemical and electrical synapses, pharyngeal nervous system included
# usage: python3 tools/convert_herm_full.py herm_full.csv herm_302.csv
import csv
import sys


def main():
    if len(sys.argv) != 3:
        print("usage: convert_herm_full.py <in.csv> <out.csv>")
        return 1
    rows = list(csv.reader(open(sys.argv[1])))[1:]
    seen = set()
    n_chem = n_gap = n_skip = 0
    with open(sys.argv[2], "w") as f:
        f.write("Source,Target,Type,Weight\n")
        for r in rows:
            if len(r) < 4:
                n_skip += 1
                continue
            src, dst, w, typ = r[0].strip(), r[1].strip(), r[2].strip(), r[3].strip()
            if not src or not dst:
                n_skip += 1
                continue
            # chemical edges are single listed here, reciprocal pairs are real
            # biology. electrical edges are listed from both sides, the loader
            # deduplicates them by unordered pair
            kind = "S" if typ == "chemical" else "EJ"
            key = (src, dst, kind)
            if kind == "EJ" and tuple(sorted((src, dst))) in seen:
                n_skip += 1
                continue
            if kind == "EJ":
                seen.add(tuple(sorted((src, dst))))
            f.write("%s,%s,%s,%s\n" % (src, dst, kind, w))
            if kind == "S":
                n_chem += 1
            else:
                n_gap += 1
    print("chemical %d, gap %d, skipped %d, wrote %s" %
          (n_chem, n_gap, n_skip, sys.argv[2]))
    return 0


if __name__ == "__main__":
    sys.exit(main())