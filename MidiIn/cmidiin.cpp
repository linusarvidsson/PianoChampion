#include <iostream>
#include <cstdlib>
#include "cmidiin.h"

bool done;
static void finish(int ignore){ done = true; }

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
    // Install an interrupt handler function.
    done = false;
    (void) signal(SIGINT, finish);
}

void MidiInputReader:: getUserInput(){
    // Periodically check input queue.
    std::cout << "Reading MIDI from port ... quit with Ctrl-C.\n";
    while ( !done ) {
        midiin->getMessage( &inputQueue );
        
        if (!inputQueue.empty()) {
            int instruction = (int)inputQueue[0];
            int key = (int)inputQueue[1];
            
            if (instruction == 144) {playerInput[key] = true;}
            if (instruction == 128) {playerInput[key] = false;}
        }
    }
}

void MidiInputReader:: cleanup(){
    std::cout << "No ports available! Ending the program\n ";
    delete midiin;
    exit(0);
}
