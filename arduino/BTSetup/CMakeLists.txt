cmake_minimum_required(VERSION 2.8.4)

#Define your port to arduino here
set(${FIRMWARE_NAME}_PORT /dev/cu.wchusbserial1420)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME BTSetup)
project(${PROJECT_NAME})

set(${CMAKE_PROJECT_NAME}_SKETCH BTSetup.ino)

generate_arduino_firmware(${CMAKE_PROJECT_NAME})
