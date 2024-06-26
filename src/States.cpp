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




/*****MOVEEEE*/
// ddmm.mmmmm
struct decimalDegreesMinutes_Minutes {
    int degrees;
    double minutes;
};

/**
 * Example RMC Sentences consists of 10 sections. 
 * $GPRMC,010530.00,A,0000.00000,N,0000.00000,W,6.367,76.77,050524
*/
class RMCNmeaSentence {
    public:
        decimalDegreesMinutes_Minutes latitude;
        decimalDegreesMinutes_Minutes longitude;
        char lat_cardinal = 0;
        char long_cardinal = 0;
        bool tryParseSentence(std::vector<std::string> sentence) {
            
            if(sentence.size() < 6) return false;

            std::string latitudeStr = sentence[3]; 
            lat_cardinal = sentence[4][0];
            std::string longitudeStr = sentence[5];
            long_cardinal = sentence[6][0];

            latitude = getDegreesMinutes(latitudeStr);
            longitude = getDegreesMinutes(longitudeStr);      

            std::cout << latitude.degrees << " Degrees "  << latitude.minutes << " Mins " << lat_cardinal << endl;
            std::cout << longitude.degrees << " Degrees " << longitude.minutes << " Mins " << long_cardinal << endl;

            bool isValidCardinals = (lat_cardinal + long_cardinal) > 0;
            bool isValidLatLong = (longitude.degrees + longitude.minutes + latitude.degrees + latitude.minutes) > 0; 
            return isValidCardinals && isValidLatLong;
        }
        /**
         * convert string containg positional information into struct containing numeric values
         * the Nmea Sentence stores the Latitude coord as ddmm.mmmmm and longitude coord as dddmm.mmmmm
         * dd (int) degrees
         * mm.mmmmm (double) minutes 
        */
        decimalDegreesMinutes_Minutes getDegreesMinutes(std::string decimalPositionStr) {
            // if the converstion fails return 0s 
            int degInt = 0;
            double minsDoub = 0;

            if(decimalPositionStr.length() == 10) { // lat
                degInt = std::stoi(decimalPositionStr.substr(0,2));
                minsDoub = std::stod(decimalPositionStr.substr(2,9));
            }
            if(decimalPositionStr.length() == 11) { // long
                degInt = std::stoi(decimalPositionStr.substr(0,3));
                minsDoub = std::stod(decimalPositionStr.substr(3,10));
            }
            return {degInt, minsDoub};
        }



};
/****MOVE */


int ConcreteState::run() {
    std::cout << "base run";
    return 1;
}
// Initial Is just really an entry point to start the state machine in motion just so I dont need a messy state table. should only be called one time and never looped back to
Initial::Initial() : ConcreteState("Initial", INIT) {}
int Initial::run() {
    std::cout << "initializing: " << name;
    return 0;
}


// move to initial state
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

    std::cout << "configureing GPS module";
    for (int i = 0; i < 7; i++) {
       std::cout << "." << std::endl;
        uart_puts(UART_ID, configMsgs[i]);
        sleep_ms(1000);
    }
    uint32_t sleep = 5000;
    sleep_ms(sleep);
    std::cout << "Finished configuring GPS Module" << std::endl;
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

//check at a hight level if the sentnece is valid. 
bool ReadingGPS::isValidNmea(std::vector<std::string> nmeaSent) {
    if(nmeaSent.size() < 0) return false; // if the sentence was void throw it out
    if(nmeaSent[0] != "$GPRMC") return false; // all sentences should start with $GXXXX
    return true;
}

// create vector containg each section of the nmea sentence 
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

// TODO 2: uart_read_blocking may be more reliable to read until a full sentences is received
string ReadingGPS::nextSentence() {
    char newNmeaMessage[64];
    int index = 0;
    bool readingSentence = false;
    sleep_ms(10);
    if(uart_is_readable(UART_ID)){
        while (true) {
            char ch = uart_getc(UART_ID);

            if(ch == '$') {
                readingSentence = true;
            }

            if (readingSentence && ch == '\n') {
                readingSentence = false;
                break;
            }
 
            if(readingSentence && ch != '\n', ch != '\r') {
                newNmeaMessage[index] = ch;
                index++;
            }
        }
        sleep_ms(5);
    } 
    string result = std::string(newNmeaMessage);
    return result;
};

// reading gps state will utilize the NEO-6m gps breakout board to retrieve NMEA sentences, serialize them.
ReadingGPS::ReadingGPS() : ConcreteState("ReadingGPS", READGPS) {
    int buad_rate = 9600;
    int data_bits = 8;
    int stop_bits = 1;
    int uart_tx = 4;
    int uart_rx = 5;
    setup( buad_rate, data_bits, stop_bits, uart_rx, uart_tx );
}


int ReadingGPS::run() {
    std::cout << "Running ReadingGPS\n";

    int numberOfRetreivedMsgs = 0;
    std::vector<std::string> nmeaSentences[10];

    // by default neo-6m provices msgs at 1hz freq
    int nextSentenceFreq_ms = 1500;
    // build up 10 valid sentences for better precision 
    while (numberOfRetreivedMsgs < 10) {
        std::cout << "gettingNextSent";
        string newMsg =  this->nextSentence();
        std::vector<std::string> deconstructedNmea = splitNmeaSentence(newMsg);
        std::cout << "isValid:" << isValidNmea(deconstructedNmea) << "Sent: " << newMsg << endl;

        if( !isValidNmea(deconstructedNmea) ) {
            continue;
        }

        RMCNmeaSentence sentence = RMCNmeaSentence();
        bool isParsed = sentence.tryParseSentence(deconstructedNmea);
        if(isParsed){
            std::cout << isParsed << endl;
            nmeaSentences[numberOfRetreivedMsgs] = deconstructedNmea;
            numberOfRetreivedMsgs++;
        }

        sleep_ms(nextSentenceFreq_ms);// I think polling in this way, if you get to the end of the buffer it hands you a return line intead of waiting for one from the buffer.
    }
    
    return 0;
}



// evaluate coordinates state will utilize the coordinate plus the previous coordinate (by default 0Lat 0Long) to see if the movement was detected
EvaluateCoord::EvaluateCoord() : ConcreteState("EvaluateCoord", EVALCOORD) {}
int EvaluateCoord::run() {
    std::cout << "Run EvaluateCoord\n";
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
