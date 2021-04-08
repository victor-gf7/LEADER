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

        grafo.GRAPHshow(graph);

        //Encapsula oObjetos em uma mensagem
        beaconMessage->setRoadSender(mobility->getRoadId().c_str());
        beaconMessage->setIdSender(myId);
        beaconMessage->setTypeDevice("CAR");
        if(!vizinhosList.empty()){
            beaconMessage->setVizinhos(vizinhosList);
        }
        beaconMessage->setMatrixAdj(std::pair<int, int**>(myId, graph->adj));

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
    }case MAKE_DBSCAN_EVT: {
        std::cout << "VAI DAR CERTO" << endl;
        break;
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
 * verifica se os nos tem distancia suficiente para serem vizinhos
 */
bool MyApplicaton::checkNeighbor(Coord pointCore, Coord pointTarget){

    double distance = pow(pointCore.x - pointTarget.x,2)+pow(pointCore.y - pointTarget.y,2)+pow(pointCore.z - pointTarget.z,2);

    std::cout << "A distancia entre eles é: " << distance <<endl;
    if(distance > 1.0 && distance <= 10000.0){
        return true;
    }

    return false;
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
            //preenchendo posições
            target.x = bsm->getSenderPos().x;
            target.y = bsm->getSenderPos().y;

            std::cout << "Calculando distancia entre "<< myId << " e " << BC->getIdSender() << endl;
            if(checkNeighbor(curPosition, target)){
                grafo.GRAPHinsertArc(graph, myId, BC->getIdSender());
                vizinhosList.insert(std::pair<int, std::string>(BC->getIdSender(), posString));


                if(!BC->getVizinhos().empty()){
                    std::map<int, std::string> vizinhosSenderList = BC->getVizinhos();

                    for(itVizinhosList = vizinhosSenderList.begin(); itVizinhosList != vizinhosSenderList.end(); ++itVizinhosList){
                         grafo.GRAPHinsertArc(graph, BC->getIdSender(), itVizinhosList->first);
                    }
                }
            }
        } else{
            if(BC->getDbscanExecutado()){
                std::cout << "DBSCAN já executado. Limpando matriz..." << endl;
                grafo.clearAdjMatrix(graph);
            }
        }

        break;
    }case MAKE_DBSCAN_EVT: {
        std::cout << "VAI DAR CERTO" << endl;
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
