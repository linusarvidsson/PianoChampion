#include "Note.hpp"

// Constructor
Note::Note(int noteNumber_, GLfloat start_, GLfloat end_){
    noteNumber = noteNumber_;
    startPoint = start_;
    endPoint = end_;
    
    // Lokalt notnummer för positionsberäkningar. C4 är centrum med position 0 och lokalt notnummer 0.
    localNote = noteNumber -60;
    
    // OBS. EJ äkta oktav. Justerad för beräkningar där 3:e och 4:e oktaven har oktavnummer 0 vilket ger symmetri.
    if(localNote > 0) octave = glm::sign(localNote) * floor(abs((float)localNote / 12.0f));
    else octave = glm::sign(localNote) * floor((abs((float)localNote + 1) / 12.0f));
    
    setPosition();
    blackOrWhite();
    setNoteColor();
    noteHeight = 0.0f;
    
    //std::cout << "\nNote number: " << noteNumber_ << "\nLocal note: " << localNote << "\nPosition: " << position << "\nBlack: " << black << std::endl;
    
    if(black) setBlackOffset();
}

// Funktioner för åtkomst till notens egenskaper
GLfloat Note::start() { return startPoint; }
GLfloat Note::end() { return endPoint; }
GLfloat Note::right() { return rightBorder; }
GLfloat Note::left() { return leftBorder; }
GLfloat Note::height() { return noteHeight; }
glm::vec3 Note::color() { return noteColor; }
bool Note::isBlack() { return black; }

// Bestämmer notens färg
void Note::setNoteColor(){
    if(black) noteColor = glm::vec3(0.0f, 0.2f, 0.4f);
    else noteColor = glm::vec3(0.015f, 0.517f, 1.0f);
}

// Bestämmer notens position i x-planet
void Note::setPosition(){
    // Tangenter över och inklusive C4
    if(localNote >= 0){
        // Intervallet C - E
        if(localNote % 12 < 5){
            position = (GLfloat)(localNote + 2*octave) / 10.f;
        }
        // Intervallet F - B
        else{
            position = (GLfloat)(localNote + 2*octave + 1) / 10.f;
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
void Note::blackOrWhite(){
    if ((int)(position * 10) % 2 == 0) {
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
void Note::setBlackOffset(){
    GLfloat offsetCD = 0.06f;
    GLfloat offsetFA = 0.058f;
    
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

void Note::pianoSettings(){
    if(black){
        startPoint = -3.4f;
        endPoint = 5.0f; //Was -2.5
        noteHeight = -0.001f;
    }
    else{
        startPoint = -3.8f;
        endPoint = 5.0f;
        noteHeight = -0.002f;
    }
}
