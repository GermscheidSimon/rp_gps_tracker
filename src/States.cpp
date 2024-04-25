#include <string>
#include "States.h"
#include "iostream"
#include <map>
#include "string"

using namespace std;

namespace state {

    class ConcreteState {
        public:
            string name;
            int errorCode;
    };

    enum States {
        INIT,
        READGPS,
        EVALCOORD,
        CONNECT
    };


    map<States, map<int, ConcreteState*>> stateTransitionTable = {
        {
            States::INIT, {
                {0, new ReadingGPS()},
            }
        },
        {
            States::READGPS, {
                {0, new ReadingGPS()},
                {1, new EvaluateCoord()}
            }
        },
        {
            States::EVALCOORD, {
                {0, new ReadingGPS()},
                {1, new Connect()}
            }
        },
        {
            States::CONNECT, {
                {0, new ReadingGPS()}
            }
        }
    };

    class ConcreteSateService {
    
        int initialSate = 0;

        public: 
            void Initialize() {
                
        }
    };

    class Initial: public ConcreteState {

        public: 
            int stateId = state::INIT;
            string name = "Initial";

            int run() {
                std::cout << "initializing: " << name;

                return 0;

            };
    };

    class ReadingGPS: public ConcreteState {

        public:
            int stateId = state::READGPS;
            string name = "ReadingGPS";

            int run() {
                cout << "initializing: " << name;

                return 0;

            };
    };


     class EvaluateCoord: public ConcreteState {

        public: 
            int stateId = state::EVALCOORD;
            string name = "EvaluateCoord";

            int run() {
                std::cout << "EvaluateCoord: " << name;

                return 0;

            };
    };

    class Connect: public ConcreteState {

        public: 
            int stateId = state::EVALCOORD;
            string name = "Connect";

            int run() {
                std::cout << "Connect: " << name;

                return 0;

            };
    };
};