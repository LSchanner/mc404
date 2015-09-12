#define MaxHash 11

typedef char * String;


typedef void * Tabela;  /* tipo efetivo escondido */

typedef struct {
  String key;
  void* val;
} Registro;
  
Tabela CriaTabela();
/* Devolve apontador para uma tabela vazia */

bool InsereTabela(Tabela p, Registro *a);
/* Insere o registro 'a' na tabela 'p' se o registro n�o
   existir na tabela retorna true; caso contr�rio
   devolve 'false' */
   
bool RemoveTabela(Tabela p, String key);
/* Remove da tabela 'p' o aluno com 'ra' dado; devolve 'true'
   se ele existiu e 'false' caso contr�rio. */

bool ConsultaTabela(Tabela p, String key, void* a);
/* Devolve 'true' se existe um registro com key dado na
   base 'p';  caso contr�rio devolve 'false'.  'a' conter� 
   os dados do aluno, se encontrado. */
   
int NumeroRegsTabela(Tabela p);
/* Devolve o n�mero de registros (alunos) na tabela 'p'. */

void LiberaTabela(Tabela p);
/* Libera toda a mem�ria utilizada pela base 'p', bem como
   todas as cadeias que guardam os nomes. */
