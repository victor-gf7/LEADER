

#ifndef SRC_MYRSUAPP_H_
#define SRC_MYRSUAPP_H_

#include <omnetpp.h>

#include "msg/BeaconMessage_m.h"
#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"

namespace veins_manhattan {

    class MyRSUApp : public BaseWaveApplLayer {

        protected:

            virtual void onWSA(WaveServiceAdvertisment* wsa);
            virtual void onWSM(WaveShortMessage* wsm);

            /*Envio e recebimento de Mensagens*/
            virtual void handleLowerMsg(cMessage* msg);
            virtual void handleSelfMsg(cMessage* msg);

        };

};
#endif
