#include <iostream>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "header.h"
#include <bits/algorithmfwd.h>
#include <memory.h>

using namespace std;

int main(){

    leArquivo("instances/inst5.txt");

    //escolheHubs();
    //for(int i=0; i<HUBS; i++){
    //    cout << hubs[i] << " ";
    //}

    escolheHubsAleatoriaGulosa();

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

//Aleatoria
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
    
    qsort(hubs, HUBS, sizeof(int), [](const void* a, const void* b) -> int {
        return *(int*)a - *(int*)b;
    });
}

void escolheHubsAleatoriaGulosa(){

    double distancia_total_x = 0;
    double distancia_total_y = 0;

    for(int i = 0; i < num_nos; i++){
        distancia_total_x += coordenadas[i].x;
        distancia_total_y += coordenadas[i].y;
    }

    Coord ponto_central;

    ponto_central.x = distancia_total_x/num_nos;
    ponto_central.y = distancia_total_y/num_nos;

    printf("Ponto central: %f %f\n", ponto_central.x, ponto_central.y);

    double x_min = ponto_central.x - ponto_central.x*range_escolha_pontos;
    double x_max = ponto_central.x + ponto_central.x*range_escolha_pontos;

    double y_min = ponto_central.y - ponto_central.y*range_escolha_pontos;
    double y_max = ponto_central.y + ponto_central.y*range_escolha_pontos;

    int vetor_pontos_candidatos[num_nos];

    memset(vetor_pontos_candidatos, -1, sizeof(vetor_pontos_candidatos));

    for(int i = 0; i < num_nos; i++){
        if(coordenadas[i].x >= x_min && coordenadas[i].x <= x_max && coordenadas[i].y >= y_min && coordenadas[i].y <= y_max){
            vetor_pontos_candidatos[i] = i;
        }
    }

    int i = 0;
    int j = 0;

    while(i < HUBS){
        if(vetor_pontos_candidatos[j] != -1){
            hubs[i] = vetor_pontos_candidatos[j];
            i++;
        }
        j++;
    }
}