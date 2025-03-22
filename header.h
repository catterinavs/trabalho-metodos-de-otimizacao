

//Quantidade de hubs


typedef struct coord{
    float x;
    float y;
}Coord;

typedef struct solucao{
    int caminho[4];
    float fo;
}Solucao; 




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

void calculaMatrizDistancias();