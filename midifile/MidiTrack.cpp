#include "MidiTrack.hpp"
#include <iostream>


MidiTrack::MidiTrack(const std::string& filename, int track, int bpm){
    //trackPath = filename;
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
            trackNotes.push_back({midifile[track][event].tick / trackTPS, (midifile[track][event].tick + midifile[track][event].getTickDuration()) / trackTPS, midifile[track][event].getKeyNumber(), false});
            current_note++;
        }
    }
}

MidiTrack::MidiTrack() {
    trackNumber = 0;
    trackTPQ = 0;
    trackBPM = 0;
    trackTPS = 0;
    trackNotes.clear();
}

double MidiTrack:: duration(){ //returns song duration in seconds 

    return trackNotes.back().end;
}

// Sök not. Returnerar notens index.
int MidiTrack::searchNote(double time, int key){
    for(int n = 0; n < numNotes; n++){
        if(key == trackNotes[n].keyNumber && trackNotes[n].start <= time && trackNotes[n].end >= time){
            return n;
        }
    }
    return 0;
}

void MidiTrack::searchNotes(double time, std::vector<int> &noteNumbers, bool notes[]){
    noteNumbers.clear();
    for(int n = 0; n < numNotes; n++){
        if(trackNotes[n].start <= time && trackNotes[n].end >= time){
            if(notes[trackNotes[n].keyNumber]) {
                noteNumbers.push_back(n);
            }
        }
    }
    //if (!noteNumbers.empty()) std::cout << noteNumbers[0];
}

void MidiTrack::triggerNotes(std::vector<int> &noteNumbers) {
    int n;
    for (int i = 0; i < noteNumbers.size(); i++){
        n = noteNumbers[i];
        trackNotes[n].triggered = true;
    }
}

void MidiTrack::triggerNote(int n){
    trackNotes[n].triggered = true;
}

int MidiTrack::countTriggeredNotes(){
    int result = 0;
    for (int i = 0; i < numNotes; i++){
        if (trackNotes[i].triggered) result++;
    }
    return result;
}

int MidiTrack::availableForBonus(double time, int key){
    MidiNote note;
    for(int n = 0; n < numNotes; n++){
        note = trackNotes[n];
        if(note.start <= time && note.start + 0.3 >= time){
            if (note.keyNumber == key && !note.triggered){
                return n;
            }
        }
    }
    return 0;
}

bool MidiTrack::missedNotes(double time, double offset){
    for(int n = 0; n < numNotes; n++){
        if(trackNotes[n].end < time && trackNotes[n].end > time - offset){
            if (trackNotes[n].triggered == false) return true;
        }
    }
    return false;
}

void MidiTrack::updateCurrentNotes(bool currentNotes[], double time){
    
    for(int i = 0; i < 128; i++){
        currentNotes[i] = false;
    }
    
    for(int n = 0; n < numNotes; n++){
        if(trackNotes[n].start <= time && trackNotes[n].end >= time){
            currentNotes[ trackNotes[n].keyNumber ] = true;
        }
    }
}

void MidiTrack::updateQueues(double last_time, double current_time) {
    toBeTurnedOn.clear();
    toBeTurnedOff.clear();
    for(int n = 0; n < numNotes; n++){
        if(trackNotes[n].start <= current_time && trackNotes[n].start >= last_time){
            toBeTurnedOn.push_back(trackNotes[n].keyNumber);
        }
        if(trackNotes[n].end <= current_time && trackNotes[n].end >= last_time){
            toBeTurnedOff.push_back(trackNotes[n].keyNumber);
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
