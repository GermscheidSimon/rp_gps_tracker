# rp_gps_tracker
Raspberry Pi Pico GPS tracking State machine. 

## Some background
When I was learning software development in 2021 One weekend my friends and I came across a mod for Minecraft called ComputerCraft which provided a Lua runtime inside the game and turtle (bots) to perform tasks in game. I learned about State Machines and while the interest in Minecraft didn’t last the interest in messing with State Machines for automated tasks stuck around

Fast forward a few months the Raspberry Pi Pico was launched. I wanted to build some sort of state machine program using it. You can read more about my first attempts on that project [here](https://github.com/GermscheidSimon/RP_PICO_GPS-Tracker).  Unfortunately the project was cut short due to chip shortages in 2021-2022 I wasn't able to obtain Rasperry Pi Zero boards after damaging my only one. I shelved the idea for a few years, but lately I've been interested in learning a more low level programming language and decided it would be a good oppourtunity to finally finish this project!

This is redesign of that project but staying true to the core design (even if in some cases I know better now)! 
## Components
 * Raspberry Pi Pico
 * Raspberry Pi Zero
 * U-blox Neo-6m GPS breakout board
 * SixFab LTE top hat (w/Telit LE910C4-NF modem)
 * 5v relay switch
## Environment and tools
 * [C/C++ Pico SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf?_gl=1*bzn8dl*_ga*MTc3NTk3MDA2LjE3MTM2MjI5NDU.*_ga_22FD70LWDS*MTcxNDgzNTAxNi42LjEuMTcxNDgzNjcyNi4wLjAuMA..)
 * Cmake
 * vscode
## How it works

### RP Pico 

