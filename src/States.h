#pragma once

#include "map"
#include "string"

using namespace std;

enum StatesEnum {
    INIT,
    READGPS,
    EVALCOORD,
    CONNECT
};

class ConcreteState {
public:
    string name;
    StatesEnum id;
    ConcreteState(string stateName, StatesEnum stateID) : name(stateName), id(stateID) {};
    virtual int run();
};

class Initial : public ConcreteState {
public:
    Initial();
    int run();
};

class ReadingGPS : public ConcreteState {
public:
    ReadingGPS();
    int run();
};

class EvaluateCoord : public ConcreteState {
public:
    EvaluateCoord();
    int run();
};

class Connect : public ConcreteState {
public:
    Connect();
    int run();
};

class ConcreteStateService {


public:
    map<StatesEnum, map<int, ConcreteState*>> _stateTransitionTable;
    int currentStateErrorCode = 0;
    StatesEnum currentState = INIT;
    ConcreteStateService(map<StatesEnum, map<int, ConcreteState*>> stateTransitionTable);
    const ConcreteState getNextState(StatesEnum currentState, int currentStateErrorCode);
    void Initialize();
};