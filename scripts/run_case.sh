#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <testcase_dir>" >&2
  exit 2
fi

CASE_DIR="$1"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BINARY="$ROOT_DIR/MIPS_pipeline"

if [ ! -x "$BINARY" ]; then
  echo "Error: $BINARY not found. Run 'make mips' first." >&2
  exit 1
fi

IMEM="$CASE_DIR/imem.txt"
DMEM="$CASE_DIR/dmem.txt"
if [ ! -f "$IMEM" ] || [ ! -f "$DMEM" ]; then
  echo "Error: missing imem.txt or dmem.txt in $CASE_DIR" >&2
  exit 1
fi

if [ -d "$CASE_DIR/expected_results" ]; then
  EXPECTED_DIR="$CASE_DIR/expected_results"
elif [ -d "$CASE_DIR/expected_results " ]; then
  EXPECTED_DIR="$CASE_DIR/expected_results "
else
  echo "Error: expected_results directory not found in $CASE_DIR" >&2
  exit 1
fi

TMPDIR="$(mktemp -d)"
cleanup() {
  rm -rf "$TMPDIR"
}
trap cleanup EXIT

OUTDIR="$TMPDIR/out"
mkdir -p "$OUTDIR"

"$BINARY" --imem "$IMEM" --dmem "$DMEM" --outdir "$OUTDIR"

status=0
shopt -s nullglob
for expected in "$EXPECTED_DIR"/*_ans.txt; do
  base="$(basename "$expected")"
  output_name="${base/_ans/}"
  output_path="$OUTDIR/$output_name"
  if [ ! -f "$output_path" ]; then
    echo "Missing output: $output_path" >&2
    status=1
    continue
  fi
  if ! diff -u "$expected" "$output_path"; then
    echo "Diff failed for $output_name in $CASE_DIR" >&2
    status=1
  fi
done
shopt -u nullglob

if [ "$status" -ne 0 ]; then
  exit 1
fi

echo "PASS: $CASE_DIR"
