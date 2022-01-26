#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class Camera {
public:
	Camera(void) {
		roll = 0.0f;
		yaw = 0.0f;
		pitch = 0.0f;
		spood = 1.0f;
		rolling = false;
	}

	Camera(float pitch, float yaw, float roll, Vector3 position) {
		spood = 50.0f;
		this->rolling = false;
		this->roll = roll;
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
	}

	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetRoll() const { return roll; }
	void SetRoll(float r) { roll = r; }

	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }

	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }

protected:
	bool rolling;
	float spood;
	float roll;
	float yaw;
	float pitch;
	Vector3 position;
};

