//
//  ScoreHandler.cpp
//  PianoChamp
//
//  Created by Ludvig Kratzert on 2019-03-25.
//  Copyright © 2019 Ludvig Kratzert. All rights reserved.
//

#include "ScoreHandler.hpp"

bool wrongNotes(bool midi[], bool player[], int size);
int n_true(bool a[], int size);

ScoreHandler::ScoreHandler(){
    score = 0;
}

void ScoreHandler::scoreHeldNotes(bool midi[], bool player[], float dt){
    int points = 0;
    if (!wrongNotes(midi, player, 127)){
        points = 100 * dt * n_true(player, 127);
    }
    score += points;
}

int ScoreHandler::getScore(){
    return score;
}

bool wrongNotes(bool midi[], bool player[], int size){
    for (int i = 0; i < size; i++){
        if (player[i] && !midi[i]) return true;
    }
    return false;
}

int n_true(bool a[], int size){
    int n = 0;
    for (int i = 0; i < size; i++){
        if (a[i]) n++;
    }
    return n;
}
