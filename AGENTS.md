# Repository Guidelines

## Structure
- `MIPS_pipeline.cpp`: single-file MIPS 5-stage pipeline simulator.
- `Makefile`: build/test targets.
- `imem.txt`, `dmem.txt`: default inputs when no CLI flags are provided.
- `lab2_testcase2/`, `lab2_testcase3/`, `test_cases/`: inputs and golden outputs.
- `out/`: test outputs (generated).

## Command Cookbook
- Build: `make` or `make mips`
- Run (defaults): `./MIPS_pipeline`
- Run (custom paths): `./MIPS_pipeline --imem <path> --dmem <path> --outdir <dir>`
- Test all: `make test` or `bash scripts/run_all.sh`
- Test one case: `bash scripts/run_case.sh <case_dir>`

## Conventions
- Keep core logic in `MIPS_pipeline.cpp`.
- Preserve default I/O behavior when no CLI args are provided.
- Test outputs are diffed against `expected_results` (lines with `\tX` are treated as wildcards) and written under `out/`. Use `STRICT_STATE=1` to enforce `stateresult.txt` diffs.
