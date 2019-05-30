#ifndef TextureQuad_hpp
#define TextureQuad_hpp

#include <stdio.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "GraphicsTools.hpp"

class TextureQuad{
public:
    // Class TextureQuad. Create a simple quadratic object with a texture.
    TextureQuad(const char* texturepath, GLfloat width, GLfloat height, glm::vec3 position, GLuint& shader, bool alphaTexture, glm::mat4 _projection, glm::mat4 _view);
    ~TextureQuad();
    
    // Render texture quad.
    void render();
    
    // Scale quad. Set scale factor scale.
    void scaleQuad(GLfloat scale);
    // Set the position of the quad.
    void position(glm::vec3 position);
    // Translate (Model projection)
    void translate(GLfloat x, GLfloat y, GLfloat z);
    // Scale (Model projection)
    void scale(GLfloat scale);
    // Rotate (Model projection)
    void rotate(GLfloat angle);
    // Reset model
    void reset();
    
    void setColor(glm::vec3 texColor);
    void setColor(glm::vec3 texColor, GLfloat texAlpha);
    void setColor(glm::vec4 texColor);
    
private:
    // Quad data
    GLfloat quadWidth, quadHeight;
    glm::vec3 quadPosition;
    glm::vec4 color;
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
