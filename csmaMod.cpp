/*
 * SaneehaNode.cc
 *
 *  Created on: Sep 12, 2013
 *      Author: Saneeha
 */

#include <SaneehaNode.h>
#include <CSMASaneeha.h>
#include <omnetpp.h>


#include <cassert>

#include "Packet.h"
#include "BaseMacLayer.h"
#include "FindModule.h"
#include "NetwToMacControlInfo.h"
//#include "NetwToMacControlInfo.h"
#include "AddressingInterface.h"
#include "Flood.h"

#define MIXIM_INET

Define_Module(SaneehaNode);

void SaneehaNode::initialize(int stage)
{
    EV<<" Saneeha Node init called";
   // BaseLayer::initialize(stage);
    Flood::initialize(stage);
       if(stage == 0) {
        world = FindModule<BaseWorldUtility*>::findGlobalModule();
        delayTimer   = new cMessage("delay-timer", TRAFFIC_TIMER);

        arp          = FindModule<ArpInterface*>::findSubModule(findHost());
         packetLength = par("packetLength");

        packetTime = par("packetTime");
          pppt = par("packetsPerPacketTime");
        burstSize = par("burstSize");
        //rssi=par("rssi");
         destination = LAddress::L3Type(par("destination").longValue());
         source = LAddress::L3Type(par("source").longValue());
       // destination =4;
         nbPacketDropped = 0;
         seenSeq=NULL;
                nbDuplicate =0;
        BaseMacLayer::catPacketSignal.initialize();
    }
    else if (stage == 1) {
        AddressingInterface* addrScheme = FindModule<AddressingInterface*>
                                                    ::findSubModule(findHost());
        if(addrScheme) {
            myNetwAddr = addrScheme->myNetwAddr(this);
        } else {
            myNetwAddr = LAddress::L3Type( getId() );
        }
        if(burstSize > 0) {
            remainingBurst = burstSize;
            scheduleAt(dblrand() * packetTime * burstSize / pppt, delayTimer);
        }
    } else {

    }
}

SaneehaNode::~SaneehaNode() {
    EV<<" Saneeha Node constructor";

    cancelAndDelete(delayTimer);
}


void SaneehaNode::finish()
{
    //recordScalar("rssi",rssi);
    //recordScalar("nbHops",nbHops);
    recordScalar("nbDataPacketsRecvd",nbDataPacketsReceived);
    recordScalar("nbDataPacketsSent",nbDataPacketsSent);
    recordScalar("nbDataPacketsForwarded",nbDataPacketsForwarded);
    recordScalar("dropped", nbPacketDropped);
    recordScalar("nbDuplicate",nbDuplicate);
    //if (nbDataPacketsReceived > 0) {
      //    recordScalar("meanNbHops", (double) nbHops / (double) nbDataPacketsReceived);
        //} else {
          //  recordScalar("meanNbHops", 0);
        //}
    //Flood::finish();
}

void SaneehaNode::handleSelfMsg(cMessage *msg)
{
    EV<<" Saneeha Node handle self msg called";
    switch( msg->getKind() )
    {
    case TRAFFIC_TIMER:
        assert(msg == delayTimer);

    if(destination !=myNetwAddr )
    {
        sendBroadcast();


        remainingBurst--;
    }
    else {
        if (msg->hasPar("seqNum")) {
            bool seen =0;
            for (unsigned int i=0;i<sizeof(seenSeq)/sizeof(long) ; i++)
            {
                if (msg->par("seqNum").longValue()==seenSeq[i]){
                    seen=true; nbDuplicate++;
                }
            }
            if (seen==0) {
                *(seenSeq++)=msg->par("seqNum").longValue();
                Flood::nbDataPacketsReceived ++;
            }
        }
        else ;


    }
        if(remainingBurst == 0) {
            remainingBurst = burstSize;
            scheduleAt(simTime() + (dblrand()*1.4+0.3)*packetTime * burstSize / pppt, msg);
        } else {
            scheduleAt(simTime() + packetTime * 2, msg);
        }

        break;
    default:
        EV << "Unknown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
        delete msg;
        break;
    }
}


void SaneehaNode::handleLowerMsg(cMessage *msg)
{
    /* if(msg->arrivedOn(lowerLayerIn)){
       if(msg->hasPar("SrcAddr") && msg->par("SrcAddr").longValue() != long(this->getId())){
                    EV<<"\n"<< msg->par("SrcAddr").longValue() << "Is in my Range";
          }}*/
    Packet p(packetLength, 1, 0);
    p.setHost( destination ); // I added-- SaneehaS
    emit(BaseMacLayer::catPacketSignal, &p);
      // EV<<" Saneeha Node handle lower msg called between"<<this->getId() <<" and "<<msg->getSenderModuleId();
         delete msg;
    msg = NULL;
}


void SaneehaNode::handleLowerControl(cMessage *msg)
{
    EV<<" Saneeha Node handle lower ctrl called";
    if(msg->getKind() == BaseMacLayer::PACKET_DROPPED) {
        nbPacketDropped++;
    }
    else if (msg->hasPar("seqNum")) {
        ev<< "seqNum seen "<<msg->par("seqNum").longValue();
    }
    else {}


    delete msg;
    msg = NULL;
}

void SaneehaNode::sendBroadcast()
{
    LAddress::L2Type macAddr;
    LAddress::L3Type netwAddr = destination;
    //Changed by Saneeha
   // long SenderNodeID=long(this->getID());

    netwpkt_ptr_t pkt = new netwpkt_t(LAddress::isL3Broadcast( netwAddr ) ? "TRAFFIC->ALL" : "TRAFFIC->TO", LAddress::isL3Broadcast( netwAddr ) ? BROADCAST_MESSAGE : TARGET_MESSAGE);
    //pkt->addPar("SenderNodeID").longValue(SenderNodeI.D);
    pkt->addPar("seqNum").setLongValue(Flood::seqNum);
    pkt->setBitLength(packetLength);
    //EV<<"Node ID appended by Saneeha";
    //changed by Saneeha


    Packet appPkt(packetLength, 0, 1);
    emit(BaseMacLayer::catPacketSignal, &appPkt);
    if (source != myNetwAddr){
        Flood::nbDataPacketsForwarded ++;
          }

    else {
    Flood::nbDataPacketsSent ++;
    Flood::seqNum ++; }

   // pkt->setSrcAddr(myNetwAddr);
    pkt->setSrcAddr(source);
    pkt->setDestAddr(netwAddr);

    if(LAddress::isL3Broadcast( netwAddr )) {
        EV<<"Broadcast sent by"<<pkt->getSrcAddr() <<"through "<<myNetwAddr;
        pkt->addPar("SrcAddr").setLongValue(pkt->getSrcAddr());
        macAddr = LAddress::L2BROADCAST;
    }
    else{
        EV<<" Traffic sent to "<<pkt->getDestAddr() <<"by "<<pkt->getSrcAddr() <<"through "<<myNetwAddr<<" seq no"<<pkt->par("seqNum").longValue();
        macAddr = arp->getMacAddr(netwAddr);
    }
  //  pkt->addPar("seqNum").setLongValue(Flood::seqNum);
     NetwToMacControlInfo::setControlInfo(pkt, macAddr);

    sendDown(pkt);
}


//#endif
