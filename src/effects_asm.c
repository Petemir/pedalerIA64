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
        for (unsigned int eff_i = 0; eff_i < fmin(framesRead, maxDelayInFrames); eff_i = eff_i) {
            v4sf index_vector;
            // Número que se utiliza para calcular el índice que se usará para el efecto
            for (unsigned int j = 0; j < 4; j++) {
                index_vector[j] = 2*M_PI*((framesReadTotal+1)+eff_i)*(rate/inFileStr.samplerate);
                eff_i++;
            }

            MEDIR_TIEMPO_START(start);
            index_vector = sin_ps(index_vector);
            // index_vector = Sin(index_vector);  // TODO -> REMOVE
            index_vector = _mm_and_ps(index_vector, *(v4sf*)_ps_inv_sign_mask);
            MEDIR_TIEMPO_STOP(end);
            cantCiclos += end-start;

            // Guardo los índices en el buffer que se le pasará a la rutina en ASM
            eff_i-=4;
            for (unsigned int j = 0; j <= 3 && eff_i < framesRead; j++) {
                dataBuffIndex[eff_i] = (int) ((framesReadTotal+(eff_i)-ceil(index_vector[j]*delayInFrames)))%maxDelayInFrames;
                eff_i++;
            }

        }

        MEDIR_TIEMPO_START(start);
        flanger_asm(dataBuffIn, dataBuffOut, dataBuffEffect, dataBuffIndex, framesRead*inFileStr.channels, inFileStr.channels, amp);
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
        bitcrusher_asm(dataBuffIn, dataBuffOut, framesRead*inFileStr.channels, &step, &normFreq, &phasor, &last, inFileStr.channels);
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
