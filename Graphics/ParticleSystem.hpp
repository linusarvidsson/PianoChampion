#ifndef ParticleSystem_hpp
#define ParticleSystem_hpp

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "TextureQuad.hpp"

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    GLfloat life;
    
    Particle()
    : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {};
};

class ParticleSystem{
public:
    ParticleSystem(GLuint &particleShader, TextureQuad &particleQuad, GLuint _numParticles, glm::vec3 position, bool _bonus);
    
    void update(GLfloat dt, GLuint newParticles);
    void updateNotePosition(glm::vec3 position);
    void render(bool black);
    
private:
    bool bonus;
    std::vector<Particle> particles;
    GLuint numParticles;
    GLuint *shader;
    TextureQuad *quad;
    glm::vec3 sourcePosition, notePosition;
    
    GLuint lastUsedParticle = 0;
    // Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    GLuint firstUnusedParticle();
    
    // Respawns particle
    void respawnParticle(Particle &particle);
    
};

#endif
