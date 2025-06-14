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

typedef struct {
    long tempo;
    char id_sensor[50];
    void* valor;
    TipoDado tipo_valor;
} LeituraSensor;

TipoDado descobreTipos(const char* str) {
    if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0) return TIPO_BOOLEANO;
    char* resto;
    strtol(str, &resto, 10);
    if (*resto == '\0') return TIPO_INTEIRO;
    strtof(str, &resto);
    if (*resto == '\0') return TIPO_DECIMAL;
    return TIPO_TEXTO;
}



int buscaMaisProxima(LeituraSensor** lista, int total, long alvo) {
    int esq = 0, dir = total - 1, melhor = 0;
    long menor = -1;

    while (esq <= dir) {
        int meio = (esq + dir) / 2;
        long t = lista[meio]->tempo;
        long diff = labs(t - alvo);

        if (menor == -1 || diff < menor) {
            menor = diff;
            melhor = meio;
        }

        if (t < alvo)
            esq = meio + 1;
        else if (t > alvo)
            dir = meio - 1;
        else
            return meio;
    }
    return melhor;
}

void mostraLeitura(LeituraSensor* l) {
    printf("%ld %s ", l->tempo, l->id_sensor);
    switch (l->tipo_valor) {
        case TIPO_INTEIRO: printf("%d\n", *((int*)l->valor)); break;
        case TIPO_BOOLEANO: printf("%s\n", *((bool*)l->valor) ? "true" : "false"); break;
        case TIPO_DECIMAL: printf("%.2f\n", *((float*)l->valor)); break;
        case TIPO_TEXTO: printf("%s\n", (char*)l->valor); break;
    }
}

void libera(LeituraSensor* l) {
    if (l->valor) free(l->valor);
}

long converteTimestamp(const char* data, const char* hora) {
    struct tm tm = {0};
    if (sscanf(data, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) != 3) return -1;
    if (sscanf(hora, "%d:%d:%d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 3) return -1;
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    return mktime(&tm);
}

void* leValores(const char* str, TipoDado tipo) {
    void* dado = NULL;
    switch (tipo) {
        case TIPO_INTEIRO:
            dado = malloc(sizeof(int));
            if (dado) *(int*)dado = atoi(str); break;
        case TIPO_BOOLEANO:
            dado = malloc(sizeof(bool));
            if (dado) *(bool*)dado = (strcmp(str, "true") == 0); break;
        case TIPO_DECIMAL:
            dado = malloc(sizeof(float));
            if (dado) *(float*)dado = atof(str); break;
        case TIPO_TEXTO:
            dado = malloc(strlen(str) + 1);
            if (dado) strcpy((char*)dado, str); break;
    }
    return dado;
}




int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Uso: %s <sensor> <data AAAA-MM-DD> <hora HH:MM:SS>\n", argv[0]);
        return 1;
    }

    long tempo_desejado = converteTimestamp(argv[2], argv[3]);
    if (tempo_desejado == -1) {
        printf("data ou hora inválida. Faça no formato -> AAAA-MM-DD HH:MM:SS\n");
        return 1;
    }

    char nomeArquivo[100];
    sprintf(nomeArquivo, "%s.txt", argv[1]);

    FILE* arq = fopen(nomeArquivo, "r");
    if (!arq) {
        printf("Arquivo %s não encontrado.\n", nomeArquivo);
        return 1;
    }

    LeituraSensor** leituras = malloc(10000 * sizeof(LeituraSensor*));
    int qtd = 0;
    char linha[256];

    while (fgets(linha, sizeof(linha), arq)) {
        long t;
        char id[50], valor[100];
        if (sscanf(linha, "%ld %49s %99s", &t, id, valor) != 3) {
            printf("Linha nao existe: %s", linha);
            continue;
        }

        TipoDado tipo = descobreTipos(valor);
        void* dado = leValores(valor, tipo);
        if (!dado) continue;

        LeituraSensor* nova = malloc(sizeof(LeituraSensor));
        nova->tempo = t;
        strcpy(nova->id_sensor, id);
        nova->valor = dado;
        nova->tipo_valor = tipo;

        leituras[qtd++] = nova;
    }
    fclose(arq);

    if (qtd == 0) {
        printf("Nenhuma leitura encontrada.\n");
        free(leituras);
        return 0;
    }

    int pos = buscaMaisProxima(leituras, qtd, tempo_desejado);
    mostraLeitura(leituras[pos]);

    for (int i = 0; i < qtd; i++) {
        libera(leituras[i]);
        free(leituras[i]);
    }
    free(leituras);
    return 0;
}
