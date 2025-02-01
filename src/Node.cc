#include "Node.h"

using namespace std;

Define_Module(Node);

void Node::initialize()
{
    // Common members
    role = 0; // all nodes are initialized with undefined roles
    WS = getParentModule()->par("WS").intValue();
    PT = getParentModule()->par("PT").doubleValue();
    TD = getParentModule()->par("TD").doubleValue();
    OPFILE = "C:/Users/m3ngmn/Desktop/Uni/networks/omnetpp-6.1/samples/proj/src/output.txt";
    // Receiver members
    RSEQN = 0;
    LP = getParentModule()->par("LP").intValue();
    // Sender members
    SSEQN = 0;
    SEXPN = 0;
    BUFFERED = 0;
    PBACK = 0;
    messagesIndex = 0;
    windowIndex = 0;
    ST = 0;
    TO = getParentModule()->par("TO").doubleValue();
    ED = getParentModule()->par("ED").doubleValue();
    DD = getParentModule()->par("DD").doubleValue();
    for (int i = 0; i < WS; i++) { window.push_back(nullptr); timeouts.push_back(nullptr); codes.push_back(0); }
}

void Node::handleMessage(cMessage *msg)
{
    // Node is Sender
    if (dynamic_cast<MyMessage_Base*>(msg) && role == 1)
    { actAsSender(check_and_cast<MyMessage_Base*>(msg)); }
    // Node is Receiver
    else if (dynamic_cast<MyMessage_Base*>(msg) && role == 2)
    { actAsReceiver(check_and_cast<MyMessage_Base*>(msg)); }
    // Start Sender
    else if (strcmp(msg->getName(), "START") == 0)
    { cancelAndDelete(msg); startSender(); }
    // Batch
    else if (strcmp(msg->getName(), "BATCH") == 0)
    { cancelAndDelete(msg); sendFrames(); }
    // Clean Batch
    else if (strcmp(msg->getName(), "CLEAN") == 0)
    { cancelAndDelete(msg); if (windowIndex > 0) { sendCleanFrames(); } }
    // Timeout
    else if (strcmp(msg->getName(), "TIMEOUT") == 0)
    { print3(); stopTimers(); windowIndex = 0; sendCleanFrames(); }
    // Next
    else if (strcmp(msg->getName(), "NEXT") == 0)
    { cancelAndDelete(msg); nextFrames(); }
    // Initialize Node's role
    else { setRole(msg); }
}

string Node::byteStuffing(string s)
{
    string frame("$");
    for (auto& c : s)
    {
        if (c == '$' || c == '/')
        { frame.push_back('/'); }
        frame.push_back(c);
    }
    frame.push_back('$');
    return frame;
}

string Node::deFrame(string s)
{
    string deframed;
    bool escaping = false;
    for (int i = 1; i < s.size() - 1; i++)
    {
        char current = s[i];
        if (escaping)
        { deframed.push_back(current); escaping = false; }
        else if (current == '/') { escaping = true; }
        else { deframed.push_back(current); }
    }
    return deframed;
}

char Node::generateParity(string s)
{
    bitset<8> parity = s[0];
    for (int i = 1; i < s.size(); i++)
    {
        bitset<8> temp = s[i];
        parity ^= temp;
    }
    return (char)parity.to_ulong();
}

bool Node::isError(string s, char parity)
{
    bitset<8> error = parity;
    for (auto& c : s)
    { bitset<8> temp(c); error ^= temp; }
    return error.to_ulong();
}

void Node::readMessages()
{
    string filename;
    if (getIndex() == 0) { filename = "C:/Users/m3ngmn/Desktop/Uni/networks/omnetpp-6.1/samples/proj/src/input0.txt"; }
    else { filename = "C:/Users/m3ngmn/Desktop/Uni/networks/omnetpp-6.1/samples/proj/src/input1.txt"; }
    ifstream file(filename);
    string line;
    while (getline(file, line))
    { messages.push_back(line); }
    file.close();
}

MyMessage_Base* Node::generateMessage(std::string s)
{
    MyMessage_Base *msg = new MyMessage_Base;
    msg->setHeader(SSEQN);
    s.erase(0, 5);
    string frame = byteStuffing(s);
    msg->setPayload(frame.c_str());
    msg->setParity(generateParity(frame));
    msg->setType(2);
    return msg;
}

void Node::sendFrame(MyMessage_Base *msg, int code)
{
    print1(code);
    MyMessage_Base *msg1 = msg->dup();
    MyMessage_Base *msg2;
    string s(msg->getPayload());
    switch (code)
    {
    case 0: // Nothing
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 0, 0, 0);
        sendDelayed(msg1,PT + TD, "out");
        break;
    case 1: // Delay
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 0, 0, ED);
        sendDelayed(msg1, PT + TD + ED, "out");
        break;
    case 2: // Duplication
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 0, 1, 0);
        sendDelayed(msg1, PT + TD, "out");
        msg2 = msg1->dup();
        print2(simTime() + PT + DD, "sent", msg->getHeader(), s, msg->getParity(), -1, 0, 2, 0);
        sendDelayed(msg2, PT + TD + DD, "out");
        break;
    case 3: // Duplication + Delay
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 0, 1, ED);
        sendDelayed(msg1, PT + TD + ED, "out");
        msg2 = msg1->dup();
        print2(simTime() + PT + DD, "sent", msg->getHeader(), s, msg->getParity(), -1, 0, 2, ED);
        sendDelayed(msg2, PT + TD + ED + DD, "out");
        break;
    case 4: // Loss
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 1, 0, 0);
        cancelAndDelete(msg1);
        break;
    case 5: // Loss
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 1, 0, 0);
        cancelAndDelete(msg1);
        break;
    case 6: // Loss
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 1, 0, 0);
        cancelAndDelete(msg1);
        break;
    case 7: // Loss
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 1, 0, 0);
        cancelAndDelete(msg1);
        break;
    case 8: // Modification
        s[0] += 1;
        msg1->setPayload(s.c_str());
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), 0, 0, 0, 0);
        sendDelayed(msg1, PT + TD, "out");
        break;
    case 9: // Modification + Delay
        s[0] += 1;
        msg1->setPayload(s.c_str());
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), 0, 0, 0, ED);
        sendDelayed(msg1, PT + TD + ED, "out");
        break;
    case 10: // Modification + Duplication
        s[0] += 1;
        msg1->setPayload(s.c_str());
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), 0, 0, 1, 0);
        sendDelayed(msg1, PT + TD, "out");
        msg2 = msg1->dup();
        print2(simTime() + PT + DD, "sent", msg->getHeader(), s, msg->getParity(), 0, 0, 2, 0);
        sendDelayed(msg2, PT + TD + DD, "out");
        break;
    case 11: // Modification + Duplication + Delay
        s[0] += 1;
        msg1->setPayload(s.c_str());
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), 0, 0, 2, ED);
        sendDelayed(msg1, PT + TD + ED, "out");
        msg2 = msg1->dup();
        print2(simTime() + PT + DD, "sent", msg->getHeader(), s, msg->getParity(), 0, 0, 2, ED);
        sendDelayed(msg2, PT + TD + ED + DD, "out");
        break;
    case 12: // Loss
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 1, 0, 0);
        cancelAndDelete(msg1);
        break;
    case 13: // Loss
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 1, 0, 0);
        cancelAndDelete(msg1);
        break;
    case 14: // Loss
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 1, 0, 0);
        cancelAndDelete(msg1);
        break;
    case 15: // Loss
        print2(simTime() + PT, "sent", msg->getHeader(), s, msg->getParity(), -1, 1, 0, 0);
        cancelAndDelete(msg1);
        break;
    }
}

void Node::sendFrames()
{
    if (BUFFERED == WS || messagesIndex == messages.size()) { return; }
    string message = messages[messagesIndex];
    MyMessage_Base *msg = generateMessage(message);
    SSEQN = (SSEQN + 1) % (WS + 1);
    window[windowIndex] = msg;
    cMessage *timeout = new cMessage("TIMEOUT");
    timeouts[windowIndex] = timeout;
    scheduleAt(simTime() + PT + TO, timeout);
    int code = stoi(message.substr(0, 4), nullptr, 2);
    codes[windowIndex] = code;
    sendFrame(msg, code);
    BUFFERED++;
    messagesIndex++;
    windowIndex = (windowIndex + 1) % WS;
    scheduleAt(simTime() + PT, new cMessage("BATCH"));
}

void Node::sendCleanFrames()
{
    if (!window[windowIndex]) { return; }
    cMessage *timeout = new cMessage("TIMEOUT");
    timeouts[windowIndex] = timeout;
    scheduleAt(simTime() + PT + TO, timeout);
    if (windowIndex == 0) { codes[windowIndex] = 0; }
    sendFrame(window[windowIndex], codes[windowIndex]);
    windowIndex = (windowIndex + 1) % WS;
    scheduleAt(simTime() + PT, new cMessage("CLEAN"));
}

void Node::nextFrame()
{
    for (int i = 0; i < BUFFERED - 1; i++)
    {
        if (i == 0)
        { cancelAndDelete(window[i]); cancelAndDelete(timeouts[i]); }
        window[i] = window[i + 1];
        timeouts[i] = timeouts[i + 1];
        codes[i] = codes[i + 1];
    }
    if (BUFFERED == 1)
    { cancelAndDelete(window[0]); cancelAndDelete(timeouts[0]); }
    window[BUFFERED - 1] = nullptr;
    timeouts[BUFFERED - 1] = nullptr;
    if (messagesIndex == messages.size())
    {
        BUFFERED--;
        return;
    }
    string message = messages[messagesIndex];
    MyMessage_Base *msg = generateMessage(message);
    SSEQN = (SSEQN + 1) % (WS + 1);
    window[BUFFERED - 1] = msg;
    cMessage *timeout = new cMessage("TIMEOUT");
    timeouts[BUFFERED - 1] = timeout;
    scheduleAt(simTime() + PT + TO, timeout);
    int code = stoi(message.substr(0, 4), nullptr, 2);
    codes[BUFFERED - 1] = code;
    sendFrame(msg, code);
    messagesIndex++;
}

void Node::nextFrames()
{
    if (inWindow(SEXPN, PBACK, SSEQN))
    {
        nextFrame();
        if (BUFFERED == 0) { simFinish(); return; }
        SEXPN = (SEXPN + 1) % (WS + 1);
        if (BUFFERED == WS) { scheduleAt(simTime() + PT, new cMessage("NEXT")); }
    }
}

bool Node::inWindow(int a, int b, int c)
{
    return (a <= b && b < c) || (c < a && a <= b) || (b < c && c < a);
}

void Node::stopTimers()
{
    for (auto& timeout : timeouts)
    { cancelAndDelete(timeout); timeout = nullptr; }
}

void Node::print1(int code)
{
    EV << "At time[" << simTime() << "], Node[" << getIndex()
       << "] introducing channel error with code[" << code << "]" << endl;
    ofstream file(OPFILE, ios::app);
    file << "At time[" << simTime() << "], Node[" << getIndex()
         << "] introducing channel error with code[" << code << "]" << endl;
    file.close();
}

void Node::print2(const omnetpp::SimTime time, string s, int seq, string pl, char p, int m, int l, int dup, int del)
{
    bitset<8> parity = p;
    EV << "At time[" << time << "], Node[" << getIndex() << "] " << s << " frame with seq_num["
       << seq << "], payload["<< pl << "], trailer[" << parity.to_string() << "], Modified["
       << m << "], Lost[" << l << "], Duplicated[" << dup << "], Delayed[" << del << "]" << endl;
    ofstream file(OPFILE, ios::app);
    file << "At time[" << time << "], Node[" << getIndex() << "] " << s << " frame with seq_num["
         << seq << "], payload["<< pl << "], trailer[" << parity.to_string() << "], Modified["
         << m << "], Lost[" << l << "], Duplicated[" << dup << "], Delayed[" << del << "]" << endl;
    file.close();
}

void Node::print3()
{
    EV << "Time out event at time[" << simTime() << "] at Node["
       << getIndex() << "] for frame with seq_num[" << SEXPN << "]" << endl;
    ofstream file(OPFILE, ios::app);
    file << "Time out event at time[" << simTime() << "] at Node["
         << getIndex() << "] for frame with seq_num[" << SEXPN << "]" << endl;
    file.close();
}

void Node::print4(string s, string l)
{
    EV << "At time[" <<simTime() + PT << "], Node[" << getIndex() << "] sending ["
       << s << "] with seq_num[" << RSEQN << "], Loss [" << l << "]" << endl;
    ofstream file(OPFILE, ios::app);
    file << "At time[" <<simTime() + PT << "], Node[" << getIndex() << "] sending ["
         << s << "] with seq_num[" << RSEQN << "], Loss [" << l << "]" << endl;
    file.close();
}

void Node::setRole(cMessage *msg)
{
    string s(msg->getName());
    cancelAndDelete(msg);
    role = (int)s.back() - 48; s.pop_back();
    if (s.size() != 0) { ST = stoi(s); scheduleAt(ST, new cMessage("START")); } // set ST for sender
}

void Node::startSender()
{
    ofstream file(OPFILE);
    file.close();
    readMessages();
    sendFrames();
}

void Node::actAsSender(MyMessage_Base *msg)
{
    if (msg->getType() == 0)
    {
        ofstream file(OPFILE, ios::app);
        file << "At time[" << simTime() << "], Node[" << getIndex()
             << "] received [NACK] with seq_num[" << msg->getAckNumber() << "]" << endl;
        file.close();
        cancelAndDelete(msg); stopTimers(); windowIndex = 0; sendCleanFrames();
    }
    else
    {
        ofstream file(OPFILE, ios::app);
            file << "At time[" << simTime() << "], Node[" << getIndex()
                 << "] received [ACK] with seq_num[" << msg->getAckNumber() << "]" << endl;
            file.close();
            PBACK = msg->getAckNumber(); cancelAndDelete(msg); nextFrames();
    }
}

void Node::actAsReceiver(MyMessage_Base *msg)
{
    string s(msg->getPayload());
    print2(simTime(), "received", msg->getHeader(), s, msg->getParity(), -1, 0, 0, 0);
    EV << "Node[" << getIndex() << "] received: " << deFrame(msg->getPayload()) << endl;
    ofstream file(OPFILE, ios::app);
    file << "Node[" << getIndex() << "] received: " << deFrame(msg->getPayload()) << endl;
    file.close();

    if (msg->getHeader() != RSEQN) // Unexpected frame
    {
        ofstream file(OPFILE, ios::app);
        file << "Out of Order" << endl;
        file.close();
        EV << "OOO" << endl; cancelAndDelete(msg); return;
    }

    int rand = uniform(0, 1) * 100;

    if (isError(msg->getPayload(), msg->getParity())) // Erroneous frame
    {
        msg->setType(0); msg->setAckNumber(RSEQN);
        if (rand > LP) // No loss
        {
            print4("NACK", "No");
            sendDelayed(msg, PT + TD, "out");
        }
        else
        {
            print4("NACK", "Yes");
            cancelAndDelete(msg);
        }
    }
    else // Correct frame
    {
        msg->setType(1); msg->setAckNumber(RSEQN);
        if (rand > LP) // No loss
        {
            print4("ACK", "No");
            sendDelayed(msg, PT + TD, "out");
        }
        else
        {
            print4("ACK", "Yes");
            cancelAndDelete(msg);
        }
        RSEQN = (RSEQN + 1) % (WS + 1);
    }
}

void Node::simFinish()
{
    messages.clear();
    window.clear();
    timeouts.clear();
    codes.clear();
    endSimulation();
}
