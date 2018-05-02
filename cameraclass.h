#pragma once

#include <directxmath.h>
#include <AntTweakBar.h>
using namespace DirectX;

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

	void SetTweakBar(TwBar* bar);

	XMVECTOR GetUp();
	XMVECTOR GetForward();
	XMVECTOR GetRight();

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
	XMVECTOR m_up, m_forward, m_right;
};
