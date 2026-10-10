cmake_minimum_required(VERSION 3.16)

set(TEENSY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/teensy)
set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/app)
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(CMAKE_TOOLCHAIN_FILE ${TEENSY_DIR}/cmake/toolchains/imxrt1062.cmake)

project(RoboSidTeensy LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_executable(RoboSidTeensy)
include(${TEENSY_DIR}/CMakeLists.txt)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
 COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
)

target_include_directories(RoboSidTeensy PRIVATE
    ${TEENSY_INC}
    ${FreeRTOS_TEENSY_INC}
    ${APP_DIR}
    ${APP_DIR}/Services
    ${APP_DIR}/Miscellaneous
)

file(GLOB_RECURSE APP_SOURCES
    "${APP_DIR}/*.cpp"
    "${MCU_APP_DIR}/Services/*/*.cpp"
    "${MCU_APP_DIR}/Miscellaneous/*/*.cpp"
)

target_sources(RoboSidTeensy PRIVATE 
    ${APP_SOURCES} 
    ${TEENSY_SOURCES} 
    ${FreeRTOS_TEENSY_SOURCES}
)

add_subdirectory(${LIBS_DIR}/etl)
target_link_libraries(RoboSidTeensy PRIVATE etl::etl)
