#include <stdio.h>
#include "pico/stdlib.h"
#include "States.h"
#include <string>
#include <vector>
#include <iostream>
#include "hardware/uart.h"
#include "States.h"

#define UART_ID uart1
#define BUAD_RATE 9600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

#define UART_TX_PIN 4
#define UART_RX_PIN 5

// RX interrupt handler
void on_uart_rx() {
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

int main() {
    std::string printString = "Hello from String literal\n";

    uart_init(UART_ID, BUAD_RATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART_ID, false);

    // // Set up a RX interrupt
    // // We need to set up the handler first
    // // Select correct interrupt for the UART we are using
    // int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // // And set up and enable the interrupt handlers
    // irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    // irq_set_enabled(UART_IRQ, true);

    // // Now enable the UART to send interrupts - RX only
    // uart_set_irq_enables(UART_ID, true, false);


    stdio_init_all();
    while (true) {
        on_uart_rx();
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
        sleep_ms(1000);
        ReadingGPS* _readingGPS = new ReadingGPS();
        EvaluateCoord* _evaluateCoord = new EvaluateCoord();
        Connect* _connect = new Connect();

        map<int, ConcreteState*> Init = {
            {0, _readingGPS}
        };

        map<int, ConcreteState*> ReadingGPS = {
            {0, _readingGPS},
            {1, _evaluateCoord}
        };

        map<int, ConcreteState*> EvaluateCoord = {
            {0, _readingGPS},
            {1, _connect}
        };

        map<int, ConcreteState*> Connect = {
            {0, _readingGPS}
        };

    
        map<StatesEnum, map<int, ConcreteState*>> stateTable =
        {
            { StatesEnum::INIT, Init },
            { StatesEnum::READGPS, ReadingGPS },
            { StatesEnum::EVALCOORD, EvaluateCoord },
            { StatesEnum::CONNECT, Connect}
        };

        std::string printString = "Hello from String literal\n";
        printf("%s\n", printString.c_str());
        ConcreteStateService(stateTable).Initialize();

    };
};
