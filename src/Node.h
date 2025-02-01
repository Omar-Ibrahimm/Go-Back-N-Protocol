#ifndef __PROJ_NODE_H_
#define __PROJ_NODE_H_

#include <omnetpp.h>

#include "MyMessage_m.h"
#include <string>
#include <fstream>
#include <bitset>
#include <vector>
#include <algorithm>

using namespace omnetpp;

class Node : public cSimpleModule
{
  protected:
    // Common members
    int role; // 0 --> undefined, 1 --> sender, 2 --> receiver
    int WS;
    double PT;
    double TD;
    std::string OPFILE;
    // Receiver members
    int RSEQN;
    int LP;
    // Sender members
    int SSEQN;
    int SEXPN;
    int BUFFERED;
    int PBACK;
    std::vector<std::string> messages;
    int messagesIndex;
    std::vector<MyMessage_Base*> window;
    int windowIndex;
    std::vector<cMessage*> timeouts;
    std::vector<int> codes;
    double ST;
    double TO;
    double ED;
    double DD;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    // Framing + Error detection
    std::string byteStuffing(std::string s);
    std::string deFrame(std::string s);
    char generateParity(std::string s);
    bool isError(std::string s, char parity);

    // Simulation utility functions
    void readMessages();
    MyMessage_Base* generateMessage(std::string s);
    void sendFrame(MyMessage_Base *msg, int code);
    void sendFrames();
    void sendCleanFrames();
    void nextFrame();
    void nextFrames();

    bool inWindow(int a, int b, int c);
    void stopTimers();

    // Logging
    void print1(int code);
    void print2(const SimTime time, std::string s, int seq, std::string pl, char p, int m, int l, int dup, int del);
    void print3();
    void print4(std::string s, std::string l);

    // Simulation main functions
    void setRole(cMessage *msg);
    void startSender();
    void actAsSender(MyMessage_Base *msg);
    void actAsReceiver(MyMessage_Base *msg);

    void simFinish();
};

#endif
