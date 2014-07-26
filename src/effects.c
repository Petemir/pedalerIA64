// PedalerIA64 Copyright [2014] <Matias Laporte>
#include "./effects.h"

void copy_c() {
    bufferSize = BUFFERSIZE*inFileStr.channels;

    // TODO -> CHEQUEO DE ERRORES
    dataBuffIn = (double*)malloc(bufferSize*sizeof(double));  // Buffers de entrada y salida
    if (dataBuffIn == NULL) exit (1);
    dataBuffOut = (double*)malloc(bufferSize*sizeof(double));
    if (dataBuffOut == NULL) exit (1);

    // Limpio buffers
    for (unsigned int i = 0; i < bufferSize; i++) {
        dataBuffIn[i] = 0;
        dataBuffOut[i] = 0;
    }

    unsigned long int start, end, cantCiclos = 0;
    while ((framesRead = sf_readf_double(inFilePtr, dataBuffIn, BUFFERSIZE))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0; i < bufferSize; i++) {
            dataBuffOut[i] = dataBuffIn[i];
        }
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesWritten = sf_write_double(outFilePtr, dataBuffOut, framesRead*inFileStr.channels);
        sf_write_sync(outFilePtr);
    }
 
    printf("\tTiempo de ejecución:\n");
    printf("\t  Comienzo                          : %lu\n", start);
    printf("\t  Fin                               : %lu\n", end);
    // printf("\t  # iteraciones                     : %d\n", cant_iteraciones);
    printf("\t  # de ciclos insumidos totales     : %lu\n", cantCiclos);
    // printf("\t  # de ciclos insumidos por llamada : %.3f\n", (float)cantCiclos/(float)cant_iteraciones);
 
    free(dataBuffIn);
    free(dataBuffOut);
}

void copy_asm_caller() {
    bufferSize = BUFFERSIZE*inFileStr.channels;

    dataBuffIn = (double*)malloc(bufferSize*sizeof(double));   // Buffers de entrada y salida
    dataBuffOut = (double*)malloc(bufferSize*sizeof(double));

    // Limpio buffers
    for (unsigned int i = 0; i < bufferSize; i++) {
        dataBuffIn[i] = 0;
        dataBuffOut[i] = 0;
    }

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
}

void delay_c(double delayInSec, double decay) {
    unsigned int delayInFrames = ceil(delayInSec*inFileStr.samplerate);  // Frames de delay
    unsigned int bufferFrameSize = delayInFrames*inFileStr.channels;  // Tamaño del buffer en frames
    unsigned int bufferSize = bufferFrameSize*sizeof(double);       // Tamaño del buffer en bytes
    
    dataBuffIn = (double*)malloc(bufferSize);
    dataBuffOut = (double*)malloc(bufferSize);
    double *dataBuffEffect = (double*)malloc(bufferSize);
    // Limpio buffers
    for (unsigned int i = 0; i < bufferFrameSize; i++) {
        dataBuffIn[i] = 0;
        dataBuffOut[i] = 0;
        dataBuffEffect[i] = 0;
    }

    unsigned long int start, end, cantCiclos = 0;
    // [Lecto-escritura de datos]
    while ((framesRead = sf_readf_double(inFilePtr, dataBuffIn, delayInFrames))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0; i < bufferFrameSize; i++) {
            dataBuffOut[i] = dataBuffIn[i] + dataBuffEffect[i];
            dataBuffEffect[i] = dataBuffIn[i] * decay;  // Delay simple
        }
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesWritten = sf_write_double(outFilePtr, dataBuffOut, framesRead*inFileStr.channels);
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
    // [/Limpieza]
}

void delay_asm_caller(double delayInSec, double decay) {
    unsigned int delayInFrames = ceil(delayInSec*inFileStr.samplerate);  // Frames de delay
    unsigned int bufferFrameSize = delayInFrames*inFileStr.channels;  // Tamaño del buffer en frames
    unsigned int bufferSize = bufferFrameSize*sizeof(double);         // Tamaño del buffer en bytes

    dataBuffIn = (double*)malloc(bufferSize);
    dataBuffOut = (double*)malloc(bufferSize);
    double *dataBuffEffect = (double*)malloc(bufferSize);
    // Limpio buffers
    for (unsigned int i = 0; i < bufferFrameSize; i++) {
        dataBuffIn[i] = 0;
        dataBuffOut[i] = 0;
        dataBuffEffect[i] = 0;
    }

    unsigned long int start, end = 0;
    while ((framesRead = sf_readf_double(inFilePtr, dataBuffIn, delayInFrames))) {
        MEDIR_TIEMPO_START(start);
        delay_asm(dataBuffIn, dataBuffOut, dataBuffEffect, framesRead*inFileStr.channels, &decay);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos+= end-start;

        framesWritten = sf_write_double(outFilePtr, dataBuffOut, framesRead*inFileStr.channels);
        sf_write_sync(outFilePtr);
    }

    printf("\tTiempo de ejecución:\n");
    printf("\t  Comienzo                          : %lu\n", start);
    printf("\t  Fin                               : %lu\n", end);
    // printf("\t  # iteraciones                     : %d\n", cant_iteraciones);
    printf("\t  # de ciclos insumidos totales     : %lu\n", cantCiclos);
    // printf("\t  # de ciclos insumidos por llamada : %.3f\n", (float)cantCiclos/(float)cant_iteraciones);

    free(dataBuffIn);
    free(dataBuffOut);
    free(dataBuffEffect);
}