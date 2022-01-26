#include "Firefly.h"
#include "../nclgl/Light.h"

Firefly::Firefly(Mesh* cube, Light* light) {
	SceneNode* body = new  SceneNode(cube, Vector4(light->GetEmissColour().x + 0.5, light->GetEmissColour().y + 0.5, light->GetEmissColour().z + 0.5, light->GetEmissColour().w)); //(0.5f, 0.35f, 0.05f, 1));
	body->SetModelScale(Vector3(1, 1, 1));
	haveTexture = false;
	AddChild(body);
	l = light;
}

void Firefly::Update(float dt) {
	transform = Matrix4::Translation(l->GetPosition());
	SceneNode::Update(dt);
}