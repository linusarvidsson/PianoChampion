#ifndef Font_hpp
#define Font_hpp

#include <stdio.h>
#include <iostream>
#include <map>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H


/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

class Font{
public:
    Font(const char* fontPath, GLuint& shader, GLuint windowWidth, GLuint windowHeight);
    
    void renderText(std::string text, GLfloat x, GLfloat y);
    void setScale(GLfloat scale_);
    void setColor(glm::vec3 color_);
    
private:
    GLuint *shaderID;
    GLuint VAO, VBO;
    glm::mat4 projection;
    
    std::map<GLchar, Character> Characters;
    GLfloat scale;
    glm::vec3 color;
    
    void initFont(const char* fontPath);
};



#endif
