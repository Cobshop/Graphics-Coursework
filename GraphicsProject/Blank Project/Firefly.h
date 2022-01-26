#pragma once
#include "nclgl/SceneNode.h"

class Firefly : public  SceneNode {
public:
	Firefly(Mesh* cube, Light* light);
	~Firefly(void) {};
	void Update(float dt) override;

	Light* l;
};


