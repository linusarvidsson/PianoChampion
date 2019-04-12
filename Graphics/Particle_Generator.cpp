/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "particle_generator.h"

#include <iostream>

ParticleGenerator::ParticleGenerator(GLuint* shader, GLuint* texture, GLuint amount, GLint spawnFrequency)
	: shader(shader), texture(texture), amount(amount), spawnFrequency(spawnFrequency), trackSpawn(0)
{
	this->init();
}

void ParticleGenerator::Update(GLfloat dt, glm::vec2 position, GLuint newParticles, GLboolean ParticlesActive)
{
	if (trackSpawn == spawnFrequency && ParticlesActive)
	{
		// Add new particles 
		for (GLuint i = 0; i < newParticles; ++i)
		{
			int unusedParticle = this->firstUnusedParticle();
			this->respawnParticle(this->particles[unusedParticle], position);
		}
		trackSpawn = 0;
	}
	else if (ParticlesActive)
		trackSpawn++;

	// Update all particles
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// particle is alive, thus update
			// GLfloat x = 1/(p.Angle[0] + p.Angle[1])
			// p.Position = { p.Position[0] - x*p.Angle[0], p.Position[1] - x*p.Angle[1] };
			p.Position = { p.Position[0] - 1.0f*p.Angle[0], p.Position[1] - 1.0f*p.Angle[1] };
		};
		p.Color.a -= dt *2.5;
	}
}


// Render all particles
void ParticleGenerator::Draw()
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glUseProgram(*shader);  
	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			glUniform2f(glGetUniformLocation(*shader, "offset"), particle.Position.x, particle.Position.y);
			glUniform4f(glGetUniformLocation(*shader, "color"), particle.Color.x, particle.Color.y, particle.Color.z, particle.Color.w);
			glBindTexture(GL_TEXTURE_2D, *texture);
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (GLuint i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, glm::vec2 position)
{
	GLfloat randomX = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomY = ((rand() % 100) - 50) / 10.0f;
	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.Position = { position[0] + randomX , position[1] + randomY };
	//particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Color = glm::vec4(0.5f, 0.3f, 0.0f, 1.0f);
	particle.Life = 1.0f;
	GLfloat randomAngle = ((rand() % 100) - 50) / 80.0f;
	//std::cout << randomAngle << std::endl;
	GLfloat angleY = 1 - abs(randomAngle);
	particle.Angle = { randomAngle, angleY };
}