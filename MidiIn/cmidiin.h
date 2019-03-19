//
//  cmidiin.h
//  RTMIDI2
//
//  Created by Marwan Asker on 2019-03-13.
//  Copyright © 2019 Marwan Asker. All rights reserved.
//

#ifndef cmidiin_h
#define cmidiin_h
#include <iostream>
#include <cstdlib>
#include <RtMidi.h>

class MidiInputReader{
public:
    MidiInputReader();
    void getUserInput();
    RtMidiIn *midiin;
    bool playerInput[127] = {false};
    std::vector<int> toBeTurnedOn;
    std::vector<int> toBeTurnedOff;
private:
    std::vector<unsigned char> inputQueue;
    unsigned int nPorts ;
    void cleanup();
};

extern bool keys[127];

#endif /* cmidiin_h */
