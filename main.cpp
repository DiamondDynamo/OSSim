#include <iostream>
#include <fstream>
#include <list>
#include <ctime>
#include <chrono>
#include <unistd.h>
#include <cstdlib>

using namespace std;
using namespace std::chrono;

//Gloabl variable to auto-assign IDs dynamically
int NEXTID = 0;

//Enumerator to assign the process's next state
enum FromList {
    START = 1,
    READY = 2,
    WAIT = 3,
    RUN = 4,
    TERM = 5
};

//Custom class to hold info and timing for each process
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

    ifstream inputFile;
    inputFile.open("input.txt");

    //Read in the required number of runs from the input file, and push the node onto the Start state
    while (inputFile >> runs[count]) {
        auto *node = new ProcNode(runs[count]);
        start.push_back(*node);
    }
    inputFile.close();

    //Run clock cycles until all processes are terminated
    while (term.size() != numProcs) {
        //If the wait state is not empty, move the contents (should be a single process) to ready
        if (!wait.empty()) {
            ProcNode front = wait.front();
            moveNode(front, wait, ready, READY);
        }
        //If the running state is not empty, check if its contents (should be a single process) is ready to terminate
        if (!running.empty()) {
            ProcNode front = running.front();
            front.doneRuns++;
            //If the process has run enough time, move it to terminated
            if (front.doneRuns == front.getReqRuns()) {
                moveNode(front, running, term, TERM);
                //If the process still needs to run, move it to wait
            } else {
                moveNode(front, running, wait, WAIT);
            }
        }
        //If the ready state is not empty, and the running state is, move the front process in ready to running
        if (!ready.empty()) {
            ProcNode front = ready.front();
            if (running.empty()) {
                moveNode(front, ready, running, RUN);
            }
        }
        //If the start state is not empty, move the front process to the back of ready
        if (!start.empty()) {
            ProcNode front = start.front();
            moveNode(front, start, ready, READY);
        }

        //Sleep for 1 second, to simulate process time
        sleep(1);
    }

    //Iterate through the terminated list, to get timing info for output
    for (auto active : term) {
        cout << "Process " << active.getProcId() << ":\t\tStarted " << active.getStartTime() << "\t\t\t\tEnded "
             << active.getEndTime() << endl;
    }

    return 0;
}

//moveNode handles moving a process from one state to another, as well as updating timing
void moveNode(ProcNode &active, list<ProcNode> &from, list<ProcNode> &to, int toId) {
    //Depending on the state that the process is coming from, perform different actions with timing
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
    //If the process is being moved to terminated, log the end time, and calculate the total computing time of the process
    if (toId == TERM) {
        time_t ttNow = system_clock::to_time_t(system_clock::now());
        active.endTime = ttNow;
        active.totTime = difftime(active.endTime, active.startTime);
    }
    //Push the process onto the destination state, and remove it from the origin
    active.fromId = toId;
    to.push_back(active);
    from.erase(from.begin());
}

//When moving from start, log the start time and begin logging the ready time
void fromStart(ProcNode &active) {
    time_t ttNow = system_clock::to_time_t(system_clock::now());
    active.startTime = ttNow;
    active.tempTime = ttNow;
}

//When moving from ready, stop logging the ready time and add it to the total, then begin logging the running time
void fromReady(ProcNode &active) {
    time_t ttNow = system_clock::to_time_t(system_clock::now());
    double dReady = difftime(ttNow, active.tempTime);
    active.readyTime += dReady;
    active.tempTime = ttNow;
}

//When moving from running, stop logging the running time and add it to the total
void fromRunning(ProcNode &active) {
    time_t ttNow = system_clock::to_time_t(system_clock::now());
    double dRun = difftime(ttNow, active.tempTime);
    active.runningTime += dRun;
}

//When moving from wait, start logging the ready time
void fromWait(ProcNode &active) {
    time_t ttNow = system_clock::to_time_t(system_clock::now());
    active.tempTime = ttNow;
}