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
    float *dataBuffEffect = (float*)malloc(bufferSizeOut/2);  // El buffer de efecto sólo va a contener el canal derecho de la salida

    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffEffect, bufferFrameSizeOut/2);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);

    start = end = cantCiclos = 0;
    // [Lecto-escritura de datos]
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, delayInFrames))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, eff_i = 0, out_i = 0; i < bufferFrameSize; i++) {
            dataBuffOut[out_i+1] = dataBuffEffect[eff_i]*decay;  // En el canal derecho, va el efecto (entrada ciclo anterior*decay)

            // Stereo: promedio de los dos canales; Mono: entrada normal
            if (inFileStr.channels == 2) {
                dataBuffEffect[eff_i] = 0.5*dataBuffIn[i] + 0.5*dataBuffIn[i+1];
                i++;    // Avanzo de a dos en dataBuffIn
            } else {
                dataBuffEffect[eff_i] = dataBuffIn[i];
            }

            dataBuffOut[out_i++] = dataBuffEffect[eff_i];  // En el canal izquierdo, va la entrada sin efecto (entrada ciclo actual)

            out_i++;
            eff_i++;
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
    unsigned int maxDelayInFrames = (int)fmax((float)(BUFFERSIZE-(BUFFERSIZE%delayInFrames)), (float)delayInFrames);
    unsigned int bufferFrameSize = maxDelayInFrames*inFileStr.channels;
    unsigned int bufferSize = bufferFrameSize*sizeof(float);
    unsigned int bufferFrameSizeOut = maxDelayInFrames*outFileStr.channels;
    unsigned int bufferSizeOut = bufferFrameSizeOut*sizeof(float);

    dataBuffIn = (float*)malloc(bufferSize);
    dataBuffOut = (float*)malloc(bufferSizeOut);
    float *dataBuffEffect = (float*)malloc(maxDelayInFrames*sizeof(float)); 

    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffEffect, maxDelayInFrames);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);

    start = end = cantCiclos = 0;
    framesReadTotal = 0;
    // [Lecto-escritura de datos
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, maxDelayInFrames))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, eff_i = 0, out_i = 0; i < bufferFrameSize; i++) {
            float current_sin = fabs(sinf(2*M_PI*((framesReadTotal+1)+eff_i)*(rate/inFileStr.samplerate)));
            unsigned int current_delay = ceil(current_sin*delayInFrames);
            int eff_index = (((framesReadTotal)+eff_i)-current_delay);  // Indice del efecto

            if (inFileStr.channels == 2) {
                dataBuffEffect[eff_i] = 0.5*dataBuffIn[i] + 0.5*dataBuffIn[i+1];
                i++;    // Avanzo de a dos en dataBuffIn
            } else {
                dataBuffEffect[eff_i] = dataBuffIn[i];
            }

            // printf("fr+eff_i: %d, eff_i: %d, index: %d, dataBuffIn: %.12f, dataBuffEffect: %.12f, dataBuffOut: %.12f\n", framesReadTotal+eff_i, eff_i, eff_index+1, dataBuffEffect[i], dataBuffEffect[eff_index%maxDelayInFrames], (dataBuffEffect[eff_i]*amp + amp*(eff_index < 0 ? 0:dataBuffEffect[eff_index%maxDelayInFrames])));
            dataBuffOut[out_i++]  = dataBuffEffect[eff_i];  // Sonido seco en mono, promedio de los canales en stereo
            dataBuffOut[out_i++]  = dataBuffEffect[eff_i]*amp + amp*(eff_index < 0 ? 0:dataBuffEffect[eff_index%maxDelayInFrames]);  // Audio con efecto - si el índice es negativo, pongo un 0

            eff_i++;
        }
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
    // [/Limpieza]
}

void vibrato_c(float depth, float mod) {
    float delay = depth;
    delay = round(delay*inFileStr.samplerate);
    depth = round(depth*inFileStr.samplerate);

    unsigned int delayInFrames = floor(2+delay+depth*2);
    unsigned int maxDelayInFrames = (int)fmax((float)(BUFFERSIZE-(BUFFERSIZE%delayInFrames)), (float)delayInFrames);
    unsigned int bufferFrameSize = maxDelayInFrames*inFileStr.channels;
    unsigned int bufferSize = bufferFrameSize*sizeof(float);
    unsigned int bufferFrameSizeOut = maxDelayInFrames*outFileStr.channels;
    unsigned int bufferSizeOut = bufferFrameSizeOut*sizeof(float);

    dataBuffIn = (float*)malloc(bufferSize);
    dataBuffOut = (float*)malloc(bufferSizeOut);

    // Bufer circular
    float *dataBuffEffect = (float*)malloc(maxDelayInFrames*sizeof(float));

    int dataBuffEffectEnd, dataBuffEffectHead, dataBuffEffectTail;
    dataBuffEffectEnd = maxDelayInFrames;
    dataBuffEffectHead = dataBuffEffectTail = 0;

    mod = mod/inFileStr.samplerate;
    printf("%d\n", maxDelayInFrames);
    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffEffect, maxDelayInFrames);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);

    // TODO -> DESFAZADO POR maxDelayInFrames, ¿por que?
    start = end = cantCiclos = 0;
    framesReadTotal = 0;
    // [Lecto-escritura de datos
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, maxDelayInFrames))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, eff_i = 0, out_i = 0; i < bufferFrameSize; i++) {
            float current_mod = sinf(mod*2*M_PI*(i+1));
            float tap = 1+delay+depth*current_mod;
            int n = floor(tap);
            float frac = tap - n;

            printf("mod: %f\n", mod);
            printf("current_mod: %f\n", current_mod);
            printf("tap: %f\n", tap);
            printf("n: %d\n", n);
            printf("frac: %f\n", frac);

            if (inFileStr.channels == 2) {
                dataBuffEffect[eff_i] = 0.5*dataBuffIn[i] + 0.5*dataBuffIn[i+1];
                i++;    // Avanzo de a dos en dataBuffIn
            } else {
                dataBuffEffect[dataBuffEffectHead] = dataBuffIn[i];
                // head += sizeof(float);
                dataBuffEffectHead++;
                if (dataBuffEffectHead == dataBuffEffectEnd) { dataBuffEffectHead = 0; }
            }

            // if (dataBuffEffectHead+n >= dataBuffEffectEnd) { new_n = (dataBuffEffectHead+n)}

            // printf("fr+eff_i: %d, eff_i: %d, index: %d, dataBuffIn: %.12f, dataBuffEffect: %.12f, dataBuffOut: %.12f\n", framesReadTotal+eff_i, eff_i, eff_index+1, dataBuffEffect[i], dataBuffEffect[eff_index%maxDelayInFrames], (dataBuffEffect[eff_i]*amp + amp*(eff_index < 0 ? 0:dataBuffEffect[eff_index%maxDelayInFrames])));
            dataBuffOut[out_i++]  = dataBuffIn[i];  // Sonido seco en mono, promedio de los canales en stereo
            dataBuffOut[out_i++]  = dataBuffEffect[(dataBuffEffectHead+n+1)%dataBuffEffectEnd]*frac+dataBuffEffect[(dataBuffEffectHead+n)%dataBuffEffectEnd]*(1-frac);  // dataBuffEffect[eff_i]*amp + amp*(eff_index < 0 ? 0:dataBuffEffect[eff_index%maxDelayInFrames]);  // Audio con efecto - si el índice es negativo, pongo un 0

            // eff_i++;
        }
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

    // [Limpieza]
    free(dataBuffIn);
    free(dataBuffOut);
    free(dataBuffEffect);
    // [/Limpieza]
}

void flanger_asm_caller(float delayInMsec, float rate, float amp) {
    unsigned int delayInFrames = floor(delayInMsec*inFileStr.samplerate);
    unsigned int maxDelayInFrames = (int)fmax((float)(BUFFERSIZE-(BUFFERSIZE%delayInFrames)), (float)delayInFrames);
    unsigned int bufferFrameSize = maxDelayInFrames*inFileStr.channels;
    unsigned int bufferSize = bufferFrameSize*sizeof(float);
    unsigned int bufferFrameSizeOut = maxDelayInFrames*outFileStr.channels;
    unsigned int bufferSizeOut = bufferFrameSizeOut*sizeof(float);

    dataBuffIn = (float*)malloc(bufferSize);
    dataBuffOut = (float*)malloc(bufferSizeOut);
    float *dataBuffEffect = (float*)malloc(maxDelayInFrames*sizeof(float));  // El buffer de efecto sólo va a contener el canal derecho de la salida
    unsigned int *dataBuffIndex = (unsigned int*)malloc(maxDelayInFrames*sizeof(unsigned int));
    // unsigned int *dataBuffIndex = (unsigned int*)_mm_malloc(maxDelayInFrames*sizeof(unsigned int),16);
    // printf("dbIn: %d, dbOut: %d, dbEffect: %d, dbIndex: %d.\n", dataBuffIn, dataBuffOut, dataBuffEffect, dataBuffIndex);
    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);
    clean_buffer(dataBuffEffect, maxDelayInFrames);
    clean_buffer((float*)dataBuffIndex, maxDelayInFrames);

    start = end = cantCiclos = 0;
    framesReadTotal = 0;
    printf("Length dbi: %d.\n", bufferFrameSize);
    printf("Length dbo: %d.\n", bufferFrameSizeOut);
    printf("Length dbe: %d.\n", maxDelayInFrames);
    printf("Length dbix: %d.\n", maxDelayInFrames);
    // [Lecto-escritura de datos
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
            // _mm_store_ps((float*)&(dataBuffIndex[eff_i-4]), sin_ps(index_vector));
            MEDIR_TIEMPO_STOP(end);
            MEDIR_TIEMPO_START(start);
            index_vector = _mm_and_ps(index_vector, *(v4sf*)_ps_inv_sign_mask);
            // _mm_store_ps((float*)&(dataBuffIndex[eff_i-4]), _mm_and_ps((v4sf)&(dataBuffIndex[eff_i-4]), *(v4sf*)_ps_inv_sign_mask));
            MEDIR_TIEMPO_STOP(end);

            // Guardo los índices en el buffer que se le pasará a la rutina en ASM
            eff_i-=4;
            for (unsigned int j = 0; j <= 3 && eff_i < framesRead; j++) {
                // printf("%d %d\n", eff_i-j, (int) ((framesReadTotal+(eff_i-j)-ceil(index_vector[3-j]*delayInFrames)))%maxDelayInFrames);
                dataBuffIndex[eff_i] = (int) ((framesReadTotal+(eff_i)-ceil(index_vector[j]*delayInFrames)))%maxDelayInFrames;
                eff_i++;
            }

            cantCiclos += end-start;
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
// printf("dbIn: %d, dbOut: %d, dbEffect: %d, dbIndex: %d.\n", dataBuffIn, dataBuffOut, dataBuffEffect, dataBuffIndex);
    // [Limpieza]
    free(dataBuffIn);
    free(dataBuffOut);
    free(dataBuffEffect);
    free(dataBuffIndex);
    // [/Limpieza]
}
