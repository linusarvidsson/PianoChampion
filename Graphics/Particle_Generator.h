/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
//#include "game_object.h"


// Represents a single particle and its state
struct Particle {
	glm::vec2 Position, Angle;
	glm::vec4 Color;
	GLfloat Life;

	Particle() : Position(0.0f), Angle(0.0f), Color(1.0f), Life(0.0f) { }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
	// Constructor
	ParticleGenerator(GLuint* shader, GLuint* texture, GLuint amount, GLint spawnFrequency);
	// Update all particles
	void ParticleGenerator::Update(GLfloat dt, glm::vec2 position, GLuint newParticles, GLboolean ParticlesActive);
	// Render all particles
	void Draw();
private:
	// State
	std::vector<Particle> particles;
	GLuint amount;
	GLint spawnFrequency;
	GLint trackSpawn;
	// Render state
	GLuint* shader;
	GLuint* texture;
	GLuint VAO;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	GLuint firstUnusedParticle();
	// Respawns particle
	void respawnParticle(Particle &particle, glm::vec2 position);
};

#endif
