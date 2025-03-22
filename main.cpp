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

typedef struct solucao
{
    int hubs[HUBS];
    float fo;
} Solucao;

int num_nos = 0;
int hubs[HUBS];
//Solucao solucao[200][200];

float matriz_distancias[MAX_PONTOS][MAX_PONTOS];
Coord coordenadas[MAX_PONTOS];
float fo = 0;

void criaSolucao(Solucao *solucao);
void printaSolucaoArquivo(char *nome_arquivo, Solucao *solucao);

int main()
{

    srand(time(NULL));

    // le o arquivo de instancias
    leArquivo("instances/inst20.txt");

    calculaMatrizDistancias();
    // escolhe aleatoriamente os hubs que serão usados na solução
    

    // Verifica se está funcionando com os melhores hubs
    /*hubs[0] = 1;
    hubs[1] = 5;
    hubs[2] = 13;
    hubs[3] = 16;*/
    
    Solucao solucao;

    int execucoes = 100;
    grasp(execucoes);

    printaSolucaoArquivo("sol.txt", &solucao);
    
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

void criaSolucao(Solucao *solucao)
{
    float menorCusto = std::numeric_limits<float>::max();
    solucao->fo = 0;

    int melhorHubL = -1, melhorHubK = -1;
    float cik, ckl, clj, custoTotal;

    for(int i = 0; i < HUBS; i++){
        solucao->hubs[i] = hubs[i];
    }

    //Verifica todos os nós
    for (int i = 0; i < num_nos; i++)
    {
        //Lembrar de verificar somente a metade dos nós
        for (int j = 0; j < num_nos; j++)
        {
            melhorHubK = -1;
            melhorHubL = -1;
            menorCusto = std::numeric_limits<float>::max();
            cik = 0;
            ckl = 0;
            clj = 0;
            custoTotal = 0;

            // testa todas as combinações de hubs
            for (int k = 0; k < HUBS; k++)
            {
                for (int l = 0; l < HUBS; l++)
                {
                    // custo de tranferencia entre o nó de origem e o primeiro hub (k)
                    cik = matriz_distancias[i][hubs[k]];

                    // custo de transferência entre os hubs (k e l)
                    ckl = matriz_distancias[hubs[k]][hubs[l]];

                    // custo de tranferencia entre o segundo hub (l) e o nó de destino
                    clj = matriz_distancias[hubs[l]][j];

                    // custo total do caminho
                    custoTotal = 1.0 * cik + 0.75 * ckl + 1.0 * clj;

                    // verifica se este caminho é o melhor até agora
                    if (custoTotal < menorCusto)
                    {
                        menorCusto = custoTotal;
                        melhorHubK = hubs[k];
                        melhorHubL = hubs[l];
                    }
                }
            }
            // Salva a maior fo (maior dos menores)
            if(menorCusto > solucao->fo){
                solucao->fo = menorCusto;
            }
        }
    }
}

//mexer kkkkkk
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

// mexer kkkkkk
// void printaSolucaoConsole()
// {
//     printf("n: %d\tp: %d\n", num_nos, HUBS);
//     printf("FO:\t%f\n", fo);
//     printf("HUBS:\t[");
//     for (int i = 0; i < HUBS; i++)
//     {
//         printf("%d", hubs[i]);
//         if (i < HUBS - 1)
//         {
//             printf(", ");
//         }
//     }
//     printf("]\n");
//     printf("OR\tH1\tH2\tDS\tCUSTO\n");
//     for (int i = 0; i < num_nos; i++)
//     {
//         for (int j = 0; i >= j; j++)
//         {
//             printf("%d\t%d\t%d\t%d\t%f\n", solucao[i][j].caminho[0], solucao[i][j].caminho[1], solucao[i][j].caminho[2], solucao[i][j].caminho[3], solucao[i][j].fo);
//         }
//     }
// }


// GRASP

void grasp(int execucoes) {
    Solucao melhorSolucao;
    melhorSolucao.fo = std::numeric_limits<float>::max();

    for (int i = 0; i <= execucoes; i++) {
        Solucao solucaoInicial;

        escolheHubs();
        criaSolucao(&solucaoInicial);

        // Fase de melhoria
        buscaLocal(&solucaoInicial);

        // Atualiza a melhor solução
        if (solucaoInicial.fo < melhorSolucao.fo) {
            clonarSolucao(&melhorSolucao, &solucaoInicial);
        }
    }
}

// Função de busca local por troca de hubs
void buscaLocal(Solucao *solucao) {
    
}

void clonarSolucao(Solucao *novaSolucao, Solucao *velhaSolucao) {
    memcpy(novaSolucao->hubs, velhaSolucao->hubs, sizeof(velhaSolucao->hubs));

    for (int i = 0; i < HUBS; i++) {
        novaSolucao->hubs[i] = velhaSolucao->hubs[i];
    }

    novaSolucao->fo = velhaSolucao->fo;
}