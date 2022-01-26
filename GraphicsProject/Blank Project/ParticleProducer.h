#include "Particle.h"
#include "nclgl/SceneNode.h"
#include "nclgl/Mesh.h"

#pragma once
class ParticleProducer
{
public:
	ParticleProducer();
	~ParticleProducer(void) {};

	void Update(float dt, SceneNode* producer, int newParticles, Vector2 offset);
	void Draw(Shader* shader, Mesh* mesh);
	int firstParticle();
	void respawnParticle(Particle* particle, SceneNode* producer, Vector2 offset);

protected:
	GLuint VAO;
	int lastParticle = 0;
	int maxParticles = 500;
	std::vector<Particle*> particles;
};

