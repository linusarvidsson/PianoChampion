#ifndef note_hpp
#define note_hpp

#include <GL/glew.h>
#include <glm/glm.hpp>

struct vertex {
    GLfloat x, y, z;
};

class note{
private:
    // Noternas bredd. Avståndet mätt från centrum av noten till kanten i x-led, den riktiga bredden är alltså x2.
    const GLfloat BLACK_WIDTH = 0.06;
    const GLfloat WHITE_WIDTH = 0.09;
    
    bool black;
    GLfloat startPoint;
    GLfloat endPoint;
    GLfloat position;
    GLfloat rightBorder;
    GLfloat leftBorder;
    glm::vec3 noteColor;
    int noteNumber;
    int localNote;
    int octave;
    
    void setPosition();
    void blackOrWhite();
    void setNoteColor();
    void setBlackOffset();

    
public:
    note(int noteNumber_, GLfloat start_, GLfloat end_);
    
    GLfloat start();
    GLfloat end();
    GLfloat right();
    GLfloat left();
    glm::vec3 color();
    bool isBlack();
    
};

#endif
