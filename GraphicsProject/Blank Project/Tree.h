#pragma once
#include "nclgl/SceneNode.h"

class Tree : public  SceneNode {
public:
	Tree(Mesh* cube, GLuint texture, GLuint bump);
	~Tree(void) {};
	void Update(float dt) override;
protected:
};

