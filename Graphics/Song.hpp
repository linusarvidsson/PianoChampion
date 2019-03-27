#ifndef Song_hpp
#define Song_hpp

#include <stdio.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "../midifile/MidiTrack.hpp"
#include "Note.hpp"

class Song{
public:
    Song(MidiTrack track);
    void updateNotes();
    void renderNotes();
    
private:
    GLuint *shaderID;
    GLuint noteVAO;
    GLuint noteVertexBuffer, noteColorBuffer, noteElementBuffer;
    
    std::vector<glm::vec3> noteVertices;
    std::vector<glm::vec3> noteColors;
    std::vector<GLuint> noteIndices;
    
    glm::mat4 Projection, View, Model;
    
};

#endif
