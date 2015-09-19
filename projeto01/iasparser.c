#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "iasparser.h"
#include "hashtable.h"

// Funções utilitárias
bool match_rotulo(String);

bool match_symbol(String);

int parse_num(String);

void write_word(MemoryMap* map,PosicaoMontagem pos,long long data);

void write_instruction(MemoryMap* map,PosicaoMontagem pos,long data);

void instrucao_com_argumento(long codigo, MemoryMap* map, PosicaoMontagem* pos,bool first,Tabela rotulos); 

void instrucao_sem_argumento(long codigo, MemoryMap* map, PosicaoMontagem* pos,bool first,Tabela rotulos); 

void instrucao_com_condicional(long codigo1,long codigo2, MemoryMap* map, PosicaoMontagem* pos,bool first,Tabela rotulos); 

void erro(String message);

void upper_case(String str);


// Variável global de linha. Serve para que qualquer função possa chamar 
// erro() sem nencessidade de ter o número da linha incluso entre seus 
// argumentos
int linenum;

MemoryMap parse_ias_string(String* lines, Tabela rotulos,Tabela symbols,bool first){
    char* token;
    PosicaoMontagem pos = {0,0};
    MemoryMap mapa;
    linenum = 0;
    
    // Inicializa o mapa de memória
    memset(&mapa,0,sizeof(mapa));

    while(lines[linenum] != NULL){

        // strtok() destrói a string, então primeiro fazemos uma cópia
        String line = (String) malloc(sizeof(char) * (strlen(lines[linenum]) + 1));
        strcpy(line,lines[linenum]);
        token = strtok(line," \t");

        // Adiciona um rótulo na hashtable
        if(token && match_rotulo(token)){
            upper_case(token);
            Registro rotulo = {token, pos};
            InsereTabela(rotulos, rotulo);
            token = strtok(NULL," \t");
        }

        // Diretivas
        if(token && token[0] == '.'){
            // Diretiva .org
            if(strcmp(token,".org") == 0){
                token = strtok(NULL," \t");
                pos.pos_mapa = parse_num(token);
                pos.pos_instrucao = 0;

            // Diretiva .set
            }else if(strcmp(token,".set") == 0){
                String symbol = strtok(NULL," \t");

                if(match_symbol(symbol)){

                    // Guardamos todos símbolos como sendo maísculo,
                    // para ficar case-insensitive 
                    upper_case(symbol);
                    long long value = parse_num(strtok(NULL," \t"));
                    Registro macro = {symbol,{value,0}};
                    InsereTabela(symbols,macro);

                }else{
                    erro("Simbolo inválido");
                }


            // Diretiva .align
            }else if(strcmp(token,".align") == 0){
                int num = parse_num(strtok(NULL," \t"));
                while(pos.pos_mapa % num != 0 || pos.pos_instrucao != 0){

                    if(pos.pos_instrucao == 1){
                        pos.pos_instrucao = 0;
                    }
                    pos.pos_mapa++;
                }

                // Diretiva .wfill
            }else if(strcmp(token,".wfill") == 0){
                token = strtok(NULL," \t");

                if(!token){
                    erro("Argumento esperado");
                }

                long long data;
                int num = parse_num(token);
                Registro reg;
                token = strtok(NULL," \t");

                if(!token){
                    erro("Argumento esperado");
                }

                // Utilizamos a mesma implementação de hashtable para
                // rotulos e symbolos. 
                // O valor do símbolo é guardada no campo "pos_mapa"
                // Convertemos para upper case para deixar case_insensitive
                upper_case(token);
                if(first){
                    data = 0;
                }else if(ConsultaTabela(symbols,token,&reg)){
                    data = reg.val.pos_mapa;

                }else if(ConsultaTabela(rotulos,token,&reg)){
                    data = reg.val.pos_mapa;

                }else{
                    data = parse_num(token);

                }

                for(int i = 0; i < num; i++){
                    write_word(&mapa,pos,data);
                    pos.pos_mapa++;

                }

                // Diretiva .word
            }else if(strcmp(token,".word") == 0){
                long long data;
                Registro reg;
                token = strtok(NULL," \t");

                if(!token){
                    erro("Argumento esperado");
                }

                // Convertemos o token para upper case pq tanto
                // símbolo quanto rótulo são case insensitive
                upper_case(token);
                if(first){
                    data = 0;
                }else if(ConsultaTabela(symbols,token,&reg)){
                    data = reg.val.pos_mapa;
                }else if(ConsultaTabela(rotulos,token,&reg)){
                    data = reg.val.pos_mapa;
                }else{
                    data = parse_num(token);
                }

                write_word(&mapa,pos,data);
                pos.pos_mapa++;
            }else{
                erro("Diretiva inválida");
            }

            // Instruções com argumento
        }else if(token){
            if(!strcmp(token,"LD")){
                instrucao_com_argumento(0x01, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"LD-")){
                instrucao_com_argumento(0x02, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"LD|")){
                instrucao_com_argumento(0x03, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"LDmq_mx")){
                instrucao_com_argumento(0x09, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"ST")){
                instrucao_com_argumento(0x21, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"ADD")){
                instrucao_com_argumento(0x05, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"ADD|")){
                instrucao_com_argumento(0x07, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"SUB")){
                instrucao_com_argumento(0x06, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"SUB|")){
                instrucao_com_argumento(0x08, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"MUL")){
                instrucao_com_argumento(0x0b, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"DIV")){
                instrucao_com_argumento(0x0c, &mapa, &pos, first, rotulos);

                // Instruções sem argumentos
            }else if(!strcmp(token,"LDmq")){
                instrucao_sem_argumento(0x0a, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"LSH")){
                instrucao_sem_argumento(0x14, &mapa, &pos, first, rotulos);
            }else if(!strcmp(token,"RSH")){
                instrucao_sem_argumento(0x15, &mapa, &pos, first, rotulos);

                // Casos especiais
            }else if(!strcmp(token,"JMP")){
                instrucao_com_condicional(0x0D, 0x0E, &mapa, &pos,first,rotulos);
            }else if(!strcmp(token,"JUMP+")){
                instrucao_com_condicional(0x0F, 0x10, &mapa, &pos,first,rotulos);
            }else if(!strcmp(token,"STaddr")){
                instrucao_com_condicional(0x12, 0x13, &mapa, &pos,first,rotulos);
            }else{
                erro("instrução não existente");
            }
        }

        if(strtok(NULL," \t")){
            erro("Token inesperado");
        }
        free(line);
        linenum++;
    }

    return mapa;
}

void instrucao_com_argumento(long codigo, MemoryMap* mapa, PosicaoMontagem* pos,bool first,Tabela rotulos){
    codigo = codigo << 12;
    String token = strtok(NULL," \t");
    Registro reg;

    if(!token){
        erro("Argumento esperado");
    }
    if(token[0] != '"' || token[strlen(token)-1] != '"'){
        erro("O argumento da instrucao deve estar envolto em aspas");
    }

    // remove as aspas do começo da string
    token++;

    // as aspas do final viram :, para podermos buscar na hashtable
    token[strlen(token)-1] = ':';

    upper_case(token);
    if(!first){
        if(ConsultaTabela(rotulos,token,&reg)){
            codigo += ((PosicaoMontagem) reg.val).pos_mapa;
        }else{
            // Tiramos o :, para parsear um número
            token[strlen(token)-1] = 0;
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

void instrucao_sem_argumento(long codigo, MemoryMap* mapa, PosicaoMontagem* pos,bool first,Tabela rotulos){
    codigo = codigo << 12;
    write_instruction(mapa,*pos,codigo);
    if(pos->pos_instrucao == 1){
        pos->pos_instrucao = 0;
        pos->pos_mapa++;
    }else{
        pos->pos_instrucao = 1;
    }
}

void instrucao_com_condicional(long cod_esq, long cod_dir,MemoryMap* mapa, PosicaoMontagem* pos,bool first,Tabela rotulos){
    long codigo = cod_esq  << 12;
    Registro reg;
    String token = strtok(NULL," \t");
    if(!token){
        erro("Argumento esperado");
    }
    if(token[0] != '"' || token[strlen(token)-1] != '"'){
        erro("O argumento da instrucao deve estar envolto em aspas");
    }

    // remove as aspas do começo da string
    token++;

    // as aspas do final viram :, para podermos buscar na hashtable
    token[strlen(token)-1] = ':';


    upper_case(token);
    if(!first){
        // A instrução a ser escrita depende do rótulo usado como 
        // argumento
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

            // Tiramos o :, para parsear um número
            token[strlen(token)-1] = 0;

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

// Retorna true se um caracter for alfanumérico
bool alfanumerico(char c){
    return  (c < 91 && c > 65) ||
            (c > 96 && c < 123) || 
            (c > 47 && c < 58) ||
            (c == 95);
}

// Converte String para maiúsculas
void upper_case(String str){
    while(*str){
        *str = toupper(*str);
        str++;
    }
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
    if(!input){
        erro("Número esperado");
    }

    int base = 10;
    int pos = 0;

    // Primeiro testamos a hipótese de ser um número deciaml
    while(input[pos] && base == 10){
        if(input[pos] < '0' || input[pos] > '9'){
            base = 16;
        }
        pos++;
    }

    // Agora testamos se é hexadecimal. Base 0 indica um dígito inválido
    if(base == 16){
        if(input[0] != '0' || (input[1] != 'X' && input[1] != 'x')){
            base = 0;
        }
        pos = 2;
        while(input[pos]){
            if((input[pos] < '0' || input[pos] > '9') &&
                    (input[pos] < 'A' || input[pos] > 'F') &&
                    (input[pos] < 'a' || input[pos] > 'f')){
                base = 0;
            }
            pos++;
        }
    }


    if(base == 0){
        erro("Número inválido");
    }

    

    return strtol(input,NULL,0);
}

// Escreve em um palavra de memória.
void write_word(MemoryMap* map,PosicaoMontagem pos,long long data){
    if(pos.pos_mapa > 1023){
        erro("Memória lotada");
    }
    if(pos.pos_instrucao == 1){
        erro(" Não é possível escrever em memória desalinhada");
    }
    if(map->map[pos.pos_mapa].used_esq || map->map[pos.pos_mapa].used_dir){
        erro(" Posição de memória já utilizada");
    }else{
        map->map[pos.pos_mapa].dado = data;
        map->map[pos.pos_mapa].used_dir = map->map[pos.pos_mapa].used_esq = true;
        map->map[pos.pos_mapa].is_data = true;
    }

}

// Escreve em um instrução na memória.
void write_instruction(MemoryMap* map,PosicaoMontagem pos,long data){
    if(pos.pos_mapa > 1023){
        erro("Memória lotada");
    }
    if(pos.pos_instrucao == 0){
        if(map->map[pos.pos_mapa].used_esq){
            erro(" Posição de memória já utilizada");
        }
        else{
            map->map[pos.pos_mapa].instrucoes[0] = data;  
            map->map[pos.pos_mapa].used_esq = true;
        }
    }
    if(pos.pos_instrucao == 1){
        if(map->map[pos.pos_mapa].used_dir){
            erro(" Posição de memória já utilizada");
        }
        else{
            map->map[pos.pos_mapa].instrucoes[1] = data;  
            map->map[pos.pos_mapa].used_dir = true;
        }
    }

}

void erro(String message){
    fprintf(stderr,"ERROR on line %d\n",linenum + 1);
    fprintf(stderr,"-----------------------\n");
    fprintf(stderr,"%s\n",message);
    exit(1);
}

MemoryMap assemble_ias(String input){
    Tabela rotulos = CriaTabela();
    Tabela symbols = CriaTabela();

    // Conta o número de linhas
    int numlines = 1;
    int pos = 0;
    while(input[pos]){
        if(input[pos] == '\n'){
            numlines++;
        }
        pos++;
    }

    // Aloca um vetor de strings do tamanho do número de linhas
    String* lines = (String*) malloc(sizeof(String) *( numlines + 1));

    // Copia o input para uma outra posição de memória, para usarmos strtok
    String inputcopy = (String) malloc(sizeof(char) * (strlen(input) + 1));
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

        lines[numlines] = (String) malloc(sizeof(char) * (strlen(line) + 1));
        strcpy(lines[numlines],line);

        line = strtok(NULL,"\n");
        numlines++;
    }

    // A última posição do vetor é zerada para indicar 
    lines[numlines] = NULL;

    // Passa o parseador pela primeira vez, para construir a tabela
    // de Rótulos
    parse_ias_string(lines,rotulos,symbols,true);
    LiberaTabela(symbols);

    // Passa o parseador a segunda vez, para construir o mapa de memória
    symbols = CriaTabela();
    MemoryMap mapa = parse_ias_string(lines,rotulos,symbols,false);

    // Libera a memória
    LiberaTabela(rotulos);
    LiberaTabela(symbols);

    for(int i = 0; i < numlines; i++){
        free(lines[i]);
    }
    free(lines);
    free(inputcopy);

    return mapa;
}

// Printa o mapa de memória
void printMemoryMap(MemoryMap object,FILE* target){
    for(int i = 0; i < 1024; i++){

        if(object.map[i].used_dir || object.map[i].used_esq){

            fprintf(target,"%03X ",i);
            if(object.map[i].is_data){

                // essas operações bitwise servem para selecionar os bits mais significativos e menos signitficativos
                long long data = object.map[i].dado;
                fprintf(target,"%02llX %03llX %02llX %03llX",data >> 32, (data >> 20) & 0xfff ,(data >> 12) & 0xff, data & 0xfff);
            }else{

                // essas operações bitwise servem para selecionar os bits mais significativos e menos signitficativos
                fprintf(target, "%02lX %03lX ",object.map[i].instrucoes[0] >> 12, object.map[i].instrucoes[0] & 0xfff);
                fprintf(target, "%02lX %03lX",object.map[i].instrucoes[1] >> 12, object.map[i].instrucoes[1] & 0xfff);
            }
            fprintf(target,"\n");
        }
    }
}
