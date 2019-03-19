#ifndef MidiTrack_hpp
#define MidiTrack_hpp

#include <stdio.h>
#include <vector>
#include "MidiFile.h"
using namespace smf;

struct MidiNote {
    double start;
    double end;
    int keyNumber;
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
    MidiTrack(const std::string& filename, int track, int BPM);

    int size();
    int tpq();
    int bpm();
    void bpm(int BPM);
    float tps();
    MidiNote* note(int index);
    void updateCurrentNotes(bool (&currentNotes)[], double time);
    int searchNote(double time, int key);
    void transpose(int keyshift);
};

#endif /* MidiTrack_hpp */
