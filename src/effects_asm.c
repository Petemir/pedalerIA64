// PedalerIA64 Copyright [2014] <Matias Laporte>
//#include "./effects.h"

/*void copy_asm_caller() {
    bufferSize = BUFFERSIZE*inFileStr.channels;

    dataBuffIn = (double*)malloc(bufferSize*sizeof(double));   // Buffers de entrada y salida
    dataBuffOut = (double*)malloc(bufferSize*sizeof(double));

    // Limpio buffers
    clean_buffer_c(dataBuffIn, bufferSize);
    clean_buffer_c(dataBuffOut, bufferSize);

    unsigned long int start, end, cantCiclos = 0;
    while ((framesRead = sf_readf_double(inFilePtr, dataBuffIn, BUFFERSIZE))) {
        MEDIR_TIEMPO_START(start);
        copy_asm(dataBuffIn, dataBuffOut, framesRead*inFileStr.channels);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos+= end-start;

        framesWritten = sf_write_double(outFilePtr, dataBuffOut, framesRead*inFileStr.channels);
        sf_write_sync(outFilePtr);
    }

    free(dataBuffIn);
    free(dataBuffOut);
}*/


float maxsamp_right_asm_caller() {
    float maxSamp = 0;
    while ((framesRead = sf_readf_float(outFilePtr, dataBuffIn, BUFFERSIZE))) {
        MEDIR_TIEMPO_START(start);
        maxsamp_right_asm(dataBuffIn, &maxSamp, framesRead*outFileStr.channels);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;
    }
    sf_seek(outFilePtr, 0, SEEK_SET);

    return maxSamp;
}


void normalization_right_asm_caller() {
    // Siempre que normalice, la entrada va a ser stereo (porque va a ser la salida de un efecto anterior), así que usan buffer de mismo tamaño
    unsigned int bufferFrameSize = BUFFERSIZE*outFileStr.channels;
    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSize*sizeof(float));

    clean_buffer_c(dataBuffIn, bufferFrameSize);
    clean_buffer_c(dataBuffOut, bufferFrameSize);

    float maxSamp = 1/maxsamp_right_asm_caller();

    start = end = cantCiclos = framesReadTotal = 0;
    while ((framesRead = sf_readf_float(outFilePtr, dataBuffIn, BUFFERSIZE))) {
        MEDIR_TIEMPO_START(start);
        normalization_right_asm(dataBuffIn, dataBuffOut, &maxSamp, framesRead*outFileStr.channels);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRead*outFileStr.channels);
        sf_write_sync(outFilePtr);
    }

    free(dataBuffIn);
    free(dataBuffOut);
}


void delay_simple_asm_caller(float delayInSec, float decay) {
    unsigned int delayInFrames = ceil(delayInSec*inFileStr.samplerate);
    unsigned int maxDelayInFrames = (int)fmax((float)(BUFFERSIZE-(BUFFERSIZE%delayInFrames)), (float)delayInFrames);
    unsigned int bufferFrameSize = maxDelayInFrames*inFileStr.channels;
    unsigned int bufferFrameSizeOut = maxDelayInFrames*outFileStr.channels;

    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));
    float *dataBuffEffect = (float*)malloc(bufferFrameSize*sizeof(float));

    // Limpio buffers
    clean_buffer_c(dataBuffIn, bufferFrameSize);
    clean_buffer_c(dataBuffEffect, bufferFrameSize);
    clean_buffer_c(dataBuffOut, bufferFrameSizeOut);

    start = end = cantCiclos = 0;
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, maxDelayInFrames))) {
        MEDIR_TIEMPO_START(start);
        delay_simple_asm(dataBuffIn, dataBuffOut, dataBuffEffect, bufferFrameSize, &decay, inFileStr.channels);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesWritten = sf_write_float(outFilePtr, dataBuffOut, bufferFrameSizeOut);
        sf_write_sync(outFilePtr);

        clean_buffer_c(dataBuffIn, bufferFrameSize);
        framesRemaining = framesRead;
    }

    // Completo con los frames de efecto que quedaron cuando framesRead < bufferFrameSize
    MEDIR_TIEMPO_START(start);
    delay_simple_asm(dataBuffIn, dataBuffOut, dataBuffEffect, framesRemaining*inFileStr.channels, &decay, inFileStr.channels);
    MEDIR_TIEMPO_STOP(end);
    cantCiclos += end-start;
    framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRemaining*outFileStr.channels);
    sf_write_sync(outFilePtr);

    // [Limpieza]
    free(dataBuffIn);
    free(dataBuffOut);
    free(dataBuffEffect);
    // [/Limpieza]
}


void flanger_asm_caller(float delayInSec, float rate, float amp) {
    unsigned int delayInFrames = floor(delayInSec*inFileStr.samplerate);
    unsigned int maxDelayInFrames = (int)fmax((float)(BUFFERSIZE-(BUFFERSIZE%delayInFrames)), (float)delayInFrames);
    unsigned int bufferFrameSize = maxDelayInFrames*inFileStr.channels;
    unsigned int bufferFrameSizeOut = maxDelayInFrames*outFileStr.channels;

    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));
    float *dataBuffEffect = (float*)malloc(maxDelayInFrames*sizeof(float));  // El buffer de efecto sólo va a contener el canal derecho de la salida
    unsigned int *dataBuffIndex = (unsigned int*)malloc(maxDelayInFrames*sizeof(unsigned int));
    // Limpio buffers
    clean_buffer_c(dataBuffIn, bufferFrameSize);
    clean_buffer_c(dataBuffOut, bufferFrameSizeOut);
    clean_buffer_c(dataBuffEffect, maxDelayInFrames);
    clean_buffer_c((float*)dataBuffIndex, maxDelayInFrames);

    start = end = cantCiclos = 0;
    framesReadTotal = 0;
    // [Lecto-escritura de datos]
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, maxDelayInFrames))) {
        /*for (unsigned int eff_i = 0; eff_i < fmin(framesRead, maxDelayInFrames); eff_i = eff_i) {
            v4sf index_vector;
            // Número que se utiliza para calcular el índice que se usará para el efecto
            for (unsigned int j = 0; j < 4; j++) {
                index_vector[j] = 2*M_PI*((framesReadTotal+1)+eff_i)*(rate/inFileStr.samplerate);
                eff_i++;
            }

            MEDIR_TIEMPO_START(start);
            index_vector = sin_ps(index_vector);    // Seno
            index_vector = _mm_and_ps(index_vector, *(v4sf*)_ps_inv_sign_mask);             // Valor absoluto
            MEDIR_TIEMPO_STOP(end);
            cantCiclos += end-start;

            // Guardo los índices en el buffer que se le pasará a la rutina en ASM
            eff_i-=4;
            for (unsigned int j = 0; j <= 3 && eff_i < framesRead; j++) {
                dataBuffIndex[eff_i] = (int) ((framesReadTotal+(eff_i)-ceil(index_vector[j]*delayInFrames)))%maxDelayInFrames;
                eff_i++;
            }
        }*/

        MEDIR_TIEMPO_START(start);
        flanger_index_calc(dataBuffIndex, framesRead, framesReadTotal, rate/inFileStr.samplerate, delayInFrames, maxDelayInFrames);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;
        MEDIR_TIEMPO_START(start);
        flanger_asm(dataBuffIn, dataBuffOut, dataBuffEffect, dataBuffIndex, framesRead*inFileStr.channels, inFileStr.channels, amp);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesReadTotal += framesRead;
        framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRead*outFileStr.channels);
        sf_write_sync(outFilePtr);
    }
    // [/Lecto-escritura de datos]

    // [Limpieza]
    free(dataBuffIn);
    free(dataBuffOut);
    free(dataBuffEffect);
    free(dataBuffIndex);
    // [/Limpieza]
}


void bitcrusher_asm_caller(int bits, int freq) {
    unsigned int bufferFrameSize = BUFFERSIZE*inFileStr.channels;
    unsigned int bufferFrameSizeOut = BUFFERSIZE*outFileStr.channels;

    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));

    // Limpio buffers
    clean_buffer_c(dataBuffIn, bufferFrameSize);
    clean_buffer_c(dataBuffOut, bufferFrameSizeOut);

    float last = 0, phasor = 0;   // Las variables se definen acá porque se necesita que se conserven entre ciclo y ciclo
    float normFreq = roundf(((float)freq/inFileStr.samplerate)*100)/100;
    float step = pow(0.5, bits);

    start = end = cantCiclos = framesReadTotal = 0;
    // [Lecto-escritura de datos]
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, BUFFERSIZE))) {
        MEDIR_TIEMPO_START(start);
        bitcrusher_asm(dataBuffIn, dataBuffOut, framesRead*inFileStr.channels, &step, normFreq, &phasor, &last, inFileStr.channels);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesReadTotal += framesRead;
        framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRead*outFileStr.channels);
        sf_write_sync(outFilePtr);
    }
    // [/Lecto-escritura de datos]

    // [Limpieza]
    free(dataBuffIn);
    free(dataBuffOut);
    // [/Limpieza]
}


void vibrato_asm_caller(float depth, float mod) {
    float delay = depth;    // Se usan dos nombres por claridad, pero valen siempre lo mismo
    delay = round(delay*inFileStr.samplerate);
    depth = round(depth*inFileStr.samplerate);
    mod = mod/inFileStr.samplerate;

    unsigned int delayInFrames = floor(2+delay+depth*2);
    unsigned int maxDelayInFrames = (int)fmax((float)(BUFFERSIZE-(BUFFERSIZE%delayInFrames)), (float)delayInFrames);
    unsigned int bufferFrameSize = maxDelayInFrames*inFileStr.channels;
    unsigned int bufferFrameSizeOut = maxDelayInFrames*outFileStr.channels;

    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));
    // Buffer circular
    float *dataBuffEffect = (float*)malloc(maxDelayInFrames*sizeof(float));
    int dataBuffEffectHead = maxDelayInFrames-1;

    float *dataBuffIndex = (float*)malloc(maxDelayInFrames*sizeof(float));

    // Limpio buffers
    clean_buffer_c(dataBuffIn, bufferFrameSize);
    clean_buffer_c(dataBuffOut, bufferFrameSizeOut);
    clean_buffer_c(dataBuffEffect, maxDelayInFrames);
    clean_buffer_c(dataBuffIndex, maxDelayInFrames);

    start = end = cantCiclos = framesReadTotal = 0;
    // [Lecto-escritura de datos]
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, maxDelayInFrames))) {
        /*for (unsigned int eff_i = 0; eff_i < framesRead; eff_i = eff_i) {
            v4sf index_vector;

            for (unsigned int j = 0; j < 4; j++) {
                index_vector[j] = mod*2*M_PI*(framesReadTotal+eff_i+1);
                eff_i++;
            }

            MEDIR_TIEMPO_START(start);
            index_vector = sin_ps(index_vector); // Acá tengo current_mod para cada indice
            MEDIR_TIEMPO_STOP(end);
            cantCiclos += end-start;

            eff_i -= 4;
            for (unsigned int j = 0; j < 4 && eff_i < framesRead; j++) {
                dataBuffIndex[eff_i] = 1+delay+depth*index_vector[j];
                eff_i++;
            }
        }*/
        MEDIR_TIEMPO_START(start);
        vibrato_index_calc(dataBuffIndex, framesRead, framesReadTotal, mod, depth);
        MEDIR_TIEMPO_STOP(end);

        MEDIR_TIEMPO_START(start);
        vibrato_asm(dataBuffIn, dataBuffOut, dataBuffEffect, dataBuffIndex, &dataBuffEffectHead, &maxDelayInFrames, framesRead*inFileStr.channels, inFileStr.channels);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesReadTotal += framesRead;
        framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRead*outFileStr.channels);
        sf_write_sync(outFilePtr);
    }
    // [/Lecto-escritura de datos]

    // [Limpieza]
    free(dataBuffIn);
    free(dataBuffOut);
    free(dataBuffIndex);
    free(dataBuffEffect);
    // [/Limpieza]
}

void wah_wah_asm_caller(float damp, int minf, int maxf, int wahfreq) {
    unsigned int bufferFrameSize = BUFFERSIZE*inFileStr.channels;
    unsigned int bufferFrameSizeOut = BUFFERSIZE*outFileStr.channels;

    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));
    float *dataBuffMod = (float*)malloc(BUFFERSIZE*sizeof(float));

    // Limpio buffers
    clean_buffer_c(dataBuffIn, bufferFrameSize);
    clean_buffer_c(dataBuffOut, bufferFrameSizeOut);
    clean_buffer_c(dataBuffMod, BUFFERSIZE);

    float delta = (float)wahfreq/inFileStr.samplerate;
    float yh = 0, yb = 0, yl = 0;

    start = end = cantCiclos = 0;
    framesReadTotal = 0;
    // [Lecto-escritura de datos]
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, BUFFERSIZE))) {
        /*for (unsigned int eff_i = 0; eff_i < framesRead; eff_i = eff_i) {
            v4sf index_vector;

            for (unsigned int j = 0; j < 4; j++) {
                int triangleWaveSize = floor((maxf-minf)/delta)+1;
                int parityCycle = ((framesReadTotal+eff_i)/(triangleWaveSize))%2;   // Ciclo positivo (par, 0) o negativo (impar, 1)
                int thisCycle = (framesReadTotal+eff_i) % (triangleWaveSize)+1;     // A qué punto del ciclo correspondería
                float fc = (1-parityCycle)*(minf+(thisCycle-1)*delta)+(parityCycle)*(maxf-(thisCycle)*delta);   // Valor del punto

                index_vector[j] = M_PI*fc/inFileStr.samplerate;
                eff_i++;
            }

            MEDIR_TIEMPO_START(start);
            index_vector = sin_ps(index_vector); // Acá tengo current_mod para cada indice
            MEDIR_TIEMPO_STOP(end);
            cantCiclos += end-start;

            eff_i -= 4;
            for (unsigned int j = 0; j < 4 && eff_i < framesRead; j++) {
                dataBuffMod[eff_i] = 2*index_vector[j];
                eff_i++;
            }
        }*/
        MEDIR_TIEMPO_START(start);
        wah_wah_index_calc(dataBuffMod, framesRead, framesReadTotal, minf, maxf, delta, inFileStr.samplerate);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        MEDIR_TIEMPO_START(start);
        wah_wah_asm(dataBuffIn, dataBuffOut, dataBuffMod, framesRead*inFileStr.channels, inFileStr.channels, 2*damp, &yh, &yb, &yl);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesReadTotal += framesRead;
        framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRead*outFileStr.channels);
        sf_write_sync(outFilePtr);
    }
    // [/Lecto-escritura de datos]

    // [Limpieza]
    free(dataBuffIn);
    free(dataBuffOut);
    free(dataBuffMod);
    // [/Limpieza]
}
