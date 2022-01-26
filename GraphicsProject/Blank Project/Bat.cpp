#include "Bat.h"

Bat::Bat(Mesh* cube, MeshAnimation* anim, MeshMaterial* material, int scale) {
	SceneNode* body = new  SceneNode(cube, Vector4(1,1,1,1));
	body->SetModelScale(Vector3(scale, scale, scale));
	body->SetAnimation(anim);
	body->SetMaterial(material);
	body->animated = true;
	AddChild(body);
}
