#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
    TIPO_INTEIRO,
    TIPO_BOOLEANO,
    TIPO_DECIMAL,
    TIPO_TEXTO
} TipoDado;

typedef struct {
    long tempo;
    char id_sensor[50];
    void* valor;
    TipoDado tipo_valor;
} LeituraSensor;

void* leValores(const char* str, TipoDado tipo) {
    void* dado = NULL;
    switch (tipo) {
        case TIPO_INTEIRO:
            dado = malloc(sizeof(int));
            if (dado) *(int*)dado = atoi(str);
            break;
        case TIPO_BOOLEANO:
            dado = malloc(sizeof(bool));
            if (dado) *(bool*)dado = (strcmp(str, "true") == 0 || strcmp(str, "1") == 0);
            break;
        case TIPO_DECIMAL:
            dado = malloc(sizeof(float));
            if (dado) *(float*)dado = atof(str);
            break;
        case TIPO_TEXTO:
            dado = malloc(strlen(str) + 1);
            if (dado) strcpy((char*)dado, str);
            break;
    }
    return dado;
}

TipoDado descobreTipos(const char* str) {
    if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0) return TIPO_BOOLEANO;
    char* resto;
    strtol(str, &resto, 10);
    if (*resto == '\0') return TIPO_INTEIRO;
    strtof(str, &resto);
    if (*resto == '\0') return TIPO_DECIMAL;
    return TIPO_TEXTO;
}

void liberaValores(LeituraSensor* leitura) {
    if (leitura && leitura->valor) {
        free(leitura->valor);
        leitura->valor = NULL;
    }
}

void mostraLeituras(LeituraSensor* leitura) {
    printf("%ld %s ", leitura->tempo, leitura->id_sensor);
    switch (leitura->tipo_valor) {
        case TIPO_INTEIRO:
            printf("%d\n", *((int*)leitura->valor)); break;
        case TIPO_BOOLEANO:
            printf("%s\n", *((bool*)leitura->valor) ? "true" : "false"); break;
        case TIPO_DECIMAL:
            printf("%.2f\n", *((float*)leitura->valor)); break;
        case TIPO_TEXTO:
            printf("%s\n", (char*)leitura->valor); break;
        default:
            printf("tipo inválido\n"); break;
    }
}
