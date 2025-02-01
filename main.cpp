#include <iostream>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "header.h"
#include <memory.h>

using namespace std;

// LEMBRAR DE FAZER ESCRITA DA SOLUÇÂO EM ARQUIVO
// Tentar pegar hubs mais proximos e mais longe do centro meio a meio

int main(){
    srand(time(NULL));

    memset(solucao, -1, sizeof(solucao));

    leArquivo("instances/inst20.txt");
    escolheHubs();

    // procedimento de criação de solução (todos para todos)
    fo = 0;
    for (int i = 0; i < num_nos; i++){
        for (int j = 0; j < num_nos; j++){
            solucao[i][j] = criaSolucao(i, j);
            if(solucao[i][j].fo > fo){
                fo = solucao[i][j].fo;
            }
        }
    }
    
    printaSolucaoConsole();
    printaSolucaoArquivo("sol.txt");

    return 0;
}

void leArquivo(char* nome_arquivo){
    FILE* arq = fopen(nome_arquivo, "r");

    if(arq == NULL){
        cout << "Erro ao abrir o arquivo" << endl;
        return;
    }

    fscanf(arq, "%d", &num_nos);

    for (int i = 0; i<num_nos; i++){
        fscanf(arq, "%f %f\n", &coordenadas[i].x, &coordenadas[i].y);
    }

    fclose(arq);
}

float distancia(Coord a, Coord b){
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void escolheHubs(){
    srand(time(NULL));

    for(int i = 0; i < HUBS; i++) {
        bool unique;
        do {
            unique = true;
            hubs[i] = rand() % num_nos;
            for(int j = 0; j < i; j++) {
                if(hubs[i] == hubs[j]) {
                    unique = false;
                    break;
                }
            }
        } while(!unique);
    }
}

float calculaFOPorCaminho(int* caminho){
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
}

int isHub(int no){
    for(int i = 0; i < HUBS; i++){
        if(hubs[i] == no){
            return 1;
        }
    }
    return 0;

}

Solucao criaSolucao(int origem, int destino){
    int caminho[4];
    memset(caminho, -1, sizeof(caminho));

    if(origem == destino || isHub(origem)){
        caminho[0] = origem;
        caminho[1] = caminho[2] = hubMaisProximo(origem);
        caminho[3] = destino;
    }
    if(isHub(destino)){
        caminho[0] = origem;
        caminho[1] = caminho[2] = caminho[3] = destino;
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
    solucao.fo = calculaFOPorCaminho(caminho);

    return solucao;
}

int hubMaisProximo(int pontoOrigem){
    int hubMaisProximo = -1;
    float distanciaMinima = 999999999;

    for(int i = 0; i < HUBS; i++){
        if(distancia(coordenadas[pontoOrigem], coordenadas[hubs[i]]) < distanciaMinima && pontoOrigem != hubs[i]){
            distanciaMinima = distancia(coordenadas[pontoOrigem], coordenadas[hubs[i]]);
            hubMaisProximo = i;
        }
    }

    return hubMaisProximo;
}

void printaSolucaoConsole(){
    printf("n: %d\tp: %d\n", num_nos, HUBS);
    printf("FO:\t%f\n", fo);
    printf("HUBS:\t[");
    for(int i = 0; i < HUBS; i++){
        printf("%d, ", hubs[i]);
    }
    printf("]\n");
    printf("OR\tH1\tH2\tDS\tCUSTO\n");
    for(int i = 0; i < num_nos; i++){
        for(int j = 0; j < num_nos; j++){
            printf("%d\t%d\t%d\t%d\t%f\n", solucao[i][j].caminho[0], solucao[i][j].caminho[1], solucao[i][j].caminho[2], solucao[i][j].caminho[3], solucao[i][j].fo);
        }
    }
}

void printaSolucaoArquivo(char* nome_arquivo){
    FILE* arq = fopen(nome_arquivo, "w");

    if(arq == NULL){
        cout << "Erro ao abrir o arquivo para escrita" << endl;
    }

    fprintf(arq, "n: %d\tp: %d\n", num_nos, HUBS);
    fprintf(arq, "FO:\t%f\n", fo);
    fprintf(arq, "HUBS:\t[");
    for(int i = 0; i < HUBS; i++){
        fprintf(arq, "%d, ", hubs[i]);
    }
    fprintf(arq, "]\n");
    fprintf(arq, "OR\tH1\tH2\tDS\tCUSTO\n");
    for(int i = 0; i < num_nos; i++){
        for(int j = 0; j < num_nos; j++){
            fprintf(arq, "%d\t%d\t%d\t%d\t%f\n", solucao[i][j].caminho[0], solucao[i][j].caminho[1], solucao[i][j].caminho[2], solucao[i][j].caminho[3], solucao[i][j].fo);
        }
    }

    fclose(arq);
}

//Se o Nó origem é igual ao destino, vai ao hub mais próximo e volta - OK
//Se o Nó origem é um hub, vai ao hub mais próximo e volta - OK
//Se o Nó origem não é um hub, vai ao hub mais próximo verifica se o destino é mais proximo do hub, se for, vai ao hub, hub destino.

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
