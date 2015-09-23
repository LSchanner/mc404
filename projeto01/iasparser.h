#pragma once
#include <stdio.h>
#include <stdbool.h>

typedef struct PosicaoMontagem{
    long long pos_mapa;
    int pos_instrucao;
}PosicaoMontagem;

typedef struct Palavra{
    union{
        long long dado;
        long instrucoes[2];
    }bits;
    bool used_esq,used_dir;
    bool is_data;
}Palavra;

typedef struct MemoryMap{
    Palavra map[1024];
}MemoryMap;


// Parseia um arquivo em assembly de ias. Devolve um Mapa de Memoria 
MemoryMap assemble_ias(char* input);

// Printa o MemoryMap
void printMemoryMap(MemoryMap object, FILE* target);
