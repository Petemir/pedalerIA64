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

    printf("\tTiempo de ejecución:\n");
    printf("\t  Comienzo                          : %lu\n", start)    ;
    printf("\t  Fin                               : %lu\n", end);
    // printf("\t  # iteraciones                     : %d\n", cant_iteraciones);
    printf("\t  # de ciclos insumidos totales     : %lu\n", cantCiclos);
    // printf("\t  # de ciclos insumidos por llamada : %.3f\n", (float)cantCiclos/(float)cant_iteraciones);

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
    printf("max: %f\n", maxSamp);

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

    printf("\tTiempo de ejecución:\n");
    printf("\t  Comienzo                          : %lu\n", start);
    printf("\t  Fin                               : %lu\n", end);
    // printf("\t  # iteraciones                     : %d\n", cant_iteraciones);
    printf("\t  # de ciclos insumidos totales     : %lu\n", cantCiclos);
    // printf("\t  # de ciclos insumidos por llamada : %.3f\n", (float)cantCiclos/(float)cant_iteraciones);

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
    //float *dataBuffSin = (float*)malloc(maxDelayInFrames*sizeof(float));
    unsigned int *dataBuffIndex = (unsigned int*)malloc(maxDelayInFrames*sizeof(unsigned int));
    // Limpio buffers
    clean_buffer_c(dataBuffIn, bufferFrameSize);
    clean_buffer_c(dataBuffOut, bufferFrameSizeOut);
    clean_buffer_c(dataBuffEffect, maxDelayInFrames);
    //clean_buffer_c(dataBuffSin, maxDelayInFrames);
    clean_buffer_c((float*)dataBuffIndex, maxDelayInFrames);

    start = end = cantCiclos = 0;
    framesReadTotal = 0;
    // [Lecto-escritura de datos]
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, maxDelayInFrames))) {
        /*for (unsigned int eff_i = 0; eff_i < framesRead; eff_i++) {
            float x = 2*M_PI*((framesReadTotal+1)+eff_i)*(rate/inFileStr.samplerate);
            while (x > M_PI) { x -= 2*M_PI; }
            dataBuffSin[eff_i] = x;
        }

        sine_asm(dataBuffSin, framesRead);*/

        MEDIR_TIEMPO_START(start);
        flanger_index_calc(dataBuffIndex, framesRead, framesReadTotal, rate/inFileStr.samplerate, delayInFrames, maxDelayInFrames);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;
        MEDIR_TIEMPO_START(start);
        flanger_asm(dataBuffIn, dataBuffOut, dataBuffEffect, dataBuffIndex, framesRead*inFileStr.channels, inFileStr.channels, amp);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

/*        for (unsigned int eff_i = 0; eff_i < framesRead; eff_i++) {
            printf("%d %d\n", framesReadTotal+eff_i, dataBuffIndex[eff_i]);
//            dataBuffIndex[eff_i] = (int)((framesReadTotal+eff_i-ceil((fabs(dataBuffSin[eff_i]))*delayInFrames)))%maxDelayInFrames;
        }*/

        framesReadTotal += framesRead;
        framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRead*outFileStr.channels);
        sf_write_sync(outFilePtr);
    }

    // [/Lecto-escritura de datos]
    printf("\tTiempo de ejecución:\n");
    printf("\t  Comienzo                          : %lu\n", start);
    printf("\t  Fin                               : %lu\n", end);
    // printf("\t  # iteraciones                     : %d\n", cant_iteraciones);
    printf("\t  # de ciclos insumidos totales     : %lu\n", cantCiclos);
    // printf("\t  # de ciclos insumidos por llamada : %.3f\n", (float)cantCiclos/(float)cant_iteraciones);
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
    printf("\tTiempo de ejecución:\n");
    printf("\t  Comienzo                          : %lu\n", start);
    printf("\t  Fin                               : %lu\n", end);
    // printf("\t  # iteraciones                     : %d\n", cant_iteraciones);
    printf("\t  # de ciclos insumidos totales     : %lu\n", cantCiclos);
    // printf("\t  # de ciclos insumidos por llamada : %.3f\n", (float)cantCiclos/(float)cant_iteraciones);

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
    //printf("start %x end %x delay %f depth %f\n", dataBuffIndex, &dataBuffIndex[maxDelayInFrames], delay, depth);

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
        /*printf("%x.\n", dataBuffIn);
        printf("%x.\n", dataBuffOut);
        printf("%x.\n", dataBuffEffect);
        printf("%x.\n", dataBuffIndex);
        printf("%f.\n", delay);
        printf("%x.\n", &dataBuffEffectHead);
        printf("%x.\n", &dataBuffEffectEnd);
        printf("%d.\n", inFileStr.channels);
        */
        MEDIR_TIEMPO_START(start);
        vibrato_index_calc(dataBuffIndex, framesRead, framesReadTotal, mod, depth);
        MEDIR_TIEMPO_STOP(end);

        /*for (unsigned int eff_i = 0; eff_i < framesRead; eff_i++) {
            printf("%d %f\n", framesReadTotal+eff_i, dataBuffIndex[eff_i]);
        }*/
        //printf("head: %x %d end %x %d \n", &dataBuffEffectHead, dataBuffEffectHead, dataBuffEffectEnd, dataBuffEffectEnd);
        MEDIR_TIEMPO_START(start);
        vibrato_asm(dataBuffIn, dataBuffOut, dataBuffEffect, dataBuffIndex, &dataBuffEffectHead, &maxDelayInFrames, framesRead*inFileStr.channels, inFileStr.channels);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

/*        for (unsigned int eff_i = 0, i = 0; eff_i < framesRead; eff_i++) {
            printf("%d %d %f %f %f\n", eff_i, framesReadTotal+eff_i, dataBuffIndex[eff_i],  dataBuffOut[i++], dataBuffOut[i++]);
        }*/
        framesReadTotal += framesRead;
        framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRead*outFileStr.channels);
        sf_write_sync(outFilePtr);
    }

    // [/Lecto-escritura de datos]
    printf("\tTiempo de ejecución:\n");
    printf("\t  Comienzo                          : %lu\n", start);
    printf("\t  Fin                               : %lu\n", end);
    // printf("\t  # iteraciones                     : %d\n", cant_iteraciones);
    printf("\t  # de ciclos insumidos totales     : %lu\n", cantCiclos);
    // printf("\t  # de ciclos insumidos por llamada : %.3f\n", (float)cantCiclos/(float)cant_iteraciones);

    // [Limpieza]
    free(dataBuffIn);
    free(dataBuffOut);
    free(dataBuffIndex);
    free(dataBuffEffect);
    // [/Limpieza]
}
