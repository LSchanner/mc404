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

void instrucao_com_argumento(long codigo, MemoryMapImpl, PosicaoMontagem* pos,bool first,Tabela rotulos); 

void instrucao_sem_argumento(long codigo, MemoryMapImpl, PosicaoMontagem* pos,bool first,Tabela rotulos); 

void erro(String message);

MemoryMap parse_ias_string(String* lines, Tabela rotulos,Tabela symbols,bool first){
    char* token;
    PosicaoMontagem pos = {0,0};
    MemoryMapImpl mapa;
    int linenum = 0;

    while(lines[linenum] != NULL){
        String line = (String) malloc(sizeof(char) * strlen(lines[linenum]));
        strcpy(line,lines[linenum]);
        token = strtok(line," \t");

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
                    erro("Simbolo inválido");
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

        // Instruções com argumento
        }else if(strcmp(token,"LD")){
            instrucao_com_argumento(0x01, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"LD-")){
            instrucao_com_argumento(0x02, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"LD|")){
            instrucao_com_argumento(0x03, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"LDmq_mx")){
            instrucao_com_argumento(0x09, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"ST")){
            instrucao_com_argumento(0x21, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"ADD")){
            instrucao_com_argumento(0x05, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"ADD|")){
            instrucao_com_argumento(0x07, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"SUB")){
            instrucao_com_argumento(0x06, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"SUB|")){
            instrucao_com_argumento(0x08, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"MUL")){
            instrucao_com_argumento(0x0b, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"DIF")){
            instrucao_com_argumento(0x0c, mapa, &pos, first, rotulos);

        // Instruções sem argumentos
        }else if(strcmp(token,"LDmq")){
            instrucao_sem_argumento(0x0a, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"LSH")){
            instrucao_sem_argumento(0x14, mapa, &pos, first, rotulos);
        }else if(strcmp(token,"RSH")){
            instrucao_sem_argumento(0x15, mapa, &pos, first, rotulos);

        // Casos especiais
        }else if(strcmp(token,"JMP")){
            long codigo = 0x0d << 12;
            Registro reg;
            String token = strtok(NULL," \t");
            if(!first){
                if(ConsultaTabela(rotulos,token,&reg)){
                    PosicaoMontagem pos_jump =((PosicaoMontagem) reg.val);
                    if(pos_jump.pos_instrucao == 0){
                        codigo = 0x0d; 
                    }else{
                        codigo = 0x0e; 
                    }
                    codigo = codigo << 12;
                    codigo += pos_jump.pos_mapa;
                }else{
                    codigo = 0x0d; 
                    codigo = codigo << 12;
                    codigo += parse_num(token);
                }
            }
            write_instruction(mapa,pos,codigo);
            if(pos.pos_instrucao == 1){
                pos.pos_instrucao = 0;
                pos.pos_mapa++;
            }else{
                pos.pos_instrucao = 1;
            }
        }else if(strcmp(token,"JMP+")){
            long codigo = 0x0F << 12;
            Registro reg;
            String token = strtok(NULL," \t");
            if(!first){
                if(ConsultaTabela(rotulos,token,&reg)){
                    PosicaoMontagem pos_jump =((PosicaoMontagem) reg.val);
                    if(pos_jump.pos_instrucao == 0){
                        codigo = 0x0F; 
                    }else{
                        codigo = 0x10; 
                    }
                    codigo = codigo << 12;
                    codigo += pos_jump.pos_mapa;
                }else{
                    codigo = 0x0F; 
                    codigo = codigo << 12;
                    codigo += parse_num(token);
                }
            }
            write_instruction(mapa,pos,codigo);
            if(pos.pos_instrucao == 1){
                pos.pos_instrucao = 0;
                pos.pos_mapa++;
            }else{
                pos.pos_instrucao = 1;
            }
        }else if(strcmp(token,"STaddr")){
            long codigo = 0x12 << 12;
            Registro reg;
            String token = strtok(NULL," \t");
            if(!first){
                if(ConsultaTabela(rotulos,token,&reg)){
                    PosicaoMontagem pos_jump =((PosicaoMontagem) reg.val);
                    if(pos_jump.pos_instrucao == 0){
                        codigo = 0x12; 
                    }else{
                        codigo = 0x13; 
                    }
                    codigo = codigo << 12;
                    codigo += pos_jump.pos_mapa;
                }else{
                    codigo = 0x12; 
                    codigo = codigo << 12;
                    codigo += parse_num(token);
                }
            }
            write_instruction(mapa,pos,codigo);
            if(pos.pos_instrucao == 1){
                pos.pos_instrucao = 0;
                pos.pos_mapa++;
            }else{
                pos.pos_instrucao = 1;
            }
        }



        linenum++;
    }


    return NULL;
}

void instrucao_com_argumento(long codigo, MemoryMapImpl mapa, PosicaoMontagem* pos,bool first,Tabela rotulos){
    codigo = codigo << 12;
    String token = strtok(NULL," \t");
    Registro reg;
    if(!first){
        if(ConsultaTabela(rotulos,token,&reg)){
            if(((PosicaoMontagem) reg.val).pos_instrucao == 1){
                erro("Rótulo inválido para esta operação");
            }
            codigo += ((PosicaoMontagem) reg.val).pos_mapa;
        }else{
            codigo += parse_num(token);
        }
    }
    write_instruction(mapa,*pos,codigo);
    if(pos->pos_instrucao == 1){
        pos->pos_instrucao = 0;
        pos->pos_mapa++;
    }else{
        pos->pos_instrucao = 1;
    }
}

void instrucao_sem_argumento(long codigo, MemoryMapImpl mapa, PosicaoMontagem* pos,bool first,Tabela rotulos){
    codigo = codigo << 12;
    write_instruction(mapa,*pos,codigo);
    if(pos->pos_instrucao == 1){
        pos->pos_instrucao = 0;
        pos->pos_mapa++;
    }else{
        pos->pos_instrucao = 1;
    }
}
