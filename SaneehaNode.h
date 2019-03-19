/*
 * SaneehaNode.h
 *
 *  Created on: Sep 12, 2013
 *      Author: Saneeha
 */

#ifndef SANEEHANODE_H_
#define SANEEHANODE_H_
#include <omnetpp.h>
#ifndef MIXIM_INET
#define MIXIM_INET
#include "FindModule.h"
#include "NetwPkt_m.h"
#include "SimpleAddress.h"
#include "BaseLayer.h"
#include "ArpInterface.h"
#include "BaseWorldUtility.h"
#include "BaseMacLayer.h"
#include "NetwToMacControlInfo.h"
#include "AddressingInterface.h"
#include "Flood.h"

/**
 * @brief A module to generate traffic for the NIC, used for testing purposes.
 *
 * @ingroup exampleIEEE802154Narrow
 */
class SaneehaNode : public Flood
{
public:
    typedef NetwPkt    netwpkt_t;
    typedef netwpkt_t* netwpkt_ptr_t;
private:
    /** @brief Copy constructor is not allowed.
     */
    SaneehaNode(const SaneehaNode&);
    /** @brief Assignment operator is not allowed.
     */
    SaneehaNode& operator=(const SaneehaNode&);

public:
    enum TrafficGenMessageKinds{
        TRAFFIC_TIMER = 1,
        BROADCAST_MESSAGE,
        TARGET_MESSAGE
    };

protected:

    int packetLength;
    simtime_t packetTime;
     double pppt;
    int burstSize;
    int remainingBurst;
    LAddress::L3Type destination;
    LAddress::L3Type source;
    double rssi;
    long nbPacketDropped;
    ArpInterface* arp;
    LAddress::L3Type myNetwAddr;
   // long recvSeq[100];
    cMessage *delayTimer;
   unsigned long* seenSeq;
   int nbDuplicate =0;
    BaseWorldUtility* world;

public:
    SaneehaNode()
        : Flood()
        , packetLength(0)
        , packetTime()
        , pppt(0)
        , burstSize(0)
        , remainingBurst(0)
        , destination()
        , nbPacketDropped(0)
        , arp(NULL)
        , myNetwAddr()
        , delayTimer(NULL)
        , world(NULL)
    {}

    virtual ~SaneehaNode();

    virtual void initialize(int stage);

    virtual void finish();

protected:
    /** @brief Handle self messages such as timer... */
    virtual void handleSelfMsg(cMessage *msg);

    /** @brief Handle messages from lower layer */
    virtual void handleLowerMsg(cMessage *msg);

    /** @brief Handle control messages from lower layer */
    virtual void handleLowerControl(cMessage *msg);

    /** @brief Handle messages from upper layer */
    virtual void handleUpperMsg(cMessage *msg)
    {
        Flood::handleUpperMsg(msg);
        //opp_error("SaneehaNode has no upper layers!");
        //delete msg;
    }

    /** @brief Handle control messages from upper layer */
    virtual void handleUpperControl(cMessage *msg)
    //virtual Flood::netwpkt_ptr_t handleUpperControl(cMessage *msg)
    {
        //netwpkt_ptr_t pkt=Flood::encapsMsg(msg);
        opp_error("SaneehaNode has no upper layers!");
        delete msg;
     //   return pkt;
    }

    /** @brief Send a broadcast message to lower layer. */
    virtual void sendBroadcast();
};

#endif



#endif /* SANEEHANODE_H_ */
