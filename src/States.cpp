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

    class ConcreteSateService {

        public: map<int, ConcreteState> States = {
            {0, Initial("INIT")},
            {1, ReadingGPS("READGPS")}
        };

        int initialSate = 0;

        public: int Initialize() {
                
        }

    };

    class ReadingGPS: public ConcreteState {

        public:
            ReadingGPS(string state_name) {
                name = state_name;
            }
            int run() {
                cout << "initializing: " << name;

                return 0;

            };
    };

    class Initial: public ConcreteState {

        public: 
            Initial(string state_name) {
                name = state_name;
            };
            
            int run() {
                std::cout << "initializing: " << name;

                return 0;

            };
    };

}

