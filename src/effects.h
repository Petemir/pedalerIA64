// PedalerIA64 Copyright [2014] <Matias Laporte>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "./libs/tiempo.h"
#include "./libs/sse_mathfun.h"

#define BUFFERSIZE 8192  // en frames
#define M_PI 3.14159265358979323846264338327

// [Variables globales]
int distValue;

unsigned int framesWritten = 0;     // Frames escritos en el archivo de salida
unsigned int framesRead = 0;        // Frames que se leen del archivo de entrada
unsigned int framesRemaining = 0;   // Frames que quedan por usar en el buffer de efecto cuando ya no hay más entrada
unsigned int framesReadTotal = 0;   // Frames totales leidos hasta el momento

// Variables para contar ciclos de reloj
unsigned long int cantCiclos = 0;
unsigned long int start = 0;
unsigned long int end = 0;

// Buffers de entrada y salida
float *dataBuffIn;
float *dataBuffOut;

// Pointer a los archivos
SNDFILE *inFilePtr;
SNDFILE *outFilePtr;

// Estructuras de los archivos
SF_INFO inFileStr;
SF_INFO outFileStr;
// [/Variables globales]

// [Estructuras auxiliares]
/*
typedef struct PANPOS {
    double left;
    double right;
} PANPOS;*/
// [/Estructuras auxiliares]

// [Auxiliares]
// void volume_c(double ampfac);

void clean_buffer_int(int *buffer, int bufferLen);  // TODO -> Sacar
void clean_buffer(float *buffer, int bufferLen);


// void copy_c();
// void copy_asm_caller();
// extern int copy_asm(double *bufferIn, double *bufferOut, int bufferLen) __asm__("copy_asm");
// [/Auxiliares]

float maxsamp_right_c();
float maxsamp_right_asm_caller();
extern void maxsamp_right_asm(float *bufferIn, float *maxTemp, unsigned int framesRead);

void normalization_right_c();
void normalization_right_asm_caller();
extern void normalization_right_asm();

// [Efectos C]
// PANPOS simplepan_c(double position);

void delay_simple_c(float delayInSec, float decay);
void flanger_c(float delay, float rate, float amp);
void vibrato_c(float depth, float mod);
void bitcrusher_c(int bitDepth, int bitRate);
void wah_wah_c(float damp, int minf, int maxf, int wahfreq);
// [/Efectos C]

// [Efectos ASM]
void delay_simple_asm_caller(float delayInSec, float decay);
extern void delay_simple_asm(float *bufferIn, float *bufferOut, float *bufferEffect, int bufferLen, float *decay, int channels) __asm__("delay_simple_asm");

void flanger_asm_caller(float delay, float rate, float amp);
extern void flanger_asm(float *bufferIn, float *bufferOut, float *bufferEffect, unsigned int *bufferIndex,  unsigned int framesReadTotal, int channels, float amp);
// [/Efectos ASM]



// [TEMPLATE]
/*
void effect_c(params) {
    // Si es efecto con delay, tenerlo en cuenta
    unsigned int bufferFrameSize = BUFFERSIZE*inFileStr.channels;
    unsigned int bufferFrameSizeOut = BUFFERSIZE*outFileStr.channels;

    // Chequear bien los tamaños, particularmente dataBuffEffect
    dataBuffIn = (float*)malloc(bufferFrameSize*sizeof(float));
    dataBuffOut = (float*)malloc(bufferFrameSizeOut*sizeof(float));
    float *dataBuffEffect = (float*)malloc(BUFFERSIZE*sizeof(float));

    // Limpio buffers
    clean_buffer(dataBuffIn, bufferFrameSize);
    clean_buffer(dataBuffEffect, BUFFERSIZE);
    clean_buffer(dataBuffOut, bufferFrameSizeOut);

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

            // aplicar efecto

            dataBuffOut[out_i++] = dataBuffEffect[eff_i];   // Sonido original
            dataBuffOut[out_i++] = ¿?;

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
}*/
