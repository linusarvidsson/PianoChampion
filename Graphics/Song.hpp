#ifndef Song_hpp
#define Song_hpp

#include <stdio.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "GraphicsTools.hpp"
#include "TextureQuad.hpp"
#include "../midifile/MidiTrack.hpp"
#include "Note.hpp"

static bool black[128] = {
    0,1,0,1,0,0,1,0,1,0,1,0,
    0,1,0,1,0,0,1,0,1,0,1,0,
    0,1,0,1,0,0,1,0,1,0,1,0,
    0,1,0,1,0,0,1,0,1,0,1,0,
    0,1,0,1,0,0,1,0,1,0,1,0,
    0,1,0,1,0,0,1,0,1,0,1,0,
    0,1,0,1,0,0,1,0,1,0,1,0,
    0,1,0,1,0,0,1,0,1,0,1,0,
    0,1,0,1,0,0,1,0,1,0,1,0,
    0,1,0,1,0,0,1,0,1,0,1,0,
    0,1,0,1,0,0,1,0
};

class Song{
public:
    ~Song();
    Song(MidiTrack& track, GLuint& colorShader, GLuint& textureShader, glm::mat4 viewProjection_);
    
    void updateNotes(bool matchingKeys[]);
    void updatePiano(bool playerInput[]);
    void render();

    
private:
    // Note data
    GLuint* noteShader;
    GLuint noteVAO;
    GLuint noteVertexBuffer, noteColorBuffer, noteElementBuffer;
    MidiTrack* songTrack;
    std::vector<glm::vec3> noteVertices;
    std::vector<glm::vec3> noteColors;
    std::vector<GLuint> noteIndices;
    void initNotes();
    
    // Projection components
    glm::mat4 model, viewProjection;
    
    // Background & Strike Bar
    TextureQuad* background;
    TextureQuad* strikeBar;
    
    // Piano data
    GLuint pianoVAO;
    GLuint pianoVertexBuffer, pianoColorBuffer, pianoElementBuffer;
    std::vector<glm::vec3> pianoKeyVertices;
    std::vector<glm::vec3> pianoKeyColors;
    std::vector<GLuint> pianoKeyIndices;
    void initPiano();
    void renderPiano();

};

#endif
