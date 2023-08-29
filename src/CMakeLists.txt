cmake_minimum_required (VERSION 3.22)

add_executable(jack_test)

target_sources(jack_test PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Main.cpp
        )

# In addition to pico_stdlib required for common PicoSDK functionality, give access to double button reboot.
target_link_libraries(jack_test PUBLIC
        #pico_stdlib
        #hardware_i2c
        #pico_bootsel_via_double_reset
        )