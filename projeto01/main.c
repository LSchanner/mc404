#include <stdio.h>
#include <stdlib.h>

int main ( int argc, char *argv[] ){
    // Tenta abrir o arquivo de entrada
    FILE* input_file = fopen(argv[1], "r");
    /* fopen returns 0, the NULL pointer, on failure */
    if ( input_file == NULL ){
        printf( "Could not open file\n" );
        return -1;
    }


    // Encontra o tamanho do arquivo
    char* input_text;
    long input_file_size = ftell(input_file);
    rewind(input_file);

    // Lê o arquivo
    input_text = malloc(input_file_size * (sizeof(char)));
    fread(input_text, sizeof(char), input_file_size, input_file);

    


    
}
