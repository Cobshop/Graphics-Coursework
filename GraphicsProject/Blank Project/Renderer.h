#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "Tree.h"
#include "Firefly.h"
#include "Bat.h"
#include "ParticleProducer.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/Frustrum.h"

class HeightMap;
class Camera;
class Light;
class Shader;
class SceneNode;
class Tree;

class  Renderer : public  OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawNode(SceneNode* n);
	void DrawPointLights();
	void DrawDirectionalLights();
	void DrawParticles();
	void DrawShadowScene();
	void DrawShadowNode(SceneNode* n);
	void CombineBuffers();
	void DrawBlur();
	void CombineBlur();

	void GenerateScreenTexture(GLuint& into, bool  depth = false);

	void UpdateSun();
	void UpdateLights();
	void UpdateBat(float dt);
	void AutomateCamera(float dt);

	Shader* objectShader;
	Shader* animatedShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* skyboxShader2;
	Shader* sceneShader;
	Shader* pointlightShader;
	Shader* directionalLightShader;
	Shader* combineShader;
	Shader* combineBlurShader;
	Shader* particleShader;
	Shader* shadowShader;
	Shader* blurShader;

	GLuint bufferFBO;
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;
	
	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;

	GLuint skyboxFBO;
	GLuint skyboxColourTex;
	GLuint skyboxNormalTex;

	GLuint shadowFBO;
	GLuint shadowTex;

	GLuint blurFBO;
	GLuint blurColourTex;

	Frustum frameFrustum;

	HeightMap* heightMap;
	Mesh* sphere;
	Mesh* cube;
	Mesh* quad;
	Mesh* tree;
	Mesh* house;
	Mesh* batMesh;
	MeshAnimation* batAnim;
	MeshMaterial* batMaterial;

	Camera* camera;
	Light* pointLights;
	Light* Sun;

	SceneNode* root;

	GLuint waterMap;
	GLuint cubeMap;
	GLuint cubeMap2;
	GLuint waterTex;
	GLuint waterBump;
	GLuint treeTex;
	GLuint treeBump;
	GLuint earthTex;
	GLuint earthBump;
	GLuint grassTex;
	GLuint grassBump;
	GLuint rockTex;
	GLuint rockBump;
	GLuint houseTex;
	GLuint houseBump;
	GLuint batTex;
	vector<GLuint> batTextures;

	float col;
	float waterRotate;
	float waterCycle;
	int sunSpeed;
	bool daytime;
	float ToD;
	float FFtimer;
	int currentFrame;
	float frameTime;
	float pathTimer;
	float cVelocity;
	float vVelocity;
	int checkpoint;
	bool control;
	bool sunStop;
	Vector3 cDestination;
	Vector3 vDestination;
	Vector3 Destinations[4] = { Vector3(1503, 3750, 15528), Vector3(4267, 1555, 4310), Vector3(14037, 1555, 5160), Vector3(14920.0f, 3750.0f, 16530.0f) };
	Vector3 viewDirections[4] = { Vector3(8826, 466, 9331), Vector3(2684, 1739, 2358), Vector3(14037, 1555, 5160), Vector3(2684, 1739, 2358) };

	vector<float> pathPlaces;
	vector<Vector3> seaLevelVertices;
	vector<SceneNode> bats;

	ParticleProducer* particles;
};