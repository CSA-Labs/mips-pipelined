#!/usr/bin/env bash
set -euo pipefail

usage() {
  echo "Usage:" >&2
  echo "  $0 <testcase_dir>" >&2
  echo "  $0 --imem <path> --dmem <path> --expected <dir> [--outdir <dir>] [--case-name <name>]" >&2
}

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BINARY="$ROOT_DIR/MIPS_pipeline"

if [ ! -x "$BINARY" ]; then
  echo "Error: $BINARY not found. Run 'make mips' first." >&2
  exit 1
fi

CASE_DIR=""
IMEM=""
DMEM=""
EXPECTED_DIR=""
OUTDIR=""
CASE_NAME=""

if [ "$#" -ge 1 ] && [ "${1#--}" = "$1" ]; then
  CASE_DIR="$1"
  shift
fi

if [ "$#" -gt 0 ]; then
  while [ "$#" -gt 0 ]; do
    case "$1" in
      --imem)
        IMEM="$2"; shift 2 ;;
      --dmem)
        DMEM="$2"; shift 2 ;;
      --expected)
        EXPECTED_DIR="$2"; shift 2 ;;
      --outdir)
        OUTDIR="$2"; shift 2 ;;
      --case-name)
        CASE_NAME="$2"; shift 2 ;;
      -h|--help)
        usage; exit 0 ;;
      *)
        echo "Unknown option: $1" >&2
        usage
        exit 2
        ;;
    esac
  done
fi

if [ -n "$CASE_DIR" ]; then
  IMEM="$CASE_DIR/imem.txt"
  DMEM="$CASE_DIR/dmem.txt"
  if [ -d "$CASE_DIR/expected_results" ]; then
    EXPECTED_DIR="$CASE_DIR/expected_results"
  elif [ -d "$CASE_DIR/expected_results " ]; then
    EXPECTED_DIR="$CASE_DIR/expected_results "
  else
    echo "Error: expected_results directory not found in $CASE_DIR" >&2
    exit 1
  fi
  CASE_NAME="${CASE_NAME:-$(basename "$CASE_DIR")}"
fi

if [ -z "$IMEM" ] || [ -z "$DMEM" ] || [ -z "$EXPECTED_DIR" ]; then
  usage
  exit 2
fi

if [ ! -f "$IMEM" ] || [ ! -f "$DMEM" ]; then
  echo "Error: missing imem.txt or dmem.txt" >&2
  exit 1
fi

if [ -z "$OUTDIR" ]; then
  CASE_NAME="${CASE_NAME:-case}"
  OUTDIR="$ROOT_DIR/out/$CASE_NAME"
fi

mkdir -p "$OUTDIR"
rm -f "$OUTDIR/RFresult.txt" "$OUTDIR/dmemresult.txt" "$OUTDIR/stateresult.txt"

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
  if [ "$output_name" = "stateresult.txt" ] && [ "${STRICT_STATE:-0}" != "1" ]; then
    echo "SKIP: $output_name (set STRICT_STATE=1 to enforce)" >&2
    continue
  fi
  if grep -q $'\tX' "$expected"; then
    if ! awk -F '\t' '
      NR==FNR { expected_lines[NR]=$0; total=NR; next }
      {
        if (FNR > total) { exit 2 }
        exp_line = expected_lines[FNR]
        if ($0 == exp_line) { next }
        split(exp_line, eparts, "\t")
        split($0, oparts, "\t")
        if (eparts[2] == "X" && oparts[1] == eparts[1]) { next }
        exit 1
      }
      END { if (FNR < total) { exit 3 } }
    ' "$expected" "$output_path"; then
      diff -u "$expected" "$output_path" || true
      echo "Diff failed for $output_name (X treated as wildcard)" >&2
      status=1
    fi
  else
    if ! diff -u "$expected" "$output_path"; then
      echo "Diff failed for $output_name" >&2
      status=1
    fi
  fi
done
shopt -u nullglob

if [ "$status" -ne 0 ]; then
  exit 1
fi

echo "PASS: ${CASE_NAME:-$EXPECTED_DIR}"
