#include "ScoreHandler.hpp"
#include <math.h>

// Reset score.
void ScoreHandler::reset(){
    score = 0;
    streakScore = 0;
    multiplier = 1;
}

bool wrongNotes(bool midi[], bool player[], int size);
bool identical(bool midi[], bool player[], int size);
int n_true(bool a[], int size);
int matching(bool a[], bool b[], int size);

ScoreHandler::ScoreHandler(){
    score = 0;
    streakScore = 0;
    multiplier = 1;
}

void ScoreHandler::scoreNotes(MidiTrack* track, bool midi[], bool player[], float time, float dt){
    
    //Score the held notes
    int points = 0;
    if (!wrongNotes(midi, player, 127)){
        points = 100 * dt * matching(player, midi, 127);
    } else {
        streakScore = 0;
    }
    
    score += multiplier*points;
    
    //Update multiplier
    if (identical(midi, player, 127)){
        streakScore += points;
    } else {
        streakScore = 0;
    }
    
    multiplier = 1 + round((float)streakScore / (float)500);
    if (multiplier > 4) multiplier = 4;
    
    //Add bonus notes
    int bonusNote;
    for (int i = 0; i < 127; i++){
        if (player[i]){
            bonusNote = track->availableForBonus(time, i);
            if (bonusNote != 0) score += 300;
        }
    }
}

int ScoreHandler::getScore(){
    return score;
}

int ScoreHandler::getMultiplier(){
    return multiplier;
}

void ScoreHandler::stopStreak(){
    streakScore = 0;
    multiplier = 1;
}

bool wrongNotes(bool midi[], bool player[], int size){
    for (int i = 0; i < size; i++){
        if (player[i] && !midi[i]) return true;
    }
    return false;
}

bool identical(bool midi[], bool player[], int size){
    for (int i = 0; i < size; i++){
        if (player[i] != midi[i]) return false;
    }
    return true;
}

int n_true(bool a[], int size){
    int n = 0;
    for (int i = 0; i < size; i++){
        if (a[i]) n++;
    }
    return n;
}

int matching(bool a[], bool b[], int size){
    int result = 0;
    for (int i = 0; i < size; i++){
        if(a[i] && b[i]) result++;
    }
    return result;
}
