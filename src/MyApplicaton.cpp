#include "MyApplicaton.h"

using namespace veins_manhattan;

Define_Module(MyApplicaton);

void MyApplicaton::initialize(int stage) {

    if (outputFile.is_open()) {
        outputFile.close();
    }

    BaseWaveApplLayer::initialize(stage);

    if (stage == 0) {
        //setup veins pointers
        mobility = TraCIMobilityAccess().get(getParentModule());
        traci = mobility->getCommandInterface();
        traciVehicle = mobility->getVehicleCommandInterface();

        lastDroveAt = simTime();
        // traciVehicle->setLaneChangeMode(0);

        sentMessage = false;
        lastDroveAt = simTime();
        graph = grafo.GRAPHinit(30);


        std::cout
                << " \n ---------------------------------------- Código do Veículo =  "
                << myId << endl;

        std::cout
                << " \n ---------------------------------------- Edge do Veículo   =  "
                << traciVehicle->getRouteId() << "\n" << endl;

        string str = "car-added;id:" + std::to_string(myId);
        //in.SendMessage(str);
    }

}

void MyApplicaton::receiveSignal(cComponent* source, simsignal_t signalID,
        cObject* obj, cObject* details) {
    //std::cout << "PASSOU AQUI: RECIVE SIGNAL" << endl;
    Enter_Method_Silent();
    if (signalID == mobilityStateChangedSignal) {
        handlePositionUpdate(obj);
    }
}

void MyApplicaton::handlePositionUpdate(cObject* obj) {

    BaseWaveApplLayer::handlePositionUpdate(obj);

    // stopped for for at least 10s?
    if (mobility->getSpeed() < 1) {
        if (simTime() - lastDroveAt >= 10 && sentMessage == false) {
            //findHost()->getDisplayString().updateWith("r=16,red"); // mostra um circulo indicandoo raio de comunicação
            sentMessage = true;

            WaveShortMessage* wsm = new WaveShortMessage();
            populateWSM(wsm);
            wsm->setWsmData(mobility->getRoadId().c_str());

            //host is standing still due to crash
            if (dataOnSch) {
                startService(Channels::SCH2, 42, "Traffic Information Service");
                //started service and server advertising, schedule message to self to send later
                scheduleAt(computeAsynchronousSendingTime(1, type_SCH), wsm);
            } else {
                //send right away on CCH, because channel switching is disabled
                sendDown(wsm);
            }
        }
    }
}
/**
 * Envia as mensagens
 */
void MyApplicaton::handleSelfMsg(cMessage* msg) {
    std::cout << "Veiculo: " << myId << " -> Enviando Mensagen" << endl;
    switch (msg->getKind()) {
    case SEND_BEACON_EVT: {

        std::cout << "Tipo de Mensagem: BEACON" << endl;
        //Cria um objeto  BasicSafetyMessage
        BasicSafetyMessage* bsm = new BasicSafetyMessage();

        BeaconMessage* beaconMessage = new BeaconMessage("beacon");

        //Encapsula oObjetos em uma mensagem
        beaconMessage->setRoadSender(mobility->getRoadId().c_str());
        beaconMessage->setIdSender(myId);
        beaconMessage->setTypeDevice("CAR");
        if(!vizinhosList.empty()){
            beaconMessage->setVizinhos(vizinhosList);

        }

        std::ostringstream strs;
        strs << mobility->getCurrentSpeed().length();
        const char* myS = strs.str().c_str();
        beaconMessage->setCarSpeed(myS);
        bsm->encapsulate(beaconMessage);

        //popula a memsagem com os dados encapsulados
        populateWSM(bsm);

        //Envia para a infraestrutura
        sendDown(bsm);

        std::cout << " \n ------------------------------------- "
                << "\n [CAR] :: ENVIANDO MENSAGEM ------"
                << "\n -- Mensagem enviada pelo veículo: " << myId
                << "\n ------------------------------------- \n\n\n";

        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
        break;
    }
    case SEND_WSA_EVT: {
        WaveServiceAdvertisment* wsa = new WaveServiceAdvertisment();
        populateWSM(wsa);
        sendDown(wsa);
        scheduleAt(simTime() + wsaInterval, sendWSAEvt);
        break;
    }
    case MAKE_DBSCAN_EVT: {

        eventoExecutado = true;
        std::cout << "O veiculo " << myId << "executou o DBSCAN" << endl;

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
            std::cout << "Inicio da instancia" << endl;
            EgoBetweenness egoBetweenness;
            std::cout << "Fim instancia, inicio load matriz" << egoBetweenness.getEgoBetweenness() << endl;
            egoBetweenness.loadInitialAdjMatrix(graph->adj, 30);
            std::cout << "fim load,inicio betweeness " << endl;
            grafo.GRAPHshow(graph);
            std::cout << "Vehicle: " << myId <<" Ego-Value: " << egoBetweenness.getEgoBetweenness() << endl;
            std::cout << "Vehicle: " << myId <<" dimensão matriz: " << egoBetweenness.getDimensionMatrix() << endl;
            std::cout << "Vehicle: " << myId <<" max ego-value: " << egoBetweenness.getMaxEgoBetweenness() << endl;
            egoBetweenness.egoBetweenness(graph->adj);
            std::cout << "Vehicle: " << myId <<" Ego-Value: " << egoBetweenness.getEgoBetweenness() << endl;
            //MOSTRAR RESULTADOS
            iDBSCAN = 0;
            std::cout << "number of points: " << ds.getTotalPointSize() << endl;
            //outputFile << ";" << ds.getTotalPointSize() << "\n";
            pointsResult = ds.m_points;
            while (iDBSCAN < ds.getTotalPointSize()) {
                posString = std::to_string(pointsResult[iDBSCAN].x) + ";"
                        + std::to_string(pointsResult[iDBSCAN].y) + ";"
                        + std::to_string(pointsResult[iDBSCAN].z);
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

        //outputFi1le << "END_MAKE_DBSCAN_EVT\n";
        points.clear();
        pointsResult.clear();
        mapDBSCAN.clear();

        scheduleAt(simTime().dbl() + windowTime, makeDBSCANEvt);

    }
    default: {
        if (msg)
            DBG_APP << "APP: Error: Got Self Message of unknown kind! Name: "
                           << msg->getName() << endl;
        break;
    }
    }
}

void MyApplicaton::onWSM(WaveShortMessage* wsm) {
    std::cerr << "\n Chegou uma Wave Short Message em: onWSM";
}

/**
 * recebe as mensagens
 */
void MyApplicaton::onBSM(BasicSafetyMessage* bsm) {

    std::cout << "Veiculo: " << myId << "-> Recebeu uma mensagem" << endl;

    BeaconMessage* BC = dynamic_cast<BeaconMessage*>(bsm->decapsulate());

    std::cout << BC->getTypeDevice() << " que enviou " << BC->getIdSender()
            << endl;

    switch (bsm->getKind()) {
    case SEND_BEACON_EVT: {
        std::cout << "Menssagem de BEACON recebida" << endl;
        posString = std::to_string(bsm->getSenderPos().x) + ";"
                + std::to_string(bsm->getSenderPos().y) + ";"
                + std::to_string(bsm->getSenderPos().z);
        //count =  std::to_string(BC->getSrcProcId());
        if ((std::string) BC->getTypeDevice() != "RSU") {

            grafo.GRAPHinsertArc(graph, myId, BC->getIdSender());
            vizinhosList.insert(std::pair<int, std::string>(BC->getIdSender(), posString));

            std::map<int, std::string> vizinhosSenderList = BC->getVizinhos();

            if(!BC->getVizinhos().empty()){
                for(itVizinhosList = vizinhosSenderList.begin(); itVizinhosList != vizinhosSenderList.end(); ++itVizinhosList){
                    grafo.GRAPHinsertArc(graph, BC->getIdSender(), itVizinhosList->first);
               }
            }


            mapDBSCAN.insert(
                    std::pair<int, std::string>(BC->getIdSender(), posString));
            DBSCANmap.insert(
                    std::pair<std::string, int>(posString, BC->getIdSender()));
        }

        if (eventoEscalonado == 0) {
            std::cout << "Escalonamento DBSCAN realizado pelo veiculo: " << myId
                    << endl;
            eventoEscalonado = 1;
            makeDBSCANEvt = new cMessage("Make DBSCAN Event", MAKE_DBSCAN_EVT);
            windowTime = par("windowTime").doubleValue();

            scheduleAt(simTime().dbl() + windowTime, makeDBSCANEvt);
        }
        break;
    }
    default: {
        std::cout << "onBSM - O tipo da mensagem não foi detectado."
                << bsm->getKind() << endl;
        break;
    }
    }

    //if (myId == 12) {
    //     traciVehicle -> setMaxSpeed(50);
    //     traciVehicle -> setSpeed(50);
    // }

    int velo_antiga = traciVehicle->getMaxSpeed();

    if (BC != NULL) {

        string id = std::to_string(myId);
        string tipo = "CAR";
        string pista = BC->getRoadSender();
        string veloact = BC->getCarSpeed();
        string velomax = std::to_string(traciVehicle->getMaxSpeed());
        string stime = simTime().str();

//        std::cout << "CAR;" << ";" << "id:" << id << ";" << "tipo:" << tipo
//                << ";" << "pista:" << pista << ";" << "Cveloact:" << veloact
//                << ";" << "velomax:" << velomax << ";\n";

        string QS = "car-velo;";
        QS = QS + "id:" + id + ";";
        QS = QS + "tipo:" + tipo + ";";
        QS = QS + "pista:" + pista + ";";
        QS = QS + "veloact:" + veloact + ";";
        QS = QS + "velomax:" + velomax + ";";
        QS = QS + "simtime:" + stime + ";\n";
        //std::cout << QS;

        //MENSAGEM ENCAPSULADA
        std::cout << "\n ------------------------------------- " << "\n ["
                << BC->getTypeDevice()
                << "] X [CAR] :: MENSAGEM RECEBIDA -------"
                << "\n -- Veículo          : " << myId
                << "\n -- Velocidade atual : " << BC->getCarSpeed()
                << "\n -- Desencapsulando a Mensagem ----------- "
                << "\n    -- Dispositivo Emissor   : " << BC->getTypeDevice()
                << "\n    -- Còdigo do Dispositivo : " << BC->getIdSender()
                << "\n    -- Estrada : " << BC->getRoadSender()
                << "\n    -- Posição X: " << bsm->getSenderPos().x
                << "Posição Y: " << bsm->getSenderPos().y
                << "\n ------------------------------------- \n\n\n";
    } else {

        std::cout << " \n ------------------------------------- "
                << "\n [My CAR] :: RECEBIDA :: MENSAGEM NORMAL ----"
                << "\n -- Mensagem recebida pelo veículo: " << myId
                << "\n -------------------------------------\n\n\n";
    }
}

void MyApplicaton::writelogfile() {

    ofstream myfile;
    myfile.open("exemplo.txt");
    myfile << "Writing this to a file.\n";
    myfile.close();

}

// TRASH
/*
 //LISTA DE PISTAS
 std::list<std::string> rotas;
 rotas = traci->getRouteIds();
 for (std::list<std::string>::iterator it = rotas.begin(); it != rotas.end(); it++){
 std::cout << " rotas: " << *it << "";
 std::cout << "\n";
 }
 */

/* add a vehicle */
//traci.vehicle.add(vehID=vehicle_id, routeID="", typeID="veh_passenger")
/* call until a route is provided:*/
//traci.vehicle.moveToXY(vehicle_id, "", 0, initial_x, initial_y, keepRoute=0)
/* once the target edge is provided: */
//edge = traci.vehicle.getRoadID(vehicle_id)
//route = traci.simulation.findRoute(edge, target_edge)
//traci.route.add(route_id, route.edges)
//traci.vehicle.setRouteID(vehicle_id, route_id)
