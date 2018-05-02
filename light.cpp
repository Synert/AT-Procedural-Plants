#include "light.h"

Light::Light()
{

}

Light::~Light()
{

}

void Light::SetLightAmbient(XMFLOAT4 set)
{
	m_light.ambientColor = set;
}

void Light::SetLightDiffuse(XMFLOAT4 set)
{
	m_light.diffuseColor = set;
}

void Light::SetLightDirection(XMFLOAT3 set)
{
	m_light.lightDirection = set;
}

XMFLOAT4 Light::GetLightAmbient()
{
	return m_light.ambientColor;
}

XMFLOAT4 Light::GetLightDiffuse()
{
	return m_light.diffuseColor;
}

XMFLOAT3 Light::GetLightDirection()
{
	return m_light.lightDirection;
}