// PedalerIA64 Copyright [2014] <Matias Laporte>
#include "./effects.c"
#include "./effects_asm.c"

const char* nombrePrograma;

void imprimir_ayuda() {
    printf(" Uso: %s nombre_archivo_entrada nombre_archivo_salida cant_iteraciones opciones\n", nombrePrograma);
    printf(" Opciones disponibles:\n");
    printf("Comando\t\t Descripción\t\t\t\t Parámetros\n");
    printf("------------------------------------------------------------------------------------\n");
    printf("-c \t\t Copiar sin efecto (en C)\t\t Sin parámetros\n");
    printf("-C \t\t Copiar sin efecto (en ASM)\t\t Sin parámetros\n");
    printf("-d a b\t\t Delay simple (en C)\t\t\t a: segundos de delay (float), b: decay (float)\n");
    printf("-D a b\t\t Delay simple (en ASM)\t\t\t a: segundos de delay (float), b: decay (float)\n");
    printf("(Rango para delay: a entre 0.0 y 5.0s, b entre 0.00 y 1.00 \n");
    printf("-f a b c\t Flanging (en C)\t\t\t a: segundos de delay (float), b: flanger rate (float), c: amplificacion (float)\n");
    printf("-F a b c\t Flanging (en ASM)\t\t\t a: segundos de delay (float), b: flanger rate (float), c: amplificacion (float)\n");
    printf("(Rango para flanging: a entre 0.003s y 0.015s, b entre 0.10 y 1.00Hz, c entre 0.65 y 0.75)\n");
    printf("-v a b \t\t Vibrato (en C)\t\t\t\t a: segundos de delay (float), b: modulation rate (float)\n");
    printf("-v a b \t\t Vibrato (en C)\t\t\t\t a: segundos de delay (float), b: modulation rate (float)\n");
    printf("(Rango para vibrato: a entre 0.000s y 0.003s, b entre 0.10 y 5.00Hz\n");
    printf("-b a b \t\t Bitcrusher (en C)\t\t\t a: bits de resolución (int), b: frecuencia de sampleo (int)\n");
    printf("-B a b \t\t Bitcrusher (en ASM)\t\t\t a: bits de resolución (int), b: frecuencia de sampleo (int)\n");
    printf("(Rango para bitcrusher: a entre 1 y 16, b entre 2048 y 11025Hz\n");
    printf("-w a b c d\t\t WahWah (en C)\t\t\t a: damping (float), b: min. freq. pasabandas (int), c: max. freq. pasabandas (int), d: freq. wahwah (int)\n");
    printf("-W a b c d\t\t WahWah (en ASM)\t\t a: damping (float), b: min. freq. pasabandas (int), c: max. freq. pasabandas (int), d: freq. wahwah (int)\n");
    printf("(Rango para wahwah: a entre 0.01 y 0.10, b entre  400 y 1000, c entre 2500 y 3500, d entre 1000 y 3000\n");

    printf("\n\nEjemplo: %s input.wav output.wav -d 1.5 0.6\n", nombrePrograma);
    printf("Aplica un efecto de delay (implementado en C) sobre el archivo input.wav, con 1.5 segundos de delay y 0.6 como coeficiente de decay, con el resultado en output.wav.\n");
}

int main(int argc, char* argv[]) {
    nombrePrograma = argv[0];
    if (argc < 5) {  // Por lo menos 5 argumentos (nombre de programa, archivo entrada, archivo salida, cant_iter y opcion)
        imprimir_ayuda();
        return 1;
    }
    int siguienteOpcion = 4;  // Hay, por lo menos, 5 argumentos

// [Declaración de variables de archivo]
    char *inFileName = argv[1];
    char *outFileName = argv[2];
    cantIteraciones = atoi(argv[3]);
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

    cantCiclosTotales = 0;
    for(int i = 1; i <= cantIteraciones; i++) {
        start = end = cantCiclos = 0;  // Inicializo contador de ticks de procesador
        sf_seek(inFilePtr, 0, SEEK_SET);
        sf_seek(outFilePtr, 0, SEEK_SET);

        siguienteOpcion = 4;
        while (siguienteOpcion < argc) {
            if ((strlen(argv[siguienteOpcion]) != 2) || (argv[siguienteOpcion][0] != '-')) {
                printf("%d\n", argv[siguienteOpcion][0]);
                printf("Opción inválida. \n");
                printf("Releer ayuda del programa ejecutando: %s\n", nombrePrograma);
                break;
            }

            sf_seek(inFilePtr, 0, SEEK_SET);  // Si aplico más de un efecto, empiezo de cero en el archivo de entrada

            switch (argv[siguienteOpcion][1]) {
                case 'c':
                    printf("Copy c.\n");
                    outFileStr.channels = inFileStr.channels;
                    copy_c();
                    siguienteOpcion+=1;
                    break;
                case 'C':
                    printf("Copy asm.\n");
                    outFileStr.channels = inFileStr.channels;
                    copy_asm_caller();
                    siguienteOpcion+=1;
                    break;
                case 'd':
                    printf("Delay simple c: iteración %d.\n", i);
                    delay_simple_c((float)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]));  // delay, decay
                    siguienteOpcion+=3;
                    break;
                case 'D':
                    printf("Delay simple asm: iteración %d.\n", i);
                    delay_simple_asm_caller((float)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]));  // delay, decay
                    siguienteOpcion+=3;
                    break;
                case 'f':
                    printf("Flanger c: iteración %d.\n", i);
                    flanger_c((float)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]), (float)atof(argv[siguienteOpcion+3]));  // delay, rate, amp
                    siguienteOpcion+=4;
                    break;
                case 'F':
                    printf("Flanger asm: iteración %d.\n", i);
                    flanger_asm_caller((float)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]), (float)atof(argv[siguienteOpcion+3]));  // delay, rate, amp
                    siguienteOpcion+=4;
                    break;
                case 'v':
                    vibrato_c((float)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]));  // delay depth, modulation frequency
                    printf("Vibrato c: iteración %d.\n", i);
                    siguienteOpcion+=3;
                    break;
                case 'V':
                    printf("Vibrato asm: iteración %d.\n", i);
                    vibrato_asm_caller((float)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]));  // delay depth, modulation frequency
                    siguienteOpcion+=3;
                    break;
                case 'b':
                    printf("Bitcrusher c: iteración %d.\n", i);
                    bitcrusher_c((int)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]));  // int bitDepth, int bitRate
                    siguienteOpcion+=3;
                    break;
                case 'B':
                    printf("Bitcrusher asm: iteración %d.\n", i);
                    bitcrusher_asm_caller((int)atof(argv[siguienteOpcion+1]), (float)atof(argv[siguienteOpcion+2]));  // int bitDepth, int bitRate
                    siguienteOpcion+=3;
                    break;
                case 'w':
                    printf("Wahwah c: iteración %d.\n", i);
                    wah_wah_c((float)atof(argv[siguienteOpcion+1]), (int)atof(argv[siguienteOpcion+2]), (int)atof(argv[siguienteOpcion+3]), (int)atof(argv[siguienteOpcion+4]));  // float damp, int minf, int maxf, int wahfreq
                    sf_seek(outFilePtr, 0, SEEK_SET);       // Reinicio puntero al archivo
                    normalization_right_c();                // Normalizo el canal de efecto
                    siguienteOpcion+=5;
                    break;
                case 'W':
                    printf("Wahwah asm: iteración %d.\n", i);
                    wah_wah_asm_caller((float)atof(argv[siguienteOpcion+1]), (int)atof(argv[siguienteOpcion+2]), (int)atof(argv[siguienteOpcion+3]), (int)atof(argv[siguienteOpcion+4]));  // float damp, int minf, int maxf, int wahfreq
                    sf_seek(outFilePtr, 0, SEEK_SET);   // Reinicio puntero al archivo
                    normalization_right_asm_caller();
                    siguienteOpcion+=5;
                    break;

            }
        }
        cantCiclosTotales += cantCiclos;
        // printf("\tCiclos: %lu\n", cantCiclos);
    }
    printf("\tTiempo de ejecución:\n");
    //printf("\t  Comienzo                          : %lu\n", start);
    //printf("\t  Fin                               : %lu\n", end);
    printf("\t  # de ciclos insumidos totales     : %lu\n", cantCiclosTotales);
    printf("\t  # iteraciones                     : %d\n", cantIteraciones);
    printf("\t  # de ciclos insumidos por llamada : %.3f\n", (float)cantCiclosTotales/(float)cantIteraciones);

    /* [Libero archivos y memoria] */
    sf_close(inFilePtr);
    sf_close(outFilePtr);
    /* [/Libero archivos y memoria] */

    return 0;
}
