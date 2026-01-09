#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
RUN_CASE="$SCRIPT_DIR/run_case.sh"

if [ ! -x "$RUN_CASE" ]; then
  echo "Error: $RUN_CASE not found or not executable." >&2
  exit 1
fi

total=0
pass=0
fail=0

for root in "$ROOT_DIR/lab2_testcase2" "$ROOT_DIR/lab2_testcase3" "$ROOT_DIR/test_cases"; do
  [ -d "$root" ] || continue
  while IFS= read -r -d '' imem; do
    case_dir="$(dirname "$imem")"
    if [ ! -f "$case_dir/dmem.txt" ]; then
      continue
    fi
    if [ -d "$case_dir/expected_results" ] || [ -d "$case_dir/expected_results " ]; then
      total=$((total + 1))
      if bash "$RUN_CASE" "$case_dir"; then
        pass=$((pass + 1))
      else
        fail=$((fail + 1))
      fi
    fi
  done < <(find "$root" -type f -name "imem.txt" -print0)

done

echo "Summary: $pass passed, $fail failed, $total total"

if [ "$total" -eq 0 ] || [ "$fail" -ne 0 ]; then
  exit 1
fi
