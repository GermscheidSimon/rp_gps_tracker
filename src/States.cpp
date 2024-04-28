#include <string>
#include "iostream"
#include <map>
#include "string"
#include "States.h"

using namespace std;

int ConcreteState::run() const{
    return 1;
}

Initial::Initial() {
    
}
int Initial::run() const{
    std::cout << "initializing: " << name;
    return 0;
}

ReadingGPS::ReadingGPS() {

}
int ReadingGPS::run() const{
    std::cout << "ReadingGPS: " << name;
    return 0;
}

EvaluateCoord::EvaluateCoord() {

}
int EvaluateCoord::run() const {
    std::cout << "EvaluateCoord: " << name;
    return 0;
}

Connect::Connect() {

}
int Connect::run() const {
    std::cout << "Connect: " << name;
    return 0;
}

void ConcreteStateService::Initialize() {
    std::cout << "Connect: ";
    while(true) {
        const ConcreteState& nextState = getNextState(currentState, currentStateErrorCode);
        std::cout << "nextState name:" + nextState.name + "\n";
        int currentState = nextState.stateId;
        int currentStateErrorCode = nextState.run();
    }
};

const ConcreteState& ConcreteStateService::getNextState(StatesEnum currentState, int currentStateErrorCode) {
    std::cout << "Error:" + std::to_string(currentStateErrorCode);
    const ConcreteState& nextState = ConcreteStateService::stateTransitionTable[currentState][currentStateErrorCode];
    return nextState;
};