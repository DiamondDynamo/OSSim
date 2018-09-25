#include <iostream>
#include <list>
using namespace std;

void moveNode(int, list<int>&, list<int>&);

int main() {

    list<int> start;
    list<int> ready;
    list<int> wait;
    list<int> running;
    list<int> term;

    start.push_back(1);


    moveNode(start.front(), start, ready);

    moveNode(ready.front(), ready, running);

    moveNode(running.front(), running, term);


    return 0;
}

void moveNode(int id, list<int>& from, list<int>& to) {
    to.push_back(id);
    from.remove(id);
}