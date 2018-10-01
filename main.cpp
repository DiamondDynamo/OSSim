#include <iostream>
#include <fstream>
#include <list>
#include <ctime>
#include <chrono>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>

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
    explicit ProcNode(int runs);

    int getReqRuns();

    int getProcId();

    bool equals(int);

    string getStartTime();

    string getEndTime();


    int fromId;
    double readyTime;
    double runningTime;
    double totTime;
    time_t startTime;
    time_t endTime;
    time_t tempTime;
    int doneRuns;

private:
    int procId;
    int reqRuns;
};

ProcNode::ProcNode(int runs) {
    procId = NEXTID;
    NEXTID++;
    fromId = START;
    readyTime = 0;
    runningTime = 0;
    totTime = 0;
    startTime = 0;
    endTime = 0;
    tempTime = 0;
    doneRuns = 0;
    reqRuns = runs;

}

int ProcNode::getReqRuns() { return reqRuns; }

int ProcNode::getProcId() { return procId; }

bool ProcNode::equals(int checkId) { return procId == checkId; }

string ProcNode::getStartTime() { return ctime(&startTime); }

string ProcNode::getEndTime() { return ctime(&endTime); }

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

    int numProcs = 5;
    int runs[numProcs];
    int count = 0;

//    for(int i = 0; i < numProcs; i++)
//    {
//        ProcNode node;
//        start.push_back(node);
//    }

    ifstream inputFile;
    inputFile.open("input.txt");

    while (inputFile >> runs[count]) {
        ProcNode *node = new ProcNode(runs[count]);
        start.push_back(*node);
    }

    while (term.size() != numProcs) {
        if (!wait.empty()) {
            ProcNode front = wait.front();
            moveNode(front, wait, ready, READY);
        }
        if (!running.empty()) {
            ProcNode front = running.front();
            front.doneRuns++;
            if (front.doneRuns == front.getReqRuns()) {
                moveNode(front, running, term, TERM);
            } else {
//                if(!ready.empty())
//                {
                moveNode(front, running, wait, WAIT);
//                }
            }
        }
        if (!ready.empty()) {
            ProcNode front = ready.front();
            if (running.empty()) {
                moveNode(front, ready, running, RUN);
            }
        }
        if (!start.empty()) {
            ProcNode front = start.front();
            moveNode(front, start, ready, READY);
        }

        sleep(1);
    }

    for (auto it = term.begin(); it != term.end(); ++it) {
        ProcNode active = *it;
        cout << "Process " << active.getProcId() << ":\t\tStarted " << active.getStartTime() << "\t\t\t\tEnded "
             << active.getEndTime() << endl;
    }

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