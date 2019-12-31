# Brief
This is a project for task: 
- convert WAV files to MP3 files in multithread manner.

# Description
The project contains WAV file parser for data types:
- unsinged integer 8 bit
- signed integer 16 bit
- signed integer 24 bit
- signed integer 32 bit
- float 32 bit
- float 64 bit

It should works and for unsigned 2..7 bit data, and signed 9..31 data, but i have no files to check.

To start encoding we should run the program with args like: `wav2mp3 ./directory_with_wav_files`

# Result
Now the application can read WAV file and convert it to MP3 file using Lame library 3.100 in single thread.
The files for encoding should be described in the main function.
It works only in Linux (debian, ubuntu) yet. 

# Plan

1. Add multithreading for files
2. Add simple logger for multithread environment
3. Try to compile in Windows
4. Think about ID3 tags

#### Notes

./configure --prefix=.../wav2mp3/lame-3.100/build --with-pic
