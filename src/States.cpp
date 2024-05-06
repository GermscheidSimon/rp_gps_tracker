#include "States.h"
#include <stdlib.h>
#include <string>
#include "iostream"
#include <map>
#include "string"
#include "States.h"
#include "pico/stdlib.h"
#include "sstream"
#include "vector"

#define UART_ID uart1
#define PARITY  UART_PARITY_NONE

int ConcreteState::run() {
    return 1;
}

// Initial Is just really an entry point to start the state machine in motion just so I dont need a messy state table. should only be called one time and never looped back to
Initial::Initial() : ConcreteState("Initial", INIT) {}
int Initial::run() {
    std::cout << "initializing: " << name;
    return 0;
}





void ReadingGPS::setup(    
    int buad_rate,
    int data_bits,
    int stop_bits,
    int uart_rx,
    int uart_tx
){
    uart_init(UART_ID, buad_rate);

    gpio_set_function(uart_tx, GPIO_FUNC_UART);
    gpio_set_function(uart_rx, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, data_bits, stop_bits, PARITY);

    const char gll_off[] = "$PUBX,40,GLL,0,0,0,0,0,0*5C\r\n";
    const char gsa_off[] = "$PUBX,40,GSA,0,0,0,0,0,0*4E\r\n";
    const char gga_off[] = "$PUBX,40,GGA,0,0,0,0,0,0*5A\r\n";
    const char gsv_off[] = "$PUBX,40,GSV,0,0,0,0,0,0*59\r\n";
    const char zda_off[] = "$PUBX,40,ZDA,0,0,0,0,0,0*45\r\n";
    const char vtg_off[] = "$PUBX,40,VTG,0,0,0,0,0,0*5F\r\n"; // fix checksum
    const char rmc_on[]  = "$PUBX,40,RMC,0,1,0,0,0,0*47\r\n";

    const char* configMsgs[7] = {
        gga_off,
        gll_off,
        vtg_off,
        gsa_off,
        gsv_off,
        zda_off,
        rmc_on
    };

    // sleep_ms(10000);
    std::cout << "initializing";

    while( !uart_is_writable(UART_ID)) {
        std::cout << "waiting for uart TX available";
        sleep_ms(1000);
    } 

    for (int i = 0; i < 7; i++)
    {
        printf("test %s, %d", configMsgs[i], i);
        uart_puts(UART_ID, configMsgs[i]);
        sleep_ms(1000);
    }
        
}


// interupt based read example
// void ReadingGPS::on_uart_rx() {
//     char newNmeaMessage[200];
//     int i = 0;
//     if(uart_is_readable(UART_ID)){
//         while (i < 100) {
//             char ch = uart_getc(UART_ID);
//             newNmeaMessage[i] = ch;

//             if (i == 200) {
//                 break;
//             }
//             newNmeaMessage[i++] = ch;
//         }
//     } else {
//         std::cout << "Nothing to report\n";
//     };
// };

bool ReadingGPS::isValidNmea(std::vector<std::string> nmeaSent) {
    if(nmeaSent.size() < 0) return false; // if the sentence was void throw it out
    if(nmeaSent[0].substr(0,2) != "$G") return false; // all sentences should start with $GXXXX
    return true;
}

std::vector<std::string> ReadingGPS::splitNmeaSentence(string nmeaSent) {
    std::istringstream inputStream(nmeaSent);
    std::vector<std::string> nmea_encoding_vector;
    string nmeaElement;
    while (std::getline(inputStream, nmeaElement, ',')) {
        nmea_encoding_vector.push_back(nmeaElement);
    }
    return nmea_encoding_vector;
}

// I believe ths is I/O blocking while this loop is active kinda clunky but simple to work with the data
// clears the buffer until the begining of a sentence then reads until
// TODO validate that End lines are being properly evaluated and not fed into returned sentence.
string ReadingGPS::nextSentence() {
    char newNmeaMessage[64];
    int index = 0;
    if(uart_is_readable(UART_ID)){
        bool readingSentence = false;
        while (true) {
            char ch = uart_getc(UART_ID);

            if(ch == '$') {
                readingSentence = true;
            }

            if (readingSentence && ch == '\n' || index >= 64) {
                readingSentence = false;
                break;
            }
 
            if(readingSentence) {
                newNmeaMessage[index] = ch;
                index++;
            }
        }
        
    } 
    string result = std::string(newNmeaMessage);
    return result;
};

// reading gps state will utilize the NEO-6m gps breakout board to retrieve NMEA sentences, serialize them.
ReadingGPS::ReadingGPS() : ConcreteState("ReadingGPS", READGPS) {}
int ReadingGPS::run() {
    int numberOfRetreivedMsgs = 0;
    std::vector<std::string> nmeaSentences[10];

    // todo: eventually move this somewhere gooder?
    int buad_rate = 9600;
    int data_bits = 8;
    int stop_bits = 1;
    int uart_tx = 4;
    int uart_rx = 5;
    
    // by default neo-6m provices msgs at 1hz freq
    int nextSentenceFreq_ms = 1000;

    //initialize UART 
    setup( buad_rate, data_bits, stop_bits, uart_rx, uart_tx );

    // build up 10 valid sentences for better precision 
    while (numberOfRetreivedMsgs < 10) {
        string newMsg =  this->nextSentence();
        std::vector<std::string> deconstructedNmea = splitNmeaSentence(newMsg);
        std::cout << "isValid:" << isValidNmea(deconstructedNmea) << "Sent: " << newMsg << endl;

        if( isValidNmea(deconstructedNmea) ) {
            nmeaSentences[numberOfRetreivedMsgs] = deconstructedNmea;
            numberOfRetreivedMsgs++;
        }
        sleep_ms(nextSentenceFreq_ms);
    }
    
    std::cout << "ReadingGPS: " << name << endl;
    return 0;
}



// evaluate coordinates state will utilize the coordinate plus the previous coordinate (by default 0Lat 0Long) to see if the movement was detected
EvaluateCoord::EvaluateCoord() : ConcreteState("EvaluateCoord", EVALCOORD) {}
int EvaluateCoord::run() {
    std::cout << "EvaluateCoord: " << name << endl;
    return 0;
}

// used to control uplink to RP Zero. will first need to power on the PI Zero and then transmit data over Serial connection
Connect::Connect() : ConcreteState("Connect", CONNECT) {}
int Connect::run() {
    std::cout << "Connect: " << name << endl;
    return 0;
}


ConcreteStateService::ConcreteStateService(map<StatesEnum, map<int, ConcreteState*>> stateTransitionTable) {
    _stateTransitionTable = stateTransitionTable;
}


void ConcreteStateService::Initialize() {
    std::cout << "Starting State Machine: " << endl;
    while (true) {
        ConcreteState* nextState = getNextState(currentState, currentStateErrorCode);
        std::cout << "Next State: " << nextState->name << "\n";
        currentState = nextState->id;
        currentStateErrorCode = nextState->run();
    }
}

ConcreteState* ConcreteStateService::getNextState(StatesEnum currentState, int currentStateErrorCode) {
    std::cout << "Error:" + std::to_string(currentStateErrorCode)  << endl;
    ConcreteState *nextState = _stateTransitionTable[currentState][currentStateErrorCode];
    return nextState;
};