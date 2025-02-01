#include "Coordinator.h"

using namespace std;

Define_Module(Coordinator);

void Coordinator::initialize()
{
    // Read "coordinator.txt"
    ifstream file("C:/Users/m3ngmn/Desktop/Uni/networks/omnetpp-6.1/samples/proj/src/coordinator.txt");
    string node; file >> node;
    string time; file >> time;
    file.close();
    // Use read info to configure nodes
    string mS = time + "1";
    string mR = "2";
    cMessage* msgSender = new cMessage(mS.c_str());
    cMessage* msgReceiver = new cMessage(mR.c_str());
    send(msgSender, "out", stoi(node));
    send(msgReceiver, "out", 1 - stoi(node));
}

void Coordinator::handleMessage(cMessage *msg)
{
    // Nothing
}
