#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"

/* DECLARAÇÕES DE TIPOS */
/* -------------------- */

typedef struct RegLista *Lista;
typedef struct RegLista {
  Registro reg;
  Lista prox;
} RegLista;

typedef struct RegBase *ImplTabela;
typedef struct RegTabela {
  int numregs;
  Lista tabela[MaxHash];
} RegTabela;

/* FUNÇÃO DE ESPALHAMENTO */
/* ---------------------- */

int Espalha(String str) {
    int acc = 0;
    while(*str){
        acc += *str;
        str++;
    }
    return (acc % MaxHash);
} /* Espalha */

/* FUNÇÕES AUXILIARES */
/* ------------------ */

/* Funções de Manipulação de Listas ligadas */

/* Insere o Registro a em uma lista */ 
void InsereLista(Lista lista, Registro a){

    Lista no = (RegLista*) malloc(sizeof(RegLista));

    /* Insere as informações no registro */
    no->reg = a;

    /* Insere o registro na lista */
    no->prox = lista->prox;
    lista->prox = no;
    
}

/* Remove um registro de key  da lista */
Boolean RemoveLista(Lista lista,String key){

     /* percorre lista até chegar ao ultimo ponto antes da remoção */
    while(lista->prox != NULL && lista->prox->aluno.ra < ra){
        lista = lista->prox;
    }

    /* ra não encontrado */
    if(lista->prox->aluno.ra != ra){
        return false;
    }
    
    /* remove da lista */
    Lista remov = lista->prox;
    lista->prox = remov->prox;

    /* Libera */
    FREE(remov->aluno.nome);
    FREE(remov->aluno);
    
    return true;
}


/* Verifica se o aluno de ra "ra" se econtra na lista, devolve os
 * dados no ponteiro a */
Boolean ConsultaLista(Lista lista,int ra,Aluno *a){

    /* Percorre a lista até encontrar o nó do aluno*/
    while(lista != NULL && lista->aluno.ra < ra){
        lista = lista->prox;
    }
    /* Aluno não encontrado */
    if(lista->aluno.ra != ra){
        return false;
    }

    *a = lista->aluno;
    return true;
}



/* FUNÇÕES DA INTERFACE */
/* -------------------- */

/* Devolve um apontador para uma base vazia com tabela incializada */
Base CriaBase(){
    ImplBase base =  MALLOC(sizeof(RegBase));

    /* inicializa todos os campos da tabela com listas vazias */
    for(int i = 0; i < MaxHash; i++){
        base->tabela[i] = MALLOC(sizeof(RegLista));
        base->tabela[i]->ra = -1 ;
        base->tabela[i]->prox = NULL ;
    }

    /* numero de registros inicial é 0 */
    base->numregs = 0;

    return base;
} /* CriaBase() */

/* Insere o aluno "a" na base "p". Se não for possível, devolve false*/
Boolean InsereBase(Base p, Aluno *a){
    ImplBase b = (ImplBase) p;
    int indHash = Espalha(a->ra);
    Boolean res;

    /* delega a inserção em si à uma função auxiliar */
    res = InsereLista(b->tabela[indHash], a);
    if(res){
        b->numregs++;
    }

    return res;
} /* InsereBase */

/* remove de uma base p. Se não for possível, retorna false */
Boolean RemoveBase(Base p, int ra){
    Boolean res;
    ImplBase b = (ImplBase) p;
    int indHash = Espalha(ra);

    res = RemoveLista(b->tabela[indHash],ra);
    if(res){
        b->numregs--;
    }

    return res;
}

Boolean ConsultaBase(Base p,int ra, Aluno *a){
    Boolean res;
    ImplBase b = (ImplBase) p;
    int indHash = Espalha(ra);

    res = ConsultaLista(b->tabela[indHash], ra, a);
    
    return res;
}

int NumeroRegsBase(Base p){
    return ((ImplBase) p)->numregs;
}

void ImprimeBase(Base p){
    ImplBase b = (ImplBase) p;

    for(int i = 0; i < MaxHash; i++){
        ImprimeLista(b->tabela[i],i);
    }
}

void LiberaBase(Base p){
    ImplBase b = (ImplBase) p;

    for(int i = 0; i < MaxHash; i++){
        LiberaLista(b->tabela[i]);
    }

    FREE(b);
}



    


