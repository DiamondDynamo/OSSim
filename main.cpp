#include <iostream>
#include <list>
#include <ctime>
#include <chrono>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

int NEXTID = 0;

enum FromList {
    START = 1,
    READY = 2,
    WAIT = 3,
    RUN = 4,
    TERM = 5
};

class ProcNode {
public:
    ProcNode();

    int procId;
    int fromId;
    double readyTime;
    double runningTime;
    double totTime;
    time_t startTime;
    time_t endTime;
    time_t tempTime;
};

ProcNode::ProcNode() {
    procId = NEXTID;
    NEXTID++;
    fromId = START;
    readyTime = 0;
    runningTime = 0;
    totTime = 0;
    startTime = 0;
    endTime = 0;
    tempTime = 0;
}

void moveNode(ProcNode &active, list<ProcNode> &from, list<ProcNode> &to, int toId);

void fromStart(ProcNode &active);

void fromReady(ProcNode &active);

void fromRunning(ProcNode &active);

void fromWait(ProcNode &active);

int main() {


    list<ProcNode> start;
    list<ProcNode> ready;
    list<ProcNode> wait;
    list<ProcNode> running;
    list<ProcNode> term;

    //TODO: Replace test code with clock cycle sim (while loop)

    ProcNode node;
    start.push_back(node);

    sleep(1);

    moveNode(start.front(), start, ready, READY);

    sleep(3);

    moveNode(ready.front(), ready, running, RUN);

    sleep(2);

    moveNode(running.front(), running, wait, WAIT);

    sleep(1);

    moveNode(wait.front(), wait, ready, READY);

    sleep(1);

    moveNode(ready.front(), ready, running, RUN);

    sleep(1);

    moveNode(running.front(), running, term, TERM);

    ProcNode test = term.front();

    cout << "Start Time: " << ctime(&test.startTime);
    cout << "End Time: " << ctime(&test.endTime);
    cout << "Total Time: " << test.totTime << endl;
    cout << "Ready Time: " << test.readyTime << endl;
    cout << "Running Time: " << test.runningTime << endl;

    return 0;
}

void moveNode(ProcNode &active, list<ProcNode> &from, list<ProcNode> &to, int toId) {
    switch (active.fromId) {
        case START:
            fromStart(active);
            break;
        case READY:
            fromReady(active);
            break;
        case RUN:
            fromRunning(active);
            break;
        case WAIT:
            fromWait(active);
            break;
        default:
            cout << "Something went horribly, horribly wrong in the fromId case-switch";
    }
    if (toId == TERM) {
        time_t ttNow = system_clock::to_time_t(system_clock::now());
        active.endTime = ttNow;
        active.totTime = difftime(active.endTime, active.startTime);
    }
    active.fromId = toId;
    to.push_back(active);
    from.erase(from.begin());
}

void fromStart(ProcNode &active) {
    time_t ttNow = system_clock::to_time_t(system_clock::now());
    active.startTime = ttNow;
    active.tempTime = ttNow;
}

void fromReady(ProcNode &active) {
    time_t ttNow = system_clock::to_time_t(system_clock::now());
    double dReady = difftime(ttNow, active.tempTime);
    active.readyTime += dReady;
    active.tempTime = ttNow;
}

void fromRunning(ProcNode &active) {
    time_t ttNow = system_clock::to_time_t(system_clock::now());
    double dRun = difftime(ttNow, active.tempTime);
    active.runningTime += dRun;
}

void fromWait(ProcNode &active) {
    time_t ttNow = system_clock::to_time_t(system_clock::now());
    active.tempTime = ttNow;
}