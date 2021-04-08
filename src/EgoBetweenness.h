//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef SRC_EGOBETWEENNESS_H_
#define SRC_EGOBETWEENNESS_H_

//#pragma once

//#include <AllocatingMemory.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class EgoBetweenness {
protected:

    //int *memory;
    int dimensionMatrix;
    double ego;
    double maxEgo;

public:
    //int **AdjMatrix;
    EgoBetweenness();
    virtual ~EgoBetweenness();
    virtual void loadMatrixTest(int **Matrix);
    virtual int **MATRIXint( int r, int c);
    virtual void clearAdjMatrix(int **AdjMatrix);
    virtual void loadInitialAdjMatrix(int **Matrix, int dimension);
    virtual int getDimensionMatrix();
    virtual double getEgoBetweenness();
    virtual double getMaxEgoBetweenness();
    virtual void updateMatrix(int **Matrix, int **AdjMatrix);
    virtual void printMatrix(int **Matrix);
    virtual void printAdjMatrix(int **AdjMatrix, int dimension);
    virtual void egoBetweenness(int **AdjMatrix);
};

#endif /* SRC_EGOBETWEENNESS_H_ */
