#!/bin/sh
# validation manifest: sha256 of every published artifact
set -e
cd "$(dirname "$0")/.."
OUT=results/validation_manifest.txt
{
    echo "# petri validation manifest, $(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo "commit: $(git rev-parse HEAD 2>/dev/null || echo unknown)"
    echo "compiler: $(cc --version | head -1)"
    echo "flags: $(grep '^CFLAGS' Makefile)"
    echo ""
    echo "## binary"
    echo " petri: $(sha256sum petri 2>/dev/null | cut -d' ' -f1 || echo not-built)"
    echo ""
    echo "## datasets"
    for f in data/*.csv data/*.txt; do
        [ -f "$f" ] && echo " $f: $(sha256sum "$f" | cut -d' ' -f1)"
    done
    echo ""
    echo "## results"
    for f in results/*.csv results/*.txt; do
        [ -f "$f" ] && echo " $f: $(sha256sum "$f" | cut -d' ' -f1) ($(wc -l < "$f") lines)"
    done
    echo ""
    echo "## figures"
    for f in figures/*.png; do
        [ -f "$f" ] && echo " $f: $(sha256sum "$f" | cut -d' ' -f1)"
    done
} > "$OUT"
echo "validation manifest written: $OUT"
