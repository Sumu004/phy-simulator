cmake_minimum_required(VERSION 3.14)
project(phy_simulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimised release build
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(SOURCES
    src/main.cpp
    src/bit_generator.cpp
    src/modulation/qpsk.cpp
    src/modulation/qam16.cpp
    src/ofdm_tx.cpp
    src/ofdm_rx.cpp
    src/channel.cpp
    src/ber.cpp
)

add_executable(phy_simulator ${SOURCES})

target_include_directories(phy_simulator PRIVATE src)
