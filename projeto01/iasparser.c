#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iasparser.h"
#include "hashtable.h"

typedef struct Palavra{
    union{
        long long dado;
        long instrucoes[2];
    };
    bool used_esq,used_dir;
}Palavra;

typedef struct MemoryMapImpl{
    Palavra map[2048];
}MemoryMapImpl;

// Funções utilitárias
bool match_rotulo(String);

bool match_symbol(String);

int parse_num(String);

void write_word(MemoryMapImpl map,PosicaoMontagem pos,long long data);

void write_instruction(MemoryMapImpl map,PosicaoMontagem pos,long data);

void instrucao_com_argumento(long codigo, MemoryMapImpl, PosicaoMontagem* pos,bool first,Tabela rotulos); 

void instrucao_sem_argumento(long codigo, MemoryMapImpl, PosicaoMontagem* pos,bool first,Tabela rotulos); 

void instrucao_com_condicional(long codigo1,long codigo2, MemoryMapImpl, PosicaoMontagem* pos,bool first,Tabela rotulos); 

void erro(String message);

MemoryMap parse_ias_string(String* lines, Tabela rotulos,Tabela symbols,bool first){
    char* token;
    PosicaoMontagem pos = {0,0};
    MemoryMapImpl mapa;
    int linenum = 0;
    
    // Inicializa o mapa de memória
    memset(&mapa,0,sizeof(mapa));

    while(lines[linenum] != NULL){
        String line = (String) malloc(sizeof(char) * strlen(lines[linenum]));
        strcpy(line,lines[linenum]);
        token = strtok(line," \t");

        if(match_rotulo(token)){
            Registro rotulo = {token, pos};
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
                    Registro macro = {symbol,{value,0}};
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

                    // Utilizamos a mesma implementação de hashtable para rotulos e symbolos
                }else if(ConsultaTabela(symbols,token,&reg)){
                    data = reg.val.pos_mapa;
                }else if(ConsultaTabela(rotulos,token,&reg)){
                    data = reg.val.pos_mapa;
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
                    data = reg.val.pos_mapa;
                }else if(ConsultaTabela(rotulos,token,&reg)){
                    data = reg.val.pos_mapa;
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
            instrucao_com_condicional(0x0D, 0x0E, mapa, &pos,first,rotulos);
        }else if(strcmp(token,"JMP+")){
            instrucao_com_condicional(0x0F, 0x10, mapa, &pos,first,rotulos);
        }else if(strcmp(token,"STaddr")){
            instrucao_com_condicional(0x12, 0x13, mapa, &pos,first,rotulos);
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

void instrucao_com_condicional(long cod_esq, long cod_dir,MemoryMapImpl mapa, PosicaoMontagem* pos,bool first,Tabela rotulos){
    long codigo = cod_esq  << 12;
    Registro reg;
    String token = strtok(NULL," \t");
    if(!first){
        if(ConsultaTabela(rotulos,token,&reg)){
            PosicaoMontagem pos_jump =((PosicaoMontagem) reg.val);
            if(pos_jump.pos_instrucao == 0){
                codigo = cod_esq; 
            }else{
                codigo = cod_dir; 
            }
            codigo = codigo << 12;
            codigo += pos_jump.pos_mapa;
        }else{
            codigo = cod_esq; 
            codigo = codigo << 12;
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

bool alfanumerico(char c){
    return  (c < 91 && c > 65) ||
            (c > 96 && c < 123) || 
            (c > 47 && c < 58) ||
            (c == 95);
}
bool match_rotulo(String rot){

    while(*rot){
        if(!alfanumerico(*rot)) {
            break;
        }
        rot++;
    }
    if(*rot == ':'){
        rot++;
        return *rot == 0;

    }else{
        return false;
    }
}

bool match_symbol(String sym){
    while(*sym){
        if(!alfanumerico(*sym)){
            break;
        }
        sym++;
    }
    return *sym == 0;
}

int parse_num(String input){
    int pos = 0;

    // Remove aspas
    if(input[pos] == '"'){
        input++;
        while(input[pos] != '"'){
            pos++;
        }
        if(input[pos+1] == 0){
            input[pos] = 0;
        }
    }

    return strtol(input,NULL,0);
}

void write_word(MemoryMapImpl map,PosicaoMontagem pos,long long data){
    if(pos.pos_instrucao == 1){
        erro(" Não é possível escrever em memória desalinhada");
    }
    if(map.map[pos.pos_mapa].used_esq || map.map[pos.pos_mapa].used_dir){
        erro(" Posição de memória já utilizada");
    }else{
        map.map[pos.pos_mapa].dado = data;
        map.map[pos.pos_mapa].used_dir = map.map[pos.pos_mapa].used_esq = true;
    }

}

void write_instruction(MemoryMapImpl map,PosicaoMontagem pos,long data){
    if(pos.pos_instrucao == 0){
        if(map.map[pos.pos_mapa].used_esq){
            erro(" Posição de memória já utilizada");
        }
        else{
            map.map[pos.pos_mapa].instrucoes[0] = data;  
            map.map[pos.pos_mapa].used_esq = true;
        }
    }
    if(pos.pos_instrucao == 1){
        if(map.map[pos.pos_mapa].used_dir){
            erro(" Posição de memória já utilizada");
        }
        else{
            map.map[pos.pos_mapa].instrucoes[1] = data;  
            map.map[pos.pos_mapa].used_dir = true;
        }
    }

}

void erro(String message){
    printf("%s\n",message);
    exit(1);
}

MemoryMap assemble_ias(String input){
    Tabela rotulos = CriaTabela();
    Tabela symbols = CriaTabela();

    // Conta o número de linhas
    int numlines = 0;
    int pos = 0;
    while(input[pos]){
        if(input[pos] == '\n'){
            numlines++;
        }
        pos++;
    }

    // Aloca um vetor de strings do tamanho do número de linhas
    String* lines = (String*) malloc(sizeof(String) * numlines);

    // Copia o input para uma outra posição de memória, para usarmos strtok
    String inputcopy = (String) malloc(sizeof(char) * strlen(input));
    strcpy(inputcopy,input);

    // Para cada linha, copia para o vetor de linhas
    String line = strtok(inputcopy,"\n");
    numlines = 0;
    while(line != NULL){

        // remove comentários
        for(int pos = 0;line[pos];pos++){
            if(line[pos] == '#'){
                line[pos] = 0;
            }
        }

        lines[numlines] = (String) malloc(sizeof(char) * strlen(line));
        strcpy(lines[numlines],line);

        line = strtok(NULL,"\n");
        numlines++;
    }

    return NULL;
}
