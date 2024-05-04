#include "States.h"
#include <string>
#include "iostream"
#include <map>
#include "string"
#include "States.h"
#include "pico/stdlib.h"

#define UART_ID uart1
#define PARITY  UART_PARITY_NONE
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

    uart_set_format(UART_ID, data_bits, stop_bits, PARITY);
    uart_set_fifo_enabled(UART_ID, false);

    // // Set up a RX interrupt
    // // We need to set up the handler first
    // // Select correct interrupt for the UART we are using
    // int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // // And set up and enable the interrupt handlers
    // irq_set_exclusive_handler(UART_IRQ, this->on_uart_rx);
    // irq_set_enabled(UART_IRQ, true);

    // // Now enable the UART to send interrupts - RX only
    // uart_set_irq_enables(UART_ID, true, false);
}

void ReadingGPS::on_uart_rx() {
    char newNmeaMessage[200];
    int i = 0;
    if(uart_is_readable(UART_ID)){
        while (i < 100) {
            char ch = uart_getc(UART_ID);
            newNmeaMessage[i] = ch;

            if (i == 200) {
                break;
            }
            newNmeaMessage[i++] = ch;
        }

        std::cout << "New Msg:" << std::string(newNmeaMessage);
    } else {
        std::cout << "Nothing to report\n";
    };
};

// reading gps state will utilize the NEO-6m gps breakout board to retrieve NMEA sentences, serialize them.
ReadingGPS::ReadingGPS() : ConcreteState("ReadingGPS", READGPS) {}
int ReadingGPS::run() {
    
    // todo: eventually move this somewhere gooder
    int buad_rate = 9600;
    int data_bits = 8;
    int stop_bits = 1;
    int uart_rx = 4;
    int uart_tx = 5;
    setup(
        buad_rate,
        data_bits,
        stop_bits,
        uart_rx,
        uart_tx
    );


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