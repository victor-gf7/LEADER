#include "MyRSUApp.h"

using Veins::TraCIMobilityAccess;
using Veins::AnnotationManagerAccess;

using namespace veins_manhattan;

Define_Module(MyRSUApp);

void MyRSUApp::onWSA(WaveServiceAdvertisment* wsa) {
    std::cout << "\n PASSOU AQUI: onWSA "<<myId;
}

void MyRSUApp::onWSM(WaveShortMessage* wsm) {
    std::cout << "\n PASSOU AQUI: onWSM "<<myId;

}


void MyRSUApp::handleLowerMsg(cMessage* msg) {

    BasicSafetyMessage* bsm = check_and_cast<BasicSafetyMessage*>(msg);

    BeaconMessage* BC = dynamic_cast<BeaconMessage*>(bsm->decapsulate());

    std::cerr        << "\n ------------------------------------- "
                     << "\n [" <<  BC->getTypeDevice() << "] X [RSU] :: MENSAGEM RECEBIDA -------"
                     << "\n -- Recebida na RSU : "<< myId
                     << "\n -- Desencapsulando a Mensagem ----------- "
                     << "\n    -- Envida pelo veiculo: " << BC->getIdSender()
                     << "\n    -- Estrada : " << BC->getRoadSender()
                     << "\n -------------------------------------\n ";
}

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
        default: {
            if (msg)
                DBG_APP << "APP: Error: Got Self Message of unknown kind! Name: " << msg->getName() << endl;
            break;
        }
    }
}

