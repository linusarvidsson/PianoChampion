#include "ParticleSystem.hpp"

ParticleSystem::ParticleSystem(GLuint &particleShader, TextureQuad &particleQuad, GLuint _numParticles, glm::vec3 position, bool _bonus)
: shader(&particleShader), quad(&particleQuad), numParticles(_numParticles), sourcePosition(position), bonus(_bonus)
{
    for (GLuint i = 0; i < numParticles; i++){
        particles.push_back(Particle());
        particles[i].position.z = 0.0003f + (float)i / 10000.0f;
    }
    
    notePosition = glm::vec3(0.0f);
}

void ParticleSystem::render(bool black){
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glUseProgram(*shader);
    
    // Loop through all the particles in the system
    for (Particle particle : particles)
    {
        // Check if particle is alive
        if (particle.life > 0.0f)
        {
            // Update color in shader
            glUniform4f(glGetUniformLocation(*shader, "color"), particle.color.r, particle.color.g, particle.color.b, particle.color.a);
            // Update Quad
            quad->reset();
            
            // If note is black, the spread of the particles should be narrower
            if(black){
                quad->translate(notePosition.x + particle.position.x * 0.6f, notePosition.y + particle.position.y, notePosition.z + particle.position.z);
            }
            else{
                quad->translate(notePosition.x + particle.position.x, notePosition.y + particle.position.y, notePosition.z + particle.position.z);
            }

            // Render Quad
            quad->render();
        }
    }
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem::update(GLfloat dt, GLuint newParticles){
    // Add new particles
    for (GLuint i = 0; i < newParticles; i++)
    {
        int unusedParticle = firstUnusedParticle();
        respawnParticle(particles[unusedParticle]);
    }
    
    // Update all particles
    for (GLuint i = 0; i < numParticles; i++)
    {
        Particle &p = particles[i];
        p.life -= dt; // reduce life
        
        // Check if particle is alive. Then update.
        if (p.life > 0.0f)
        {
            p.position -= p.velocity * dt;
            p.color.a -= dt * 2.5;
        }
    }
}

void ParticleSystem::respawnParticle(Particle &particle){
    particle.life = 0.5f;
    
    GLfloat random = ((rand() % 100) - 50) / 500.0f;
    particle.position.x = sourcePosition.x + random;
    particle.position.y = sourcePosition.y;
    
    if(bonus){
        particle.color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
        
        random = rand() % 100;
        if(random > 40){
            particle.velocity = glm::vec3(0.0f, 1.5f, 0.0f);
        }
        else{
            random = (rand() % 360) * 0.01745f;
            particle.velocity = glm::vec3(cos(random) * 0.5f, sin(random) * 0.5f, 0.0f);
        }
    }
    else{
        particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        
        random = rand() % 100;
        if(random > 40){
            particle.velocity = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        else{
            random = (rand() % 360) * 0.01745f;
            particle.velocity = glm::vec3(cos(random) * 0.3f, sin(random) * 0.3f, 0.0f);
        }
    }
    
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint ParticleSystem::firstUnusedParticle()
{
    // First search from last used particle, this will usually return almost instantly
    for (GLuint i = lastUsedParticle; i < numParticles; i++){
        if (particles[i].life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // Otherwise, do a linear search
    for (GLuint i = 0; i < lastUsedParticle; i++){
        if (particles[i].life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleSystem::updateNotePosition(glm::vec3 position){
    notePosition = position;
}
