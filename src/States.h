#pragma once

#include "map"
#include "string"
#include "pico/stdlib.h"
#include <vector>

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
private:
    void setup(
        int buad_rate,
        int data_bits,
        int stop_bits,
        int uart_rx,
        int uart_tx
    );
    //static void on_uart_rx();
    string nextSentence();
    vector<std::string> splitNmeaSentence(string nmeaSent);
    bool isValidNmea(vector<std::string> nmeaSent);
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
    ConcreteState* getNextState(StatesEnum currentState, int currentStateErrorCode);
    void Initialize();
};