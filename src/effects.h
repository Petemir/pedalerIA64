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

unsigned int bufferSize = 0;
unsigned int framesWritten = 0;
unsigned int framesRead = 0;
unsigned int framesRemaining = 0;
unsigned int framesReadTotal = 0;  // TODO -> Borrar
unsigned long int cantCiclos = 0;
unsigned long int start = 0;
unsigned long int end = 0;

float *dataBuffIn;
float *dataBuffOut;

SNDFILE *inFilePtr;
SNDFILE *outFilePtr;

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
//void volume_c(double ampfac);
//void normalization_c(double dbval);

void clean_buffer_int(int *buffer, int bufferLen);  // TODO -> Sacar
void clean_buffer(float *buffer, int bufferLen);

//double maxsamp(double *bufferIn, int bufferLen);

//void copy_c();
//void copy_asm_caller();
//extern int copy_asm(double *bufferIn, double *bufferOut, int bufferLen) __asm__("copy_asm");
// [/Auxiliares]


// [Efectos]
// PANPOS simplepan_c(double position);

void delay_simple_c(float delayInSec, float decay);
void flanger_c(float delay, float rate, float amp);
void vibrato_c(float depth, float mod);
void bitcrusher_c(int bitDepth, int bitRate);

void delay_simple_asm_caller(float delayInSec, float decay);
extern int delay_simple_asm(float *bufferIn, float *bufferOut, float *bufferEffect, int bufferLen, float *decay, int channels) __asm__("delay_simple_asm");

void flanger_asm_caller(float delay, float rate, float amp);
extern int flanger_asm(float *bufferIn, float *bufferOut, float *bufferEffect, unsigned int *bufferIndex,  unsigned int framesReadTotal, int channels, float amp);

// [/Efectos]
