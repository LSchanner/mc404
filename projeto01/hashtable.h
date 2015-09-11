#define MaxHash 11

typedef char * String;

typedef enum {false, true} Boolean;

typedef void * Tabela;  /* tipo efetivo escondido */

typedef struct {
  String key;
  void* val;
} Registro;
  
Tabela CriaTabela();
/* Devolve apontador para uma tabela vazia */

Boolean InsereHashtable(Tabela p, Registro *a);
/* Insere o registro 'a' na tabela 'p' se o registro não
   existir na tabela retorna true; caso contrário
   devolve 'false' */
   
Boolean RemoveBase(Tabela p, String key);
/* Remove da tabela 'p' o aluno com 'ra' dado; devolve 'true'
   se ele existiu e 'false' caso contrário. */

Boolean ConsultaBase(Tabela p, String key, void* a);
/* Devolve 'true' se existe um registro com key dado na
   base 'p';  caso contrário devolve 'false'.  'a' conterá 
   os dados do aluno, se encontrado. */
   
int NumeroRegsBase(Tabela p);
/* Devolve o número de registros (alunos) na tabela 'p'. */

void LiberaBase(Tabela p);
/* Libera toda a memória utilizada pela base 'p', bem como
   todas as cadeias que guardam os nomes. */
