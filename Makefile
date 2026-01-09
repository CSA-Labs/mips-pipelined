CXX ?= $(shell command -v g++-11 >/dev/null 2>&1 && echo g++-11 || echo g++)

mips: MIPS_pipeline.cpp
	$(CXX) MIPS_pipeline.cpp -o MIPS_pipeline
test: mips
	bash scripts/run_all.sh
clean:
	rm -f MIPS_pipeline RFresult.txt dmemresult.txt stateresult.txt
	rm -rf out
