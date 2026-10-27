#!/usr/bin/env python3
# converts the cook et al 2019 male connectome supplement into the simulator
# csv format. source is the corrected july 2020 adjacency matrix workbook
# from openworm ConnectomeToolbox, sheets "male chemical" and
# "male gap jn symmetric". pre cells live in column c, post cells in row 3,
# values start at row 4 column 4, all 1 based like in the spreadsheet.
# usage: convert_cook2019_male.py <corrected.xlsx> <male_csv>
import re
import sys
import zipfile
import xml.etree.ElementTree as ET

NS = {"m": "http://schemas.openxmlformats.org/spreadsheetml/2006/main"}


def col_to_num(ref):
    m = re.match(r"([A-Z]+)(\d+)$", ref)
    col = 0
    for ch in m.group(1):
        col = col * 26 + ord(ch) - 64
    return col, int(m.group(2))


def load_sheet(z, sheet_xml, shared):
    grid = {}
    root = ET.fromstring(z.read(sheet_xml))
    for c in root.iter("{%s}c" % NS["m"]):
        ref = c.get("r")
        t = c.get("t")
        v = c.find("{%s}v" % NS["m"])
        if v is None or v.text is None:
            continue
        if t == "s":
            val = shared[int(v.text)]
        else:
            val = v.text
        col, row = col_to_num(ref)
        grid[(row, col)] = val.strip()
    return grid


def main():
    if len(sys.argv) != 3:
        print("usage: convert_cook2019_male.py <corrected.xlsx> <out.csv>")
        return 1
    z = zipfile.ZipFile(sys.argv[1])
    shared = []
    if "xl/sharedStrings.xml" in z.namelist():
        sroot = ET.fromstring(z.read("xl/sharedStrings.xml"))
        for si in sroot.findall("{%s}si" % NS["m"]):
            shared.append("".join(t.text or "" for t in si.iter("{%s}t" % NS["m"])))
    # resolve sheets by title from the workbook, positional sheetN.xml
    # paths break when a workbook revision reorders the tabs
    wb = z.read("xl/workbook.xml").decode("utf-8", "replace")
    rels = z.read("xl/_rels/workbook.xml.rels").decode("utf-8", "replace")
    rid2file = dict(re.findall(r'Id="(rId\d+)"[^>]*Target="(worksheets/[^"]+)"', rels))
    name2file = {}
    for m in re.finditer(r'<sheet name="([^"]+)"[^>]*r:id="(rId\d+)"', wb):
        if m.group(2) in rid2file:
            name2file[m.group(1)] = "xl/" + rid2file[m.group(2)]
    chem = load_sheet(z, name2file["male chemical"], shared)
    gap = load_sheet(z, name2file["male gap jn symmetric"], shared)

    n_chem = n_gap = 0
    with open(sys.argv[2], "w") as f:
        f.write("Source,Target,Type,Weight\n")
        # chemical send matrix, pre in column 3 rows 4..385, post in row 3
        # columns 4..578
        pre = [chem[(r, 3)] for r in range(4, 386)]
        post = [chem[(3, c)] for c in range(4, 579)]
        for i, src in enumerate(pre):
            if not src:
                continue
            for j, dst in enumerate(post):
                if not dst:
                    continue
                val = chem.get((4 + i, 4 + j))
                try:
                    w = float(val) if val else 0.0
                except ValueError:
                    continue
                if w >= 1.0:
                    f.write("%s,%s,S,%d\n" % (src, dst, int(w)))
                    n_chem += 1
        # gap junctions, symmetric square matrix rows 4..588, emit the upper
        # triangle once, the loader dedups by unordered pair anyway
        names = [gap[(r, 3)] for r in range(4, 589)]
        for i, a in enumerate(names):
            if not a:
                continue
            for j in range(i + 1, len(names)):
                b = names[j]
                if not b:
                    continue
                val = gap.get((4 + i, 4 + j))
                try:
                    w = float(val) if val else 0.0
                except ValueError:
                    continue
                if w >= 1.0:
                    f.write("%s,%s,EJ,%d\n" % (a, b, int(w)))
                    n_gap += 1
    print("chemical %d, gap %d" % (n_chem, n_gap))
    return 0


if __name__ == "__main__":
    sys.exit(main())
