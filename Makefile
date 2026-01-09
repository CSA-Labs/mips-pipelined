mips: MIPS_pipeline.cpp
	g++-11 MIPS_pipeline.cpp -o MIPS_pipeline
test: mips
	bash scripts/run_all.sh
clean:
	rm MIPS_pipeline
