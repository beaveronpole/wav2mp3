cmake_minimum_required(VERSION 3.15)
project(wav2mp3)

set(CMAKE_CXX_STANDARD 98)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(MAIN_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(wav2mp3 main.cpp lame/lame.h
        WavFileReader.h WavFileReader.cpp)

target_link_libraries(wav2mp3 ${CMAKE_SOURCE_DIR}/lib/libmp3lame.a)