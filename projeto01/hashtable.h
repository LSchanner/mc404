#pragma once
#define MaxHash 11
#include "iasparser.h"
#include <stdbool.h>

typedef char* String;

typedef PosicaoMontagem DataType;

typedef void* Tabela;  /* tipo efetivo escondido */

typedef struct {
  String key;
  DataType val;
} Registro;
  
Tabela CriaTabela();
/* Devolve apontador para uma tabela vazia */

bool InsereTabela(Tabela p, Registro a);
/* Insere o registro 'a' na tabela 'p' se o registro não
   existir na tabela retorna true; caso contrário
   devolve 'false' */
   
bool RemoveTabela(Tabela p, String key);
/* Remove da tabela 'p' o registro com 'key' dado; devolve 'true'
   se ele existiu e 'false' caso contrário. */

bool ConsultaTabela(Tabela p, String key, Registro* a);
/* Devolve 'true' se existe um registro com key dado na
   base 'p';  caso contrário devolve 'false'.  'a' conterá 
   os dados do aluno, se encontrado. */
   
int NumeroRegsTabela(Tabela p);
/* Devolve o número de registros na tabela 'p'. */

void LiberaTabela(Tabela p);
/* Libera toda a memória utilizada pela Tabela 'p', bem como
   todas as cadeias que guardam os nomes. */
