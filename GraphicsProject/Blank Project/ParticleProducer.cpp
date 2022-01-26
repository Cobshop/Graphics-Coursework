#include "ParticleProducer.h"

ParticleProducer::ParticleProducer(){
	for (int i = 0; i < maxParticles; i++)
		particles.push_back(&Particle());

	GLuint VBO;
	float particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

void ParticleProducer::Update(float dt, SceneNode* producer, int newParticles, Vector2 offset) {
	for (int i = 0; i < newParticles; i++) {
		int noParticle = firstParticle();
		respawnParticle(particles[noParticle], producer, offset);
	}

	for (int i = 0; i < maxParticles; i++) {
		Particle* p = particles[i];
		p->life -= dt;
		if (p->life > 0.0f) {
			p->Position.x += p->Velocity.x * dt;
			p->Position.y += p->Velocity.y * dt;
			p->Colour.w -= dt * 0.025f;
		}
	}
}

int ParticleProducer::firstParticle() {
	for (int i = lastParticle; i < maxParticles; i++) {
		if (particles[i]->life <= 0.0f) {
			lastParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastParticle; i++) {
		if (particles[i]->life <= 0.0f) {
			lastParticle = i;
			return i;
		}
	}

	lastParticle = 0;
	return 0;
}

void ParticleProducer::respawnParticle(Particle* particle, SceneNode* producer, Vector2 offset) {
	float k = ((rand() % 100) - 50) / 10.0f;
	float Colour = 0.5 + ((rand() % 100) / 100.0f);
	particle->Position.x = (producer->GetWorldTransform() * Vector4(1,1,1,1)).x + k + offset.x;
	particle->Position.y = (producer->GetWorldTransform() * Vector4(1, 1, 1, 1)).y + k + offset.y;
	particle->Colour = Vector4(Colour, Colour, Colour, 1.0f);
	particle->life = 1.0f;
	particle->Velocity = Vector2(50, 50);
}

void ParticleProducer::Draw(Shader* shader, Mesh* mesh) {
	for (Particle* particle : particles) {
		if (particle->life > 0.0f) {
			glUniform2fv(glGetUniformLocation(shader->GetProgram(), "offset"), 1, (float*)& particle->Position);
			glUniform4fv(glGetUniformLocation(shader->GetProgram(), "colour"), 1, (float*)& particle->Colour);
			//mesh->Draw();
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
}