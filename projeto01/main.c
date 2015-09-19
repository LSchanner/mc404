#include <stdio.h>
#include <stdlib.h>
#include "iasparser.h"

int main ( int argc, char *argv[] ){

    // Tenta abrir o arquivo de entrada
    FILE* input_file = fopen(argv[1], "r");

    /* fopen returns 0, the NULL pointer, on failure */
    if ( input_file == NULL ){
        printf( "Could not open file\n" );
        return -1;
    }

    // Encontra o tamanho do arquivo
    fseek(input_file, 0, SEEK_END);
    long input_file_size = ftell(input_file);
    rewind(input_file);

    // Lê o arquivo
    char* input_text;
    input_text = (char*) malloc(input_file_size * (sizeof(char)) + 1);
    fread(input_text, sizeof(char), input_file_size, input_file);
    input_text[input_file_size-1] = 0;
    fclose(input_file);

    // Chama o montador
    MemoryMap mapa = assemble_ias(input_text);
    FILE* output_file = stdout;
    
    // Se o usuário passar um arquivo para escrita, use ele
    if(argc >= 3){
        output_file = fopen(argv[2],"w");
        if(output_file == NULL){
            printf("Could not open file for writing\n");
            return -1;
        }
    }

    // Printa o Mapa de memória
    printMemoryMap(mapa,output_file);

    // libera recursos utilizados;
    fclose(output_file);
    free(input_text);
    
    return 0;
}
