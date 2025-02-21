#define MAX_PONTOS 200

//Quantidade de hubs
#define HUBS 4

typedef struct coord{
    float x;
    float y;
}Coord;

typedef struct solucao{
    int caminho[4];
    float fo;
}Solucao; 

int num_nos = 0;
int hubs[HUBS];
Solucao solucao[200][200];

// float range_escolha_pontos = 0.5;

float fo = 0;

Coord coordenadas[MAX_PONTOS];

void leArquivo(char* nome_arquivo);

float distancia(Coord a, Coord b);

void escolheHubs();

float calculaFOPorCaminho(int* caminho);

Solucao criaSolucao(int origem, int destino);

int isHub(int no);

int hubMaisProximo(int pontoOrigem);

void printaSolucao();

void printaSolucaoArquivo(char* nome_arquivo);

void printaSolucaoConsole();

void leArquivoSolucao(char* nome_arquivo);

void buscaLocal();

void grasp(int maxIteracoes);

float calculaFO();

float calculaCustoMinimo(int origem, int destino);