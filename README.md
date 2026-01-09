# MIPS Pipelined Simulator

Cycle-accurate 5-stage pipelined MIPS simulator with basic hazard handling. It reads instruction/data memories from text files, simulates the pipeline, and writes register/memory/state traces to output files.

## Build
- `make` or `make mips` builds the simulator binary `MIPS_pipeline` using `g++-11` if available, otherwise `g++`.

## Run (default behavior)
- `./MIPS_pipeline`
- Expects: `./imem.txt`, `./dmem.txt`
- Produces: `./RFresult.txt`, `./dmemresult.txt`, `./stateresult.txt`

## Run with CLI paths (optional)
```
./MIPS_pipeline --imem path/to/imem.txt --dmem path/to/dmem.txt --outdir path/to/output
```
- Defaults remain `imem.txt`, `dmem.txt`, and current directory if flags are omitted.
- `--help` prints usage.

## Tests
- `make test` (builds and runs the full testcase suite)
- Or run directly: `bash scripts/run_all.sh`
- Outputs are written under `out/<case_name>/` and diffed against each testcase’s `expected_results`.
- Lines with `\tX` in golden files are treated as “don’t care” and ignored during comparison.
- State trace diffs are skipped by default; set `STRICT_STATE=1` to enforce `stateresult.txt` comparisons.

## Project Structure
- `MIPS_pipeline.cpp`: simulator implementation.
- `Makefile`: build target (`make mips`) and test target (`make test`).
- `lab2_testcase2/`, `lab2_testcase3/`: sample inputs and golden outputs.
- `test_cases/`: additional instruction/memory inputs and nested testcases.

## Outputs
- `RFresult.txt`: register file state after execution.
- `dmemresult.txt`: data memory contents after execution.
- `stateresult.txt`: pipeline state per cycle.
