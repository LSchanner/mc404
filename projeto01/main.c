#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
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
    input_text = (char*) malloc(input_file_size * (sizeof(char)));
    fread(input_text, sizeof(char), input_file_size, input_file);


    // Chama o parseador pela primeira vez, para preencher a tabela
    Tabela hashtable = CriaTabela();
    parse_ias_string(input_text,hashtable);

    // Chama o parseador pela segunda vez, dessa vez com a hashtable já construída.
    String output = parse_ias_string(input_text,hashtable);
    
    return 0;
}
