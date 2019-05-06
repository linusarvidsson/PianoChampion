#include "TextureQuad.hpp"

const GLuint TextureQuad::indices[] = {
    0,1,2,2,1,3
};

const GLfloat TextureQuad::uv[] = {
    0.0f, 1.0f,   // top-left corner
    1.0f, 1.0f,  // top-right corner
    0.0f, 0.0f,  // lower-left corner
    1.0f, 0.0f,  // lower-right corner
    
};


TextureQuad::TextureQuad(const char* texturepath, GLfloat width, GLfloat height, glm::vec3 position, GLuint& textureShader, bool alphaTexture, glm::mat4 _projection, glm::mat4 _view)
    :quadWidth(width), quadHeight(height), alpha(alphaTexture), quadPosition(position), projection(_projection), view(_view)
{
    texture = GraphicsTools::loadTexture(texturepath, alpha);
    shader = &textureShader;
    
    vertices.reserve(4);
    vertices.push_back(glm::vec3( position.x - width/2.0f, position.y - height/2.0f, position.z ));
    vertices.push_back(glm::vec3( position.x + width/2.0f, position.y - height/2.0f, position.z ));
    vertices.push_back(glm::vec3( position.x - width/2.0f, position.y + height/2.0f, position.z ));
    vertices.push_back(glm::vec3( position.x + width/2.0f, position.y + height/2.0f, position.z ));

    // Set default model
    model = glm::mat4(1.0f);
    
    // VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // Create vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // Create UV buffer
    glGenBuffers(1, &uvBuffer);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    // Create index buffer
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    
    // Set "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(glGetUniformLocation(*shader, "myTextureSampler"), 0);
    
}

TextureQuad::~TextureQuad(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteBuffers(1, &elementBuffer);
}

void TextureQuad::render(){
    if(alpha) glEnable(GL_BLEND);
    
    glm::mat4 MVP = projection * view * model;
    
    glUseProgram(*shader);
    glUniformMatrix4fv(glGetUniformLocation(*shader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    
    glBindVertexArray(VAO);
    
    // Bind texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Draw the textured elements
    glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, NULL);
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if(alpha) glDisable(GL_BLEND);
}

void TextureQuad::updateVertices(){
    vertices[0] = glm::vec3( quadPosition.x - quadWidth/2.0f, quadPosition.y - quadHeight/2.0f, quadPosition.z );
    vertices[1] = glm::vec3( quadPosition.x + quadWidth/2.0f, quadPosition.y - quadHeight/2.0f, quadPosition.z );
    vertices[2] = glm::vec3( quadPosition.x - quadWidth/2.0f, quadPosition.y + quadHeight/2.0f, quadPosition.z );
    vertices[3] = glm::vec3( quadPosition.x + quadWidth/2.0f, quadPosition.y + quadHeight/2.0f, quadPosition.z );
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
}

void TextureQuad::scaleQuad(GLfloat scale){
    quadWidth *= scale;
    quadHeight *= scale;
    updateVertices();
}

void TextureQuad::position(glm::vec3 position){
    quadPosition = position;
    updateVertices();
}

void TextureQuad::translate(GLfloat x, GLfloat y, GLfloat z){
    model = glm::translate(model, glm::vec3(x, y, z));
}

void TextureQuad::scale(GLfloat scale){
    model = glm::scale(model, glm::vec3(scale));
}

void TextureQuad::reset(){
    model = glm::mat4(1.0f);
}
