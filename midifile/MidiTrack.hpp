#ifndef MidiTrack_hpp
#define MidiTrack_hpp

#include <stdio.h>
#include <vector>
#include "MidiFile.h"
using namespace smf;

struct MidiNote {
    int start;
    int keyNumber;
    int duration;
};

class MidiTrack{
private:
    int trackNumber;
    int numNotes;
    int trackTPQ;
    int trackBPM;
    float trackTPS;
    
    std::vector<MidiNote> trackNotes;
    
public:
    MidiTrack(const std::string& filename, int track);
    
    int size();
    int tpq();
    int bpm();
    float tps();
    MidiNote* note(int index);
};

#endif /* MidiTrack_hpp */
