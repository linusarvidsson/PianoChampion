#include "note.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <iostream>
using namespace std;

// Constructor
note::note(int noteNumber_, GLfloat start_, GLfloat end_){
    noteNumber = noteNumber_;
    startPoint = start_;
    endPoint = end_;
    
    // Lokalt notnummer för positionsberäkningar. C4 är centrum med position 0 och lokalt notnummer 0.
    localNote = noteNumber - 60;
    
    // OBS. Ej äkta oktav. Justerad för beräkningar där 3:e och 4:e oktaven har oktavnummer 0 vilket ger symmetri.
    if(localNote > 0) octave = glm::sign(localNote) * floor(abs((float)localNote / 12.0f));
    else octave = glm::sign(localNote) * floor((abs((float)localNote + 1) / 12.0f));
    
    setPosition();
    blackOrWhite();
    setNoteColor();
    if(black) setBlackOffset();
    
    cout << "\nNote number: " << noteNumber + 24 << "\nLocal note: " << localNote << "\nOctave: " << octave << "\nPosition: " << position << endl;
}

// Funktioner för åtkomst till notens egenskaper
GLfloat note::start() { return startPoint; }
GLfloat note::end() { return endPoint; }
GLfloat note::right() { return rightBorder; }
GLfloat note::left() { return leftBorder; }
glm::vec3 note::color() { return noteColor; }
bool note::isBlack() { return black; }

// Bestämmer notens färg
void note::setNoteColor(){
    if(black) noteColor = glm::vec3(1.0f, 0.0f, 0.0f);
    else noteColor = glm::vec3(0.0f, 1.0f, 0.0f);
}

// Bestämmer notens position i x-planet
void note::setPosition(){
    // Tangenter över och inklusive C4
    if(localNote >= 0){
        // Intervallet C - E
        if(localNote % 12 < 5){
            position = (GLfloat)(localNote + 2*octave) / 10.f;
        }
        // Intervallet F - B
        else{
            localNote = (GLfloat)(localNote + 2*octave + 1) / 10.f;
        }
    }
    
    // Tangenter under C4
    if(localNote < 0){
        // Intervallet B - F
        if((-localNote -1) % 12 < 7){
            position = (GLfloat)(localNote + 2*octave - 1) / 10.f;
        }
        // Intervallet E - C
        else{
            position = (GLfloat)(localNote + 2*octave -2) / 10.f;
        }
    }
}

// Kollar om det är en vit eller svart tangent
void note::blackOrWhite(){
    if ((int)(position * 60) % 2 == 0) {
        black = false;
        rightBorder = position + WHITE_WIDTH;
        leftBorder = position - WHITE_WIDTH;
    }
    else {
        black = true;
        rightBorder = position + BLACK_WIDTH;
        leftBorder = position - BLACK_WIDTH;
    }
}

// Korrigerar svarta tangenters placering. C#, D#, F# och A# är inte helt centrerade mellan två vita tangenter.
void note::setBlackOffset(){
    GLfloat offsetCD = 0.06f;
    GLfloat offsetFA = 1.0f / 17.5f;
    
    if(localNote > 0){
        if (localNote - 12*octave == 1) position -= offsetCD;
        else if (localNote - 12*octave == 3) position += offsetCD;
        else if (localNote - 12*octave == 6) position -= offsetFA;
        else if (localNote - 12*octave == 10) position += offsetFA;
    }
    else{
        if (localNote - 12*octave == -11) position -= offsetCD;
        else if (localNote - 12*octave == -9) position += offsetCD;
        else if (localNote - 12*octave == -6) position -= offsetFA;
        else if (localNote - 12*octave == -2) position += offsetFA;
    }
}
