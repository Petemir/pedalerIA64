#!/bin/sh
# Guitar delay simple
./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-c-0.8-0.9.wav -d 0.8 0.9
./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-c-1.0-0.7.wav -d 1.0 0.7
./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-c-2.5-0.6.wav -d 2.5 0.6
./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-c-4.5-0.3.wav -d 4.5 0.3

./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-asm-0.8-0.9.wav -D 0.8 0.9
./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-asm-1.0-0.7.wav -D 1.0 0.7
./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-asm-2.5-0.6.wav -D 2.5 0.6
./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-asm-4.5-0.3.wav -D 4.5 0.3


./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-c-0.8-0.9.wav -d 0.8 0.9
./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-c-1.0-0.7.wav -d 1.0 0.7
./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-c-2.5-0.6.wav -d 2.5 0.6
./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-c-4.5-0.3.wav -d 4.5 0.3

./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-asm-0.8-0.9.wav -D 0.8 0.9
./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-asm-1.0-0.7.wav -D 1.0 0.7
./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-asm-2.5-0.6.wav -D 2.5 0.6
./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-asm-4.5-0.3.wav -D 4.5 0.3
