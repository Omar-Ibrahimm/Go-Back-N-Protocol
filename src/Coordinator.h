#ifndef __PROJ_COORDINATOR_H_
#define __PROJ_COORDINATOR_H_

#include <omnetpp.h>

#include <string>
#include <fstream>

using namespace omnetpp;

class Coordinator : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
