

#ifndef SRC_MYAPPLICATON_H_
#define SRC_MYAPPLICATON_H_

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

#include "msg/BeaconMessage_m.h"
#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "dbscan.h"
#include "Grafo.h"
#include "EgoBetweenness.h"
#define MINIMUM_POINTS 4     // minimum number of cluster
#define EPSILON_DBSCAN (50.0)  //distance for clustering, metre^2

using namespace std;

using std::string;

using Veins::TraCIMobility;
using Veins::TraCICommandInterface;

namespace veins_manhattan {

    class MyApplicaton : public BaseWaveApplLayer {

        public:
            virtual void initialize(int stage);
            virtual void receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj, cObject* details);
        protected:
            enum selfMessageKinds {
                SEND_CLOUD_MSG_EVT,
                MSG_CLOUD_EVT,
                MAKE_DBSCAN_EVT,
                BEACON_MSG_EVT,
            };
            TraCIMobility* mobility;
            TraCICommandInterface* traci;
            TraCICommandInterface::Vehicle* traciVehicle;

            simtime_t lastDroveAt; // the last time this sent a message
            bool sentMessage;
            int currentSubscribedServiceId;

            virtual void onWSM(WaveShortMessage* wsm);
            virtual void onBSM(BasicSafetyMessage* bsm);
            virtual void handlePositionUpdate(cObject* obj);
            virtual void handleSelfMsg(cMessage* msg);
            virtual bool checkNeighbor(Coord pointCore, Coord pointTarget);


            string posString;
            Coord ponto;
            Coord target;
            Graph graph;
            Grafo grafo;
            std::map<int, std::string> vizinhosList;
            std::map<int, std::string>::iterator itVizinhosList;


            //WRITE FILE
            ofstream outputFile;


            //inventando moda!
            virtual void writelogfile();


    };
}
#endif /* SRC_MYNODES_H_ */



