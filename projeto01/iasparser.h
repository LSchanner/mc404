#pragma once
#include <stdio.h>
#include <stdbool.h>

typedef void* MemoryMap;

typedef char* String;

typedef struct PosicaoMontagem{
    long long pos_mapa;
    int pos_instrucao;
}PosicaoMontagem;

// Parseia um arquivo em assembly de ias. Devolve um Mapa de Memoria 
MemoryMap assemble_ias(String input);

// Printa o MemoryMap
void printMemoryMap(MemoryMap object, FILE* target);
