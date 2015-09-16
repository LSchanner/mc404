#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iasparser.h"

typedef struct Palavra{
    union{
        long long dado;
        long instrucoes[2];
    };
}Palavra;

typedef struct MemoryMapImpl{
    Palavra map[2048];
}MemoryMapImpl;

typedef struct posicao_montagem{
    int pos_mapa;
    int pos_instrucao;
}PosicaoMontagem;

bool match_rotulo(String,bool declaration);
bool match_instrucao(String);
int parse_num(String);

Tabela Instrucoes;

MemoryMap parse_ias_string(String input, Tabela rotulos,bool first){
    char* token;
    PosicaoMontagem pos = {0,0};
    MemoryMapImpl mapa;


    token = strtok(input," \t");
    while(token != NULL){

        if(match_rotulo(token,true)){
            Registro rotulo = {token,(void*) pos};
            InsereTabela(rotulos, rotulo);
            token = strtok(NULL," \t");
        }

        if(token[0] == '.'){
        

        }
    }


    return NULL;
}

