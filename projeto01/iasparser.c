#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iasparser.h"

typedef struct Palavra{
    union{
        long long dado;
        long instrucoes[2];
    };
    bool used = 0;
    bool is_data = 0;
}Palavra;

typedef struct MemoryMapImpl{
    Palavra map[2048];
}MemoryMapImpl;

typedef struct posicao_montagem{
    int pos_mapa;
    int pos_instrucao;
}PosicaoMontagem;

// Funções utilitárias
bool match_rotulo(String);

bool match_symbol(String);

int parse_num(String);

void write_word(MemoryMapImpl map,PosicaoMontagem pos,long long data);

void write_instruction(MemoryMapImpl map,PosicaoMontagem pos,long data);

void erro(String message);

MemoryMap parse_ias_string(String input, Tabela rotulos,Tabela symbols,bool first){
    char* token;
    PosicaoMontagem pos = {0,0};
    MemoryMapImpl mapa;


    token = strtok(input," \t");
    while(token != NULL){

        if(match_rotulo(token)){
            Registro rotulo = {token,(void*) pos};
            InsereTabela(rotulos, rotulo);
            token = strtok(NULL," \t");
        }

        // Diretivas
        if(token[0] == '.'){
            // Diretiva .org
            if(strcmp(token,".org") == 0){
                token = strtok(NULL," \t");
                pos.pos_mapa = parse_num(token);
                pos.pos_instrucao = 0;

            // Diretiva .set
            }else if(strcmp(token,".set") == 0){
                String symbol = strtok(NULL," \t");
                if(match_symbol(symbol)){
                    long long value = parse_num(strtok(NULL," \t"));
                    Registro macro = {symbol,(void*)value};
                    InsereTabela(symbols,macro);
                }else{
                    erro("Simbolo inválido",pos);
                }
                

            // Diretiva .align
            }else if(strcmp(token,".align") == 0){
                int num = parse_num(strtok(NULL," \t"));
                while(pos.pos_mapa % num != 0 && pos.pos_instrucao != 0){

                    write_instruction(mapa,pos,0);
                    if(pos.pos_instrucao == 1){
                        pos.pos_instrucao = 0;
                        pos.pos_mapa++;
                    }else{
                        pos.pos_instrucao = 1;
                    }
                }

            // Diretiva .wfill
            }else if(strcmp(token,".wfill") == 0){
                int num = parse_num(strtok(NULL," \t,"));
                long long data;
                Registro reg;
                token = strtok(NULL," \t");
                if(first){
                    data = 0;
                }else if(ConsultaTabela(symbols,token,&reg)){
                    data = (long long) reg.val;
                }else if(ConsultaTabela(rotulos,token,&reg)){
                    data = (long long) reg.val;
                }

                for(int i = 0; i < data; i++){
                    write_word(mapa,pos,num);
                    pos.pos_mapa++;
                }

            // Diretiva .word
            }else if(strcmp(token,".word") == 0){
                long long data;
                Registro reg;
                token = strtok(NULL," \t");
                if(first){
                    data = 0;
                }else if(ConsultaTabela(symbols,token,&reg)){
                    data = (long long) reg.val;
                }else if(ConsultaTabela(rotulos,token,&reg)){
                    data = (long long) reg.val;
                }
                write_word(mapa,pos,data);
                pos.pos_mapa++;
            }

        }else if(strcmp(token,"ADD")){
        }
    }


    return NULL;
}

