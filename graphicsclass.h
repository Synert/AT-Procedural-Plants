#pragma once
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"
#include "colors.h"
#include "textureshaderclass.h"
#include "light.h"

#include <chrono>
#include <fstream>
#include <list>
#include <vector>
#include <AntTweakBar.h>

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false; //evil
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

struct Transform
{
	Transform() {};
	Transform(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rot, XMFLOAT4 _color):Transform()
	{
		pos = _pos;
		scale = _scale;
		rot = _rot;
		color = _color;
	}
	XMFLOAT3 pos;
	XMFLOAT3 scale;
	XMFLOAT3 rot;
	XMFLOAT4 color;
};

struct Parameters
{
	float upperBound;
	float lowerBound;
	int segments;
	int rings;
	float startWidth;
	float endWidth;
	float segHeight;

	float upperBoundX;
	float lowerBoundX;

	float upperBoundY;
	float lowerBoundY;

	float upperBoundZ;
	float lowerBoundZ;

	int recursion;
};

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(CameraClass* m_Camera, TwBar* bar);
	void SetTweakBar(TwBar* bar);

	//generation functions
	void CreateCube(Transform transform);
	void CreateSphere(Transform transform);
	void CreateTriangle(Transform transform);
	void CreateLeaves(Transform transform, Parameters param);
	void CreateMoreLeaves(Transform transform, Parameters param);
	void CreateRose(Transform transform, Parameters param);
	Transform CreateTrunk(Transform transform, Parameters param, int recursion, bool leaves, Parameters leafParam);
	void CreateBush(Transform transform, Parameters param);
	void CreateTree(Transform transform, Parameters param, Parameters leafParam);

	void PopModel(bool clearAll);
	void ExportModel();

	Transform new_transform;

	Parameters trunk_param;
	Parameters leaf_param;
	Parameters rose_param;

	TexParam tex_param;
	TexParam leaf_tex_param;
	TexParam trunk_tex_param;
	TexParam rose_tex_param;

	string export_path;

private:
	bool Render(CameraClass* m_Camera, TwBar* bar);

private:
	D3DClass* m_Direct3D;
	ModelClass* m_Model;
	std::vector<ModelClass*> m_objects;
	ColorShaderClass* m_ColorShader;
	TextureShaderClass* m_TextureShader;
	Light* m_light;
};