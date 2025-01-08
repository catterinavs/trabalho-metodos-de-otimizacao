#include <iostream>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "header.h"

using namespace std;

int main(){

    leArquivo("instances/inst5.txt");

    for (int i = 0; i<5; i++){
        cout << coordenadas[i].x << " " << coordenadas[i].y << endl;
    }

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