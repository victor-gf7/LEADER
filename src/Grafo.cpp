/*
 * Grafo.cpp
 *
 *  Created on: Mar 22, 2021
 *      Author: joao
 */

#include "Grafo.h"

Grafo::Grafo() {
    // TODO Auto-generated constructor stub

}

Grafo::~Grafo() {
    // TODO Auto-generated destructor stub
}

Grafo::Grafo(int V)
{
    this->V = V; // atribui o número de vértices
    adj = new list<int>[V]; // cria as listas
}

void Grafo::adicionarAresta(int v1, int v2)
{
    // adiciona vértice v2 à lista de vértices adjacentes de v1
    adj[v1].push_back(v2);
}

int Grafo::obterGrauDeSaida(int v)
{
    // basta retornar o tamanho da lista que é a quantidade de vizinhos
    return adj[v].size();
}

bool Grafo::existeVizinho(Graph G, int v1, int v2)
{
    std::cout << "Checking..." << v1<< v2 <<endl;
    GRAPHshow(G);
    if (G->adj[v1][v2] == 1) {
        std::cout << "Ainda é" << endl;
        return true;
    }
    std::cout << "Não é" << endl;
    return false;

//    if(find(adj[v1].begin(), adj[v1].end(), v2) != adj[v1].end())
//        return true;
//    return false;
}

/* REPRESENTAÇÃO POR MATRIZ DE ADJACÊNCIAS: A função GRAPHinit() constrói um grafo com vértices 0 1 .. V-1 e nenhum arco. */
Graph Grafo::GRAPHinit( int V) {
   Graph G = (Graph) malloc( sizeof *G);
   G->V = V;
   G->A = 0;
   G->adj = MATRIXint( V, V, 0);
   return G;
}
/* REPRESENTAÇÃO POR MATRIZ DE ADJACÊNCIAS: A função MATRIXint() aloca uma matriz com linhas 0..r-1 e colunas 0..c-1. Cada elemento da matriz recebe valor val. */
int **Grafo::MATRIXint( int r, int c, int val) {
   int **m = (int **) malloc( r * sizeof (int *));
   for (int i = 0; i < r; ++i)
      m[i] = (int *) malloc( c * sizeof (int));
   for (int i = 0; i < r; ++i)
      for (int j = 0; j < c; ++j)
         m[i][j] = val;
   return m;
}
/* REPRESENTAÇÃO POR MATRIZ DE ADJACÊNCIAS: A função GRAPHinsertArc() insere um arco v-w no grafo G. A função supõe que v e w são distintos, positivos e menores que G->V. Se o grafo já tem um arco v-w, a função não faz nada. */
void Grafo::GRAPHinsertArc(Graph G, int v, int w) {
   if (G->adj[v][w] == 0) {
      G->adj[v][w] = 1;
      G->A++;
   }
}
/* REPRESENTAÇÃO POR MATRIZ DE ADJACÊNCIAS: A função GRAPHremoveArc() remove do grafo G o arco v-w. A função supõe que v e w são distintos, positivos e menores que G->V. Se não existe arco v-w, a função não faz nada. */
void Grafo::GRAPHremoveArc( Graph G, int v, int w) {
   if (G->adj[v][w] == 1) {
      G->adj[v][w] = 0;
      G->A--;
   }
}
/* REPRESENTAÇÃO POR MATRIZ DE ADJACÊNCIAS: A função GRAPHshow() imprime, para cada vértice v do grafo G, em uma linha, todos os vértices adjacentes a v. */
void Grafo::GRAPHshow( Graph G) {
   for (int v = 0; v < G->V; ++v) {
      for (int w = 0; w < G->V; ++w)
            printf( " %2d", G->adj[v][w]);
      printf( "\n");
   }
}

void Grafo::clearAdjMatrix(Graph G){

    for(int i = 0; i < G->V; i++){
        for(int j = 0; j < G->V; j++){
            G->adj[i][j] = 0;
        }
    }
}
