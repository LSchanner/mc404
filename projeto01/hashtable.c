#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/* DECLARAÇÕES DE TIPOS */
/* -------------------- */

char empty_char = 0;
String empty_string = &empty_char;


typedef struct RegLista *Lista;
typedef struct RegLista {
  Registro reg;
  Lista prox;
} RegLista;

typedef struct RegTabela *ImplTabela;
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
bool InsereLista(Lista lista, Registro a){

    // Procura o elemento na lista
    while(strcmp(lista->reg.key,a.key) < 0 && lista->prox != NULL){
        lista = lista->prox;
    }

    // Se o elemento já existe, ele é substituido
    if(strcmp(lista->reg.key,a.key) == 0){
        lista->reg.val = a.val;
        return false;
    }
    else{
        Lista no = (RegLista*) malloc(sizeof(RegLista));

        /* Insere as informações no registro */
        no->reg = a;

        /* Insere o registro na lista */
        no->prox = lista->prox;
        lista->prox = no;
        return true;
    }
}

/* Remove um registro de key  da lista */
bool RemoveLista(Lista lista,String key){

     /* percorre lista até chegar ao ultimo ponto antes da remoção */
    while(lista->prox != NULL && strcmp(lista->prox->reg.key , key) < 0){
        lista = lista->prox;
    }

    /* ra não encontrado */
    if(strcmp(lista->prox->reg.key , key) > 0){
        return false;
    }
    
    /* remove da lista */
    Lista remov = lista->prox;
    lista->prox = remov->prox;

    /* Libera */
    free(remov->reg.key);
    free(remov);
    
    return true;
}


/* Verifica se o registro de "ra" se econtra na lista, devolve os
 * dados no ponteiro a */
bool ConsultaLista(Lista lista,String key,Registro *a){

    /* Percorre a lista até encontrar o nó do aluno*/
    while(lista != NULL && strcmp(lista->reg.key, key) < 0){
        lista = lista->prox;
    }

    /* Aluno não encontrado */
    if(lista == NULL || strcmp(lista->reg.key, key) > 0){
        return false;
    }

    *a = lista->reg;
    return true;
}

void LiberaLista(Lista lista){
    Lista anterior;
    while(lista){
        anterior = lista;
        lista = lista->prox;
        free(anterior->reg.key);
        free(anterior);
    }
}

/* FUNÇÕES DA INTERFACE */
/* -------------------- */

/* Devolve um apontador para uma base vazia com tabela incializada */
Tabela CriaTabela(){
    ImplTabela base = (RegTabela*) malloc(sizeof(RegTabela));

    /* inicializa todos os campos da tabela com listas vazias */
    for(int i = 0; i < MaxHash; i++){
        base->tabela[i] = (RegLista*) malloc(sizeof(RegLista));
        base->tabela[i]->reg.key = empty_string;
        base->tabela[i]->prox = NULL;
    }

    /* numero de registros inicial é 0 */
    base->numregs = 0;

    return base;
} /* CriaBase() */

/* Insere o registro "a" na tabela "p". Se não for possível, devolve false*/
bool InsereTabela(Tabela p, Registro a){
    ImplTabela b = (ImplTabela) p;
    int indHash = Espalha(a.key);
    bool res;
    String key_copy = (String) malloc(sizeof(char) * strlen(a.key));
    strcpy(key_copy,a.key);
    a.key = key_copy;

    /* delega a inserção em si à uma função auxiliar */
    res = InsereLista(b->tabela[indHash], a);
    if(res){
        b->numregs++;
    }

    return res;
} /* InsereBase */

/* remove de uma Tabela p. Se não for possível, retorna false */
bool RemoveTabela(Tabela p, String key){
    bool res;
    ImplTabela b = (ImplTabela) p;
    int indHash = Espalha(key);

    res = RemoveLista(b->tabela[indHash],key);
    if(res){
        b->numregs--;
    }

    return res;
}

bool ConsultaTabela(Tabela p, String key, Registro* a){
    bool res;
    ImplTabela b = (ImplTabela) p;
    int indHash = Espalha(key);

    res = ConsultaLista(b->tabela[indHash], key, a);
    
    return res;
}

int NumeroRegsTabela(Tabela p){
    return ((ImplTabela) p)->numregs;
}


void LiberaTabela(Tabela p){
    ImplTabela b = (ImplTabela) p;

    for(int i = 0; i < MaxHash; i++){
        LiberaLista(b->tabela[i]);
    }

    free(b);
}



    


