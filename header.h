#define MAX_PONTOS 200

typedef struct coord{
    float x;
    float y;
}Coord;

int num_nos = 0;
Coord coordenadas[MAX_PONTOS];

void leArquivo();

float distancia(Coord a, Coord b);