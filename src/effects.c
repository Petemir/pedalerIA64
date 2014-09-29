// PedalerIA64 Copyright [2014] <Matias Laporte>
#include "./effects.h"

double maxsamp(double *bufferIn, int bufferLen) {
    double absval, peak = 0.0;

    for (int i = 0; i < bufferSize; i++) {
        absval = fabs(dataBuffIn[i]);
        if (absval > peak) {
            peak = absval;
        }
    }
    return peak;
}

PANPOS simplepan_c(double position) {
    PANPOS pos;

    position *= 0.5; 
    pos.left = position - 0.5;
    pos.right = position + 0.5;

    return pos;    
}

void panning_c(double panpos) {
    unsigned int bufferSizeIn = BUFFERSIZE*inFileStr.channels;
    unsigned int bufferSizeOut = BUFFERSIZE*outFileStr.channels;

    PANPOS thispos = simplepan_c(panpos);

    // TODO -> CHEQUEO DE ERRORES
    dataBuffIn = (double*)malloc(bufferSizeIn*sizeof(double));  // Buffers de entrada y salida
    if (dataBuffIn == NULL) exit (1);
    dataBuffOut = (double*)malloc(bufferSizeOut*sizeof(double));
    if (dataBuffOut == NULL) exit (1);

    // Limpio buffers
    for (unsigned int i = 0; i < bufferSizeIn; i++) {
        dataBuffIn[i] = 0;
    }
    for (unsigned int i = 0; i < bufferSizeOut; i++) {
        dataBuffOut[i] = 0;
    }

    unsigned long int start, end, cantCiclos = 0;
    while ((framesRead = sf_readf_double(inFilePtr, dataBuffIn, bufferSizeIn))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, out_i = 0; i < bufferSizeIn; i++) {
            dataBuffOut[out_i++] = dataBuffIn[i]*thispos.left;
            dataBuffOut[out_i++] = dataBuffIn[i]*thispos.right;
        }
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesWritten = sf_write_double(outFilePtr, dataBuffOut, framesRead*outFileStr.channels);
        sf_write_sync(outFilePtr);
    }
}

void normalization_c(double dbval) {
    double ampfac = (double) pow(10.0, dbval/20.0);
    double inpeak = 0.0;
    double scalefac;

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

    while ((framesRead = sf_readf_double(inFilePtr, dataBuffIn, BUFFERSIZE))) {
        double thispeak = maxsamp(dataBuffIn, bufferSize);
        if (thispeak > inpeak) {
            inpeak = thispeak;
        }
    }

    sf_seek(inFilePtr, 0, SEEK_SET);

    scalefac = (float) (ampfac/inpeak);

    unsigned long int start, end, cantCiclos = 0;
    while ((framesRead = sf_readf_double(inFilePtr, dataBuffIn, BUFFERSIZE))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0; i < bufferSize; i++) {
            dataBuffOut[i] = dataBuffIn[i]*scalefac;
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

void volume_c(double ampfac) {
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
            dataBuffOut[i] = dataBuffIn[i]*ampfac;
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
    unsigned int bufferFrameSizeOut = delayInFrames*2;
    unsigned int bufferSizeOut = bufferFrameSize*bufferFrameSizeOut;

    dataBuffIn = (double*)malloc(bufferSize);
    dataBuffOut = (double*)malloc(bufferSizeOut);
    double *dataBuffEffect = (double*)malloc(bufferSize);
    // Limpio buffers
    for (unsigned int i = 0; i < bufferFrameSize; i++) {
        dataBuffIn[i] = 0;
        dataBuffEffect[i] = 0;
    }
    for (unsigned int i = 0; i < bufferFrameSizeOut; i++) {
        dataBuffOut[i] = 0;
    }

    unsigned long int start, end, cantCiclos = 0;
    // [Lecto-escritura de datos]
    while ((framesRead = sf_readf_double(inFilePtr, dataBuffIn, delayInFrames))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, out_i = 0; i < bufferFrameSize; i++) {
            if (inFileStr.channels == 2)
            {
                dataBuffOut[out_i++] = 0.5 * (dataBuffIn[i] + dataBuffIn[i+1]);
                dataBuffOut[out_i++] = 0.5 * (dataBuffEffect[i] + dataBuffEffect[i+1]);
                i++;
            } else {
                dataBuffOut[out_i++] = dataBuffIn[i];
                dataBuffOut[out_i++] = dataBuffEffect[i];
            }

            /*dataBuffOut[i] = dataBuffIn[i] + dataBuffEffect[i]; */
            dataBuffEffect[i] = dataBuffIn[i] * decay;  // Delay simple
        }
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesWritten = sf_write_double(outFilePtr, dataBuffOut, framesRead*2);
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
    unsigned int bufferSize = bufferFrameSize*sizeof(double);       // Tamaño del buffer en bytes
    unsigned int bufferFrameSizeOut = delayInFrames*2;
    unsigned int bufferSizeOut = bufferFrameSize*bufferFrameSizeOut;

    dataBuffIn = (double*)malloc(bufferSize);
    dataBuffOut = (double*)malloc(bufferSizeOut);
    double *dataBuffEffect = (double*)malloc(bufferSize);
    // Limpio buffers
    for (unsigned int i = 0; i < bufferFrameSize; i++) {
        dataBuffIn[i] = 0;
        dataBuffEffect[i] = 0;
    }
    for (unsigned int i = 0; i < bufferFrameSizeOut; i++) {
        dataBuffOut[i] = 0;
    }

    unsigned long int start, end = 0;
    while ((framesRead = sf_readf_double(inFilePtr, dataBuffIn, delayInFrames))) {
        MEDIR_TIEMPO_START(start);
        delay_asm(dataBuffIn, dataBuffOut, dataBuffEffect, framesRead*inFileStr.channels, &decay, inFileStr.channels);
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
