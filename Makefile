
OUTDIR=build

# TODO: c'è molta ripetizione fra cmake 1 e compile 1
# se si riesce provare a toglierla, ma non è importante.

phase2: cmake2 compile2
	umps3-elf2umps -k $(OUTDIR)/phase2.kernel.elf

compile2: cmake2
	cd $(OUTDIR) && make

cmake2: CMakeLists.txt
	@mkdir -p $(OUTDIR)
	@cd $(OUTDIR) && cmake ..


phase1: cmake1 compile1
	umps3-elf2umps -k $(OUTDIR)/phase1.kernel.elf

compile1: cmake1
	cd $(OUTDIR) && make

cmake1: CMakeLists.txt
	@mkdir -p $(OUTDIR)
	@cd $(OUTDIR) && cmake -DPHASE1=ON ..

clean:
	cd $(OUTDIR) && make clean