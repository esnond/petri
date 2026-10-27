#!/bin/sh
# writes reproducibility manifest for results/
OUTFILE="${1:-results/manifest.txt}"
{
    echo "# petri reproducibility manifest, $(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo "commit: $(git rev-parse HEAD 2>/dev/null || echo unknown)"
    echo "compiler: $(gcc --version | head -1)"
    echo "make flags: $(grep CFLAGS Makefile | head -1)"
    echo ""
    echo "results files:"
    for f in results/*.csv; do
        [ -f "$f" ] || continue
        echo " $f: $(sha256sum "$f" | cut -d' ' -f1) ($(wc -l < "$f") lines)"
    done
} > "$OUTFILE"
echo "manifest written: $OUTFILE"
