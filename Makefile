
OUTDIR=build

kernel: cmake compile
	umps3-elf2umps -k $(OUTDIR)/phase1.kernel.elf

compile: cmake
	cd $(OUTDIR) && make

cmake: CMakeLists.txt
	@mkdir -p $(OUTDIR)
	@cd $(OUTDIR) && cmake ..

clean:
	cd $(OUTDIR) && make clean

	# while in the cmake is not fixed, use those cleans
	rm $(OUTDIR)/phase1.kernel.elf.core.umps 
	rm $(OUTDIR)/phase1.kernel.elf.stab.umps

