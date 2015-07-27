// PedalerIA64 Copyright [2014] <Matias Laporte>
#include "./effects.c"
#include "./effects_asm.c"

const char* nombrePrograma;

void imprimir_ayuda() {
    printf(" Uso: %s nombre_archivo_entrada nombre_archivo_salida opciones\n", nombrePrograma);
    printf(" Opciones disponibles:\n");
    printf("Comando\t\t Descripción\t\t\t\t Parámetros\n");
    printf("------------------------------------------------------------------------------------\n");
//    printf("-c \t\t Copiar sin efecto (en C)\t\t Sin parámetros\n");
//    printf("-C \t\t Copiar sin efecto (en ASM)\t\t Sin parámetros\n");
    printf("-d a b\t\t Delay simple (en C)\t\t\t a: segundos de delay (float), b: decay (float)\n");
    printf("-D a b\t\t Delay simple (en ASM)\t\t\t a: segundos de delay (float), b: decay (float)\n");
    printf("-f a b c\t Flanging (en C)\t\t\t a: segundos de delay (float), b: flanger rate (float), c: amplificacion (float)\n");
    printf("-F a b c\t Flanging (en ASM)\t\t\t a: segundos de delay (float), b: flanger rate (float), c: amplificacion (float)\n");
    printf("\t\t (Recomendado para flanging: a entre 0.003s y 0.015s, rate entre 0.1 y 5Hz, amp en 0.7)\n");
    printf("\n\nEjemplo: %s input.wav output.wav -d 1.5 0.6\n", nombrePrograma);
    printf("Aplica un efecto de delay (implementado en C) sobre el archivo input.wav, con 1.5 segundos de delay y 0.6 como coeficiente de decay, con el resultado en output.wav.\n");
}

int main(int argc, char* argv[]) {
    nombrePrograma = argv[0];
    if (argc < 4) {  // Por lo menos 4 argumentos (nombre de programa, archivo entrada, archivo salida y opcion)
        imprimir_ayuda();
        return 1;
    }
    int siguienteOpcion = 3;  // Hay, por lo menos, 4 argumentos

// [Declaración de variables de archivo]
    char *inFileName = argv[1];
    char *outFileName = argv[2];
// [/Declaración de variables de archivo]

// [Archivo de entrada]
    inFileStr.format = 0;  // Según API libsndfile
    // Manejo de errores en la apertura del archivo entrada
    if (!(inFilePtr = sf_open(inFileName, SFM_READ, &inFileStr))) {
        printf("No se pudo abrir el archivo %s.\n", inFileName);
        puts(sf_strerror(NULL));
        return 1;
    }
// [/Archivo de entrada]

// [Archivo de salida]
    outFileStr.format = inFileStr.format;
    outFileStr.channels = 2;  // La salida es siempre stereo, por 1 canal el original y por el otro el efecto

    outFileStr.samplerate = inFileStr.samplerate;
    // Manejo de errores en la apertura del archivo de salida
    if (!(outFilePtr = sf_open(outFileName, SFM_RDWR, &outFileStr))) {
        printf("No se pudo abrir el archivo %s.\n", outFileName);
        puts(sf_strerror(NULL));
        return 1;
    }
// [/Archivo de salida]

    cantCiclos = 0;  // Inicializo contador de ticks de procesador
    while (siguienteOpcion < argc) {
        if ((strlen(argv[siguienteOpcion]) != 2) || (argv[siguienteOpcion][0] != '-')) {
            printf("Opción inválida. \n");
            printf("Releer ayuda del programa ejecutando: %s\n", nombrePrograma);
            break;
        }

        sf_seek(inFilePtr, 0, SEEK_SET);  // Si aplico más de un efecto, empiezo de cero en el archivo de entrada

        switch (argv[siguienteOpcion][1]) {
/*            case 'v':
                printf("Change volume c.\n");
                volume_c(atof(argv[siguienteOpcion+1]));
                siguienteOpcion+=2;
                break;
            case 'n':
                printf("Normalize file c.\n");  // dbval < 0.0
                normalization_c(atof(argv[siguienteOpcion+1]));
                siguienteOpcion+=2;
                break;
            case 'p':
                printf("Stereo Panning c.\n");  // -1.0 < panpos < 1.0 // check it's mono
                panning_c(atof(argv[siguienteOpcion+1]));
                siguienteOpcion+=2;
                break;
            case 'c':
                printf("Copy c.\n");
                copy_c();
                siguienteOpcion+=1;
                break;
            case 'C':
                printf("Copy asm.\n");
                copy_asm_caller();
                siguienteOpcion+=1;
                break;*/
            case 'd':
                printf("Delay simple c.\n");
                delay_simple_c((float)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]));  // delay, decay
                siguienteOpcion+=3;
                break;
            case 'D':
                printf("Delay simple asm.\n");
                delay_simple_asm_caller((float)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]));  // delay, decay
                siguienteOpcion+=3;
                break;
            case 'f':
                printf("Flanger c.\n");
                flanger_c((float)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]), (float)atof(argv[siguienteOpcion+3]));  // delay, rate, amp
                siguienteOpcion+=4;
                break;
            case 'F':
                printf("Flanger asm.\n");
                flanger_asm_caller((float)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]), (float)atof(argv[siguienteOpcion+3]));  // delay, rate, amp
                siguienteOpcion+=4;
                break;
            case 'v':
                printf("Vibrato c.\n");
                vibrato_c((float)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]));  // delay depth, modulation frequency
                siguienteOpcion+=3;
                break;
            case 'b':
                printf("Bitcrusher c.\n");
                bitcrusher_c((int)atof(argv[siguienteOpcion+1]), (int)atof(argv[siguienteOpcion+2]));  // int bitDepth, int bitRate
                siguienteOpcion+=3;
                break;
            case 'w':
                printf("Wahwah c.\n");
                wah_wah_c((float)atof(argv[siguienteOpcion+1]), (int)atof(argv[siguienteOpcion+2]), (int)atof(argv[siguienteOpcion+3]), (int)atof(argv[siguienteOpcion+4]));  // float damp, int minf, int maxf, int wahfreq
                sf_seek(outFilePtr, 0, SEEK_SET);   // Reinicio puntero al archivo
                normalization_right_c();                  // Normalizo el canal de efecto
                siguienteOpcion+=5;
                break;
            case 'W':
                printf("Wahwah asm.\n");
                wah_wah_c((float)atof(argv[siguienteOpcion+1]), (int)atof(argv[siguienteOpcion+2]), (int)atof(argv[siguienteOpcion+3]), (int)atof(argv[siguienteOpcion+4]));  // float damp, int minf, int maxf, int wahfreq
                sf_seek(outFilePtr, 0, SEEK_SET);   // Reinicio puntero al archivo
                normalization_right_asm_caller();
                siguienteOpcion+=5;
                break;

        }
    }

    /* [Libero archivos y memoria] */
    sf_close(inFilePtr);
    sf_close(outFilePtr);
    /* [/Libero archivos y memoria] */

    return 0;
}
