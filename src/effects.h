// PedalerIA64 Copyright [2014] <Matias Laporte>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "./libs/tiempo.h"
#include "./libs/sse_mathfun.h"
// #include "./libs/sincos.h" // TODO -> REMOVE

/****************************** CONSTANTES ******************************/
#define BUFFERSIZE 8192     // Tamaño del buffer en frames
#define M_PI 3.14159265358979323846264338327

/************************** VARIABLES GLOBALES **************************/
// [Variables globales]
int distValue;

// Contadores de frames
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

// Punteros a los archivos de entrada y salida
SNDFILE *inFilePtr;
SNDFILE *outFilePtr;

// Estructuras de los archivos
SF_INFO inFileStr;
SF_INFO outFileStr;



/****************** FUNCIONES AUXILIARES C *********************/
void clean_buffer_c(float *buffer, int bufferLen);  // Llena el buffer de 0s
// void copy_c();

float maxsamp_right_c();
void normalization_right_c();



/***************** FUNCIONES AUXILIARES ASM ********************/
// void clean_buffer_asm_caller();
// extern void clean_buffer_asm();

// void copy_asm_caller();
// extern void copy_asm(double *bufferIn, double *bufferOut, int bufferLen) __asm__("copy_asm");

float maxsamp_right_asm_caller();
extern void maxsamp_right_asm(float *bufferIn, float *maxTemp, unsigned int framesRead);

void normalization_right_asm_caller();
extern void normalization_right_asm(float *bufferIn, float *bufferOut, float *maxValue, unsigned int framesRead);



/************************** EFECTOS C **************************/
void delay_simple_c(float delayInSec, float decay);
void flanger_c(float delay, float rate, float amp);
void vibrato_c(float depth, float mod);
void bitcrusher_c(int bitDepth, int bitRate);
void wah_wah_c(float damp, int minf, int maxf, int wahfreq);



/************************* EFECTOS ASM *************************/
void delay_simple_asm_caller(float delayInSec, float decay);
extern void delay_simple_asm(float *bufferIn, float *bufferOut, float *bufferEffect, int bufferLen, float *decay, int channels) __asm__("delay_simple_asm");

void flanger_asm_caller(float delay, float rate, float amp);
extern void flanger_asm(float *bufferIn, float *bufferOut, float *bufferEffect, unsigned int *bufferIndex,  unsigned int framesReadTotal, int channels, float amp);

// void vibrato_asm_caller();
// extern void vibrato_asm();

// void bitcrusher_asm_caller();
// extern void bitcrusher_asm();

// void wah_wah_asm_caller();
// extern void wah_wah_asm();
