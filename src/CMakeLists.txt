cmake_minimum_required(VERSION 3.15)
project(wav2mp3)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_BUILD_TYPE  DEBUG)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../build)

set(MAIN_PROJECT_DIR ..)

add_executable(wav2mp3 main.cpp lame/lame.h
        WavFileReader.h WavFileReader.cpp datareaders/BaseWaveDataReader.cpp datareaders/BaseWaveDataReader.h datareaders/SignedIntWaveDataReader.cpp datareaders/SignedIntWaveDataReader.h datareaders/UnsignedIntWaveDataReader.cpp datareaders/UnsignedIntWaveDataReader.h datareaders/FloatWaveDataReader.cpp datareaders/FloatWaveDataReader.h datareaders/DoubleWaveDataReader.cpp datareaders/DoubleWaveDataReader.h datareaders/WaveDataReaderCreator.cpp datareaders/WaveDataReaderCreator.h datareaders/datareaders.h SignalDataEncoder.cpp SignalDataEncoder.h WAVFileConverter.cpp WAVFileConverter.h FilesListMaker.cpp FilesListMaker.h WAVFilesConverter.cpp WAVFilesConverter.h)

if (WIN32)
    add_compile_definitions(PATH_SEPARATOR='\\')
endif(WIN32)

if (UNIX)
    add_compile_definitions(PATH_SEPARATOR='/')
    target_link_libraries(wav2mp3 ${CMAKE_SOURCE_DIR}/../lib/libmp3lame.a pthread)
endif(UNIX)
