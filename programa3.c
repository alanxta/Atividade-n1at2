
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
    TIPO_INTEIRO,
    TIPO_BOOLEANO,
    TIPO_DECIMAL,
    TIPO_TEXTO
} TipoDado;



float geraDecimais() { return (float)(rand() % 10000) / 10.0; }
int geraInteiros() { return rand() % 1000; }
bool geraBooleanos() { return rand() % 2 == 0; }


void geraTextos(char* destino, int tam) {
    const char* letras = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < tam; i++) {
        destino[i] = letras[rand() % 52];
    }
    destino[tam] = '\0';
}

long converteTimestamp(const char* data, const char* hora) {
    struct tm tm = {0};
    if (sscanf(data, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) != 3){ 
        return -1;
    }
    if (sscanf(hora, "%d:%d:%d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 3){
        return -1;
    }
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    return mktime(&tm);
}



int main(int argc, char* argv[]) {
    if (argc < 7 || (argc - 5) % 2 != 0) {
        printf("Uso: %s <data_inicio> <hora_inicio> <data_fim> <hora_fim> <sensor1 tipo1> [sensor2 tipo2] ...\n", argv[0]);
        return 1;
    }

    long inicio = converteTimestamp(argv[1], argv[2]);
    long fim = converteTimestamp(argv[3], argv[4]);
    if (inicio == -1 || fim == -1 || inicio >= fim) {
        printf("Erro: intervalo de tempo inválido ou datas mal formatadas.\n");
        return 1;
    }

    int sensores = (argc - 5) / 2;
    char ids[sensores][50];
    TipoDado tipos[sensores];

    for (int i = 0; i < sensores; i++) {
        strcpy(ids[i], argv[5 + i * 2]);
        char* tipo = argv[6 + i * 2];

        if (strcmp(tipo, "int") == 0){
            tipos[i] = TIPO_INTEIRO;
        } 
        else if (strcmp(tipo, "bool") == 0){
            tipos[i] = TIPO_BOOLEANO;
        } 
        else if (strcmp(tipo, "float") == 0){
            tipos[i] = TIPO_DECIMAL;
        } 
        else if (strcmp(tipo, "string") == 0){
            tipos[i] = TIPO_TEXTO;
        } 
        else {
            printf("Tipo inválido: %s\n", tipo);
            return 1;
        }
    }

    FILE* arquivo = fopen("dados.txt", "w");
    if (!arquivo) {
        printf("Erro ao criar arquivo de teste.\n");
        return 1;
    }

    srand(time(NULL));

    for (int i = 0; i < sensores; i++) {
        for (int j = 0; j < 2000; j++) {
            long tempo = inicio + rand() % (fim - inicio + 1);
            fprintf(arquivo, "%ld %s ", tempo, ids[i]);

            switch (tipos[i]) {
                case TIPO_INTEIRO:
                    fprintf(arquivo, "%d\n", geraInteiros()); break;
                case TIPO_BOOLEANO:
                    fprintf(arquivo, "%s\n", geraBooleanos() ? "true" : "false"); break;
                case TIPO_DECIMAL:
                    fprintf(arquivo, "%.2f\n", geraDecimais()); break;
                case TIPO_TEXTO: {
                    char txt[17];
                    geraTextos(txt, 16);
                    fprintf(arquivo, "%s\n", txt); break;
                }
            }
        }
    }

    fclose(arquivo);
    printf("Arquivo 'dados.txt' feito .\n");
    return 0;
}
