//
//  ScoreHandler.hpp
//  PianoChamp
//
//  Created by Ludvig Kratzert on 2019-03-25.
//  Copyright © 2019 Ludvig Kratzert. All rights reserved.
//

#ifndef ScoreHandler_hpp
#define ScoreHandler_hpp

#include <stdio.h>
#include "../midifile/MidiTrack.hpp"

class ScoreHandler{
private:

    int streakScore;
    int multiplier;
    
public:
        int score;
    ScoreHandler();
    void reset();
    void scoreNotes(MidiTrack* track, bool midi[], bool player[], float time, float dt);
    int getScore();
    int getMultiplier();
    void stopStreak();
};

#endif /* ScoreHandler_hpp */
