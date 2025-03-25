#include <iostream>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <limits>

using namespace std;

#define MAX_PONTOS 200
#define MAX_HUBS 50

#define TEMPO_MAXIMO 300

// #define PRINT
// #define DEBUG
#define PRINTA_TEMPO

typedef struct solucao
{
    int hubs[MAX_HUBS];
    float fo;
} Solucao;

typedef struct coord
{
    float x;
    float y;
} Coord;

int num_nos = 0;

// Número de hubs
int HUBS = 4;

// Matriz com todas as distancias entre todos os nós
float matriz_distancias[MAX_PONTOS][MAX_PONTOS];

// Vetor de coordenadas puxado do arquivo de instancias
Coord coordenadas[MAX_PONTOS];

// Funções
void grasp(Solucao *solucaoFinal);
void buscaLocal(Solucao *solucao);
void escolheHubs(int *hubs);
void criaSolucao(Solucao *solucao, int *hubs);
void leArquivo(char *nome_arquivo);
void leArquivoSolucao(char *nome_arquivo, Solucao *solucao);
void printaSolucaoArquivo(char *nome_arquivo, Solucao *solucao);
void calculaMatrizDistancias();
void printaSolucaoConsole(Solucao *solucao);
void clonarSolucao(Solucao *novaSolucao, Solucao *velhaSolucao);
float distancia(Coord a, Coord b);
int isHub(int no, int *hubs);

int main()
{
    srand(time(NULL));

    // Lê o arquivo de instancias
    leArquivo("instances/inst20.txt");

    calculaMatrizDistancias();

    // Solução final
    Solucao solucao;

    grasp(&solucao);

    printaSolucaoArquivo("sol.txt", &solucao);
    printaSolucaoConsole(&solucao);

    return 0;
}

void grasp(Solucao *solucaoFinal)
{
    // Iniciando timer
    clock_t inicio, tempo_atual;
    double tempo;
    inicio = clock();

    Solucao melhorSolucao;
    melhorSolucao.fo = std::numeric_limits<float>::max();
    int hubs[HUBS];

#ifdef PRINTA_TEMPO
    printf("\nTempo Inicial: %f\n\n", tempo);
#endif

    do
    {
        // tempo antes da busca
        tempo_atual = clock();
        tempo = (double)(tempo_atual - inicio) / CLOCKS_PER_SEC;

        Solucao solucaoInicial;

        escolheHubs(hubs);
        criaSolucao(&solucaoInicial, hubs);

        // Fase de melhoria
        buscaLocal(&solucaoInicial);

        if (solucaoInicial.fo < melhorSolucao.fo)
        {
            tempo_atual = clock();
            tempo = (double)(tempo_atual - inicio) / CLOCKS_PER_SEC;
#ifdef PRINTA_TEMPO
            printf("FO Melhorada: %f\n", solucaoInicial.fo);
            printf("Tempo para achar essa FO: %f\n\n", tempo);
#endif
            clonarSolucao(&melhorSolucao, &solucaoInicial);
        }

        // tempo depois da busca
        tempo_atual = clock();
        tempo = (double)(tempo_atual - inicio) / CLOCKS_PER_SEC;

    } while (tempo < TEMPO_MAXIMO);

#ifdef PRINTA_TEMPO
    printf("Tempo de execucao: %f\n\n", tempo);
#endif

    clonarSolucao(solucaoFinal, &melhorSolucao);
}

/*
Função de busca local por troca de hubs
    1. Seleciona uma posição aleatória para trocar um hub (num_nos vezes)
    2. Testa 5 combinações de hubs para a posição selecionada
    3. Se a solução melhorar, atualiza a solução
*/
void buscaLocal(Solucao *solucao)
{
    Solucao melhorSolucao;
    clonarSolucao(&melhorSolucao, solucao);

    int hubsLocais[HUBS];
    for (int i = 0; i < HUBS; i++)
    {
        hubsLocais[i] = solucao->hubs[i];
    }

    int hubsSelecionados[HUBS];
    memset(hubsSelecionados, 0, sizeof(hubsSelecionados));
    int posicaoHubSelecionado;

    for (int i = 0; i < num_nos; i++)
    {
        posicaoHubSelecionado = rand() % HUBS;
        hubsSelecionados[posicaoHubSelecionado] = 1;

        int hubSelecionado;
        for (int i = 0; i < 5; i++)
        {
            // Troca o hub na posição selecionada para uma hub que não está na solução
            while (1)
            {
                hubSelecionado = rand() % num_nos;
                if (!isHub(hubSelecionado, hubsLocais))
                {
                    hubsLocais[posicaoHubSelecionado] = hubSelecionado;
                    break;
                }
            }

            criaSolucao(&melhorSolucao, hubsLocais);

            if (melhorSolucao.fo < solucao->fo)
            {
                clonarSolucao(&melhorSolucao, solucao);
            }
        }
    }
}

/*
Escolhe os hubs iniciais de forma aleatória e gulosa:
    1. Escolhe o primeiro hub aleatóriamente
    2. Para os próximos hubs, escolhe o nó mais distante do ponto médio dos hubs já escolhidos
*/
void escolheHubs(int *hubs)
{

    int primeiroHub = rand() % num_nos;
    hubs[0] = primeiroHub;
    int maisDistante;
    Coord pontoMedio;

    for (int i = 1; i < HUBS; i++)
    {
        maisDistante = 0;
        for (int j = 0; j < num_nos; j++)
        {
            if (!isHub(j, hubs))
            {
                for (int k = 0; k < i; k++)
                {
                    pontoMedio.x += coordenadas[k].x;
                    pontoMedio.y += coordenadas[k].y;
                }
                pontoMedio.x = pontoMedio.x / i;
                pontoMedio.y = pontoMedio.y / i;

                if (distancia(pontoMedio, coordenadas[j]) > distancia(pontoMedio, coordenadas[maisDistante]))
                {
                    maisDistante = j;
                }
            }
        }
        hubs[i] = maisDistante;
    }
}

// Calcula a função objetivo para os hubs definidos
void criaSolucao(Solucao *solucao, int *hubs)
{

    float menorCusto = std::numeric_limits<float>::max();
    solucao->fo = 0;

    int melhorHubL = -1, melhorHubK = -1;
    float cik, ckl, clj, custoTotal;

    for (int i = 0; i < HUBS; i++)
    {
        solucao->hubs[i] = hubs[i];
    }

#ifdef PRINT
    printf("n: %d\tp: %d\n", num_nos, HUBS);
    printf("FO:\t%f\n", fo);
    printf("HUBS:\t[");
    for (int i = 0; i < HUBS; i++)
    {
        printf("%d", hubs[i]);
        if (i < HUBS - 1)
        {
            printf(", ");
        }
    }
    printf("]\n");
    printf("OR\tH1\tH2\tDS\tCUSTO\n");
#endif

    // Verifica todos os nós
    for (int i = 0; i < num_nos; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            melhorHubK = -1;
            melhorHubL = -1;
            menorCusto = std::numeric_limits<float>::max();
            cik = 0;
            ckl = 0;
            clj = 0;
            custoTotal = 0;

            // Testa todas as combinações de hubs
            for (int k = 0; k < HUBS; k++)
            {
                for (int l = 0; l < HUBS; l++)
                {
                    // Caminho i -> k -> l -> j
                    // k, l sendo hubs

                    cik = matriz_distancias[i][hubs[k]];
                    ckl = matriz_distancias[hubs[k]][hubs[l]];
                    clj = matriz_distancias[hubs[l]][j];

                    custoTotal = 1.0 * cik + 0.75 * ckl + 1.0 * clj;

                    if (custoTotal < menorCusto)
                    {
                        menorCusto = custoTotal;
                        melhorHubK = hubs[k];
                        melhorHubL = hubs[l];
                    }
                }
            }
            // Salva a maior fo (maior dos menores)
            if (menorCusto > solucao->fo)
            {
                solucao->fo = menorCusto;
            }
#ifdef PRINTA
            printf("%d\t%d\t%d\t%d\t%f\n", i, melhorHubK, melhorHubL, j, menorCusto);
#endif
        }
    }
}

// Lê um arquivo de instancias
void leArquivo(char *nome_arquivo)
{
    FILE *arq = fopen(nome_arquivo, "r");

    if (arq == NULL)
    {
        cout << "Erro ao abrir o arquivo" << endl;
        return;
    }

    fscanf(arq, "%d", &num_nos);

    for (int i = 0; i < num_nos; i++)
    {
        fscanf(arq, "%f %f\n", &coordenadas[i].x, &coordenadas[i].y);
    }

    fclose(arq);
}

// Lê uma solução de um arquivo
void leArquivoSolucao(char *nome_arquivo, Solucao *solucao)
{
    FILE *arq = fopen(nome_arquivo, "r");

    if (arq == NULL)
    {
        cout << "Erro ao abrir o arquivo" << endl;
        return;
    }

    int n, p;

    fscanf(arq, "n: %d\tp: %d\n", &n, &p);
    fscanf(arq, "FO:\t%f\n", &solucao->fo);
    fscanf(arq, "HUBS:\t[");

    for (int i = 0; i < p; i++)
    {
        fscanf(arq, "%d,", &solucao->hubs[i]);
    }

    num_nos = n;
    HUBS = p;

    fclose(arq);
}

// Printa uma solução em um arquivo
void printaSolucaoArquivo(char *nome_arquivo, Solucao *solucao)
{
    FILE *arq = fopen(nome_arquivo, "a");

    if (arq == NULL)
    {
        cout << "Erro ao abrir o arquivo para escrita" << endl;
    }

    fprintf(arq, "n: %d\tp: %d\n", num_nos, HUBS);
    fprintf(arq, "FO:\t%f\n", solucao->fo);
    fprintf(arq, "HUBS:\t[");
    for (int i = 0; i < HUBS; i++)
    {
        fprintf(arq, "%d", solucao->hubs[i]);
        if (i < HUBS - 1)
        {
            fprintf(arq, ", ");
        }
    }
    fprintf(arq, "]\n");

    // Print caminhos
    fprintf(arq, "OR\tH1\tH2\tDS\tCUSTO\n");
    for (int i = 0; i < num_nos; i++)
    {

        for (int j = 0; j < num_nos; j++)
        {

            float menorCusto = std::numeric_limits<float>::max();

            int H1 = -1, H2 = -1;

            // Encontra o melhor caminho para o par (i,j)
            for (int k = 0; k < HUBS; k++)
            {
                for (int l = 0; l < HUBS; l++)
                {
                    float custo = 1.0 * matriz_distancias[i][solucao->hubs[k]] + 0.75 * matriz_distancias[solucao->hubs[k]][solucao->hubs[l]] + 1.0 * matriz_distancias[solucao->hubs[l]][j];

                    if (custo < menorCusto)
                    {
                        menorCusto = custo;
                        H1 = solucao->hubs[k];
                        H2 = solucao->hubs[l];
                    }
                }
            }
            fprintf(arq, "%d\t%d\t%d\t%d\t%f\n", i, H1, H2, j, menorCusto);
        }
    }

    fclose(arq);
}

// Calcula a matriz de distancias (todos para todos)
void calculaMatrizDistancias()
{
    for (int i = 0; i < num_nos; i++)
    {
        for (int j = 0; j < num_nos; j++)
        {
            matriz_distancias[i][j] = distancia(coordenadas[i], coordenadas[j]);
        }
    }
}

// Printa uma solução no console com todos os caminhos
void printaSolucaoConsole(Solucao *solucao)
{
    printf("n: %d\tp: %d\n", num_nos, HUBS);
    printf("FO:\t%f\n", solucao->fo);
    printf("HUBS:\t[");
    for (int i = 0; i < HUBS; i++)
    {
        printf("%d", solucao->hubs[i]);
        if (i < HUBS - 1)
        {
            printf(", ");
        }
    }
    printf("]\n");

    printf("OR\tH1\tH2\tDS\tCUSTO\n");
    for (int i = 0; i < num_nos; i++)
    {
        for (int j = 0; j < num_nos; j++)
        {
            float menorCusto = std::numeric_limits<float>::max();
            int H1 = -1, H2 = -1;

            // Encontra o melhor caminho para o par (i,j)
            for (int k = 0; k < HUBS; k++)
            {
                for (int l = 0; l < HUBS; l++)
                {
                    float custo = 1.0 * matriz_distancias[i][solucao->hubs[k]] + 0.75 * matriz_distancias[solucao->hubs[k]][solucao->hubs[l]] + 1.0 * matriz_distancias[solucao->hubs[l]][j];

                    if (custo < menorCusto)
                    {
                        menorCusto = custo;
                        H1 = solucao->hubs[k];
                        H2 = solucao->hubs[l];
                    }
                }
            }
            printf("%d\t%d\t%d\t%d\t%f\n", i, H1, H2, j, menorCusto);
        }
    }
}

void clonarSolucao(Solucao *novaSolucao, Solucao *velhaSolucao)
{
    memcpy(novaSolucao->hubs, velhaSolucao->hubs, sizeof(velhaSolucao->hubs));

    for (int i = 0; i < HUBS; i++)
    {
        novaSolucao->hubs[i] = velhaSolucao->hubs[i];
    }

    novaSolucao->fo = velhaSolucao->fo;
}

// Calcula a distancia entres quaisquer nós
float distancia(Coord a, Coord b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

int isHub(int no, int *hubs)
{
    for (int i = 0; i < HUBS; i++)
    {
        if (hubs[i] == no)
        {
            return 1;
        }
    }
    return 0;
}