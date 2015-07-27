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

float maxsamp_right_c() {
    unsigned int bufferFrameSize = BUFFERSIZE*outFileStr.channels;

    float maxSamp = 0;
    while ((framesRead = sf_readf_float(outFilePtr, dataBuffIn, BUFFERSIZE))) {
        for (unsigned int i = 0; i < bufferFrameSize; i++) {
            i++;  // Avanzo de a dos porque no me interesa el canal izquierdo, dry sound
            if (fabs(dataBuffIn[i]) > maxSamp) { maxSamp = fabs(dataBuffIn[i]);}
        }
    }
    sf_seek(outFilePtr, 0, SEEK_SET);
    return maxSamp;
}

float maxsamp_right_asm_caller() {
    float maxSamp = 0;
    while ((framesRead = sf_readf_float(outFilePtr, dataBuffIn, BUFFERSIZE))) {
        maxsamp_right_asm(dataBuffIn, &maxSamp, framesRead*outFileStr.channels);
    }
    sf_seek(outFilePtr, 0, SEEK_SET);
    return maxSamp;
}

void normalization_right_c() {
    // Siempre que normalice, la entrada va a ser stereo (porque va a ser la salida de un efecto anterior), así que usan buffer de mismo tamaño
    unsigned int bufferFrameSize = BUFFERSIZE*outFileStr.channels;
    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSize*sizeof(float));

    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffOut, bufferFrameSize);

    float maxSamp = maxsamp_right_c();

    start = end = cantCiclos = framesReadTotal = 0;
    while ((framesRead = sf_readf_float(outFilePtr, dataBuffIn, BUFFERSIZE))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, out_i = 0; i < bufferFrameSize; i++) {
            dataBuffOut[out_i++] = dataBuffIn[i++];     // El canal izquierdo va igual (audio original nunca satura)
            dataBuffOut[out_i++] = (float) dataBuffIn[i]/maxSamp;
        }
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRead*outFileStr.channels);
        sf_write_sync(outFilePtr);
    }

    free(dataBuffIn);
    free(dataBuffOut);
}

void normalization_right_asm_caller() {
    // Siempre que normalice, la entrada va a ser stereo (porque va a ser la salida de un efecto anterior), así que usan buffer de mismo tamaño
    unsigned int bufferFrameSize = BUFFERSIZE*outFileStr.channels;
    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSize*sizeof(float));

    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffOut, bufferFrameSize);

    float maxSamp = maxsamp_right_asm_caller();

    start = end = cantCiclos = framesReadTotal = 0;
    while ((framesRead = sf_readf_float(outFilePtr, dataBuffIn, BUFFERSIZE))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, out_i = 0; i < bufferFrameSize; i++) {
            dataBuffOut[out_i++] = dataBuffIn[i++];     // El canal izquierdo va igual (audio original nunca satura)
            dataBuffOut[out_i++] = (float) dataBuffIn[i]/maxSamp;

        }
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRead*outFileStr.channels);
        sf_write_sync(outFilePtr);
    }

    free(dataBuffIn);
    free(dataBuffOut);
}


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

void delay_simple_c(float delayInSec, float decay) {
    unsigned int delayInFrames = ceil(delayInSec*inFileStr.samplerate);      // Frames de delay
    unsigned int maxDelayInFrames = (int)fmax((float)(BUFFERSIZE-(BUFFERSIZE%delayInFrames)), (float)delayInFrames);
    unsigned int bufferFrameSize = maxDelayInFrames*inFileStr.channels;      // Tamaño buffer entrada en frames
    unsigned int bufferFrameSizeOut = maxDelayInFrames*outFileStr.channels;  // Tamaño buffer salida en frames

    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));             // Buffer entrada en bytes
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));         // Buffer salida en bytes
    float *dataBuffEffect = (float*)malloc(maxDelayInFrames*sizeof(float));     // Buffer efecto

    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);
    clean_buffer(dataBuffEffect, maxDelayInFrames);

    start = end = cantCiclos = 0;
    // [Lecto-escritura de datos]
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, maxDelayInFrames))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, eff_i = 0, out_i = 0; i < bufferFrameSize; i++) {
            dataBuffOut[out_i+1] = dataBuffEffect[eff_i]*decay;  // En el canal derecho, va el efecto (entrada ciclo anterior*decay)

            if (i < framesRead*inFileStr.channels) {
                // Stereo: promedio de los dos canales; Mono: entrada normal
                if (inFileStr.channels == 2) {
                    dataBuffEffect[eff_i] = 0.5*dataBuffIn[i] + 0.5*dataBuffIn[i+1];
                    i++;  // Si es stereo, dataBuffIn avanza dos
                } else {
                    dataBuffEffect[eff_i] = dataBuffIn[i];
                }
            } else {  // En la última parte del último ciclo (framesRead < maxDelayInFrames), la entrada es nula
                dataBuffEffect[eff_i] = 0;
            }
            dataBuffOut[out_i++] = dataBuffEffect[eff_i];  // En el canal izquierdo, va la entrada sin efecto (entrada ciclo actual)

            out_i++;  // dataBuffOut avanza de a dos
            eff_i++;  // dataBuffEffect avanza de a uno
        }
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesWritten = sf_write_float(outFilePtr, dataBuffOut, bufferFrameSizeOut);
        sf_write_sync(outFilePtr);
        framesRemaining = framesRead;
    }

    // Completo con los frames de efecto que quedaron cuando framesRead < bufferFrameSize
    if (framesRemaining > 0) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int eff_i = 0, out_i = 0; eff_i < framesRemaining; eff_i++) {
            dataBuffOut[out_i++] = 0;   // Canal izquierdo (entrada sin efecto) sin dato
            dataBuffOut[out_i++] = dataBuffEffect[eff_i]*decay;
        }
        MEDIR_TIEMPO_STOP(end);

        framesWritten = sf_write_float(outFilePtr, dataBuffOut, framesRemaining*outFileStr.channels);
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
    unsigned int bufferFrameSizeOut = maxDelayInFrames*outFileStr.channels;

    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));
    float *dataBuffEffect = (float*)malloc(maxDelayInFrames*sizeof(float));

    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);
    clean_buffer(dataBuffEffect, maxDelayInFrames);

    start = end = cantCiclos = 0;
    framesReadTotal = 0;
    // [Lecto-escritura de datos]
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

    // Buffer circular
    float *dataBuffEffect = (float*)malloc(maxDelayInFrames*sizeof(float));

    int dataBuffEffectEnd, dataBuffEffectHead;
    dataBuffEffectEnd = dataBuffEffectHead = maxDelayInFrames;

    mod = mod/inFileStr.samplerate;

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
            float current_mod = sinf(mod*2*M_PI*(framesReadTotal+eff_i+1));
            eff_i++;
            float tap = 1+delay+depth*current_mod;
            int n = floor(tap);
            float frac = tap - n;

            if (inFileStr.channels == 2) {
                dataBuffEffect[dataBuffEffectHead] = 0.5*dataBuffIn[i] + 0.5*dataBuffIn[i+1];
                i++;    // Avanzo de a dos en dataBuffIn
            } else {
                dataBuffEffect[dataBuffEffectHead] = dataBuffIn[i];
            }

            dataBuffOut[out_i++]  = dataBuffEffect[dataBuffEffectHead];  // Sonido seco en mono, promedio de los canales en stereo
            dataBuffEffectHead--;
            dataBuffOut[out_i++]  = dataBuffEffect[((dataBuffEffectHead-1)+n+1) % dataBuffEffectEnd+1]*frac+dataBuffEffect[((dataBuffEffectHead-1)+n) % dataBuffEffectEnd+1]*(1-frac);

            if (dataBuffEffectHead == 0) { dataBuffEffectHead = dataBuffEffectEnd; }
        }
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        // printf("framesReadTotal: %d \n", framesReadTotal);
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

void bitcrusher_c(int bits, int freq) {
    unsigned int bufferFrameSize = BUFFERSIZE*inFileStr.channels;
    unsigned int bufferFrameSizeOut = BUFFERSIZE*outFileStr.channels;

    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));
    float *dataBuffEffect = (float*)malloc(BUFFERSIZE*sizeof(float));

    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffEffect, BUFFERSIZE);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);

    float step = pow(0.5, bits);
    float last = 0, phasor = 0;
    float normFreq = (float)freq/inFileStr.samplerate;
    // printf("step: %f.\n", step);
    // printf("normFreq: %f.\n", normFreq);

    start = end = cantCiclos = framesReadTotal = 0;
    // [Lecto-escritura de datos]
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, BUFFERSIZE))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, eff_i = 0, out_i = 0; i < bufferFrameSize; i++) {
            if (inFileStr.channels == 2) {
                dataBuffEffect[eff_i] = 0.5*dataBuffIn[i] + 0.5*dataBuffIn[i+1];
                i++;    // Avanzo de a dos en dataBuffIn
            } else {
                dataBuffEffect[eff_i] = dataBuffIn[i];
            }

            phasor += normFreq;

            if (phasor >= 1.0) {
               phasor -= 1.0;
               last = step * floor(dataBuffEffect[eff_i]/step + 0.5);
            }
            // printf("%d %f %f.\n", framesReadTotal+eff_i, dataBuffEffect[eff_i], step);

            dataBuffOut[out_i++] = dataBuffEffect[eff_i];  // Sonido seco en mono, promedio de los canales en stereo
            dataBuffOut[out_i++] = last;  // Audio con efecto

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

void wah_wah_c(float damp, int minf, int maxf, int wahfreq) {
    unsigned int bufferFrameSize = BUFFERSIZE*inFileStr.channels;
    unsigned int bufferFrameSizeOut = BUFFERSIZE*outFileStr.channels;

    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));
    float *dataBuffEffect = (float*)malloc(bufferFrameSize*sizeof(float));

    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffEffect, bufferFrameSize);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);

    // Generador de onda triangular.
    float delta = (float)wahfreq/inFileStr.samplerate;
    int triangleWaveSize = floor((maxf-minf)/delta)+1;

    float fc = 2*sin((float)(M_PI*minf)/inFileStr.samplerate);
    float q1 = 2*damp;
    float yh = 0, yb = 0, yl = 0;

    start = end = cantCiclos = framesReadTotal = 0;
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, BUFFERSIZE))) {
        MEDIR_TIEMPO_START(start);
        for (unsigned int i = 0, eff_i = 0, out_i = 0; i < bufferFrameSize; i++) {
            if (inFileStr.channels == 2) {
                dataBuffEffect[eff_i] = 0.5*dataBuffIn[i] + 0.5*dataBuffIn[i+1];
                i++;
            } else {
                dataBuffEffect[eff_i] = dataBuffIn[i];
            }

            yh = dataBuffEffect[eff_i] - yl - q1 * yb;
            yb = fc * yh + yb;
            yl = fc * yb + yl;

            int evenCycle = ((((framesReadTotal+eff_i)/triangleWaveSize)%2) == 0);              // Ciclo positivo o negativo
            int thisCycle = (framesReadTotal+eff_i) % (triangleWaveSize + 1);                   // A qué punto del ciclo correspondería
            fc = evenCycle*(minf+(thisCycle-1)*delta)+(1-evenCycle)*(maxf-(thisCycle)*delta);   // Valor del punto
            fc = 2*sin((float)(M_PI*fc)/inFileStr.samplerate);

            dataBuffOut[out_i++] = dataBuffIn[i];
            dataBuffOut[out_i++] = 0.1*yb;

            eff_i++;
        }
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesReadTotal += framesRead;
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

void delay_simple_asm_caller(float delayInSec, float decay) {
    unsigned int delayInFrames = ceil(delayInSec*inFileStr.samplerate);
    unsigned int maxDelayInFrames = (int)fmax((float)(BUFFERSIZE-(BUFFERSIZE%delayInFrames)), (float)delayInFrames);
    unsigned int bufferFrameSize = maxDelayInFrames*inFileStr.channels;
    unsigned int bufferFrameSizeOut = maxDelayInFrames*outFileStr.channels;

    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));
    float *dataBuffEffect = (float*)malloc(bufferFrameSize*sizeof(float));

    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffEffect, bufferFrameSize);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);

    start = end = cantCiclos = 0;
    while ((framesRead = sf_readf_float(inFilePtr, dataBuffIn, maxDelayInFrames))) {
        MEDIR_TIEMPO_START(start);
        delay_simple_asm(dataBuffIn, dataBuffOut, dataBuffEffect, bufferFrameSize, &decay, inFileStr.channels);
        MEDIR_TIEMPO_STOP(end);
        cantCiclos += end-start;

        framesWritten = sf_write_float(outFilePtr, dataBuffOut, bufferFrameSizeOut);
        sf_write_sync(outFilePtr);

        clean_buffer(dataBuffIn, bufferFrameSize);
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

void flanger_asm_caller(float delayInMsec, float rate, float amp) {
    unsigned int delayInFrames = floor(delayInMsec*inFileStr.samplerate);
    unsigned int maxDelayInFrames = (int)fmax((float)(BUFFERSIZE-(BUFFERSIZE%delayInFrames)), (float)delayInFrames);
    unsigned int bufferFrameSize = maxDelayInFrames*inFileStr.channels;
    unsigned int bufferFrameSizeOut = maxDelayInFrames*outFileStr.channels;

    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));
    float *dataBuffEffect = (float*)malloc(maxDelayInFrames*sizeof(float));  // El buffer de efecto sólo va a contener el canal derecho de la salida
    unsigned int *dataBuffIndex = (unsigned int*)malloc(maxDelayInFrames*sizeof(unsigned int));
    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);
    clean_buffer(dataBuffEffect, maxDelayInFrames);
    clean_buffer((float*)dataBuffIndex, maxDelayInFrames);

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

            // Guardo los índices en el buffer que se le pasará a la rutina en ASM
            eff_i-=4;
            for (unsigned int j = 0; j <= 3 && eff_i < framesRead; j++) {
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
    // [Limpieza]
    free(dataBuffIn);
    free(dataBuffOut);
    free(dataBuffEffect);
    free(dataBuffIndex);
    // [/Limpieza]
}
