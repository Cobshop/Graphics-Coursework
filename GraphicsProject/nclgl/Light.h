#pragma  once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
	Light() {} 
	Light(const Light&) = default;
	Light(Light&&) = default;
	Light& operator=(const Light&) = default;
	Light& operator=(Light&&) = default;
	Light(const  Vector3& position , const  Vector3& direction, const  Vector4& specColour , const Vector4& emissColour, float  radius, float cutoff ){
		this->spotDirection = direction;
		this ->position = position;
		this ->specColour    = specColour;
		this->emissColour = emissColour;
		this ->radius    = radius;
		this->cutoff = cutoff;
	}

	~Light(void ){};

	Vector3 GetDirection() const { return  spotDirection; }
	void SetDirection(const Vector3& val) { spotDirection = val; }

	Vector3 GetPosition() const { return  position; }
	void SetPosition(const Vector3& val) { position = val;}

	float GetRadius() const { return  radius; }
	void SetRadius(float  val) { radius = val; }

	float GetCutOff() const { return  cutoff; }
	void SetCutOff(float  val) { cutoff = val; }

	Vector4 GetSpecColour() const { return  specColour; }
	void SetSpecColour(const  Vector4& val) { specColour = val; }

	Vector4 GetEmissColour() const { return  emissColour; }
	void SetEmissColour(const  Vector4& val) { emissColour = val; }

protected:
	float cutoff;
	Vector3 spotDirection;
	Vector3 position;
	float radius;
	Vector4 specColour;
	Vector4 emissColour;
};

