#include "../nclgl/OGLRenderer.h"

#pragma once
class Particle
{
public:
	Particle();
	~Particle(void) {};

	Vector2 Position, Velocity;
	Vector4 Colour;
	float life;
protected:
	
};

