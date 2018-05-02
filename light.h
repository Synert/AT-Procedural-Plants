#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

using namespace DirectX;

class Light
{
public:
	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};

	Light();
	~Light();

	void SetLightAmbient(XMFLOAT4 set);
	void SetLightDiffuse(XMFLOAT4 set);
	void SetLightDirection(XMFLOAT3 set);

	XMFLOAT4 GetLightAmbient();
	XMFLOAT4 GetLightDiffuse();
	XMFLOAT3 GetLightDirection();

	LightBufferType m_light;

private:
	//LightBufferType m_light;
};