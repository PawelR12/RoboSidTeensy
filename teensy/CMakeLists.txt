cmake_minimum_required(VERSION 3.22)

set(TEENSY_SRC_PATH ${CMAKE_CURRENT_LIST_DIR}/hardware/cores/teensy4)
set(TEENSTY_LIBS_PATH ${CMAKE_CURRENT_LIST_DIR}/hardware/libraries/)
set(FREERTOS_SRC_PATH ${CMAKE_CURRENT_LIST_DIR}/freertos-teensy/src)

add_compile_definitions( 
        F_CPU=600000000 USB_SERIAL LAYOUT_US_ENGLISH USING_MAKEFILE __IMXRT1062__ ARDUINO=10813 TEENSYDUINO=159 ARDUINO_TEENSY40
)

set(TEENSY_INC 
    ${TEENSY_SRC_PATH}
    ${TEENSTY_LIBS_PATH}/Wire
)

file(GLOB_RECURSE TEENSY_SOURCES
    ${TEENSY_SRC_PATH}/*.cpp
    ${TEENSY_SRC_PATH}/*.c
    ${TEENSTY_LIBS_PATH}/Wire/*.cpp
)

list(REMOVE_ITEM TEENSY_SOURCES
    "${TEENSY_SRC_PATH}/main.cpp"
)

set(FreeRTOS_TEENSY_INC
    ${FREERTOS_SRC_PATH}
    ${FREERTOS_SRC_PATH}/inc
    ${FREERTOS_SRC_PATH}/portable
)
set(FreeRTOS_TEENSY_SOURCES
    ${FREERTOS_SRC_PATH}/croutine.c
    ${FREERTOS_SRC_PATH}/event_groups.c
    ${FREERTOS_SRC_PATH}/list.c
    ${FREERTOS_SRC_PATH}/queue.c
    ${FREERTOS_SRC_PATH}/stream_buffer.c
    ${FREERTOS_SRC_PATH}/tasks.c
    ${FREERTOS_SRC_PATH}/timers.c
    ${FREERTOS_SRC_PATH}/portable/event_responder_support.cpp
    ${FREERTOS_SRC_PATH}/portable/port.c
    ${FREERTOS_SRC_PATH}/portable/runtime_support.cpp
    ${FREERTOS_SRC_PATH}/portable/teensy_4.cpp
    ${FREERTOS_SRC_PATH}/portable/teensy_common.cpp
)