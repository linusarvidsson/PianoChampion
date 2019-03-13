#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace smf;

// create a struct that will be loaded with the midi-commands that are essential for the compare function.
//This will include the note that is being played and the tick (the precise time it's played).
struct MIDI_properties {
    int start;
    int note;
    int duration;
} ;

// the MidiReader class. This class is the foundation behind reading a midi file. it will take in a fileName string and a int track.
//the filename is a string, and the track is what part of the midiFile that should be played.
//Since we will be creating our own midifile later we can set int track_ = 0. the fileLenght gives the length of the midifile in amount of ticks.

class MidiReader{
public:
    int fileLength = 0;
    MidiReader(string fileName, int track_);
    void getMidiProperties(MIDI_properties properties[]); //this function returns the note and tick(midi_commands) of the midifile.
private:
    MidiFile midifile;
    int track;
    
};

MidiReader::MidiReader(string fileName, int track_){ //loaded constructor
    midifile.read(fileName); // read a file
    midifile.doTimeAnalysis(); // analyse the ticks and time of the midifile
    midifile.linkNotePairs(); // analyse the notes being played
    track = track_;
    fileLength = midifile[track].size(); //get size of the file
}

void MidiReader::getMidiProperties(MIDI_properties properties[]){ //get the ticks, notes and duration in ticks of the current "bar"
    int idx = 0;
    for (int event=0; event<fileLength; event++){
        properties[idx++] = {midifile[track][event].tick, (int)midifile[track][event].getKeyNumber(), (int)midifile[track][event].getTickDuration()};
        
        cout << midifile[track][event].tick << '\t' << (int)midifile[track][event].getKeyNumber() << '\t' << (int)midifile[track][event].getTickDuration() << '\n';
    }
}
