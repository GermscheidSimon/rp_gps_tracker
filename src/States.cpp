#include "States.h"
#include <string>
#include "iostream"
#include <map>
#include "string"
#include "States.h"

using namespace std;


int ConcreteState::run() {
    return 1;
}

// Initial Is just really an entry point to start the state machine in motion just so I dont need a messy state table. should only be called one time and never looped back to
Initial::Initial() : ConcreteState("Initial", INIT) {}
int Initial::run() {
    std::cout << "initializing: " << name;
    return 0;
}

// reading gps state will utilize the NEO-6m gps breakout board to retrieve NMEA sentences, serialize them.
ReadingGPS::ReadingGPS() : ConcreteState("ReadingGPS", READGPS) {}
int ReadingGPS::run() {
    std::cout << "ReadingGPS: " << name;
    return 0;
}

// evaluate coordinates state will utilize the coordinate plus the previous coordinate (by default 0Lat 0Long) to see if the movement was detected
EvaluateCoord::EvaluateCoord() : ConcreteState("EvaluateCoord", EVALCOORD) {}
int EvaluateCoord::run() {
    std::cout << "EvaluateCoord: " << name;
    return 0;
}

// used to control uplink to RP Zero. will first need to power on the PI Zero and then transmit data over Serial connection
Connect::Connect() : ConcreteState("Connect", CONNECT) {}
int Connect::run() {
    std::cout << "Connect: " << name;
    return 0;
}


ConcreteStateService::ConcreteStateService(map<StatesEnum, map<int, ConcreteState*>> stateTransitionTable) {
    _stateTransitionTable = stateTransitionTable;
}


void ConcreteStateService::Initialize() {
    std::cout << "Starting State Machine: ";
    while (true) {
        ConcreteState* nextState = getNextState(currentState, currentStateErrorCode);
        std::cout << "Next State: " << nextState->name << "\n";
        currentState = nextState->id;
        currentStateErrorCode = nextState->run();
    }
}

ConcreteState* ConcreteStateService::getNextState(StatesEnum currentState, int currentStateErrorCode) {
    std::cout << "Error:" + std::to_string(currentStateErrorCode);
    ConcreteState *nextState = _stateTransitionTable[currentState][currentStateErrorCode];
    return nextState;
};