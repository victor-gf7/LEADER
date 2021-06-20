

#ifndef SRC_MYRSUAPP_H_
#define SRC_MYRSUAPP_H_

#include <omnetpp.h>

#include "msg/BeaconMessage_m.h"
#include "dbscan.h"
#include "EgoBetweenness.h"
#include "Leader.h"
#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"

#define MINIMUM_POINTS 4     // minimum number of cluster
#define EPSILON_DBSCAN (50.0) //em metros


namespace veins_manhattan {

    class MyRSUApp : public BaseWaveApplLayer {

        protected:


            enum selfMessageKinds {
                SEND_CLOUD_MSG_EVT,
                MSG_CLOUD_EVT,
                MAKE_DBSCAN_EVT,
                BEACON_MSG_EVT,
            };

            virtual void onWSA(WaveServiceAdvertisment* wsa);
            virtual void onWSM(WaveShortMessage* wsm);
            virtual void initialize(int stage);

            /*Envio e recebimento de Mensagens*/
            virtual void handleLowerMsg(cMessage* msg);
            virtual void handleSelfMsg(cMessage* msg);


            //VAR
            int eventoEscalonado = 0;
            bool eventoExecutado = false;
            double windowTime;
            double actualTime;
            int sizeMap;
            int iDBSCAN; //Apenas um iterador para o DBSCAN
            //int matrizDeAdjacencia [][];
            std::map<int,std::string> mapDBSCAN;
            std::map<std::string,int> DBSCANmap;
            std::map<int,std::string>::iterator itmapDBSCAN;
            std::map<std::string,int>::iterator itDBSCANmap;
            std::map<int, int**> matrizAdj;
            std::map<int, double> egosByVehicles;//id e egovalue
            std::map<int, int**>::iterator itMatrizAdj;
            std::map<int, VeiculosInfo_> infos;
            VeiculosInfo info;
            string posString;
            Point ponto;
            Leader leaderEngine;
            std::map<int, LeaderInfo> leaders;

            vector<Point_> points;
            vector<Point_> pointsResult;
            std::vector<std::string> positionByNode;

            //WRITE FILE
            ofstream outputFile;

            //MSG
            cMessage* sendCloudMSGEvt;
            cMessage* msgClouEvt;
            cMessage* makeDBSCANEvt;

        };

};
#endif
