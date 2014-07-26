// PedalerIA64 Copyright [2014] <Matias Laporte>
#include "./effects.c"

const char* nombrePrograma;

void imprimir_ayuda() {
    printf(" Uso: %s nombre_archivo_entrada nombre_archivo_salida opciones\n", nombrePrograma);
    printf(" Opciones disponibles:\n");
    printf("Comando\t\t Descripción\t\t\t\t Parámetros\n");
    printf("------------------------------------------------------------------------------------\n");
    printf("-c \t\t Copiar sin efecto (en C)\t\t Sin parámetros\n");
    printf("-C \t\t Copiar sin efecto (en ASM)\t\t Sin parámetros\n");
    printf("-d a b\t\t Delay simple (en C)\t\t\t a: segundos de delay (double), b: decay (double)\n");
    printf("-D a b\t\t Delay simple (en ASM)\t\t\t a: segundos de delay (double), b: decay (double)\n");
    printf("\n\nEjemplo: %s input.wav output.wav -d 1.5 0.6\n", nombrePrograma);
    printf("Aplica un efecto de delay (implementado en C) sobre el archivo input.wav, con 1.5 segundos de delay y 0.6 como coeficiente de decay, con el resultado en output.wav.\n");
}

int main(int argc, char* argv[]) {
    nombrePrograma = argv[0];
    if (argc < 4) {  // Necesito por lo menos 4 argumentos, nombre de programa, archivo entrada, archivo salida y opcion
        imprimir_ayuda();
        return 1;
    }
    int siguienteOpcion = 3;  // Sé que hay, por lo menos, 4 argumentos ya

    // [Declaración de variables de archivo]
    char *inFileName = argv[1];
    char *outFileName = argv[2];
    // [/Declaración de variables de archivo]

    // [Archivo de entrada]
    inFileStr.format = 0;  // Según API libsndfile
    // Manejo de errores en la apertura del archivo
    if (!(inFilePtr = sf_open(inFileName, SFM_READ, &inFileStr))) {
        printf("No se pudo abrir el archivo %s.\n", inFileName);
        puts(sf_strerror(NULL));
        return 1;
    }
    // [/Archivo de entrada]

    // [Archivo de salida]
    outFileStr.format = inFileStr.format;
    outFileStr.channels = inFileStr.channels;
    outFileStr.samplerate = inFileStr.samplerate;
    // Manejo de errores en la apertura del archivo
    if (!(outFilePtr = sf_open(outFileName, SFM_WRITE, &outFileStr))) {
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

        sf_seek(inFilePtr, 0, SEEK_SET);  // Si aplico más de un efecto, empiezo de cero en el archivo

        switch (argv[siguienteOpcion][1]) {
            case 'c':
                printf("Copy c.\n");
                copy_c();
                siguienteOpcion+=1;
                break;
            case 'C':
                printf("Copy asm.\n");
                copy_asm_caller();
                siguienteOpcion+=1;
                break;
            case 'd':
                printf("Delay c.\n");
                delay_c(atof(argv[4]), atof(argv[5]));  // delay, decay
                siguienteOpcion+=3;
                break;
            case 'D':
                printf("Delay asm.\n");
                delay_asm_caller(atof(argv[4]), atof(argv[5]));  // delay, decay
                siguienteOpcion+=3;
                break;
        }
    }

    /* [Libero archivos y memoria] */
    sf_close(inFilePtr);
    sf_close(outFilePtr);
    /* [/Libero archivos y memoria] */

    return 0;
}
