
OUTDIR=build

phase1: cmake compile
	umps3-elf2umps -k $(OUTDIR)/phase1.kernel.elf

compile: cmake
	cd $(OUTDIR) && make

cmake: CMakeLists.txt
	@mkdir -p $(OUTDIR)
	@cd $(OUTDIR) && cmake -DPHASE1=ON ..

clean:
	cd $(OUTDIR) && make clean