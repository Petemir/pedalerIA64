// PedalerIA64 Copyright [2014] <Matias Laporte>
#include "./effects.h"

void clean_buffer(float *buffer, int bufferLen) {
    for (int i = 0; i < bufferLen; i++) {
        buffer[i] = 0;
    }
}
/*
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
*/
/*PANPOS simplepan_c(double position) {
    PANPOS pos;

    position *= 0.5; 
    pos.left = position - 0.5;
    pos.right = position + 0.5;

    return pos;    
}*/
/*
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
    clean_buffer(dataBuffIn, bufferSizeIn);
    clean_buffer(dataBuffOut, bufferSizeOut);

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
}*/
/*
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
    clean_buffer(dataBuffIn, bufferSize);
    clean_buffer(dataBuffOut, bufferSize);

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
}*/

/*void volume_c(double ampfac) {
    bufferSize = BUFFERSIZE*inFileStr.channels;

    // TODO -> CHEQUEO DE ERRORES
    dataBuffIn = (double*)malloc(bufferSize*sizeof(double));  // Buffers de entrada y salida
    if (dataBuffIn == NULL) exit (1);
    dataBuffOut = (double*)malloc(bufferSize*sizeof(double));
    if (dataBuffOut == NULL) exit (1);

    // Limpio buffers
    clean_buffer(dataBuffIn, bufferSize);
    clean_buffer(dataBuffOut, bufferSize);

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
}*/

/*void copy_c() {
    bufferSize = BUFFERSIZE*inFileStr.channels;

    // TODO -> CHEQUEO DE ERRORES
    dataBuffIn = (float*)malloc(bufferSize*sizeof(float));  // Buffers de entrada y salida
    if (dataBuffIn == NULL) exit (1);
    dataBuffOut = (float*)malloc(bufferSize*sizeof(float));
    if (dataBuffOut == NULL) exit (1);

    // Limpio buffers
    clean_buffer(dataBuffIn, bufferSize);
    clean_buffer(dataBuffOut, bufferSize);

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
}*/

/*void copy_asm_caller() {
    bufferSize = BUFFERSIZE*inFileStr.channels;

    dataBuffIn = (double*)malloc(bufferSize*sizeof(double));   // Buffers de entrada y salida
    dataBuffOut = (double*)malloc(bufferSize*sizeof(double));

    // Limpio buffers
    clean_buffer(dataBuffIn, bufferSize);
    clean_buffer(dataBuffOut, bufferSize);

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

void delay_c(float delayInSec, float decay) {
    unsigned int delayInFrames = ceil(delayInSec*inFileStr.samplerate);   // Frames de delay
    unsigned int bufferFrameSize = delayInFrames*inFileStr.channels;  // Tamaño del buffer de entrada y efecto en frames
    unsigned int bufferSize = bufferFrameSize*sizeof(float);          // Tamaño del buffer anterior en bytes
    unsigned int bufferFrameSizeOut = delayInFrames*outFileStr.channels;  // Tamaño del buffer de salida en frames
    unsigned int bufferSizeOut = bufferFrameSizeOut*sizeof(float);    // Tamaño del buffer anterior en bytes

    dataBuffIn = (float*)malloc(bufferSize);
    dataBuffOut = (float*)malloc(bufferSizeOut);
    float *dataBuffEffect = (float*)malloc(bufferSize);
    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffEffect, bufferFrameSize);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);

    start = end = cantCiclos = 0;
    // [Lecto-escritura de datos]
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, delayInFrames))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, out_i = 0; i < bufferFrameSize; i++) {
            // Original por el canal izquierdo, efecto por el derecho;
            if (inFileStr.channels == 2) {
                dataBuffOut[out_i++] = 0.5*dataBuffIn[i] + 0.5*dataBuffIn[i+1];
                dataBuffOut[out_i++] = 0.5 * dataBuffEffect[i] + 0.5 * dataBuffEffect[i+1];

                dataBuffEffect[i] = dataBuffIn[i] * decay;
                dataBuffEffect[i+1] = dataBuffIn[i+1] * decay;

                i++;    // Avanzo de a dos en dataBuffIn
            } else {
                dataBuffOut[out_i++] = dataBuffIn[i];
                dataBuffOut[out_i++] = dataBuffEffect[i];

                dataBuffEffect[i] = dataBuffIn[i] * decay;  // Delay simple
            }
        }
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

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
    // [/Limpieza]
}

void flanger_c(float delayInMsec, float rate, float amp) {
    unsigned int delayInFrames = floor(delayInMsec*inFileStr.samplerate);
    unsigned int bufferFrameSize = delayInFrames*inFileStr.channels;
    unsigned int bufferSize = bufferFrameSize*sizeof(float);
    unsigned int bufferFrameSizeOut = delayInFrames*outFileStr.channels;
    unsigned int bufferSizeOut = bufferFrameSizeOut*sizeof(float);

    dataBuffIn = (float*)malloc(bufferSize);
    dataBuffOut = (float*)malloc(bufferSizeOut);
    float *dataBuffEffect = (float*)malloc(bufferSizeOut);
    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffEffect, bufferFrameSizeOut/2);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);

    start = end = cantCiclos = 0;
    framesReadTotal = 0;
    printf("Delay in frames: %d.\n", delayInFrames);
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, delayInFrames))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, eff_i = 0, out_i = 0; i < bufferFrameSize; i++) {
            float current_sin = sinf(2*M_PI*(framesReadTotal+i)*(rate/inFileStr.samplerate));
            unsigned int current_delay = ceil(current_sin*delayInFrames);

            if (inFileStr.channels == 2) {
                float next_sin = sinf(2*M_PI*(framesReadTotal+i+1)*(rate/inFileStr.samplerate));
                unsigned int next_delay = ceil(next_sin*delayInFrames);

                
                dataBuffOut[out_i++] = 0.5*dataBuffIn[i] + 0.5*dataBuffIn[i+1];
                dataBuffOut[out_i++] = 0.5 * (amp*dataBuffIn[i] + amp*dataBuffEffect[(eff_i-current_delay)%bufferFrameSize]) + 0.5 * (amp*dataBuffIn[i+1] + amp*dataBuffEffect[(eff_i+1-next_delay)%bufferFrameSize]);

                dataBuffEffect[eff_i++] = 0.5*(dataBuffIn[i]+dataBuffIn[i+1]);

                i++;    // Avanzo de a dos en dataBuffIn
            } else {
                dataBuffOut[out_i++] = dataBuffIn[i];   // Sonido original
                dataBuffOut[out_i++] = amp * dataBuffIn[i] + amp * dataBuffEffect[(i-current_delay)%bufferFrameSize];  // Efecto

                dataBuffEffect[eff_i++] = dataBuffIn[i];
            }
        }
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;
        
        framesReadTotal += framesRead;
        printf("%d\n", framesReadTotal);
   
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
    // [/Limpieza]
}


void delay_asm_caller(float delayInSec, float decay) {
    unsigned int delayInFrames = ceil(delayInSec*inFileStr.samplerate);  // Frames de delay
    unsigned int bufferFrameSize = delayInFrames*inFileStr.channels;  // Tamaño del buffer en frames
    unsigned int bufferSize = bufferFrameSize*sizeof(float);       // Tamaño del buffer en bytes
    unsigned int bufferFrameSizeOut = delayInFrames*outFileStr.channels;
    unsigned int bufferSizeOut = bufferFrameSizeOut *sizeof(float);

    dataBuffIn = (float*)malloc(bufferSize);
    dataBuffOut = (float*)malloc(bufferSizeOut);
    float *dataBuffEffect = (float*)malloc(bufferSize);
    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffEffect, bufferFrameSize);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);

    start = end = cantCiclos = 0;
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, delayInFrames))) {
        MEDIR_TIEMPO_START(start);
        delay_asm(dataBuffIn, dataBuffOut, dataBuffEffect, framesRead*inFileStr.channels, &decay, inFileStr.channels);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRead*outFileStr.channels);
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