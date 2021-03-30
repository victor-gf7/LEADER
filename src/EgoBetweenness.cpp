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

#include "EgoBetweenness.h"

EgoBetweenness::EgoBetweenness() {

    //memory = new veins_manhattan::MatrixAllocateMemory();

    // each vehicle has an initial matrix of dimension equal to 30
    //AdjMatrix = (int **)malloc(30 * 30 * sizeof(int));
    //loadInitialAdjMatrix(AdjMatrix, 30);
    this->ego = 0;
    this->maxEgo = 0;
}

EgoBetweenness::~EgoBetweenness() {
    // TODO Auto-generated destructor stub
}

int EgoBetweenness::getDimensionMatrix(){
    return this->dimensionMatrix;
}

double EgoBetweenness::getEgoBetweenness(){
    return this->ego;
}

double EgoBetweenness::getMaxEgoBetweenness(){
    return this->maxEgo;
}

void EgoBetweenness::loadMatrixTest(int **Matrix){

    int dimension;

    std::ostringstream path;
    path << "./matrix2.txt";
    std::string o_trans = path.str();
    std::fstream file_trans(o_trans);

    file_trans >> dimension;

    for(int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
            file_trans >> Matrix[i][j];
        }
    }
    file_trans.close();
}

void EgoBetweenness::clearAdjMatrix(int **AdjMatrix){

    loadInitialAdjMatrix(AdjMatrix, this->dimensionMatrix);
}

void EgoBetweenness::loadInitialAdjMatrix(int **Matrix, int dimension){

    this->dimensionMatrix = dimension;

    for(int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
            if(i == 0 && j == 0) Matrix[i][j] = 0;
            else if(j == 0 || i == 0) Matrix[i][j] = 1;
            else Matrix[i][j] = 0;
        }
    }
}

void EgoBetweenness::updateMatrix(int **Matrix, int **AdjMatrix){

    for(int i = 0; i < this->dimensionMatrix; i++){
        for(int j = 0; j < this->dimensionMatrix; j++){
            if(AdjMatrix[i][j] != Matrix[i][j] && Matrix[i][j] == 1){
                AdjMatrix[i][j] = Matrix[i][j];
            }
        }
    }
}

void EgoBetweenness::printMatrix(int **Matrix){

    for(int i = 0; i < this->dimensionMatrix; i++){
        for(int j = 0; j < this->dimensionMatrix; j++){
            std::cout << Matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void EgoBetweenness::printAdjMatrix(int **AdjMatrix){

    for(int i = 0; i < this->dimensionMatrix; i++){
        for(int j = 0; j < this->dimensionMatrix; j++){
            std::cout << AdjMatrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int **EgoBetweenness::MATRIXint(int r, int c){
    int **m = (int **) malloc( r * sizeof (int *));
    for (int i = 0; i < r; ++i)
       m[i] = (int *) malloc( c * sizeof (int));
    for (int i = 0; i < r; ++i)
       for (int j = 0; j < c; ++j)
          m[i][j] = 0;
    return m;
}

// A*A = square
// 1-A = subst
// A*A[1-A]
void EgoBetweenness::egoBetweenness(int **AdjMatrix){

    //memory = new AllocatingMemory();

    int **subst = MATRIXint(this->dimensionMatrix, this->dimensionMatrix);
    int **square = MATRIXint(this->dimensionMatrix, this->dimensionMatrix);

    double reciprocal = 0.0;
    int sum;

    // 1-A
    for(int i = 0; i < this->dimensionMatrix; i++){
        for(int j = 0; j < this->dimensionMatrix; j++){
            subst[i][j] = 1 - AdjMatrix[i][j];
        }
    }

    // A*A
    for(int i = 0; i < this->dimensionMatrix; i++){
        for(int j = 0; j < this->dimensionMatrix; j++){
            sum = 0;
            for(int k = 0; k < this->dimensionMatrix; k++){
                sum += AdjMatrix[i][k] * AdjMatrix[k][j];
            }
            square[i][j] = sum;
        }
    }

    std::cout << "Inicio for A*A[1-A]" << endl;
    // the egocentric betweenness is the sum of the reciprocal of the resulting non-zero elements
    for(int i = 0; i < this->dimensionMatrix; i++){
        for(int j = 0; j < this->dimensionMatrix; j++){
            if(i < j && subst[i][j] != 0){
                reciprocal += (double(1.0)/double(square[i][j]));
            }
        }
    }
    this->ego = reciprocal;
    if(this->ego > this->maxEgo) this->maxEgo = this->ego;
}

