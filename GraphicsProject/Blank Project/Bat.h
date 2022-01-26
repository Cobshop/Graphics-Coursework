#pragma once
#include "nclgl/SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"

class HeightMap;

class Bat : public  SceneNode {
public:
	Bat(Mesh* cube, MeshAnimation* anim, MeshMaterial* material, int scale);
	~Bat(void) {};
protected:
};

