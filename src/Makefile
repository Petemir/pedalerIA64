CC = gcc
ASM = nasm
DBG = gdb
CPPFLAGS = -ggdb -Wall -pedantic -std=c99
LIBS = -lm -lsndfile
ASMFLAGS = -felf64 -g -F dwarf

BIN = main
#BIN_DIR = ../bin

OBJS =  main.o bitcrusher.o copy.o delay_simple.o flanger.o flanger_index_calc.o maxsamp_right.o normalization_right.o sine.o vibrato.o

.PHONY: all debug test clean

$(BIN): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@ $(LIBS)
#    cp $(BIN) $(BIN_DIR)/$(BIN)

main.o: main.c
	$(CC) $(CPPFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CPPFLAGS) -c -o $@ $<

%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f ./$(BIN)
	rm -f output*.wav
	rm -f effects.o
#    rm -f $(BIN_DIR)/$(BIN)

rmall:
	rm -f $(OBJS)
	rm -f output.wav
#    rm -f ./$(BIN)
#    rm -f $(BIN_DIR)/$(BIN)
#    rm -f ../pics/*.bmp
