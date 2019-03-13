//
//  midiReader.h
//  midiReader
//
//  Created by Marwan Asker on 2019-02-27.
//  Copyright © 2019 Marwan Asker. All rights reserved.
//
#ifndef midiReader_h
#define midiReader_h


#endif /* midiReader_h */
using namespace std;
struct MIDI_properties {
    int start; // time
    int note; // note
    int duration;
} ;

class MidiReader{
public:
MidiReader(string fileName, int track_);
void getMidiProperties(MIDI_properties commands[]);
int fileLength;
};


