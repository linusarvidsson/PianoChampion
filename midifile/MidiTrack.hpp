#ifndef MidiTrack_hpp
#define MidiTrack_hpp

#include <stdio.h>
#include <vector>
#include "Options.h"
#include "MidiFile.h"
using namespace smf;

struct MIDI_note {
    int start;
    int keyNumber;
    int duration;
};

class MidiEvent{
public:
    int trackNumber;
    
private:
    MidiEvent(const std::string& filename);
};

#endif /* MidiTrack_hpp */
