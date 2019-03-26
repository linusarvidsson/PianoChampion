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

class ScoreHandler{
private:
    int score;
    
public:
    ScoreHandler();
    void scoreHeldNotes(bool midi[], bool player[], float dt);
    void scoreBonusNotes();
    int getScore();
};

#endif /* ScoreHandler_hpp */
