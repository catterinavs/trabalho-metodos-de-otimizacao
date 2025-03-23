
//Quantidade de hubs
typedef struct coord{
    float x;
    float y;
}Coord;

void leArquivo(char* nome_arquivo);

float distancia(Coord a, Coord b);

void escolheHubs(int *hubs);

float calculaFOPorCaminho(int* caminho);

int isHub(int no);

int hubMaisProximo(int pontoOrigem);

void printaSolucao();

void leArquivoSolucao(char* nome_arquivo);

void calculaMatrizDistancias();

//grasp

