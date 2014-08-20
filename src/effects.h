// PedalerIA64 Copyright [2014] <Matias Laporte>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "./tiempo.h"

#define BUFFERSIZE 8192  // en frames

// [Variables globales]
int distValue;

unsigned int bufferSize = 0;
unsigned int framesWritten = 0;
unsigned int framesRead = 0;
unsigned int framesReadTotal = 0; // TODO -> Borrar
unsigned long int cantCiclos = 0;

double *dataBuffIn;
double *dataBuffOut;

SNDFILE *inFilePtr;
SNDFILE *outFilePtr;

SF_INFO inFileStr;
SF_INFO outFileStr;

typedef struct PANPOS {
    double left;
    double right;
} PANPOS;
// [/Variables globales]


// [Auxiliares]
void volume_c(double ampfac);
void normalization_c(double dbval);

double maxsamp(double *bufferIn, int bufferLen);
void sin(double *)

void copy_c();
void copy_asm_caller();
extern int copy_asm(double *bufferIn, double *bufferOut, int bufferLen) __asm__("copy_asm");
// [/Auxiliares]


// [Efectos]
PANPOS simplepan_c(double position);

void delay_c(double delayInSec, double decay);
void delay_asm_caller(double delayInSec, double decay);
extern int delay_asm(double *bufferIn, double *bufferOut, double *bufferEffect, int bufferLen, double *decay) __asm__("delay_asm");
// [/Efectos]


8237895

