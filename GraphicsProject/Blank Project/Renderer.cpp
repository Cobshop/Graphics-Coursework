#include "Renderer.h"
#include "array"
#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "math.h"

#define USE_MATH_DEFINES
#define  SHADOWSIZE  8192

const  int  LIGHT_NUM = 50;
const int  POST_PASSES = 10;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	cube = Mesh::LoadFromMeshFile("Cube.msh");
	quad = Mesh::GenerateQuad(true);
	tree = Mesh::LoadFromMeshFile("trunk.msh");
	house = Mesh::LoadFromMeshFile("MainHouse1.msh");
	heightMap = new HeightMap(TEXTUREDIR"heightmap_again.png");
	batMesh = Mesh::LoadFromMeshFile("Bat.msh");
	batAnim = new MeshAnimation("Bat.anm");
	batMaterial = new MeshMaterial("Role_T.mat");
	particles = new ParticleProducer();

	waterMap = SOIL_load_OGL_texture(TEXTUREDIR"wavesHeight.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterBump = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	treeTex = SOIL_load_OGL_texture(TEXTUREDIR"Bark1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	treeBump = SOIL_load_OGL_texture(TEXTUREDIR"bark1_N.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"Gravel.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"GravelBump-min.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	grassTex = SOIL_load_OGL_texture(TEXTUREDIR"Grass_dead-min.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	grassBump = SOIL_load_OGL_texture(TEXTUREDIR"GrassBump-min.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockTex = SOIL_load_OGL_texture(TEXTUREDIR"rock.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockBump = SOIL_load_OGL_texture(TEXTUREDIR"rockBump-min.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	houseTex = SOIL_load_OGL_texture(TEXTUREDIR"HouseTex2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	houseBump = SOIL_load_OGL_texture(TEXTUREDIR"HouseBump2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	batTex = SOIL_load_OGL_texture(TEXTUREDIR"Bat.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap2 = SOIL_load_OGL_cubemap(TEXTUREDIR"redsky_left.png", TEXTUREDIR"redsky_right.png", TEXTUREDIR"redsky_up.png", TEXTUREDIR"redsky_down.png",
		TEXTUREDIR"redsky_back.png", TEXTUREDIR"redsky_front.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 1);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"redsky_left.png", TEXTUREDIR"redsky_right.png", TEXTUREDIR"redsky_up_2.png", TEXTUREDIR"redsky_down.png",
		TEXTUREDIR"redsky_back.png", TEXTUREDIR"redsky_front.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 1);

	for (int i = 0; i < batMesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = batMaterial->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		batTextures.emplace_back(texID);
	}

	objectShader = new  Shader("SceneVertex.glsl", "bufferFragment.glsl");
	animatedShader = new  Shader("SkinningVertex.glsl", "bufferFragment.glsl");
	reflectShader = new  Shader("reflectVertex.glsl", "reflectFragment.glsl", "", "displaceTCS.glsl", "displaceTES.glsl");
	skyboxShader = new  Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	skyboxShader2 = new  Shader("skyboxVertex2.glsl", "skyboxFragment2.glsl");
	sceneShader = new Shader("BumpVertex.glsl", "bufferFragment.glsl");
	pointlightShader = new Shader("pointlightvert.glsl", "pointlightfrag.glsl");
	directionalLightShader = new Shader("directionallightvert.glsl", "pointlightfrag.glsl");
	combineShader = new Shader("combinevert.glsl", "combinefrag.glsl");
	particleShader = new Shader("particlevert.glsl", "particlefrag.glsl");
	shadowShader = new Shader("shadowVert.glsl", "shadowFrag.glsl");
	blurShader = new Shader("TexturedVertex.glsl", "processfrag.glsl");
	combineBlurShader = new Shader("TexturedVertex.glsl", "texturedfragment.glsl");

	if (!blurShader->LoadSuccess() || !animatedShader->LoadSuccess() || !reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !sceneShader->LoadSuccess() || !pointlightShader->LoadSuccess() || !combineShader->LoadSuccess() || !objectShader->LoadSuccess() || !shadowShader->LoadSuccess()) {
		return;
	}

	if (!rockTex || !rockBump || !earthTex || !earthBump || !cubeMap || !waterTex || !waterBump || !cubeMap2 || !grassTex || !grassBump || !treeTex) {
		return;
	}
	SetTextureRepeating(waterMap, true);
	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
	SetTextureRepeating(grassTex, true);
	SetTextureRepeating(grassBump, true);
	SetTextureRepeating(rockTex, true);
	SetTextureRepeating(rockBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(waterBump, true);
	SetTextureRepeating(treeTex, true);
	SetTextureRepeating(treeBump, true);
	SetTextureRepeating(houseTex, true);
	SetTextureRepeating(houseBump, true);
	SetTextureRepeating(batTex, true);

	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	camera = new Camera(-25.0f, 35.0f, 0.0f, Vector3(14920.0f, 3750.0f, 16530.0f));
	cDestination = Vector3(1503, 3750, 15528);
	vDestination = Vector3(1503, 3750, 15528);
	cVelocity = 300;
	vVelocity = 50;
	checkpoint = 0;
	control = false;
	sunStop = false;

	root = new SceneNode();
	for (int i = 0; i < 500; i++) {
		root->AddChild(new Tree(tree, treeTex, treeBump));
	}
	
	for (int i = 0; i < 66049; i += 7) {
		if(heightMap->vertices[i].y > heightmapSize.y * 0.85)
			seaLevelVertices.push_back(heightMap->vertices[i]);
	}
	
	for (vector <SceneNode*>::const_iterator j = root->GetChildIteratorStart(); j != root->GetChildIteratorEnd(); ++j) {
		int k = rand() % seaLevelVertices.size();
		(*j)->SetTransform(Matrix4::Translation(seaLevelVertices[k]));
		(*j)->SetTransform((*j)->GetTransform() * Matrix4::Rotation(rand()%360, Vector3(0, 1, 0)));
		seaLevelVertices.erase(seaLevelVertices.begin() + k);
	}

	pointLights = new Light[LIGHT_NUM];
	Sun = new Light(heightmapSize * Vector3(1.0f, 4.0f, 0.0f) + Vector3(50,0,50), Vector3(0.0f, 0.0f, 0.0f), Vector4(1, 0.75f, 0.75f, 1), Vector4(1, 0.75f, 0.75f, 1), heightmapSize.x * 0.5, -1.0f);

	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light& l = pointLights[i];
		int k = rand() % seaLevelVertices.size();
		l.SetPosition(Vector3(seaLevelVertices[k].x, seaLevelVertices[k].y + 50, seaLevelVertices[k].z));

		l.SetSpecColour(Vector4(0.36f, 0.25f, 0.83f, 1));
		l.SetEmissColour(Vector4(0.36f, 0.25f, 0.83f, 1));
		l.SetRadius(100.0f);
		l.SetDirection(Vector3(1, 1, 1));
		root->AddChild(new Firefly(sphere, &l));
	}

	for (int i = 0; i < 25; i++) {
		root->AddChild(new Bat(batMesh, batAnim, batMaterial, 5));
		pathPlaces.push_back(i);
	}

	for (vector <SceneNode*>::const_iterator j = root->GetChildIteratorStart() + 500 + LIGHT_NUM; j != root->GetChildIteratorEnd(); ++j) {
		int k = rand() % seaLevelVertices.size();
		(*j)->SetTransform(Matrix4::Translation(seaLevelVertices[k]));
	}

	SceneNode* houseNode = new SceneNode();
	houseNode->SetMesh(house);
	houseNode->SetTexture(houseTex);
	houseNode->SetBump(houseBump);
	root->AddChild(houseNode);
	houseNode->SetTransform(Matrix4::Translation(Vector3(2700,1200,2300)));
	houseNode->SetTransform(houseNode->GetTransform()* Matrix4::Scale(Vector3(30, 30, 30))*Matrix4::Rotation(-225, Vector3(0, 1, 0)));
	houseNode->AddChild(new Bat(batMesh, batAnim, batMaterial, 1));
	//(*(houseNode->GetChildIteratorStart()))->SetTransform(Matrix4::Translation(Vector3(1, 1, 1)));
	

	projMatrix = Matrix4::Perspective(1.0f, 17000.0f, (float)width / (float)height, 45.0f);

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	glGenFramebuffers(1, &skyboxFBO);
	glGenFramebuffers(1, &shadowFBO);
	glGenFramebuffers(1, &blurFBO);

	GLenum  buffers[2] = {
		GL_COLOR_ATTACHMENT0 ,
		GL_COLOR_ATTACHMENT1
	};

	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(skyboxColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);
	GenerateScreenTexture(blurColourTex);
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
	

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, skyboxColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, skyboxNormalTex, 0);
	glDrawBuffers(2, buffers);

	
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	sunSpeed = 50;
	ToD = 0.0f;
	FFtimer = 0.0f;
	currentFrame = 0;
	frameTime = 0.0f;
	pathTimer = 0;
	init = true;
	daytime = true;
}

Renderer::~Renderer(void) {
	delete root;
	delete camera;
	delete heightMap;
	delete quad;
	delete sphere;
	delete tree;
	delete batMesh;
	delete batAnim;
	delete batMaterial;
	delete reflectShader;
	delete skyboxShader;
	delete sceneShader;
	delete combineShader;
	delete pointlightShader;
	delete directionalLightShader;
	delete shadowShader;
	delete blurShader;
	delete[] pointLights;
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(1, &shadowTex);
	glDeleteTextures(1, &blurColourTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	glDeleteFramebuffers(1, &skyboxFBO);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &blurFBO);

}

void Renderer::GenerateScreenTexture(GLuint& into, bool  depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint  format = depth ? GL_DEPTH_COMPONENT32 : GL_RGBA8;
	GLuint  type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::UpdateScene(float dt) {
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_E)) {
		control = !control;
	}
	if(control)
		camera->UpdateCamera(dt);
	else
		AutomateCamera(dt);

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R))
		sunStop = !sunStop;

	if (sunStop)
		sunSpeed = 0;
	else
		sunSpeed = 50;

	waterRotate += 0.00002f;
	waterCycle += 0.000025f;
	root->Update(dt);
	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % batAnim->GetFrameCount();
		frameTime += 1.0f / batAnim->GetFrameRate();
	}
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	UpdateSun();
	UpdateLights();
	UpdateBat(dt);
	
	for (vector <SceneNode*>::const_iterator j = root->GetChildIteratorEnd()-1; j != root->GetChildIteratorEnd(); ++j) {
		particles->Update(dt, (*j), 2, Vector2(50, 200));
	}
}

void Renderer::UpdateSun() {
	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	if (Sun->GetPosition().x >= heightmapSize.x-50 && Sun->GetPosition().z <= 50) {
		daytime = true;
	}
	if (Sun->GetPosition().x <= 50 && Sun->GetPosition().z >= heightmapSize.z-50)
		daytime = false;

	ToD += (PI * sunSpeed) / (heightmapSize.x);

	if (daytime) {
		Sun->SetPosition(Vector3(Sun->GetPosition().x - sunSpeed, heightmapSize.y * 4, Sun->GetPosition().z + sunSpeed));
		Sun->SetEmissColour(Vector4(abs(sin(ToD)), abs(sin(ToD)), abs(sin(ToD)), 1));
		Sun->SetSpecColour(Vector4(abs(sin(ToD)), abs(sin(ToD)), abs(sin(ToD)), 1));
	}
	else {
		Sun->SetPosition(Vector3(Sun->GetPosition().x + sunSpeed, heightmapSize.y * 4, Sun->GetPosition().z - sunSpeed));
		Sun->SetEmissColour(Vector4(0, 0, 0, 0));
		Sun->SetSpecColour(Vector4(0, 0, 0, 0));
	}
}

void Renderer::UpdateLights() {
	FFtimer++;
	for (int i = 0; i < LIGHT_NUM; ++i) {
		pointLights[i].SetPosition(Vector3(pointLights[i].GetPosition().x + sin(FFtimer/50), pointLights[i].GetPosition().y, pointLights[i].GetPosition().z + cos(FFtimer/50)));
	}
	if (FFtimer >= 100 * PI)
		FFtimer -= 100 * PI;
}

void Renderer::UpdateBat(float dt) {
	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	int pathPlace = 0;
	pathTimer += 0.3 * dt;
	for (vector <SceneNode*>::const_iterator j = root->GetChildIteratorStart() + 500 + LIGHT_NUM; j != root->GetChildIteratorEnd() - 1; ++j) {
		(*j)->SetTransform(Matrix4::Translation(Vector3((cos(pathPlaces[pathPlace] + pathTimer) * heightmapSize.x/2), heightmapSize.y + 200 * sin(pathPlaces[pathPlace] + pathTimer),(heightmapSize.z/6 * sin(pathPlaces[pathPlace] + pathTimer)))));
		(*j)->SetTransform(Matrix4::Translation(Vector3(heightmapSize.x/2, 0, heightmapSize.z/2 + 100))*Matrix4::Rotation(500*pathPlaces[pathPlace], Vector3(0, 1, 0))* (*j)->GetTransform() * Matrix4::Rotation(RadToDeg(pathPlaces[pathPlace] + pathTimer) + 180, Vector3(0, 1, 0)));
		pathPlace++;
	}
	if (pathTimer >= 2 * PI)
		pathTimer -= 2 * PI;
	
	(*((*(root->GetChildIteratorEnd() - 1))->GetChildIteratorStart()))->SetTransform(Matrix4::Translation(Vector3(30 * sin(3 *pathTimer), -10, 30 * cos(3 *pathTimer))) * Matrix4::Rotation(RadToDeg(3 * pathTimer) + 180, Vector3(0, 1, 0)));
}

void Renderer::AutomateCamera(float dt) {
	cDestination = Destinations[checkpoint];
	Vector3 Direction = (cDestination - camera->GetPosition()).Normalised();
	camera->SetPosition(camera->GetPosition() + Vector3(dt *cVelocity * Direction.x, dt * cVelocity * Direction.y, dt * cVelocity * Direction.z));

	vDestination = viewDirections[checkpoint];
	Vector3 vDirection = (vDestination - camera->GetPosition()).Normalised();
	float yawAim = 0;

	if (vDirection.x > 0)
		yawAim = -RadToDeg(acos(-vDirection.z));
	else if (vDirection.x < 0)
		yawAim = -(360 - RadToDeg(acos(-vDirection.z)));
	
	camera->SetYaw(yawAim);

	if ((abs((cDestination - camera->GetPosition()).x) < 5) && (abs((cDestination - camera->GetPosition()).y) < 5) && (abs((cDestination - camera->GetPosition()).z) < 5)) {
		checkpoint++;

	}
		
	if (checkpoint >= 4)
		checkpoint = 0;
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawShadowScene();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 17000.0f, (float)width / (float)height, 45.0f);
	DrawHeightmap();
	DrawNode(root);
	DrawWater();
	DrawPointLights();
	DrawDirectionalLights();
	//DrawBlur();
	//CombineBlur();
	CombineBuffers();
	//DrawParticles();
	DrawSkybox();
}

void Renderer::DrawSkybox() {
	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, bufferFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, 1920, 1080, 0, 0, 1920, 1080, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	viewMatrix = camera->BuildViewMatrix();
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	glUniform1i(glGetUniformLocation(skyboxShader->GetProgram(), "cubeTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//quad->Draw();
}

void Renderer::DrawHeightmap() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(sceneShader);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "useTexture"), 2);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "grassTex"), 3);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "grassBumpTex"), 4);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "rockTex"), 5);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "rockBumpTex"), 6);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, grassTex);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, grassBump);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, rockTex);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, rockBump);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();
	
}

void Renderer::DrawWater() {
	BindShader(reflectShader);
	quad->type = GL_PATCHES;
	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "heightMap"), 3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterBump);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, waterMap);

	Vector3 hSize = heightMap->GetHeightmapSize();
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	modelMatrix = Matrix4::Translation(Vector3(hSize.x * 0.5f, hSize.y * 0.3f, hSize.z * 0.5f)) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(-90, Vector3(1, 0, 0));
	textureMatrix = Matrix4::Translation(Vector3(0.5 * waterCycle, 0.5 * waterCycle, 0.5 * waterCycle)) * Matrix4::Scale(Vector3(100, 100, 100));

	UpdateShaderMatrices();

	quad->Draw();
	quad->type = GL_TRIANGLE_STRIP;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPointLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointlightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "shadowTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glUniform3fv(glGetUniformLocation(pointlightShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform2f(glGetUniformLocation(pointlightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4  invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(pointlightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	UpdateShaderMatrices();
	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light& l = pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}
}

void Renderer::DrawDirectionalLights() {
	BindShader(directionalLightShader);
	glUniform1i(glGetUniformLocation(directionalLightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(directionalLightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform1i(glGetUniformLocation(directionalLightShader->GetProgram(), "shadowTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glUniform3fv(glGetUniformLocation(directionalLightShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform2f(glGetUniformLocation(directionalLightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4  invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(directionalLightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	modelMatrix = Matrix4::Scale(Vector3(2,2,2));
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();

	UpdateShaderMatrices();
	
	SetShaderLight(*Sun);
	cube->Draw();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineBuffers() {
	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	quad->Draw();
}

void Renderer::DrawNode(SceneNode* n) {
	BindShader(objectShader);
	if (n->GetMesh() && n->animated == false) {
		int texture = 0;
		if (n->GetTexture() == 0)
			texture = 0;
		else
			texture = 1;
		glUniform1i(glGetUniformLocation(objectShader->GetProgram(), "diffuseTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, n->GetTexture());
		glUniform1i(glGetUniformLocation(objectShader->GetProgram(), "bumpTex"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, n->GetBump());
		UpdateShaderMatrices();
		Matrix4  model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(objectShader->GetProgram(), "modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(objectShader->GetProgram(), "nodeColour"), 1, (float*)& n->GetColour());

		glUniform1i(glGetUniformLocation(objectShader->GetProgram(), "useTexture"), texture);
		n->Draw(*this);
	}
	else if (n->GetMesh() && n->animated == true) {
		BindShader(animatedShader);
		glUniform1i(glGetUniformLocation(animatedShader->GetProgram(), "diffuseTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, batTex);
		glUniform1i(glGetUniformLocation(animatedShader->GetProgram(), "bumpTex"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, batTex);

		UpdateShaderMatrices();

		Matrix4  model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(animatedShader->GetProgram(), "modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(animatedShader->GetProgram(), "nodeColour"), 1, (float*)& n->GetColour());

		glUniform1i(glGetUniformLocation(animatedShader->GetProgram(), "useTexture"), 1);

		vector<Matrix4> frameMatrices;

		const Matrix4* invBindPose = n->GetMesh()->GetInverseBindPose();
		const Matrix4* frameData = n->GetAnimation()->GetJointData(currentFrame);

		for (unsigned int i = 0; i < n->GetMesh()->GetJointCount(); ++i) {
			frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
		}

		int j = glGetUniformLocation(animatedShader->GetProgram(), "joints");
		glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

		for (int i = 0; i < n->GetMesh()->GetSubMeshCount(); ++i) {
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, batTextures[i]);
			n->GetMesh()->DrawSubMesh(i);
		}
	}

	for (vector <SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
	
}

void Renderer::DrawShadowNode(SceneNode* n) {
	if (n->GetMesh()) {
		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		UpdateShaderMatrices();
		n->Draw(*this);
	}
	for (vector <SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawShadowNode(*i);
	}
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	BindShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(Sun->GetPosition(), Vector3(heightMap->GetHeightmapSize().x * 0.5, 0, heightMap->GetHeightmapSize().z * 0.5));
	projMatrix = Matrix4::Perspective(100, heightMap->GetHeightmapSize().x * 3 , 1, 135);
	shadowMatrix = projMatrix * viewMatrix;

	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	heightMap->Draw();
	DrawShadowNode(root);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawParticles() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	BindShader(particleShader);

	glUniform1i(glGetUniformLocation(particleShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, batTex);

	UpdateShaderMatrices();
	particles->Draw(particleShader, quad);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::DrawBlur() {
	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurColourTex, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	BindShader(blurShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "sceneTex"), 0);

	for (int i = 0; i < POST_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, blurColourTex, 0);
		glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "isVertical"), 0);
		
		glBindTexture(GL_TEXTURE_2D, bufferColourTex);
		quad->Draw();
		
		glUniform1i(glGetUniformLocation(blurShader->GetProgram(), "isVertical"), 1);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);
		glBindTexture(GL_TEXTURE_2D, blurColourTex);
		quad->Draw();

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::CombineBlur() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(combineBlurShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);
	glUniform1i(glGetUniformLocation(combineBlurShader->GetProgram(), "diffuseTex"), 0);
	quad->Draw();
	
}