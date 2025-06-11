#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
    TIPO_INTEIRO,
    TIPO_BOOLEANO,
    TIPO_DECIMAL,
    TIPO_TEXTO,
    TIPO_DESCONHECIDO
} TipoDado;



typedef struct {
    long tempo;
    char id[50];
    void* valor;
    TipoDado tipo;
} Registro;

typedef struct {
    char nome[50];
    Registro** dados;
    int qtd;
    int tam;
} Sensor;





TipoDado pegaTipos(const char* str) {
    if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0) return TIPO_BOOLEANO;
    char* resto;
    strtol(str, &resto, 10);
    if (*resto == '\0') return TIPO_INTEIRO;
    strtof(str, &resto);
    if (*resto == '\0') return TIPO_DECIMAL;
    return TIPO_TEXTO;
}



void iniciaSensores(Sensor* s, const char* nome) {
    strcpy(s->nome, nome);
    s->qtd = 0;
    s->tam = 10;
    s->dados = malloc(s->tam * sizeof(Registro*));
}



void adicionarDado(Sensor* s, Registro* r) {
    if (s->qtd == s->tam) {
        s->tam *= 2;
        s->dados = realloc(s->dados, s->tam * sizeof(Registro*));
    }
    s->dados[s->qtd++] = r;
}




void* pegarValores(const char* str, TipoDado tipo) {
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
        default:
            break;
    }
    return dado;
}




void ordenacao(Registro** lista, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (lista[j]->tempo > lista[j + 1]->tempo) {
                Registro* aux = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = aux;
            }
        }
    }
}




void liberar(Sensor* s) {
    for (int i = 0; i < s->qtd; i++) {
        if (s->dados[i]->valor) {
            free(s->dados[i]->valor);
        }
        free(s->dados[i]);
    }
    free(s->dados);
}




int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    FILE* arquivo = fopen(argv[1], "r");
    if (!arquivo) {
        printf("Erro ao abrir arquivo\n");
        return 1;
    }

    Sensor* lista[100];
    int total = 0;
    char linha[256];

    while (fgets(linha, sizeof(linha), arquivo)) {
        long t;
        char id[50], val[100];

        if (sscanf(linha, "%lld %s %s", &t, id, val) != 3) continue;

        TipoDado tipo = pegaTipos(val);
        void* dado = pegarValores(val, tipo);
        if (!dado) continue;



        Registro* novo = malloc(sizeof(Registro));
        novo->tempo = t;
        strcpy(novo->id, id);
        novo->valor = dado;
        novo->tipo = tipo;

        int pos = -1;
        for (int i = 0; i < total; i++) {
            if (strcmp(lista[i]->nome, id) == 0) {
                pos = i;
                break;
            }
        }



        if (pos == -1) {
            if (total >= 100) continue;
            lista[total] = malloc(sizeof(Sensor));
            iniciaSensores(lista[total], id);
            pos = total;
            total++;
        }

        adicionarDado(lista[pos], novo);
    }
    fclose(arquivo);



    for (int i = 0; i < total; i++) {
        Sensor* s = lista[i];
        ordenacao(s->dados, s->qtd);

        char nome[100];
        sprintf(nome, "%s.txt", s->nome);
        FILE* out = fopen(nome, "w");
        if (!out) continue;

        for (int j = 0; j < s->qtd; j++) {
            Registro* r = s->dados[j];
            fprintf(out, "%lld %s ", r->tempo, r->id);
            switch (r->tipo) {
                case TIPO_INTEIRO:
                    fprintf(out, "%d\n", *((int*)r->valor)); break;
                case TIPO_BOOLEANO:
                    fprintf(out, "%s\n", *((bool*)r->valor) ? "true" : "false"); break;
                case TIPO_DECIMAL:
                    fprintf(out, "%.2f\n", *((float*)r->valor)); break;
                case TIPO_TEXTO:
                    fprintf(out, "%s\n", (char*)r->valor); break;
                default: break;
            }
        }



        fclose(out);
        liberar(s);
        free(s);
    }

    return 0;
}
