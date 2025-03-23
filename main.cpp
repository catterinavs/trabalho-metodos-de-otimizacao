#include <iostream>
#include <ctime>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <limits>
#include <chrono>
#include <vector>
#include <numeric>
#include <iomanip>

using namespace std;

// ================= CONFIGURAÇÕES =================
const int HUBS = 2;
const int TEMPO_MAXIMO = 60;
const char* ARQUIVO_INSTANCIA = "instances/inst10.txt";
// ==================================================

#define MAX_PONTOS 200

typedef struct solucao {
    int hubs[HUBS];
    float fo;
} Solucao;

typedef struct coord {
    float x;
    float y;
} Coord;

// Variáveis globais
int num_nos = 0;
float matriz_distancias[MAX_PONTOS][MAX_PONTOS];
Coord coordenadas[MAX_PONTOS];

// Protótipos de funções
void leArquivo(const char* nome_arquivo);
float distancia(Coord a, Coord b);
void calculaMatrizDistancias();
void escolheHubs(int *hubs);
void criaSolucao(Solucao *solucao, int *hubs);
void buscaLocal(Solucao *solucao);
void clonarSolucao(Solucao *novaSolucao, Solucao *velhaSolucao);
int isHub(int no, int *hubs);
void printaSolucaoConsole(Solucao *solucao);
void grasp(Solucao *solucaoFinal, double &tempo_melhor);

int main() {
    srand(time(NULL));
    
    leArquivo(ARQUIVO_INSTANCIA);
    calculaMatrizDistancias();

    vector<double> tempos_melhor;

    for(int exec = 0; exec < 3; exec++) {
        Solucao solucao;
        double tempo_melhor;
        
        cout << "\n=== EXECUÇÃO " << exec+1 << " ===" << endl;
        grasp(&solucao, tempo_melhor);
        tempos_melhor.push_back(tempo_melhor);
        
        cout << "\nResultado execução " << exec+1 << ":" << endl;
        printaSolucaoConsole(&solucao);
    }

    double media_tempos = accumulate(tempos_melhor.begin(), tempos_melhor.end(), 0.0) / 3;
    cout << "\n=== RESUMO FINAL ==="
         << "\nTempo médio para melhor solução: " << fixed << setprecision(2) << media_tempos << "s"
         << "\n=====================" << endl;

    return 0;
}

void leArquivo(const char* nome_arquivo) {
    FILE *arq = fopen(nome_arquivo, "r");
    if (!arq) {
        cout << "Erro ao abrir arquivo: " << nome_arquivo << endl;
        exit(1);
    }

    fscanf(arq, "%d", &num_nos);
    for(int i = 0; i < num_nos; i++) {
        fscanf(arq, "%f %f\n", &coordenadas[i].x, &coordenadas[i].y);
    }
    fclose(arq);
}

float distancia(Coord a, Coord b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void calculaMatrizDistancias() {
    for(int i = 0; i < num_nos; i++) {
        for(int j = 0; j < num_nos; j++) {
            matriz_distancias[i][j] = distancia(coordenadas[i], coordenadas[j]);
        }
    }
}

void escolheHubs(int *hubs) {
    for(int i = 0; i < HUBS; i++) {
        bool unico;
        do {
            unico = true;
            hubs[i] = rand() % num_nos;
            for(int j = 0; j < i; j++) {
                if(hubs[i] == hubs[j]) {
                    unico = false;
                    break;
                }
            }
        } while(!unico);
    }
}

void criaSolucao(Solucao *solucao, int *hubs) {
    solucao->fo = 0;
    memcpy(solucao->hubs, hubs, sizeof(int)*HUBS);

    for(int i = 0; i < num_nos; i++) {
        for(int j = 0; j < num_nos; j++) {
            float menor_custo = numeric_limits<float>::max();
            
            for(int k = 0; k < HUBS; k++) {
                for(int l = 0; l < HUBS; l++) {
                    float custo = 1.0 * matriz_distancias[i][solucao->hubs[k]]
                                + 0.75 * matriz_distancias[solucao->hubs[k]][solucao->hubs[l]]
                                + 1.0 * matriz_distancias[solucao->hubs[l]][j];
                    
                    if(custo < menor_custo) menor_custo = custo;
                }
            }
            
            if(menor_custo > solucao->fo) solucao->fo = menor_custo;
        }
    }
}

void grasp(Solucao *solucaoFinal, double &tempo_melhor_execucao) {
    Solucao melhor;
    melhor.fo = numeric_limits<float>::max();
    
    auto inicio = chrono::steady_clock::now();
    int iteracao = 0;
    double ultimo_tempo_melhor = 0.0;
    bool primeira_iteracao = true;
    
    cout << "Iniciando GRASP..." << endl;
    cout << "Tempo maximo: " << TEMPO_MAXIMO << " segundos" << endl;
    
    while(true) {
        // Verifica tempo ANTES de processar nova iteração
        auto agora_verificacao = chrono::steady_clock::now();
        double decorrido_verificacao = chrono::duration<double>(agora_verificacao - inicio).count();
        
        if(decorrido_verificacao >= TEMPO_MAXIMO) {
            cout << "\nTempo limite atingido!" << endl;
            break;
        }

        iteracao++;
        
        // Gera nova solução
        int hubs[HUBS];
        escolheHubs(hubs);
        
        Solucao atual;
        criaSolucao(&atual, hubs);
        
        if(primeira_iteracao) {
            cout << "\nSolução inicial: FO = " << fixed << setprecision(2) << atual.fo << endl;
            primeira_iteracao = false;
        }
        
        buscaLocal(&atual);

        // Mede tempo APÓS processamento
        auto agora_atualizacao = chrono::steady_clock::now();
        double decorrido_atualizacao = chrono::duration<double>(agora_atualizacao - inicio).count();
        
        // Atualiza melhor solução
        if(atual.fo < melhor.fo) {
            clonarSolucao(&melhor, &atual);
            ultimo_tempo_melhor = decorrido_atualizacao;
            cout << "Melhoria encontrada: " << fixed << setprecision(2) << melhor.fo 
                 << " | Tempo: " << fixed << setprecision(2) << ultimo_tempo_melhor << "s"
                 << " | Iter: " << iteracao 
                 << endl;
        }
    }
    
    tempo_melhor_execucao = ultimo_tempo_melhor;
    clonarSolucao(solucaoFinal, &melhor);
}

void buscaLocal(Solucao *solucao) {
    Solucao melhor_local = *solucao;
    int hubs_originais[HUBS];
    memcpy(hubs_originais, solucao->hubs, sizeof(hubs_originais));

    for(int i = 0; i < HUBS; i++) {
        int hub_original = hubs_originais[i];
        
        for(int tentativa = 0; tentativa < 10; tentativa++) {
            int novo_hub;
            do {
                novo_hub = rand() % num_nos;
            } while(isHub(novo_hub, hubs_originais));
            
            solucao->hubs[i] = novo_hub;
            criaSolucao(solucao, solucao->hubs);
            
            if(solucao->fo < melhor_local.fo) {
                melhor_local = *solucao;
            }
        }
        solucao->hubs[i] = hub_original;
    }
    
    *solucao = melhor_local;
}

void clonarSolucao(Solucao *destino, Solucao *origem) {
    memcpy(destino->hubs, origem->hubs, sizeof(int)*HUBS);
    destino->fo = origem->fo;
}

int isHub(int no, int *hubs) {
    for(int i = 0; i < HUBS; i++) {
        if(hubs[i] == no) return 1;
    }
    return 0;
}

void printaSolucaoConsole(Solucao *solucao) {
    cout << "\n=============== SOLUÇÃO ===============" << endl;
    cout << " Instância: " << ARQUIVO_INSTANCIA << endl;
    cout << " Número de nós: " << num_nos << endl;
    cout << " Número de hubs: " << HUBS << endl;
    cout << " FO (Maior custo): " << fixed << setprecision(2) << solucao->fo << endl;
    cout << " Hubs selecionados: [";
    for(int i = 0; i < HUBS; i++) {
        cout << solucao->hubs[i];
        if(i < HUBS-1) cout << ", ";
    }
    cout << "]" << endl;
    cout << "=======================================" << endl;
}