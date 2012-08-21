UBT_SRC = ./
CC = gcc
CFLAGS = -fPIC -shared -I python-stub -I ubt-stub -Wall

RIT_C_FILES = InstrUtil.c GenHelper.c GenARMInstr.c GenThumbInstr.c GenThumb32Instr.c BiasUtil.c RandomInstrGen.c InstrGenerator.c

librit.so: $(RIT_O_FILES)
	$(CC) $(CFLAGS) $(RIT_C_FILES) -o $@

clean:
	rm -f *.o
	rm -f librit.so
	
