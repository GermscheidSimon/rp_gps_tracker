add_executable(pico_gps_tracker
        pico_gps_tracker.cpp
        states.cpp
        )

target_link_libraries(pico_gps_tracker pico_stdlib pico_stdio_uart)

# create map/bin/hex file etc.
pico_add_extra_outputs(pico_gps_tracker)

# add url via pico_set_program_url
example_auto_set_url(pico_gps_tracker)
pico_enable_stdio_usb(pico_gps_tracker 1)
pico_enable_stdio_uart(pico_gps_tracker 1)