#define MAX_PONTOS 200

//Quantidade de hubs
#define HUBS 3

typedef struct coord{
    float x;
    float y;
}Coord;

int num_nos = 0;
int hubs[HUBS];

float range_escolha_pontos = 0.25;

Coord coordenadas[MAX_PONTOS];

void leArquivo(char* nome_arquivo);

float distancia(Coord a, Coord b);

void escolheHubs();

void escolheHubsAleatoriaGulosa();

//Gulosa (Nós hubs, pontos)