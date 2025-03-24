#include <iostream>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <limits>

using namespace std;

#define HUBS 4

#define MAX_PONTOS 200

// #define PRINT
// #define PRINTA_CAMINHOS
// #define DEBUG

typedef struct solucao{
    int hubs[HUBS];
    float fo;
} Solucao;

typedef struct coord{
    float x;
    float y;
}Coord;

int num_nos = 0;

//Matriz com todas as distancias entre todos os nós
float matriz_distancias[MAX_PONTOS][MAX_PONTOS];

//Vetor de coordenadas puxado do arquivo de instancias
Coord coordenadas[MAX_PONTOS];

//Funções
void criaSolucao(Solucao *solucao);
void printaSolucaoArquivo(char *nome_arquivo, Solucao *solucao);
void grasp(int execucoes, Solucao *solucaoFinal);
void buscaLocal(Solucao *solucao);
void clonarSolucao(Solucao *novaSolucao, Solucao *velhaSolucao);
int isHub(int no, int *hubs);
void printaSolucaoConsole(Solucao *solucao);
void leArquivo(char* nome_arquivo);
float distancia(Coord a, Coord b);
void escolheHubs(int *hubs);
float calculaFOPorCaminho(int* caminho);
int isHub(int no);
int hubMaisProximo(int pontoOrigem);
void leArquivoSolucao(char* nome_arquivo);
void calculaMatrizDistancias();

int main(){

    srand(time(NULL));

    // Lê o arquivo de instancias
    leArquivo("instances/inst20.txt");

    calculaMatrizDistancias();

    // Solução final
    Solucao solucao;

    int execucoes = 100;
    grasp(execucoes, &solucao);

    printaSolucaoArquivo("sol.txt", &solucao);
    printaSolucaoConsole(&solucao);

    return 0;
}



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

// Calcula a distancia entres quaisquer nós
float distancia(Coord a, Coord b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Escolhe aletaoriamente os hubs que serão utilizados
void escolheHubs(int *hubs)
{
    //Adicionar gulosidadeui
    int primeiroHub = rand() % num_nos;
    hubs[0] = primeiroHub;
    int maisDistante;
    Coord pontoMedio;

    for(int i = 1; i < HUBS; i++){
        maisDistante = 0;
        for (int j = 0; j < num_nos; j++){
            if(!isHub(j, hubs)){
                for(int k = 0; k < i; k++){
                    pontoMedio.x += coordenadas[k].x;
                    pontoMedio.y += coordenadas[k].y;
                }
                pontoMedio.x = pontoMedio.x/i;
                pontoMedio.y = pontoMedio.y/i;
    
                if(distancia(pontoMedio, coordenadas[j]) > distancia(pontoMedio, coordenadas[maisDistante])){
                    maisDistante = j;
                }
            }
        }
        hubs[i] = maisDistante;
    }
    

}

void criaSolucao(Solucao *solucao, int *hubs){

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
        // Lembrar de verificar somente a metade dos nós
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
                    // Custo de tranferencia entre o nó de origem e o primeiro hub (k)
                    cik = matriz_distancias[i][hubs[k]];

                    // Custo de transferência entre os hubs (k e l)
                    ckl = matriz_distancias[hubs[k]][hubs[l]];

                    // Custo de tranferencia entre o segundo hub (l) e o nó de destino
                    clj = matriz_distancias[hubs[l]][j];

                    // Custo total do caminho
                    custoTotal = 1.0 * cik + 0.75 * ckl + 1.0 * clj;

                    // Verifica se este caminho é o melhor até agora
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
#ifdef PRINTA_CAMINHOS
            printf("%d\t%d\t%d\t%d\t%f\n", i, melhorHubK, melhorHubL, j, menorCusto);
#endif
        }
    }
}

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

    //Print caminhos
    fprintf(arq, "OR\tH1\tH2\tDS\tCUSTO\n");
    for (int i = 0; i < num_nos; i++) {

        for (int j = 0; j < num_nos; j++) {

            float menorCusto = std::numeric_limits<float>::max();
            
            int H1 = -1, H2 = -1;

            // Encontra o melhor caminho para o par (i,j)
            for (int k = 0; k < HUBS; k++) {
                for (int l = 0; l < HUBS; l++) {
                    float custo = 1.0 * matriz_distancias[i][solucao->hubs[k]] 
                                + 0.75 * matriz_distancias[solucao->hubs[k]][solucao->hubs[l]] 
                                + 1.0 * matriz_distancias[solucao->hubs[l]][j];
                    
                    if (custo < menorCusto) {
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

void clonar(Solucao &destino, const Solucao &origem)
{
    memcpy(&destino, &origem, sizeof(Solucao));
}

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

    fclose(arq);
}

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
    for (int i = 0; i < num_nos; i++) {

        for (int j = 0; j < num_nos; j++) {

            float menorCusto = std::numeric_limits<float>::max();

            int H1 = -1, H2 = -1;

            // Encontra o melhor caminho para o par (i,j)
            for (int k = 0; k < HUBS; k++) {
                for (int l = 0; l < HUBS; l++) {
                    float custo = 1.0 * matriz_distancias[i][solucao->hubs[k]] 
                                + 0.75 * matriz_distancias[solucao->hubs[k]][solucao->hubs[l]] 
                                + 1.0 * matriz_distancias[solucao->hubs[l]][j];
                    
                    if (custo < menorCusto) {
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

void grasp(int execucoes, Solucao *solucaoFinal)
{
    Solucao melhorSolucao;
    melhorSolucao.fo = std::numeric_limits<float>::max();
    int hubs[HUBS];

    //add tempo
    for (int i = 0; i <= execucoes; i++)
    {
        Solucao solucaoInicial;

        escolheHubs(hubs);
        criaSolucao(&solucaoInicial, hubs);

        // Fase de melhoria
        buscaLocal(&solucaoInicial);

        if(solucaoInicial.fo < melhorSolucao.fo){
            clonarSolucao(&melhorSolucao, &solucaoInicial);

        }
    }
//pegar tempo limite
    clonarSolucao(solucaoFinal, &melhorSolucao);
}

// Função de busca local por troca de hubs
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

    for (int i = 0; i < num_nos; i++){
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

void clonarSolucao(Solucao *novaSolucao, Solucao *velhaSolucao)
{
    memcpy(novaSolucao->hubs, velhaSolucao->hubs, sizeof(velhaSolucao->hubs));

    for (int i = 0; i < HUBS; i++)
    {
        novaSolucao->hubs[i] = velhaSolucao->hubs[i];
    }

    novaSolucao->fo = velhaSolucao->fo;
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
