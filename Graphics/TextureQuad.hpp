#ifndef TextureQuad_hpp
#define TextureQuad_hpp

#include <stdio.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "GraphicsTools.hpp"

class TextureQuad{
public:
    TextureQuad(const char* texturepath, GLfloat width, GLfloat height, glm::vec3 position, GLuint& shader, bool alphaTexture);
    ~TextureQuad();
    
    void render();
    void scale(GLuint width, GLuint height);
    void scale(GLfloat scale);
    void position(glm::vec3 position);
    
private:
    // Quad data
    GLfloat quadWidth, quadHeight;
    glm::vec3 quadPosition;
    std::vector<glm::vec3> vertices;
    static const GLuint indices[];
    
    // Texture data
    GLuint* shader;
    bool alpha;
    GLuint texture;
    static const GLfloat uv[];
    
    // GL data
    GLuint VAO;
    GLuint vertexBuffer, uvBuffer, elementBuffer;
    glm::mat4 projection, view, model;
    
    void updateVertices();
};


#endif
