#include <iostream>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "header.h"
#include <memory.h>
#include <limits>

using namespace std;

#define HUBS 4

#define MAX_PONTOS 200

typedef struct arqsolucao{
    int hubs[HUBS];
    float fo;
}ArqSolucao;

/*
- Matriz de soluções desnecessária
- fo global desnecessária
- Função retornando tipos compostos (Solucao)
- A estrutura solução não é usada na construtiva, cálculo de FO, etc.
- X Cálculo de FO incorreto.
- Ler solução não "carrega" uma solução
- X Número de hubs deveria estar parametrizado no .cpp
- X srand não deve ser usado dentro de métodos (apenas no main)
*/

int num_nos = 0;
int hubs[HUBS];
Solucao solucao[200][200];

float matriz_distancias[MAX_PONTOS][MAX_PONTOS];
Coord coordenadas[MAX_PONTOS];
float fo = 0;

int main()
{

    srand(time(NULL));

    // le o arquivo de instancias
    leArquivo("instances/inst20.txt");
    
    calculaMatrizDistancias();
    // escolhe aleatoriamente os hubs que serão usados na solução
    // escolheHubs();

    //Verifica se está funcionando com os melhores hubs
    hubs[0] = 3;
    hubs[1] = 5;
    hubs[2] = 13;
    hubs[3] = 16;

    // procedimento de criação de solução (todos para todos)
    
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

//calcula a FO entre dois nós não hubs i e j passar por pelo menos um hub k e l podendo ser k = l
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
            //cik = distancia(coordenadas[origem], coordenadas[hubs[k]]);
            cik = matriz_distancias[origem][hubs[k]];

            //custo de transferência entre os hubs (k e l)
            //ckl = distancia(coordenadas[hubs[k]], coordenadas[hubs[l]]);
            ckl = matriz_distancias[hubs[k]][hubs[l]];

            //custo de tranferencia entre o segundo hub (l) e o nó de destino
            //clj = distancia(coordenadas[hubs[l]], coordenadas[destino]);
            clj = matriz_distancias[hubs[l]][destino];

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

void leArquivoSolucao(char *nome_arquivo, ArqSolucao* solucao)
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

void calculaMatrizDistancias(){
    for (int i = 0; i < num_nos; i++)
    {
        for (int j = 0; j < num_nos; j++)
        {
            matriz_distancias[i][j] = distancia(coordenadas[i], coordenadas[j]);
        }
    }
}