#include "ScoreHandler.hpp"
#include <math.h>

// Reset score.
void ScoreHandler::reset(){
    score = 0;
    streakScore = 0;
    multiplier = 1;
}

bool wrongNotes(bool midi[], bool player[], int size);
int n_true(bool a[], int size);

ScoreHandler::ScoreHandler(){
    score = 0;
    streakScore = 0;
    multiplier = 1;
}

void ScoreHandler::scoreHeldNotes(bool midi[], bool player[], float dt){
    int points = 0;
    if (!wrongNotes(midi, player, 127)){
        points = 100 * dt * n_true(player, 127);
    }
    score += multiplier*points;
    streakScore += points;
    
    multiplier = 1 + round((float)streakScore / (float)500);
    if (multiplier > 4) multiplier = 4;
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

int ScoreHandler::getStreakScore()
{
	return streakScore;
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
