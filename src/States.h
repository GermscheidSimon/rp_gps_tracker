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

class ConcreteState 
{
    public:
        StatesEnum stateId;
        string name;
        int errorCode;
        virtual int run() const;
};

class Initial: public ConcreteState {

    public: 
        Initial();
        StatesEnum stateId = INIT;
        string name = "Initial";
        int run() const;
};

class ReadingGPS: public ConcreteState {

    public:
        ReadingGPS();
        StatesEnum stateId = StatesEnum::READGPS;
        string name = "ReadingGPS";
        int run() const;
};

class EvaluateCoord: public ConcreteState {

    public: 
        StatesEnum stateId = StatesEnum::EVALCOORD;
        string name = "EvaluateCoord";
        EvaluateCoord();
        int run() const;
};

class Connect: public ConcreteState {

    public: 
        StatesEnum stateId = StatesEnum::EVALCOORD;
        string name = "Connect";
        Connect();
        int run() const;
};

class ConcreteStateService {
    private:
        StatesEnum initialSate;
        int currentStateErrorCode;
        StatesEnum currentState;
    public:
        ReadingGPS _readingGPS;
        EvaluateCoord _evaluateCoord;
        Connect _connect;

        map<StatesEnum, map<int, ConcreteState>> stateTransitionTable = {
            {
                StatesEnum::INIT, {
                    {0, _readingGPS},
                }
            },
            {
                StatesEnum::READGPS, {
                    {0, _readingGPS},
                    {1, _evaluateCoord}
                }
            },
            {
                StatesEnum::EVALCOORD, {
                    {0, _readingGPS},
                    {1, _connect}
                }
            },
            {
                StatesEnum::CONNECT, {
                    {0, _readingGPS}
                }
            }
        };

        ConcreteStateService() {
            currentState = initialSate;
            currentStateErrorCode = 0;
            _readingGPS = ReadingGPS();
            _evaluateCoord = EvaluateCoord();
            _connect = Connect();
        };

        const ConcreteState& getNextState(StatesEnum currentState, int currentStateErrorCode);
        void Initialize();
};