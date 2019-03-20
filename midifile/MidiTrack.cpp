//
//  MidiTrack.cpp
//  PianoMan
//
//  Created by Linus Arvidsson on 2019-03-13.
//  Copyright © 2019 Linus Arvidsson. All rights reserved.
//

#include "MidiTrack.hpp"


MidiTrack::MidiTrack(const std::string& filename, int track, int bpm){
    MidiFile midifile;
    midifile.read(filename);
    midifile.doTimeAnalysis();
    midifile.linkNotePairs();

    trackNumber = track;
    trackTPQ = midifile.getTicksPerQuarterNote();
    trackBPM = bpm;
    trackTPS = (float)trackTPQ * (float)trackBPM / 60.0f;

    // Räkna antalet notes i spåret
    numNotes = 0;
    for (int event=0; event<midifile[track].size(); event++) {
        if (midifile[track][event].isNoteOn()){
            numNotes++;
        }
    }

    // Reservera utrymme för alla noter i vektorn trackNotes
    trackNotes.reserve(numNotes);

    // Lägg till noter i trackNotes
    int current_note = 0;
    for (int event=0; event<midifile[track].size(); event++) {
        if (midifile[track][event].isNoteOn()){
            trackNotes.push_back({midifile[track][event].tick / trackTPS, (midifile[track][event].tick + midifile[track][event].getTickDuration()) / trackTPS, midifile[track][event].getKeyNumber()});
            current_note++;
        }
    }
}

// 

// Sök not. Returnerar notens index.
int MidiTrack::searchNote(double time, int key){
    for(int n = 0; n < numNotes; n++){
        if(key == trackNotes[n].keyNumber && trackNotes[n].start <= time && trackNotes[n].end >= time){
            return n;
        }
    }
    return 0;
}

void MidiTrack::searchNotes(double time, std::vector<int> *noteNumbers, bool playerInput[]){
    for(int n = 0; n < numNotes; n++){
        if(trackNotes[n].start <= time && trackNotes[n].end >= time){
            if(playerInput[trackNotes[n].keyNumber]) {
                noteNumbers->push_back(n);
            }
        }
    }
}

void MidiTrack::updateCurrentNotes(bool currentNotes[], double time){
    
    for(int i = 0; i < 127; i++){
        currentNotes[i] = false;
    }
    
    for(int n = 0; n < numNotes; n++){
        if(trackNotes[n].start <= time && trackNotes[n].end >= time){
            currentNotes[ trackNotes[n].keyNumber ] = true;
        }
    }
}

// Transpoerar alla noter med keyshift steg
void MidiTrack::transpose(int keyshift){
    for (int n = 0; n < numNotes; n++){
        trackNotes[n].keyNumber += keyshift;
    }
}

// Ändrar spårets BPM
void MidiTrack::bpm(int BPM){
    if (BPM > 0) {
        trackBPM = BPM;
        trackTPS = (float)trackTPQ * (float)trackBPM / 60.0f;
    }
}

// Funktioner för åtkomst till spårets innehåll och egenskaper
int MidiTrack::size() { return numNotes; }
int MidiTrack::tpq() { return trackTPQ; }
int MidiTrack::bpm() { return trackBPM; }
float MidiTrack::tps() { return trackTPS; }
MidiNote* MidiTrack::note(int index) { return &trackNotes[index]; }
