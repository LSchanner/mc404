#include "hashtable.h"
#include <stdio.h>

typedef void* MemoryMap;

// Parseia um arquivo em assembly de ias. Devolve um Mapa de Memoria 
MemoryMap assemble_ias(String input);

// Printa o MemoryMap
void printMemoryMap(MemoryMap object, FILE* target);
