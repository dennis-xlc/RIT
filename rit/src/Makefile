UBT_SRC = ../../../src
CC = gcc
CFLAGS = -Iubt-stub -DDEBUG

RIT_C_FILES = InstrUtil.c GenHelper.c GenARMInstr.c GenThumbInstr.c GenThumb32Instr.c BiasUtil.c RandomInstrGen.c InstrGenerator.c
RIT_O_FILES = $(patsubst %.c,%.o,$(RIT_C_FILES))

%.o: %.c
	$(CC) $(CFLAGS) -g -Wall -fPIC -c $<

random-instr-gen: $(RIT_O_FILES)
	$(CC) $(RIT_O_FILES) -static -o $@

librit.so: $(RIT_O_FILES)
	$(CC) $(RIT_O_FILES) -shared -o $@

clean:
	rm -f *.o
	rm -f random-instr-gen
	rm -f librit.so
	
