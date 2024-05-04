#include <stdio.h>
#include "pico/stdlib.h"
#include "States.h"
#include <string>
#include <vector>
#include <iostream>
#include "hardware/uart.h"
#include "States.h"
#include "pico/bootrom.h"

#define UART_ID uart1
#define BUAD_RATE 9600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY  UART_PARITY_NONE

#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define REBOOT_BUTTON 14

// debugging interrupt handler
void external_user_interupt_callback(uint gpio, uint32_t event_mask) {
    std::cout << "BUTTON PRESS";
    reset_usb_boot(0,0);
}

int main() {
    std::string printString = "Hello from String literal\n";

    uart_init(UART_ID, BUAD_RATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    stdio_init_all();
    while (true) {

        gpio_init(REBOOT_BUTTON);
        // set gpio pin to input pin
        gpio_set_dir(REBOOT_BUTTON, false);
        gpio_pull_up(REBOOT_BUTTON);

        // configure a callback function to handle interupt. Must have signature of void (*gpio_irq_callback_t)(uint gpio, uint32_t event_mask)
        // GPIO_IRQ_EDGE_FALL is means the current over this gpio is FALLING due to in this example the pin being shorted to ground with a button. Shorting to pico pin 18 (GND)
        gpio_set_irq_enabled_with_callback(REBOOT_BUTTON, GPIO_IRQ_EDGE_FALL, true, external_user_interupt_callback);

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
