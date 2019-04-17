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
    // Class TextureQuad. Create a simple quadratic object with a texture.
    TextureQuad(const char* texturepath, GLfloat width, GLfloat height, glm::vec3 position, GLuint& shader, bool alphaTexture, glm::mat4 viewProjection);
    ~TextureQuad();
    
    // Render texture quad.
    void render();
    
    // Scale quad. Set new width and height.
    void scale(GLuint width, GLuint height);
    // Scale quad. Set scale factor scale.
    void scale(GLfloat scale);
    // Set the position of the quad.
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
    glm::mat4 MVP;
    
    void updateVertices();
};


#endif
