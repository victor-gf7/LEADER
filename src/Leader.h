/*
 * Leader.h
 *
 *  Created on: Apr 5, 2021
 *      Author: joao
 */

#ifndef SRC_LEADER_H_
#define SRC_LEADER_H_

#include <omnetpp.h>

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

using std::string;

typedef struct VeiculosInfo_
{
    std::string posicao;
    int clusterID;  // clustered ID
    double egoValue;
}VeiculosInfo;

typedef struct LeaderInfo_
{
    int idVeiculo;
    double egoValue;
}LeaderInfo;

namespace veins_manhattan {

class Leader {
protected:
    std::map<int, VeiculosInfo_>::iterator itInfos;
    std::map<int, LeaderInfo> leaders;//clusterid, infoLiders
    std::map<int, LeaderInfo>::iterator itLeaders;
    LeaderInfo leaderInfo;
    //int iClusters;
    int maior = INT_MIN;
    int egoCandidatoLider = 0;
public:
    Leader();
    virtual ~Leader();
    virtual std::map<int, LeaderInfo> conductElection(std::map<int, VeiculosInfo_> infos);
};

} /* namespace veins_manhattan */

#endif /* SRC_LEADER_H_ */
