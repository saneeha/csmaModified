/*
 * CSMANic.h
 *
 *  Created on: Sep 11, 2013
 *      Author: Saneeha
 */

#ifndef __CSMASaneeha_H__
#define __CSMASaneeha_H__
#include <omnetpp.h>
#ifndef MIXIM_INET
#define MIXIM_INET
#include "CSMA802154.h"
#include "MiXiMDefs.h"
#include "csma.h"


class CSMASaneeha : public csma
{
public:
    double rssi;
    double pLoss;
    double bitrate;
   virtual void initialize(int stage);

   virtual cPacket *decapsMsg(macpkt_ptr_t macPkt);

   /** @brief Handle control messages from lower layer */
   virtual void handleLowerControl(cMessage *msg);
   double transRange(double txPower);
};

#endif
#endif


