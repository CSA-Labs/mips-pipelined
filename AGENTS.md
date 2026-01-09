# Repository Guidelines

## Project Structure & Module Organization
- `MIPS_pipeline.cpp` contains the full MIPS five-stage pipeline simulator implementation.
- `Makefile` builds the simulator binary (`MIPS_pipeline`).
- `imem.txt` and `dmem.txt` are default instruction/data memory inputs.
- Output artifacts are generated in the repo root: `RFresult.txt`, `dmemresult.txt`, and `stateresult.txt`.
- Reference inputs and expected outputs live under `lab2_testcase2/`, `lab2_testcase3/`, and `test_cases/`.

## Build, Test, and Development Commands
- `make mips` builds the simulator with `g++-11` and produces `MIPS_pipeline`.
- `./MIPS_pipeline` runs the simulator using `imem.txt` and `dmem.txt` in the repo root.
- `make clean` removes the `MIPS_pipeline` binary.
- Example workflow for a test case:
  - `cp test_cases/imem_111.txt imem.txt`
  - `cp test_cases/dmem_111.txt dmem.txt`
  - `./MIPS_pipeline`
  - `diff -u dmemresult.txt test_cases/lab2_testcase3/expected_results/dmemresult_ans.txt`

## Coding Style & Naming Conventions
- Indentation: 4 spaces; keep brace style consistent with existing K&R layout.
- Naming follows existing patterns: `PascalCase` for structs/classes, `camelCase` for methods, `snake_case` for some members, and `ALL_CAPS` for macros (e.g., `MemSize`).
- Prefer minimal includes and keep I/O behavior consistent with current file-based inputs/outputs.

## Testing Guidelines
- No automated test runner is configured. Validate by running with known input files and comparing outputs to `expected_results`.
- Keep expected output files unchanged; update inputs in `imem.txt`/`dmem.txt` when experimenting.

## Commit & Pull Request Guidelines
- Commit messages in history are short and direct (e.g., "fixed SUBU, LW, BNE"); follow that style.
- PRs should describe the behavioral change, list updated test inputs/expected outputs, and include diff comparisons when applicable.

## Configuration Notes
- The simulator reads `imem.txt` and `dmem.txt` from the working directory; ensure you run from the repo root.
