//
//  qmidiin.h
//  PianoChampion
//
//  Created by Marwan Asker on 2019-04-11.
//  Copyright © 2019 Marwan Asker. All rights reserved.
//

#ifndef qmidiin_h
#define qmidiin_h


#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <queue>
#include "RtMidi.h"

class MidiInputReader{
private:
public:
    MidiInputReader();
    ~MidiInputReader();
    RtMidiIn* midiin;
    std::queue<int> toBeTurnedOn;
    std::queue<int> toBeTurnedOff;
    bool keys[128] = {false};
    void update();
};

#endif /* qmidiin_h */

