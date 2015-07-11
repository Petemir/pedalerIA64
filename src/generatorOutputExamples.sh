#!/bin/sh
# Guitar delay simple
#./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-c-0.8-0.9.wav -d 0.8 0.9
#./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-c-1.0-0.7.wav -d 1.0 0.7
#./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-c-2.5-0.6.wav -d 2.5 0.6
#./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-c-4.5-0.3.wav -d 4.5 0.3
#
#./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-asm-0.8-0.9.wav -D 0.8 0.9
#./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-asm-1.0-0.7.wav -D 1.0 0.7
#./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-asm-2.5-0.6.wav -D 2.5 0.6
#./main inputExamples/guitar.wav outputExamples/delay_simple-guitar-asm-4.5-0.3.wav -D 4.5 0.3
#
#
#./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-c-0.8-0.9.wav -d 0.8 0.9
#./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-c-1.0-0.7.wav -d 1.0 0.7
#./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-c-2.5-0.6.wav -d 2.5 0.6
#./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-c-4.5-0.3.wav -d 4.5 0.3
#
#./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-asm-0.8-0.9.wav -D 0.8 0.9
#./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-asm-1.0-0.7.wav -D 1.0 0.7
#./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-asm-2.5-0.6.wav -D 2.5 0.6
#./main inputExamples/gibson.wav outputExamples/delay_simple-gibson-asm-4.5-0.3.wav -D 4.5 0.3

# Guitar flanger
./main inputExamples/guitar.wav outputExamples/flanger-guitar-c-0.003-5-0.7.wav -f 0.003 5 0.7
./main inputExamples/guitar.wav outputExamples/flanger-guitar-c-0.005-0.5-0.75.wav -f 0.005 0.5 0.75
./main inputExamples/guitar.wav outputExamples/flanger-guitar-c-0.010-0.3-0.65.wav -f 0.010 0.3 0.65
./main inputExamples/guitar.wav outputExamples/flanger-guitar-c-0.015-3-0.8.wav -f 0.015 3 0.8

./main inputExamples/guitar.wav outputExamples/flanger-guitar-asm-0.003-5-0.7.wav -F 0.003 5 0.7
./main inputExamples/guitar.wav outputExamples/flanger-guitar-asm-0.005-0.5-0.75.wav -F 0.005 0.5 0.75
./main inputExamples/guitar.wav outputExamples/flanger-guitar-asm-0.010-0.3-0.65.wav -F 0.010 0.3 0.65
./main inputExamples/guitar.wav outputExamples/flanger-guitar-asm-0.015-3-0.8.wav -F 0.015 3 0.8

./main inputExamples/gibson.wav outputExamples/flanger-gibson-c-0.008-0.7-0.7.wav -f 0.008 0.7 0.7
./main inputExamples/gibson.wav outputExamples/flanger-gibson-c-0.005-0.1-0.75.wav -f 0.005 0.1 0.75
./main inputExamples/gibson.wav outputExamples/flanger-gibson-c-0.012-2.5-0.65.wav -f 0.012 2.5 0.65
./main inputExamples/gibson.wav outputExamples/flanger-gibson-c-0.015-1.0-0.8.wav -f 0.015 1.0 0.8

./main inputExamples/gibson.wav outputExamples/flanger-gibson-asm-0.008-0.7-0.7.wav -F 0.008 0.7 0.7
./main inputExamples/gibson.wav outputExamples/flanger-gibson-asm-0.005-0.1-0.75.wav -F 0.005 0.1 0.75
./main inputExamples/gibson.wav outputExamples/flanger-gibson-asm-0.012-2.5-0.65.wav -F 0.012 2.5 0.65
./main inputExamples/gibson.wav outputExamples/flanger-gibson-asm-0.015-1.0-0.8.wav -F 0.015 1.0 0.8
