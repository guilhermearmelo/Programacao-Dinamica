#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int valor;
    int altura;
    int largura;
    int profundidade;
    int rotacao;
} Caixa;

// Apenas faz a leitura do arquivo de entrada no formato especificado
Caixa* le_entrada(char *arq_entrada, int *n, int *h) {
    Caixa *caixas;
    FILE *entrada;
    if((entrada = fopen(arq_entrada, "rb")) != NULL ) {

        fscanf(entrada, "%d", n);
        fscanf(entrada, "%d", h);

        caixas = (Caixa *) calloc(*n, sizeof(Caixa));

        for(int i = 0; i < *n; i++)
            fscanf(entrada, "%d", &caixas[i].valor);

        for(int i = 0; i < *n; i++) {
            fscanf(entrada, "%d", &caixas[i].largura);
            fscanf(entrada, "%d", &caixas[i].altura);
            fscanf(entrada, "%d", &caixas[i].profundidade);
            caixas[i].rotacao = 1;
        }
    } else {
        printf("O arquivo especificado nao existe.\n");
        exit(0);
    }

    fclose(entrada);
    return caixas;
}

// Escreve a saida do programa
void escreve_saida(char *arq_saida, int **resultado, Caixa *caixas, int n, int h) {
    FILE *saida;

    if((saida = fopen(arq_saida, "wb")) != NULL) {
        fprintf(saida, "%d\n\n", resultado[h][0]);
        int m = 0;
        int indice_solucao[h+1];

        /* Aqui a reconstrução das caixas é feita do começo para o inicio pois não temos certeza se a caixa armazenada na posição 0 da tabela pertence a solução,
        mas com certeza a caixa que está na posição h da tabela faz. Assim, caminhamos pela tabela de modo a voltar a altura h da caixa atual que estamos analisando
        de tras pra frente, ou seja, da caixa m até a caixa 1 em nossa pilha.
        */
        for(int i = h; i >= 0;)
            if(resultado[i][1] > 0) {
                indice_solucao[m++] = resultado[i][1];
                i = i - caixas[resultado[i][1] - 1].altura;
            } else i--;

        fprintf(saida, "%d\n\n", m);

        // Após encontras as m caixas, elas estarão em ordem invertida. Então precisamos imprimir o reverso do vetor que guardamos quais são as caixas para imprimir corretamente a solução
        for(int i = m-1; i >= 0; i--)
            fprintf(saida, "caixa%d r%d\n\n", (indice_solucao[i] + 1) / 2 , caixas[indice_solucao[i] - 1].rotacao);
    } else {
        printf("O arquivo de saida nao existe.\n");
        exit(0);
    }
    fclose(saida);
}

int** resolve(Caixa caixas[], int n, int h) {
    // Tabela para armazenar os subproblemas da programação dinâmica (Tem que ser h+1 pra salvar o resultado pra cada altura intermediária no intervalo [0, h])
    int **resultado = (int**) calloc(h+1, sizeof(int*));
    // Uma segunda linha é usada para salvar qual a última caixa do empilhamento máximo na altura i 
    for(int i = 0; i <= h; i++) resultado[i] = (int*) calloc(2, sizeof(int));

    // Construiremos a tabela resultado[][] de forma bottom-up
    for(int i = 0; i <= h; i++) { // Percorre cada uma das h alturas
        for(int j = 0; j < n; j++) { // Como pode haver repetição de caixas, percorre sempre todas as n caixas para cada altura i
            if(caixas[j].altura <= i && resultado[i - caixas[j].altura][1] != -1) { // Verifica se a altura é válida ou se a posição é só uma atualização da posição anterior
                if(resultado[i - caixas[j].altura][1] == 0 || (caixas[resultado[i - caixas[j].altura][1] - 1].largura >= caixas[j].largura && caixas[resultado[i - caixas[j].altura][1] - 1].profundidade >= caixas[j].profundidade)) { // Verifica se a posição da Tabela nunca foi utilizada ou se já foi utilizada, o empilhamento desejado é válido pelas restrições de tamanho das caixas
                    if (caixas[j].valor + resultado[i - caixas[j].altura][0] > resultado[i][0]) { // Compara se o resultado atual para a altura i é melhor do que um novo resultado empilhando a caixa j
                        // Caso seja melhor empilhar a caixa j, realiza a atualização dos valores
                        resultado[i][0] = caixas[j].valor + resultado[i - caixas[j].altura][0];
                        resultado[i][1] = j + 1; // As caixas são armazenadas usando a base 1, ou seja a caixa 1 corresponde no vetor a caixas[0]
                    }
                }
            }
        }

        // Confere se o resultado novo que foi gerado é realmente melhor que o resultado anterior
        if(i != 0 && resultado[i][0] < resultado[i-1][0]) {
            resultado[i][0] = resultado[i-1][0];
            resultado[i][1] = -1; // -1 é um flag para falar que não tem caixa no topo do empilhamento, e sim ocorreu uma atualização de valores
        }

    }

    return resultado;
}

// Driver do programa
int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Forma de uso: <executavel> <arquivo_entrada> <arquivo_saida>");
        exit(0);
    }

    int n, h;

    Caixa * caixas = le_entrada(argv[1], &n, &h);

    // Primeiro vamos gerar as rotações para facilitar o processo
    Caixa* rotacoes = (Caixa*) calloc (2*n, sizeof(Caixa)); // Gerar as rotações facilita a resolução do processo
    int indice = 0;

    // Agora rotacoes[0] e rotacoes[1] representam a caixa[0] com as rotações r1 e r2
    for(int i = 0; i < n; i++) {
        // Copia as caixas originais
        rotacoes[indice].valor = caixas[i].valor;
        rotacoes[indice].altura = caixas[i].altura;
        rotacoes[indice].profundidade = caixas[i].profundidade;
        rotacoes[indice].largura = caixas[i].largura;
        rotacoes[indice].rotacao = caixas[i].rotacao;
        indice++;

        // Insere as caixas novas
        rotacoes[indice].valor = caixas[i].valor;
        rotacoes[indice].altura = caixas[i].largura;
        rotacoes[indice].profundidade = caixas[i].profundidade;
        rotacoes[indice].largura = caixas[i].altura;
        rotacoes[indice].rotacao = 2;
        indice++;
    }

    // Agora o n é na verdade 2n, porque as rotações foram geradas anteriormente
    n = 2*n;

    // O tempo é medido só para o processamento, pré e pós processamento são desprezados na contagem de performance
    clock_t t;
    t = clock();
    int **resultado = resolve(rotacoes, n, h);
    t = clock() - t;

    escreve_saida(argv[2], resultado, rotacoes, n, h);

    printf("Tempo de execucao: %f segundos\n", (1000*((double) t/ CLOCKS_PER_SEC)));

    // Não esqueça de dar free em tudo para não ter memory-leak
    free(rotacoes);
    for(int i = 0; i <= h; i++) free(resultado[i]);
    free(resultado);
    free(caixas);

    return 0;
}
