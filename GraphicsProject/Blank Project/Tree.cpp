#include "Tree.h"

Tree::Tree(Mesh* cube, GLuint texture, GLuint bump) {
	SceneNode* body = new  SceneNode(cube, Vector4(1, 1, 1, 1)); //(0.5f, 0.35f, 0.05f, 1));
	body->SetModelScale(Vector3(10, 10, 10));
	body->SetTransform(Matrix4::Translation(Vector3(0, -120, 0)));
	body->SetTexture(texture);
	haveTexture = true;
	body->SetBump(bump);
	AddChild(body);

	/*Mesh* sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	SceneNode* head = new  SceneNode(sphere, Vector4(0, 1, 0, 1));
	head->SetModelScale(Vector3(50, 50, 50));
	head->SetTransform(Matrix4::Translation(Vector3(0, 300, 0)));
	AddChild(head);*/
}

void Tree::Update(float dt) {
	SceneNode::Update(dt);
}