# MIPS Pipelined Simulator

Cycle-accurate 5-stage pipelined MIPS simulator with basic hazard handling. It reads instruction/data memories from text files, simulates the pipeline, and writes register/memory/state traces to output files.

## Build
- `make mips` builds the simulator binary `MIPS_pipeline` using `g++-11`.

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
- The script executes each testcase and diffs outputs against golden files in each testcase directory.

## Project Structure
- `MIPS_pipeline.cpp`: simulator implementation.
- `Makefile`: build target (`make mips`) and test target (`make test`).
- `lab2_testcase2/`, `lab2_testcase3/`: sample inputs and golden outputs.
- `test_cases/`: additional instruction/memory inputs and nested testcases.

## Outputs
- `RFresult.txt`: register file state after execution.
- `dmemresult.txt`: data memory contents after execution.
- `stateresult.txt`: pipeline state per cycle.
