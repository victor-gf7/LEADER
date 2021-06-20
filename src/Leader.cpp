/*
 * Leader.cpp
 *
 *  Created on: Apr 5, 2021
 *      Author: joao
 */

#include "Leader.h"

namespace veins_manhattan {

Leader::Leader() {
    // TODO Auto-generated constructor stub

}

Leader::~Leader() {
    // TODO Auto-generated destructor stub
}

std::map<int, LeaderInfo> Leader::conductElection(std::map<int, VeiculosInfo_> infos){
    std::cout << "Eleição iniciada" << endl;

    for(itInfos = infos.begin(); itInfos != infos.end(); ++itInfos){
        std::cout << "Veiculo: " << itInfos->first << " Ego-betweeness: " << itInfos->second.egoValue <<
                " ClusterID: " << itInfos->second.clusterID << " Posição: "  << itInfos->second.posicao << endl;
        egoCandidatoLider = itInfos->second.egoValue;
        if(itInfos->second.clusterID != -1){
            std::cout << "Cluster Válido" << endl;
            if(!leaders.empty()){
                std::cout << "Map não vazio" << endl;
                std::cout << "Buscando no map cluster " << itInfos->second.clusterID << endl;
                std::cout << "Foi encontrado esse valor: " << leaders.count(itInfos->second.clusterID) << endl;
                if(leaders.count(itInfos->second.clusterID) > 0){
                    std::cout << "Cluster existente no map" << endl;
                    double egoLiderAtual = leaders.find(itInfos->second.clusterID)->second.egoValue;
                    std::cout << "Verificando se egoCandidatoLider: " << egoCandidatoLider <<
                            " é maior que egoLiderAtual: " << egoLiderAtual << endl;
                    if(egoCandidatoLider > egoLiderAtual){
                        std::cout << "Sentença verdadeira, atualizando lider..." << endl;
                        leaderInfo.idVeiculo = itInfos->first;
                        leaderInfo.egoValue = egoCandidatoLider;
                        leaders[itInfos->second.clusterID] = leaderInfo;
                        //leaders.insert(std::pair<int,LeaderInfo>(itInfos->second.clusterID, leaderInfo));
                    }
                } else{
                    std::cout << "Valor inexistente no map. Inserindo" << endl;
                    leaderInfo.idVeiculo = itInfos->first;
                    leaderInfo.egoValue = itInfos->second.egoValue;
                    leaders.insert(std::pair<int,LeaderInfo>(itInfos->second.clusterID, leaderInfo));
                }
            } else {
                std::cout << "Map vazio. Inserindo..." << endl;
                leaderInfo.idVeiculo = itInfos->first;
                leaderInfo.egoValue = itInfos->second.egoValue;
                leaders.insert(std::pair<int,LeaderInfo>(itInfos->second.clusterID, leaderInfo));
            }
        }
    }
    std::cout << "Eleição finalizada" << endl;

    for(itLeaders = leaders.begin(); itLeaders != leaders.end(); ++itLeaders){

        std::cout << "ClusterID: " << itLeaders->first << " Veiculo lider: " << itLeaders->second.idVeiculo
                << " Ego-Betweeness: "<< itLeaders->second.egoValue << endl;
    }

    return leaders;

}

} /* namespace veins_manhattan */
