#include <iostream>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "header.h"
#include <memory.h>
#include <limits>

using namespace std;

int main()
{

    // le o arquivo de instancias
    leArquivo("instances/inst20.txt");

    // escolhe aleatoriamente os hubs que serão usados na solução
    escolheHubs();

    //Verifica se está funcionando com os melhores hubs
    /*hubs[0] = 3;
    hubs[1] = 5;
    hubs[2] = 13;
    hubs[3] = 16;*/

    // procedimento de criação de solução (todos para todos)
    fo = 0;
    for (int i = 0; i < num_nos; i++)
    {
        for (int j = 0; j < num_nos; j++)
        {
            solucao[i][j] = criaSolucao(i, j);
        }
    }

    // calculo da FO
    for (int i = 0; i < num_nos; i++)
    {
        for (int j = 0; i >= j; j++)
        {
            solucao[i][j].fo = calculaFOPorCaminho(solucao[i][j].caminho);
            if (solucao[i][j].fo > fo)
            {
                fo = solucao[i][j].fo;
            }
        }
    }


    int maxIteracoes = 10; // Número de iterações do GRASP
    grasp(maxIteracoes);

    // printa soluções
    printaSolucaoConsole();
    printaSolucaoArquivo("sol.txt");

    return 0;
}

// le os arquivos
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

// calcula a distancia entres quaisquer nós
float distancia(Coord a, Coord b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// escolhe aletaoriamente os hubs que serão utilizados
void escolheHubs()
{
    srand(time(NULL));

    for (int i = 0; i < HUBS; i++)
    {
        bool unique;
        do
        {
            unique = true;
            hubs[i] = rand() % num_nos;
            for (int j = 0; j < i; j++)
            {
                if (hubs[i] == hubs[j])
                {
                    unique = false;
                    break;
                }
            }
        } while (!unique);
    }
}

//calcula a FO entre dois nós nós não hubs i e j passar por pelo menos um hub k e l podendo ser k = l
float calculaFOPorCaminho(int *caminho)
{
    float fo = 0;

    //custo de trandsfêrencia entre o nó de origem e o primeiro hub
    float cik = distancia(coordenadas[caminho[0]], coordenadas[caminho[1]]);

    //custo de transferência entre os hubs
    float ckl = distancia(coordenadas[caminho[1]], coordenadas[caminho[2]]);

    //custo de transferência entre o segundo hub e o nó de destino
    float clj = distancia(coordenadas[caminho[2]], coordenadas[caminho[3]]);

    //cálculo do custo total
    fo = 1.0 * cik + 0.75 * ckl + 1.0 * clj;

    return fo;
}

Solucao criaSolucao(int origem, int destino)
{
    int caminho[4];
    memset(caminho, -1, sizeof(caminho));

    //inicia o float no maior valor possivel para pegar sempre valores menores
    float menorCusto = std::numeric_limits<float>::max();

    int melhorHubL = -1, melhorHubK = -1;
    float cik, ckl, clj, custoTotal;

    //testa todas as combinações de hubs
    for (int k = 0; k < HUBS; k++)
    {
        for (int l = 0; l < HUBS; l++)
        {
            //custo de tranferencia entre o nó de origem e o primeiro hub (k) 
            cik = distancia(coordenadas[origem], coordenadas[hubs[k]]);

            //custo de transferência entre os hubs (k e l)
            ckl = distancia(coordenadas[hubs[k]], coordenadas[hubs[l]]);

            //custo de tranferencia entre o segundo hub (l) e o nó de destino
            clj = distancia(coordenadas[hubs[l]], coordenadas[destino]);

            //custo total do caminho
            custoTotal = 1.0 * cik + 0.75 * ckl + 1.0 * clj;

            //verifica se este caminho é o melhor até agora
            if (custoTotal < menorCusto)
            {
                menorCusto = custoTotal;
                melhorHubL = hubs[k];
                melhorHubK = hubs[l];
            }
        }
    }

    //define o melhor caminho com os hubs selecionados
    caminho[0] = origem;
    caminho[1] = melhorHubL;
    caminho[2] = melhorHubK;
    caminho[3] = destino;

    Solucao solucao;
    solucao.caminho[0] = caminho[0];
    solucao.caminho[1] = caminho[1];
    solucao.caminho[2] = caminho[2];
    solucao.caminho[3] = caminho[3];

    return solucao;
}

void printaSolucaoConsole()
{
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
    for (int i = 0; i < num_nos; i++)
    {
        for (int j = 0; i >= j; j++)
        {
            printf("%d\t%d\t%d\t%d\t%f\n", solucao[i][j].caminho[0], solucao[i][j].caminho[1], solucao[i][j].caminho[2], solucao[i][j].caminho[3], solucao[i][j].fo);
        }
    }
}

void printaSolucaoArquivo(char *nome_arquivo)
{
    FILE *arq = fopen(nome_arquivo, "a");

    if (arq == NULL)
    {
        cout << "Erro ao abrir o arquivo para escrita" << endl;
    }

    fprintf(arq, "n: %d\tp: %d\n", num_nos, HUBS);
    fprintf(arq, "FO:\t%f\n", fo);
    fprintf(arq, "HUBS:\t[");
    for (int i = 0; i < HUBS; i++)
    {
        fprintf(arq, "%d", hubs[i]);
        if (i < HUBS - 1)
        {
            fprintf(arq, ", ");
        }
    }
    fprintf(arq, "]\n");
    fprintf(arq, "OR\tH1\tH2\tDS\tCUSTO\n");
    for (int i = 0; i < num_nos; i++)
    {
        for (int j = 0; i >= j; j++)
        {
            fprintf(arq, "%d\t%d\t%d\t%d\t%f\n", solucao[i][j].caminho[0], solucao[i][j].caminho[1], solucao[i][j].caminho[2], solucao[i][j].caminho[3], solucao[i][j].fo);
        }
    }

    fclose(arq);
}

void clonar(Solucao &destino, const Solucao &origem)
{
    memcpy(&destino, &origem, sizeof(Solucao));
}

void leArquivoSolucao(char *nome_arquivo)
{
    FILE *arq = fopen(nome_arquivo, "r");

    if (arq == NULL)
    {
        cout << "Erro ao abrir o arquivo" << endl;
        return;
    }

    int n, p;
    float fo;
    int hubsTemp[HUBS];

    fscanf(arq, "n: %d\tp: %d\n", &n, &p);
    fscanf(arq, "FO:\t%f\n", &fo);
    fscanf(arq, "HUBS:\t[");

    for (int i = 0; i < p; i++)
    {
        fscanf(arq, "%d,", &hubsTemp[i]);
    }
    fscanf(arq, " ]\n");
    fscanf(arq, "OR\t	H1\t	H2\t	DS\t	CUSTO\t\n");

    num_nos = n;
    fo = fo;

    while (!feof(arq))
    {
        int origem, h1, h2, destino;
        float custo;

        fscanf(arq, "%d\t%d\t%d\t%d\t%f\n", &origem, &h1, &h2, &destino, &custo);

        solucao[origem][destino].caminho[0] = origem;
        solucao[origem][destino].caminho[1] = h1;
        solucao[origem][destino].caminho[2] = h2;
        solucao[origem][destino].caminho[3] = destino;
        solucao[origem][destino].fo = custo;
    }

    fclose(arq);
}


//grasp


void buscaLocal() {
    float melhorFO = fo;
    int melhorHubs[HUBS];
    memcpy(melhorHubs, hubs, sizeof(hubs));

    // Tenta trocar cada hub
    for (int i = 0; i < HUBS; i++) {
        int hubOriginal = hubs[i];
        for (int j = 0; j < num_nos; j++) {
            if (isHub(j)) continue; // Ignora nós que já são hubs
            hubs[i] = j; // Tenta trocar o hub
            float novaFO = calculaFO();
            if (novaFO < melhorFO) {
                melhorFO = novaFO;
                memcpy(melhorHubs, hubs, sizeof(hubs));
            }
        }
        hubs[i] = hubOriginal; // Restaura o hub original
    }

    // Atualiza a solução se encontrou uma melhoria
    if (melhorFO < fo) {
        fo = melhorFO;
        memcpy(hubs, melhorHubs, sizeof(hubs));
    }
}


void grasp(int maxIteracoes) {
    float melhorFO = fo;
    int melhorHubs[HUBS];
    memcpy(melhorHubs, hubs, sizeof(hubs));

    for (int iter = 0; iter < maxIteracoes; iter++) {
        // Fase de busca local
        buscaLocal();
        printf("%d", iter);
        // Atualiza a melhor solução encontrada
        if (fo < melhorFO) {
            melhorFO = fo;
            memcpy(melhorHubs, hubs, sizeof(hubs));
        }

        // Perturba a solução atual para diversificar
        int hub1 = rand() % HUBS;
        int hub2 = rand() % HUBS;
        std::swap(hubs[hub1], hubs[hub2]);
    }

    // Restaura a melhor solução encontrada
    fo = melhorFO;
    memcpy(hubs, melhorHubs, sizeof(hubs));
}

float calculaFO() {
    float fo = 0;
    for (int i = 0; i < num_nos; i++) {
        for (int j = 0; j < num_nos; j++) {
            float custo = calculaCustoMinimo(i, j);
            if (custo > fo) {
                fo = custo;
            }
        }
    }
    return fo;
}

float calculaCustoMinimo(int origem, int destino){
    float custoMinimo = std::numeric_limits<float>::max();
    for (int k = 0; k < HUBS; k++) {
        for (int l = 0; l < HUBS; l++) {
            float cik = distancia(coordenadas[origem], coordenadas[hubs[k]]);
            float ckl = distancia(coordenadas[hubs[k]], coordenadas[hubs[l]]);
            float clj = distancia(coordenadas[hubs[l]], coordenadas[destino]);
            float custoTotal = 1.0 * cik + 0.75 * ckl + 1.0 * clj;
            if (custoTotal < custoMinimo) {
                custoMinimo = custoTotal;
            }
        }
    }
    return custoMinimo;
}

int isHub(int no) {
    for (int i = 0; i < HUBS; i++) {
        if (hubs[i] == no) {
            return 1;
        }
    }
    return 0;
}