#include "MyRSUApp.h"

using Veins::TraCIMobilityAccess;
using Veins::AnnotationManagerAccess;

using namespace veins_manhattan;

Define_Module(MyRSUApp);

void MyRSUApp::initialize(int stage) {

    if (outputFile.is_open()) {
        outputFile.close();
    }

    BaseWaveApplLayer::initialize(stage);

    if (stage == 0) {

        std::cout
                << " \n ---------------------------------------- Código da RSU =  "
                << myId << endl;
        string str = "car-added;id:" + std::to_string(myId);
        //in.SendMessage(str);
    }

}

void MyRSUApp::onWSA(WaveServiceAdvertisment* wsa) {
    std::cout << "\n PASSOU AQUI: onWSA "<<myId;
}

void MyRSUApp::onWSM(WaveShortMessage* wsm) {
    std::cout << "\n PASSOU AQUI: onWSM "<<myId;

}

/**
 * Recebe mensagens
 */
void MyRSUApp::handleLowerMsg(cMessage* msg) {

    BasicSafetyMessage* bsm = check_and_cast<BasicSafetyMessage*>(msg);

    BeaconMessage* BC = dynamic_cast<BeaconMessage*>(bsm->decapsulate());

    posString = std::to_string(bsm->getSenderPos().x) + ";"
            + std::to_string(bsm->getSenderPos().y) + ";"
            + std::to_string(bsm->getSenderPos().z);

    matrizAdj.insert(BC->getMatrixAdj());

    mapDBSCAN.insert(
            std::pair<int, std::string>(BC->getIdSender(), posString));
    DBSCANmap.insert(
            std::pair<std::string, int>(posString, BC->getIdSender()));

    if (eventoEscalonado == 0) {
        std::cout << "Escalonamento DBSCAN realizado pelo RSU: " << myId
                << endl;
        eventoEscalonado = 1;
        makeDBSCANEvt = new cMessage("Make DBSCAN Event", MAKE_DBSCAN_EVT);
        windowTime = par("windowTime").doubleValue();

        scheduleAt(simTime().dbl() + windowTime, makeDBSCANEvt);
    }

    std::cerr        << "\n ------------------------------------- "
                     << "\n [" <<  BC->getTypeDevice() << "] X [RSU] :: MENSAGEM RECEBIDA -------"
                     << "\n -- Recebida na RSU : "<< myId
                     << "\n -- Desencapsulando a Mensagem ----------- "
                     << "\n    -- Envida pelo veiculo: " << BC->getIdSender()
                     << "\n    -- Estrada : " << BC->getRoadSender()
                     << "\n -------------------------------------\n ";
}


/**
 * Envia mensagens
 */
void MyRSUApp::handleSelfMsg(cMessage* msg) {
    switch (msg->getKind()) {
        case SEND_BEACON_EVT: {

            //Cria um objeto  BasicSafetyMessage
            BasicSafetyMessage* bsm = new BasicSafetyMessage();
            BeaconMessage* beaconMessage = new BeaconMessage("beacon");

            //Encapsula oObjetos em uma mensagem
            //beaconMessage->setRoadSender(mobility->getRoadId().c_str());
            beaconMessage->setIdSender(myId);
            beaconMessage->setTypeDevice("RSU");
            if(eventoExecutado){
                beaconMessage->setDbscanExecutado(eventoExecutado);
                eventoExecutado = false;
            }
            if(!leaders.empty()){
                beaconMessage->setLideres(leaders);
            }
            bsm->encapsulate(beaconMessage);

            populateWSM(bsm);
            sendDown(bsm);

            std::cerr       << "\n ------------------------------------- "
                            << "\n [My RSU] :: ENVIANDO MENSAGEM -------"
                            << "\n -- Mensagem enviada pela RSU: "<<myId
                            << "\n -------------------------------------\n\n\n ";

            scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
            break;
        }
        case SEND_WSA_EVT:   {
            WaveServiceAdvertisment* wsa = new WaveServiceAdvertisment();
            populateWSM(wsa);
            sendDown(wsa);
            scheduleAt(simTime() + wsaInterval, sendWSAEvt);
            break;
        }
        case MAKE_DBSCAN_EVT: {
                eventoExecutado = true;
                std::cout << "A RSU " << myId << "executou o DBSCAN" << endl;

                if (mapDBSCAN.empty()) {
                    std::cout << "Map vazio! Não fazer DBSCAN" << endl;
                } else {
                    if (!outputFile.is_open()) {
                        outputFile.open("DBSCAN_Data.txt");
                    }
                    //std::cout << "MAKE_DBSCAN_EVT \n" << simTime().dbl() << endl;
                    //outputFile << "MAKE_DBSCAN_EVT;" << simTime().dbl();
                    actualTime = simTime().dbl();
                    sizeMap = mapDBSCAN.size();
                    //SEPARAR OS PONTOS
                    iDBSCAN = 0;
                    for (itmapDBSCAN = mapDBSCAN.begin();
                            itmapDBSCAN != mapDBSCAN.end(); ++itmapDBSCAN) {

                        positionByNode = BaseWaveApplLayer::split(itmapDBSCAN->second,
                                ";");

                        std::cout << itmapDBSCAN->first << " => " << positionByNode[0]
                                << "|" << positionByNode[1] << "|" << positionByNode[2]
                                << endl;
                        ponto.x = std::stod(positionByNode[0]);
                        ponto.y = std::stod(positionByNode[1]);
                        ponto.z = std::stod(positionByNode[2]);
                        ponto.clusterID = UNCLASSIFIED;
                        points.insert(points.begin(), ponto);
                        //points[iDBSCAN] = ponto;
                        iDBSCAN++;
                    }
                    //Rodar o DBSCAN
                    DBSCAN ds(MINIMUM_POINTS, EPSILON_DBSCAN, points);
                    ds.run();
                    EgoBetweenness ego;

                    //MOSTRAR RESULTADOS
                    iDBSCAN = 0;
                    std::cout << "number of points: " << ds.getTotalPointSize() << endl;
                    //outputFile << ";" << ds.getTotalPointSize() << "\n";
                    pointsResult = ds.m_points;

                    for(itMatrizAdj = matrizAdj.begin(); itMatrizAdj != matrizAdj.end(); ++itMatrizAdj){
                         ego.egoBetweenness(itMatrizAdj->second);
                         egosByVehicles.insert(std::pair<int, double>(itMatrizAdj->first, ego.getEgoBetweenness()));
                         std::cout << "Ego-Betweeness veticulo: " << itMatrizAdj->first << " = "<< ego.getEgoBetweenness() << endl;
                    }
                    std::cout << "Ego-Betweeness Maximo: " << ego.getMaxEgoBetweenness() <<endl;


                    while (iDBSCAN < ds.getTotalPointSize()) {
                        posString = std::to_string(pointsResult[iDBSCAN].x) + ";"
                                + std::to_string(pointsResult[iDBSCAN].y) + ";"
                                + std::to_string(pointsResult[iDBSCAN].z);

                        info.clusterID = pointsResult[iDBSCAN].clusterID;
                        info.posicao = posString;
                        info.egoValue = egosByVehicles.find(DBSCANmap[posString])->second;
                        infos.insert(std::pair<int, VeiculosInfo>(DBSCANmap[posString], info));

                        std::cout << "X: " << pointsResult[iDBSCAN].x << " Y: "
                                << pointsResult[iDBSCAN].y << " Z: "
                                << pointsResult[iDBSCAN].z << " Cluster ID: "
                                << pointsResult[iDBSCAN].clusterID << " Node ID: "
                                << DBSCANmap[posString] << endl;

                        outputFile << "X: " << pointsResult[iDBSCAN].x << ";" << "Y: "
                                << pointsResult[iDBSCAN].y << ";" << "Z: "
                                << pointsResult[iDBSCAN].z << ";" << "Cluster ID: "
                                << pointsResult[iDBSCAN].clusterID << ";"
                                << DBSCANmap[posString] << ";" << actualTime << "\n";
                        iDBSCAN++;
                    }
                }
                leaders.clear();

                leaders = leaderEngine.conductElection(infos);

                //outputFi1le << "END_MAKE_DBSCAN_EVT\n";
                infos.clear();
                egosByVehicles.clear();
                points.clear();
                pointsResult.clear();
                mapDBSCAN.clear();

                scheduleAt(simTime().dbl() + windowTime, makeDBSCANEvt);
            }
        default: {
            if (msg)
                DBG_APP << "APP: Error: Got Self Message of unknown kind! Name: " << msg->getName() << endl;
            break;
        }
    }
}

