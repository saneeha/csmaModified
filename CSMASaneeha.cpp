/*
 * CSMASaneeha.cc
 *
 *  Created on: Sep 11, 2013
 *      Author: Saneeha
 */

#include "CSMASaneeha.h"
#include <cassert>

//#include "CSMA802154.h"
#include "csma.h"
#include "DeciderResult802154Narrow.h"
#include "Decider802154Narrow.h"
#include "PhyToMacControlInfo.h"
#include "MacToNetwControlInfo.h"
#include "SaneehaNode.h"
#include "MacPkt_m.h"
//#ifndef MIXIM_INET
#define MIXIM_INET

Register_Class(CSMASaneeha);

void CSMASaneeha::initialize(int stage)
{
     EV<<"Init CSMA Nic Saneeha";
     csma::initialize(stage);
 txPower=csma::txPower - 12.5;
 bitrate=csma::bitrate;
}

cPacket *CSMASaneeha::decapsMsg(macpkt_ptr_t macPkt)    {

    cPacket * msg = csma::decapsMsg(macPkt);
if (msg->hasPar("SrcAddr")) {
    EV<<"received from "<<msg->par("SrcAddr").longValue();
}

    // get bit error rate
       PhyToMacControlInfo* cinfo = static_cast<PhyToMacControlInfo*> (macPkt->getControlInfo());
    const DeciderResult802154Narrow* result = static_cast<const DeciderResult802154Narrow*> (cinfo->getDeciderResult());
    //double ber = result->getBER();
    rssi = result->getRSSI();recordScalar("rssi",rssi,"mW");
    recordScalar("TXPower",txPower,"mW");
    recordScalar("Bitrate",bitrate,"bps");

    double RDBM=10*log(rssi)+30;
       /* if(RDBM>=-100 && txPower>=50) {
            txPower=txPower - 25;
        // txPower=par("txPower");
        // recordScalar("TXPower",txPower,"mW");
        RDBM=10*log(rssi)+30;
        }
    else
        if(txPower == 25 && RDBM > -90 && bitrate < 1000000) { // limiting bit rate to 4 x 250000
            bitrate= bitrate*2;
            EV<<"current bit rate is"<<bitrate;
    }

    else if(RDBM < -100){
        txPower=txPower + 25;
   }
    else ;*/
     EV<<" Got the RSSI as " <<rssi <<"RSSI-dBM"<<RDBM<<"txPower calculated under CSMASaneeha "<<txPower ;
//get control info attached by base class decapsMsg method
//and set its rssi and ber
     assert(dynamic_cast<MacToNetwControlInfo*>(msg->getControlInfo()));
     MacToNetwControlInfo* cInfo = static_cast<MacToNetwControlInfo*>(msg->getControlInfo());
// cInfo->setBitErrorRate(ber);
    cInfo->setRSSI(rssi);
    double TXDBM=10*log(txPower/1000);
    EV<<"transmit power in dbm "<<TXDBM <<"range "<<transRange(txPower)<<"\n";
    msg->addPar("TXDBM").setDoubleValue(TXDBM);
    csma::txPower=txPower;
    csma::bitrate=bitrate;
        return msg;
}
void CSMASaneeha::handleLowerControl(cMessage *msg) {
     csma::handleLowerControl(msg);
}

double CSMASaneeha::transRange(double txPower) {
    double TXDB= 10*log(txPower/1000);
    pLoss = TXDB + 100 - 30;
    double expon = pLoss/(10*3);
    double range= pow(10,expon);
   // EV<<"Tranmission Range"<<range;
    return (range);

}
//#endif
