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

    // escolhe os hubs aleatorios
    // escolheHubs();

    hubs[0] = 3;
    hubs[1] = 5;
    hubs[2] = 13;
    hubs[3] = 16;

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

// calculo da FO
/*float calculaFOPorCaminho(int* caminho){
    float fo = 0;
    for(int i = 0; i < 4; i++){
        for(int j = 1; j < 4; j++){

            if(isHub(caminho[i]) && isHub(caminho[j])){
                fo += distancia(coordenadas[caminho[i]], coordenadas[caminho[j]])*0.75;
            }
            else{
                fo += distancia(coordenadas[caminho[i]], coordenadas[caminho[j]]);
            }
        }
    }
    return fo;
}*/

float calculaFOPorCaminho(int *caminho)
{
    float fo = 0;

    // Custo de coleta entre o nó de origem e o primeiro hub (β = 1)
    float cik = distancia(coordenadas[caminho[0]], coordenadas[caminho[1]]);

    // Custo de transferência entre os hubs (α = 0.75)
    float ckl = distancia(coordenadas[caminho[1]], coordenadas[caminho[2]]);

    // Custo de distribuição entre o segundo hub e o nó de destino (λ = 1)
    float clj = distancia(coordenadas[caminho[2]], coordenadas[caminho[3]]);

    // Cálculo do custo total
    fo = 1.0 * cik + 0.75 * ckl + 1.0 * clj;

    return fo;
}

// verifica se um nó é hub
int isHub(int no)
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

// A partir dos nós selecionados como hub ele cria a solução viavel, ligando os nós não hub aos hubs
/*Solucao criaSolucao(int origem, int destino){
    int caminho[4];
    memset(caminho, -1, sizeof(caminho));

    if(origem == destino || isHub(origem)){
        caminho[0] = origem;
        caminho[1] = hubMaisProximo(origem);
        caminho[2] = hubMaisProximo(origem);
        caminho[3] = destino;
    }
    else if(isHub(destino)){
        caminho[0] = origem;
        if (hubMaisProximo(origem) == destino)
        {
            caminho[1] = caminho[2] = caminho[3] = destino;
        }
        else
        {
            caminho[1] = hubMaisProximo(origem);
            caminho[2] = hubMaisProximo(destino);
            caminho[3] = destino;
        }


    }
    else{
        caminho[0] = origem;
        caminho[1] = hubMaisProximo(origem);
        if(distancia(coordenadas[hubMaisProximo(caminho[1])], coordenadas[destino]) < distancia(coordenadas[caminho[1]], coordenadas[destino])){
            caminho[2] = hubMaisProximo(caminho[1]);
            caminho[3] = destino;
        }
        else{
            caminho[2] = hubMaisProximo(origem);
            caminho[3] = destino;
        }
    }

    Solucao solucao;
    solucao.caminho[0] = caminho[0];
    solucao.caminho[1] = caminho[1];
    solucao.caminho[2] = caminho[2];
    solucao.caminho[3] = caminho[3];

    return solucao;
}*/

Solucao criaSolucao(int origem, int destino)
{
    int caminho[4];
    memset(caminho, -1, sizeof(caminho));

    float menorCusto = std::numeric_limits<float>::max();
    int melhorHub1 = -1, melhorHub2 = -1;

    // Testa todas as combinações de hubs
    for (int k = 0; k < HUBS; k++)
    {
        for (int l = 0; l < HUBS; l++)
        {
            // Custo de coleta entre o nó de origem e o hub k (β = 1)
            float cik = distancia(coordenadas[origem], coordenadas[hubs[k]]);
            // Custo de transferência entre o hub k e o hub l (α = 0.75)
            float ckl = distancia(coordenadas[hubs[k]], coordenadas[hubs[l]]);
            // Custo de distribuição entre o hub l e o nó de destino (λ = 1)
            float clj = distancia(coordenadas[hubs[l]], coordenadas[destino]);
            // Custo total do caminho
            float custoTotal = 1.0 * cik + 0.75 * ckl + 1.0 * clj;

            // Verifica se este caminho é o melhor até agora
            if (custoTotal < menorCusto)
            {
                menorCusto = custoTotal;
                melhorHub1 = hubs[k];
                melhorHub2 = hubs[l];
            }
        }
    }

    // Define o caminho com os melhores hubs encontrados
    caminho[0] = origem;
    caminho[1] = melhorHub1;
    caminho[2] = melhorHub2;
    caminho[3] = destino;

    Solucao solucao;
    solucao.caminho[0] = caminho[0];
    solucao.caminho[1] = caminho[1];
    solucao.caminho[2] = caminho[2];
    solucao.caminho[3] = caminho[3];

    return solucao;
}

// verifica o hub mais proximo do nó
int hubMaisProximo(int pontoOrigem)
{
    int hubMaisProximo = -1;
    double distanciaMinima = std::numeric_limits<double>::max();

    for (int i = 0; i < HUBS; i++)
    {
        double dist = distancia(coordenadas[pontoOrigem], coordenadas[hubs[i]]);
        if (dist < distanciaMinima)
        {
            distanciaMinima = dist;
            hubMaisProximo = hubs[i];
        }
    }

    return hubMaisProximo;
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

// Se o Nó origem é igual ao destino, vai ao hub mais próximo e volta - OK
// Se o Nó origem é um hub, vai ao hub mais próximo e volta - OK
// Se o Nó origem não é um hub, vai ao hub mais próximo verifica se o destino é mais proximo do hub, se for, vai ao hub, hub destino.

// void escolheHubsAleatoriaGulosa(){

//     double distancia_total_x = 0;
//     double distancia_total_y = 0;

//     for(int i = 0; i < num_nos; i++){
//         distancia_total_x += coordenadas[i].x;
//         distancia_total_y += coordenadas[i].y;
//     }

//     Coord ponto_central;

//     ponto_central.x = distancia_total_x/num_nos;
//     ponto_central.y = distancia_total_y/num_nos;

//     printf("Ponto central: %f %f\n", ponto_central.x, ponto_central.y);

//     double x_min = ponto_central.x - ponto_central.x*range_escolha_pontos;
//     double x_max = ponto_central.x + ponto_central.x*range_escolha_pontos;

//     double y_min = ponto_central.y - ponto_central.y*range_escolha_pontos;
//     double y_max = ponto_central.y + ponto_central.y*range_escolha_pontos;

//     int vetor_pontos_candidatos[num_nos];

//     memset(vetor_pontos_candidatos, -1, sizeof(vetor_pontos_candidatos));

//     for(int i = 0; i < num_nos; i++){
//         if(coordenadas[i].x <= x_min && coordenadas[i].x >= x_max && coordenadas[i].y <= y_min && coordenadas[i].y >= y_max){
//             vetor_pontos_candidatos[i] = i;
//         }
//     }

//     int i = 0;
//     int j = 0;

//     while(i < HUBS){
//         j = rand() % num_nos;
//         if(vetor_pontos_candidatos[j] != -1){
//             hubs[i] = vetor_pontos_candidatos[j];
//             vetor_pontos_candidatos[j] = -1;
//             i++;
//         }
//     }
//     qsort(hubs, HUBS, sizeof(int), [](const void* a, const void* b) -> int {
//         return *(int*)a - *(int*)b;
//     });
// }
