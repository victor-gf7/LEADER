/*
 * Grafo.h
 *
 *  Created on: Mar 22, 2021
 *      Author: joao
 */

#ifndef SRC_GRAFO_H_
#define SRC_GRAFO_H_

#include <iostream>
#include <list>
#include <algorithm>

using namespace std;

struct graph {
   int V;
   int A;
   int **adj;
};

typedef struct graph *Graph;

class Grafo {
public:
    int V; // número de vértices
    list<int> *adj;
    Grafo(int V); // construtor
    void adicionarAresta(int v1, int v2); // adiciona uma aresta no grafo
    // obtém o grau de saída de um dado vértice
    // grau de saída é o número de arcos que saem de "v"
    int obterGrauDeSaida(int v);
    bool existeVizinho(int v1, int v2);

    /*Nova abordagem*/
    void clearAdjMatrix(Graph G);
    Graph GRAPHinit( int V);
    int **MATRIXint( int r, int c, int val);
    void GRAPHinsertArc( Graph G, int v, int w);
    void GRAPHremoveArc( Graph G, int v, int w);
    void GRAPHshow( Graph G);
    Grafo();
    virtual ~Grafo();
};

#endif /* SRC_GRAFO_H_ */
