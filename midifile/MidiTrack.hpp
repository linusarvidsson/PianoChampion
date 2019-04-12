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
    bool triggered;
};

class MidiTrack{
    
public:
    MidiTrack(const std::string& filename, int track, int BPM);
    MidiTrack();
    
    int size();
    int tpq();
    int bpm();
    void bpm(int BPM);
    float tps();
    double duration();
    
    MidiNote* note(int index);
    void setStartOffset(double offset);
    void updateCurrentNotes(bool currentNotes[], double time);
    void updateQueues(double last_time, double current_time);
    int searchNote(double time, int key);
    void searchNotes(double time, std::vector<int> &noteNumbers, bool playerInput[]);
    void triggerNotes(std::vector<int> &noteNumbers);
    void triggerNote(int n);
    int countTriggeredNotes();
    int availableForBonus(double time, int key);
    bool missedNotes(double time, double offset);
    void transpose(int keyshift);
    
    std::vector<int> toBeTurnedOn;
    std::vector<int> toBeTurnedOff;
    
private:
    //std::string trackPath;
    int trackNumber;
    int numNotes;
    int trackTPQ;
    int trackBPM;
    float trackTPS;

    std::vector<MidiNote> trackNotes;


};

#endif
