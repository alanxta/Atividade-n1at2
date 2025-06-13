#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sensor_data.c"

int buscaLeituraAproximds(LeituraSensor** lista, int total,long tempo_alvo) {
    int esquerda = 0, direita = total - 1;
    int maisProxima = 0;
    long menorDif = -1;
 
    while (esquerda <= direita) {
        int meio = (esquerda + direita) / 2;
        long long t = lista[meio]->tempo;
        long long dif = llabs(t - tempo_alvo);

        if (menorDif == -1 || dif < menorDif) {
            menorDif = dif;
            maisProxima = meio;
        }

        if (t < tempo_alvo) {
            esquerda = meio + 1;
        } else if (t > tempo_alvo) {
            direita = meio - 1;
        } else {
            return meio;
        }
    }

    return maisProxima;
}





int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <id_sensor> <timestamp>\n", argv[0]);
        return 1;
    }

    char nomeArquivo[100];
    sprintf(nomeArquivo, "%s.txt", argv[1]);
    long long tempo_desejado = atoll(argv[2]);

    FILE* arq = fopen(nomeArquivo, "r");
    if (!arq) {
        printf("Arquivo %s não encontrado.\n", nomeArquivo);
        return 1;
    }

    LeituraSensor** leituras = malloc(10000 * sizeof(LeituraSensor*));
    int qtd = 0;

    char linha[256];
    while (fgets(linha, sizeof(linha), arq)) {
        long long tempo;
        char id[50], valor[100];
        if (sscanf(linha, "%lld %s %s", &tempo, id, valor) != 3){ 
            continue;
        }

        TipoDado tipo = descobreTipos(valor);
        void* dado = leValores(valor, tipo);
        if (!dado) continue;

        LeituraSensor* nova = malloc(sizeof(LeituraSensor));
        nova->tempo = tempo;
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

    int posicao = buscaLeituraAproximds(leituras, qtd, tempo_desejado);
    mostraLeituras(leituras[posicao]);

    for (int i = 0; i < qtd; i++) {
        liberaValores(leituras[i]);
        free(leituras[i]);
    }
    free(leituras);

    return 0;
}
