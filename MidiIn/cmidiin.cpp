#include <iostream>
#include <cstdlib>
#include "cmidiin.h"

MidiInputReader:: MidiInputReader(){
    // Check available ports.
    midiin = new RtMidiIn();
    nPorts = midiin->getPortCount();
    if ( nPorts == 0 ) {
        cleanup();
    }
    midiin->openPort( 0 );
    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( false, false, false );
}

void MidiInputReader:: getUserInput(){
    // Periodically check input queue.
    int check_n = 20;
    while (check_n != 0) {
        midiin->getMessage( &inputQueue );
        
        if (!inputQueue.empty()) {
            int instruction = (int)inputQueue[0];
            int key = (int)inputQueue[1];
            
            if (instruction == 144) {playerInput[key] = true; toBeTurnedOn.push_back(key);}
            if (instruction == 128) {playerInput[key] = false; toBeTurnedOff.push_back(key);}
        }
        check_n--;
    }
}

void MidiInputReader:: cleanup(){
    std::cout << "No ports available! Ending the program\n ";
    delete midiin;
    exit(0);
}
