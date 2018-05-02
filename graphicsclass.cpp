#include "graphicsclass.h"
#include <stdlib.h>
#include <time.h>
#include <AntTweakBar.h>

// ------------------
// Maths functions
// ------------------

//float shorthands
const XMFLOAT3 one = XMFLOAT3(1.0f, 1.0f, 1.0f);
const XMFLOAT3 half = XMFLOAT3(0.5f, 0.5f, 0.5f);
const XMFLOAT3 quarter = XMFLOAT3(0.25f, 0.25f, 0.25f);
const XMFLOAT3 zero = XMFLOAT3(0.0f, 0.0f, 0.0f);

XMFLOAT3 AddFloat3(XMFLOAT3 a, XMFLOAT3 b)
{
	XMFLOAT3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;

	return result;
}

XMFLOAT3 TakeFloat3(XMFLOAT3 a, XMFLOAT3 b)
{
	XMFLOAT3 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;

	return result;
}

XMFLOAT3 MultFloat3(XMFLOAT3 a, XMFLOAT3 b)
{
	XMFLOAT3 result;
	result.x = a.x * b.x;
	result.y = a.y * b.y;
	result.z = a.z * b.z;

	return result;
}

XMFLOAT3 ScalarFloat3(XMFLOAT3 a, float b)
{
	XMFLOAT3 result = a;
	result.x *= b;
	result.y *= b;
	result.z *= b;

	return result;
}

XMFLOAT4 ScalarFloat4(XMFLOAT4 a, float b)
{
	XMFLOAT4 result = a;
	result.x *= b;
	result.y *= b;
	result.z *= b;

	return result;
}

//https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.geometric.xmvector3normalize(v=vs.85).aspx
XMVECTOR XMVector3NormalizeRobust(FXMVECTOR V)
{
	// Compute the maximum absolute value component.
	XMVECTOR vAbs = XMVectorAbs(V);
	XMVECTOR max0 = XMVectorSplatX(vAbs);
	XMVECTOR max1 = XMVectorSplatY(vAbs);
	XMVECTOR max2 = XMVectorSplatZ(vAbs);
	max0 = XMVectorMax(max0, max1);
	max0 = XMVectorMax(max0, max2);

	// Divide by the maximum absolute component.
	XMVECTOR normalized = XMVectorDivide(V, max0);

	// Set to zero when the original length is zero.
	XMVECTOR mask = XMVectorNotEqual(g_XMZero, max0);
	normalized = XMVectorAndInt(normalized, mask);

	XMVECTOR t0 = XMVector3LengthSq(normalized);
	XMVECTOR length = XMVectorSqrt(t0);

	// Divide by the length to normalize.
	normalized = XMVectorDivide(normalized, length);

	// Set to zero when the original length is zero or infinity.  In the
	// latter case, this is considered to be an unexpected condition.
	normalized = XMVectorAndInt(normalized, mask);
	return normalized;
}

XMFLOAT3 ComputeNormal(XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 c)
{
	//first, convert all of these to XMVECTORs

	XMVECTOR aVec = XMLoadFloat3(&a);
	XMVECTOR bVec = XMLoadFloat3(&b);
	XMVECTOR cVec = XMLoadFloat3(&c);

	XMVECTOR resultVec = XMVector3NormalizeRobust(XMVector3Cross(bVec - aVec, cVec - aVec));

	XMFLOAT3 result;

	XMStoreFloat3(&result, resultVec);

	return result;
}

float RandFloat(float min, float max)
{
	float result = min;
	float dif = max - min;

	float mult = (float)(rand() % 100) / 100.0f;

	result += dif * mult;

	return result;
}

int RandInt(int min, int max)
{
	int result = min;
	int dif = max - min;

	result += (rand() % dif);

	return result;
}

GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
	m_Model = 0;
	m_ColorShader = 0;
	m_TextureShader = 0;

	new_transform.scale = one;
	new_transform.rot = zero;
	new_transform.pos = zero;
	new_transform.color = COL_WHITE;

	trunk_param.rings = 6;
	trunk_param.segments = 6;
	trunk_param.startWidth = 1.0f;
	trunk_param.endWidth = 0.2f;
	trunk_param.segHeight = 1.0f;
	trunk_param.upperBound = 3.0f;
	trunk_param.lowerBound = -3.0f;
	trunk_param.recursion = 3;

	leaf_param.rings = 5;
	leaf_param.segments = 8;
	leaf_param.startWidth = 0.0f;
	leaf_param.segHeight = 1.0f;
	leaf_param.lowerBound = 0.0f;
	leaf_param.upperBound = 5.0f;
	leaf_param.lowerBoundX = 0.25f;
	leaf_param.lowerBoundZ = 0.25f;
	leaf_param.upperBoundX = 0.5f;
	leaf_param.upperBoundY = 0.5f;
	leaf_param.upperBoundZ = 0.5f;

	tex_param.type = GenType::NONE;
	leaf_tex_param.type = GenType::LEAF;
	trunk_tex_param.type = GenType::BARK;
	rose_tex_param.type = GenType::ROSE;

	leaf_tex_param.baseCol = COL_LEAFGREEN;
	leaf_tex_param.baseVar = XMFLOAT4(5, 15, 12, 0);
	leaf_tex_param.altCol = COL_DARKTURQUOISE;
	leaf_tex_param.altVar = XMFLOAT4(20, 20, 20, 0);
	leaf_tex_param.thickness = 6;

	trunk_tex_param.baseCol = COL_TRUNKBROWN;
	trunk_tex_param.baseVar = XMFLOAT4(5, 15, 12, 0);
	trunk_tex_param.altCol = COL_LEAFGREEN;
	trunk_tex_param.altVar = XMFLOAT4(20, 20, 20, 0);
	trunk_tex_param.thickness = 2;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create the Direct3D object.
	m_Direct3D = new D3DClass;

	// Initialize the Direct3D object.
	m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

	m_TextureShader = new TextureShaderClass;
	m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);

	m_ColorShader = new ColorShaderClass;
	m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);

	m_light = new Light;
	m_light->SetLightAmbient(XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
	m_light->SetLightDiffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	m_light->SetLightDirection(XMFLOAT3(1.0f, -0.5f, 0.0f));

	TwInit(TW_DIRECT3D11, m_Direct3D->GetDevice()); // for Direct3D 11
	TwWindowSize(m_Direct3D->GetWindowWidth(), m_Direct3D->GetWindowHeight());

	//TwAddVarRO(m_Bar, "Camera position", TW_TYPE_DIR3F, &m_Camera, " label='# of texture units' ");

	//create 3 cubes
	float x, y, z;
	x = y = z = 0.0f;
	int size = 10;

	srand(time(NULL));

	CreateCube(Transform(XMFLOAT3(0.0f, -1.00f, 0.0f), XMFLOAT3(20.0f, 1.0f, 20.0f), zero, COL_DARKGREEN));

	//CreateCube(Transform(XMFLOAT3(0.0f, 20.0f, 0.0f), XMFLOAT3(2.0f, 1.0f, 2.0f), zero, COL_DARKTURQUOISE));

	Transform temp;
	temp.scale = one;
	temp.rot = zero;
	temp.pos = zero;

	temp.pos.y = 20;

	temp.pos.x = 0;

	temp.color = COL_LEAFGREEN;

	for (int i = 0; i < size; i++)
	{
		float scale = (size - i) / (float)size;
		//CreateCube(XMFLOAT3(x, y, z), XMFLOAT3(scale * 0.5f, scale, scale * 0.5f), XMFLOAT3(x * 45.0f, y * 45.0f, z * 45.0f));
		//CreateTriangle(XMFLOAT3(x, y, z), XMFLOAT3(scale * 0.5f, scale * 0.5f, scale * 0.5f), XMFLOAT3(180.0f, y * 45.0f, 180.0f));
		//CreateTriangle(XMFLOAT3(x + 0.5f * scale, y + 0.5f * scale, z + 0.5f * scale), XMFLOAT3(scale * 0.5f, scale * 0.5f, scale * 0.5f), XMFLOAT3(180.0f, y * 45.0f + 90.0f, 180.0f));
		
		x = (i * 10) % 40;
		z = (i * 5) - 30;
		y = 0;

		temp.pos = XMFLOAT3(x, y, z);
		temp.color = COL_RANDOM;

		//CreateTriangle(temp);
		
		//CreateSphere(temp);
		//CreateMoreLeaves(temp, 5 + rand() % 10, 10 + rand() % 10);

		x = 40 + (i * 10) % 40;
		z = (i * 5) - 30 - 30;
		y = 0;

		temp.pos = XMFLOAT3(x, y, z);
		temp.color = COL_DARKRANDOM;

		//CreateLeaves(temp, 5 + rand() % 10, 5 + rand() % 10);

		x = 0 - (i * 3) % 9;
		z = (i * 3) + 30;
		y = 0;

		temp.pos = XMFLOAT3(x, y, z);
		temp.color = COL_DARKGREEN;
		temp.scale = XMFLOAT3(0.25f, 0.1f, 0.25f);

		//Transform rose = CreateTrunk(temp, 20, 3);
		//rose.color = COL_RANDOM;
		//rose.scale = quarter;
		//CreateRose(rose, 5 + rand() % 10, 15 + rand() % 10);

		temp.scale = one;

		//CreateRose(temp, 5 + rand() % 10, 5 + rand() % 10);
		
		x += ((rand() % 5) / 10.0f - 0.25f) * (scale + 0.1f);
		y += ((rand() % 50) / 100.0f - 0.15f) * (scale + 0.1f);
		z += ((rand() % 5) / 10.0f - 0.25f) * (scale + 0.1f);
	}

	/*CreateCube(XMFLOAT3(0.0f, 3.0f, 0.0f), XMFLOAT3(0.25f, 3.0f, 0.25f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	CreateLeaves(XMFLOAT3(0.0f, 5.5f, 0.0f), one, zero, 9, 15, XMFLOAT4(0.4f, 0.0f, 0.0f, 1.0f));
	CreateLeaves(XMFLOAT3(0.0f, 6.0f, 0.0f), one, zero, 3, 7, XMFLOAT4(0.4f, 0.0f, 0.75f, 1.0f));
	CreateLeaves(XMFLOAT3(0.0f, 5.0f, 0.0f), one, zero, 2, 5, XMFLOAT4(0.4f, 0.0f, 0.4f, 1.0f));
	CreateLeaves(XMFLOAT3(0.0f, 4.0f, 0.0f), one, zero, 2, 5, XMFLOAT4(0.4f, 0.0f, 0.4f, 1.0f));
	CreateLeaves(XMFLOAT3(0.0f, 3.0f, 0.0f), one, zero, 2, 5, XMFLOAT4(0.4f, 0.0f, 0.4f, 1.0f));
	CreateLeaves(XMFLOAT3(0.0f, 2.0f, 0.0f), one, zero, 2, 5, XMFLOAT4(0.4f, 0.0f, 0.4f, 1.0f));
	CreateLeaves(XMFLOAT3(0.0f, 1.0f, 0.0f), one, zero, 2, 5, XMFLOAT4(0.4f, 0.0f, 0.4f, 1.0f));
	CreateLeaves(XMFLOAT3(0.0f, 0.0f, 0.0f), one, zero, 2, 5, XMFLOAT4(0.4f, 0.0f, 0.4f, 1.0f));
	CreateLeaves(XMFLOAT3(5.0f, 0.0f, 5.0f), one, zero, 5, 13, XMFLOAT4(0.4f, 0.0f, 0.0f, 1.0f));*/

	//CreateTrunk(new_transform, trunk_param);
	CreateLeaves(new_transform, leaf_param);

	/*CreateTrunk(Transform(XMFLOAT3(35.0f, 0.0f, 5.0f), one, zero, COL_DARKBROWN), 12, 5);
	CreateTrunk(Transform(XMFLOAT3(15.0f, 0.0f, 25.0f), XMFLOAT3(3.5f, 3.0f, 3.5f), zero, COL_DARKTAN), 8, 10);

	CreateTree(Transform(XMFLOAT3(45.0f, 0.0f, 35.0f), quarter, zero, COL_DARKPURPLE), 7, 3);
	CreateTree(Transform(XMFLOAT3(40.0f, 0.0f, 35.0f), quarter, zero, COL_DARKCYAN), 6, 4);
	CreateTree(Transform(XMFLOAT3(45.0f, 0.0f, 40.0f), quarter, zero, COL_DARKORANGE), 8, 5);

	CreateTree(Transform(XMFLOAT3(35.0f, 0.0f, 35.0f), one, zero, COL_DARKRED), 9, 4);
	CreateTree(Transform(XMFLOAT3(25.0f, 0.0f, 30.0f), one, zero, COL_DARKBLUE), 7, 3);
	CreateTree(Transform(XMFLOAT3(30.0f, 0.0f, 45.0f), one, zero, COL_DARKGREEN), 12, 6);*/

	return true;
}

//static bool deleteAll(ModelClass * theElement) { delete theElement; return true; }

void GraphicsClass::Shutdown()
{

	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	//delete the models
	m_objects.clear();

	return;
}


bool GraphicsClass::Frame(CameraClass* m_Camera, TwBar* bar)
{
	bool result;


	// Render the graphics scene.
	result = Render(m_Camera, bar);
	if (!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render(CameraClass* m_Camera, TwBar* bar)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(COL_BLACK);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	/*XMFLOAT3 oldPos = m_light->GetLightDirection();
	XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(0.0f * 0.0174532925f, 1.0f * 0.0174532925f, 0.0f * 0.0174532925f);

	XMVECTOR tempPos = XMLoadFloat3(&oldPos);
	tempPos = XMVector3TransformCoord(tempPos, rotMat);
	XMStoreFloat3(&oldPos, tempPos);

	m_light->SetLightDirection(oldPos);*/


	for each (ModelClass* model in m_objects)
	{
		model->Render(m_Direct3D->GetDeviceContext());
		//m_ColorShader->Render(m_Direct3D->GetDeviceContext(), model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

		if (model->GetTexture() != NULL)
		{
			m_TextureShader->Render(m_Direct3D->GetDeviceContext(), model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, model->GetTexture(), m_light);
		}
		else
		{
			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_light);
		}
	}

	TwRefreshBar(bar);
	TwDraw();

	// Render the model using the color shader.
	//result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), indexCount, worldMatrix, viewMatrix, projectionMatrix);
	//if (!result)
	//{
		//return false;
	//}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

void GraphicsClass::CreateCube(Transform transform)
{
	int m_vertexCount = 8;
	int m_indexCount = 36;

	ModelClass::VertexType* vertices;
	unsigned long* indices;

	vertices = new ModelClass::VertexType[m_vertexCount];
	indices = new unsigned long[m_indexCount];

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, -1.0f);  //Front, bottom left
	vertices[0].color = transform.color;
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(-1.0f, 1.0f, -1.0f);  //Front, top left
	vertices[1].color = transform.color;
	vertices[1].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, -1.0f);  //Front, bottom right
	vertices[2].color = transform.color;
	vertices[2].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[3].position = XMFLOAT3(1.0f, 1.0f, -1.0f);  //Front, top right
	vertices[3].color = transform.color;
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[4].position = XMFLOAT3(-1.0f, -1.0f, 1.0f);  //Back, bottom left
	vertices[4].color = transform.color;
	vertices[4].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[5].position = XMFLOAT3(-1.0f, 1.0f, 1.0f);  //Back, top left
	vertices[5].color = transform.color;
	vertices[5].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[6].position = XMFLOAT3(1.0f, -1.0f, 1.0f);  //Back, bottom right
	vertices[6].color = transform.color;
	vertices[6].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[7].position = XMFLOAT3(1.0f, 1.0f, 1.0f);  //Back, top right
	vertices[7].color = transform.color;
	vertices[7].texture = XMFLOAT2(1.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	indices[3] = 3;  // Bottom left.
	indices[4] = 2;  // Top middle.
	indices[5] = 1;  // Bottom right.

	indices[6] = 2;  // Bottom left.
	indices[7] = 3;  // Top middle.
	indices[8] = 6;  // Bottom right.

	indices[9] = 6;  // Bottom left.
	indices[10] = 3;  // Top middle.
	indices[11] = 7;  // Bottom right.

	indices[12] = 0;  // Bottom left.
	indices[13] = 4;  // Top middle.
	indices[14] = 1;  // Bottom right.

	indices[15] = 4;  // Bottom left.
	indices[16] = 5;  // Top middle.
	indices[17] = 1;  // Bottom right.

	indices[18] = 4;  // Bottom left.
	indices[19] = 6;  // Top middle.
	indices[20] = 5;  // Bottom right.

	indices[21] = 7;  // Bottom left.
	indices[22] = 5;  // Top middle.
	indices[23] = 6;  // Bottom right.

	indices[24] = 1;  // Bottom left.
	indices[25] = 5;  // Top middle.
	indices[26] = 3;  // Bottom right.

	indices[27] = 5;  // Bottom left.
	indices[28] = 7;  // Top middle.
	indices[29] = 3;  // Bottom right.

	indices[30] = 0;  // Bottom left.
	indices[31] = 2;  // Top middle.
	indices[32] = 4;  // Bottom right.

	indices[33] = 4;  // Bottom left.
	indices[34] = 2;  // Top middle.
	indices[35] = 6;  // Bottom right.*/

	std::vector<ModelClass::VertexType> vertexList;
	std::vector<unsigned long> indexList;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	float pitch = transform.rot.x * 0.0174532925f;
	float yaw = transform.rot.y * 0.0174532925f;
	float roll = transform.rot.z * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	for (int i = 0; i < m_vertexCount; i++)
	{
		XMVECTOR tempPos = XMLoadFloat3(&vertices[i].position);
		tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);
		XMStoreFloat3(&vertices[i].position, tempPos);
		vertices[i].position = MultFloat3(vertices[i].position, transform.scale);
		vertices[i].position = AddFloat3(vertices[i].position, transform.pos);
		vertexList.push_back(vertices[i]);
	}

	for (int i = 0; i < m_indexCount; i++)
	{
		indexList.push_back(indices[i]);
	}

	for (int i = 0; i < indexList.size(); i += 3)
	{
		XMFLOAT3 normal = ComputeNormal(vertexList[indexList[i]].position,
			vertexList[indexList[i + 1]].position, vertexList[indexList[i + 2]].position);

		vertexList[indexList[i]].normal = normal;
		vertexList[indexList[i + 1]].normal = normal;
		vertexList[indexList[i + 2]].normal = normal;
	}

	ModelClass *newModel = new ModelClass();
	newModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), vertexList, indexList, "", tex_param);
	m_objects.push_back(newModel);
}

void GraphicsClass::CreateSphere(Transform transform)
{
	std::vector<ModelClass::VertexType> vertexList;
	std::vector<unsigned long> indexList;

	std::list<ModelClass::VertexType> cubeFace;

	//create the initial face, front facing.

	//number of squares per face- will end up being doubled
	int divide = 8;

	//tesselate
	float tes = 1.0f / ((float)divide / 2.0f);

	XMFLOAT3 origin = XMFLOAT3(-1.0f, -1.0f, 0.0f);

	int pushed = 0;

	for (int x = 0; x < divide; x++)
	{
		for (int y = 0; y < divide; y++)
		{
			ModelClass::VertexType temp;
			temp.color = XMFLOAT4(0.0f, (x * tes) / 2.0f, (y * tes) / 2.0f, 1.0f);

			//start at the 'origin'
			temp.position = XMFLOAT3(x * tes, y * tes, -1.0f);
			temp.position = AddFloat3(temp.position, origin);
			cubeFace.push_back(temp);
			vertexList.push_back(temp);
			indexList.push_back(pushed);
			pushed++;

			//now go up
			temp.position = XMFLOAT3(x * tes, y * tes + tes, -1.0f);
			temp.position = AddFloat3(temp.position, origin);
			cubeFace.push_back(temp);
			vertexList.push_back(temp);
			indexList.push_back(pushed);
			pushed++;

			//and bottom right
			temp.position = XMFLOAT3(x * tes + tes, y * tes, -1.0f);
			temp.position = AddFloat3(temp.position, origin);
			cubeFace.push_back(temp);
			vertexList.push_back(temp);
			indexList.push_back(pushed);
			pushed++;

			//second face, start up
			temp.position = XMFLOAT3(x * tes, y * tes + tes, -1.0f);
			temp.position = AddFloat3(temp.position, origin);
			cubeFace.push_back(temp);
			vertexList.push_back(temp);
			indexList.push_back(pushed);
			pushed++;

			//up right
			temp.position = XMFLOAT3(x * tes + tes, y * tes + tes, -1.0f);
			temp.position = AddFloat3(temp.position, origin);
			cubeFace.push_back(temp);
			vertexList.push_back(temp);
			indexList.push_back(pushed);
			pushed++;

			//and bottom right
			temp.position = XMFLOAT3(x * tes + tes, y * tes, -1.0f);
			temp.position = AddFloat3(temp.position, origin);
			cubeFace.push_back(temp);
			vertexList.push_back(temp);
			indexList.push_back(pushed);
			pushed++;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		XMMATRIX cubeRot = XMMatrixRotationY((90.0f + (90.0f * (float)i)) * 0.0174532925f);
		for (auto it = cubeFace.begin(); it != cubeFace.end(); it++)
		{
			ModelClass::VertexType temp;
			temp = *it;
			XMVECTOR tempPos = XMLoadFloat3(&temp.position);
			tempPos = XMVector3TransformCoord(tempPos, cubeRot);
			XMStoreFloat3(&temp.position, tempPos);
			vertexList.push_back(temp);
			indexList.push_back(pushed);
			pushed++;
		}
	}

	for (int i = 0; i < 2; i++)
	{
		XMMATRIX cubeRot = XMMatrixRotationX((90.0f + (180.0f * (float)i)) * 0.0174532925f);
		for (auto it = cubeFace.begin(); it != cubeFace.end(); it++)
		{
			ModelClass::VertexType temp;
			temp = *it;
			XMVECTOR tempPos = XMLoadFloat3(&temp.position);
			tempPos = XMVector3TransformCoord(tempPos, cubeRot);
			XMStoreFloat3(&temp.position, tempPos);
			vertexList.push_back(temp);
			indexList.push_back(pushed);
			pushed++;
		}
	}


	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	float pitch = transform.rot.x * 0.0174532925f;
	float yaw = transform.rot.y * 0.0174532925f;
	float roll = transform.rot.z * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	for (auto it = vertexList.begin(); it != vertexList.end(); it++)
	{
		XMVECTOR tempPos = XMLoadFloat3(&it->position);
		tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);
		//normalize
		tempPos = XMVector3NormalizeRobust(tempPos);
		XMStoreFloat3(&it->position, tempPos);
		it->position = MultFloat3(it->position, transform.scale);
		it->position = AddFloat3(it->position, transform.pos);
		it->position = TakeFloat3(it->position, origin);
	}

	for (int i = 0; i < indexList.size(); i += 3)
	{
		XMFLOAT3 normal = ComputeNormal(vertexList[indexList[i]].position,
			vertexList[indexList[i + 1]].position, vertexList[indexList[i + 2]].position);

		vertexList[indexList[i]].normal = normal;
		vertexList[indexList[i + 1]].normal = normal;
		vertexList[indexList[i + 2]].normal = normal;
	}

	ModelClass *newModel = new ModelClass();
	newModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), vertexList, indexList, "", tex_param);
	m_objects.push_back(newModel);
}

void GraphicsClass::CreateTriangle(Transform transform)
{
	int m_vertexCount = 3;
	int m_indexCount = 6;

	ModelClass::VertexType* vertices;
	unsigned long* indices;

	vertices = new ModelClass::VertexType[m_vertexCount];
	indices = new unsigned long[m_indexCount];

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, -1.0f);  //Front, bottom left
	vertices[0].color = transform.color;
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(-1.0f, 1.0f, -1.0f);  //Front, top left
	vertices[1].color = transform.color;
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, -1.0f);  //Front, bottom right
	vertices[2].color = transform.color;
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 1;

	std::vector<ModelClass::VertexType> vertexList;
	std::vector<unsigned long> indexList;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	float pitch = transform.rot.x * 0.0174532925f;
	float yaw = transform.rot.y * 0.0174532925f;
	float roll = transform.rot.z * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	for (int i = 0; i < m_vertexCount; i++)
	{
		XMVECTOR tempPos = XMLoadFloat3(&vertices[i].position);
		tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);
		XMStoreFloat3(&vertices[i].position, tempPos);
		vertices[i].position = MultFloat3(vertices[i].position, transform.scale);
		vertices[i].position = AddFloat3(vertices[i].position, transform.pos);
		vertexList.push_back(vertices[i]);
	}

	for (int i = 0; i < m_indexCount; i++)
	{
		indexList.push_back(indices[i]);
	}

	ModelClass *newModel = new ModelClass();
	newModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), vertexList, indexList, "../Procedural-ATP/leaf.tga", tex_param);
	m_objects.push_back(newModel);
}

void GraphicsClass::CreateLeaves(Transform transform, Parameters param)
{
	std::vector<ModelClass::VertexType> vertexList;
	std::vector<unsigned long> indexList;

	XMFLOAT3 newRot;
	newRot.x = 0.0f;
	newRot.y = 0.0f;
	newRot.z = 0.0f;

	XMFLOAT3 up;
	up.x = 0.0f;
	up.y = param.segHeight;
	up.z = 0.0f;

	XMFLOAT3 tempUp;
	tempUp = newRot;

	int leaves = param.rings;
	int segments = param.segments;

	float localScale = param.startWidth;
	ModelClass::VertexType* thisRing = new ModelClass::VertexType[leaves];
	ModelClass::VertexType* prevRing = new ModelClass::VertexType[leaves];

	float angle = (360.0f / leaves) * 0.0174532925f;

	//initial ring
	for (int i = 0; i < leaves; i++)
	{
		/*prevRing[i].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMMATRIX rotMat = XMMatrixRotationY(angle * i);
		XMVECTOR tempPos = XMLoadFloat3(&prevRing[i].position);
		tempPos = XMVector3TransformCoord(tempPos, rotMat);
		XMStoreFloat3(&prevRing[i].position, tempPos);

		prevRing[i].position = AddFloat3(prevRing[i].position, origin);*/

		prevRing[i].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		prevRing[i].texture = XMFLOAT2(0.5f, 0.0f);
		prevRing[i].color = transform.color;
	}

	int segmentsAdded = 0;

	for (int i = 0; i < segments; i++)
	{
		//random rotation
		newRot.x += RandFloat(param.lowerBound, param.upperBound);
		newRot.y += RandFloat(param.lowerBound, param.upperBound);
		newRot.z += RandFloat(param.lowerBound, param.upperBound);

		float pitch = newRot.x * 0.0174532925f;
		float yaw = newRot.y * 0.0174532925f;
		float roll = newRot.z * 0.0174532925f;

		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
		XMVECTOR upVector = XMLoadFloat3(&up);

		//rotate the up vector
		upVector = XMVector3TransformCoord(upVector, rotationMatrix);

		XMStoreFloat3(&up, upVector);

		localScale = param.endWidth + ((segments - i) / segments) * (param.startWidth - param.endWidth);

		//set the new ring up
		for (int j = 0; j < leaves; j++)
		{
			thisRing[j].position = XMFLOAT3(localScale, 0.0f, 0.0f);
			thisRing[j].position = AddFloat3(up, thisRing[j].position);
			thisRing[j].position.x += RandFloat(param.lowerBoundX, param.upperBoundX) * param.segHeight;
			thisRing[j].position.y += RandFloat(param.lowerBoundY, param.upperBoundY) * param.segHeight;
			thisRing[j].position.z += RandFloat(param.lowerBoundZ, param.upperBoundZ) * param.segHeight;
			//thisRing[j].position = MultFloat3(thisRing[j].position, XMFLOAT3(scale, scale, scale));
			XMMATRIX rotMat = XMMatrixRotationY(angle * j);
			XMVECTOR tempPos = XMLoadFloat3(&thisRing[j].position);
			tempPos = XMVector3TransformCoord(tempPos, rotMat);
			tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);
			XMStoreFloat3(&thisRing[j].position, tempPos);

			//thisRing[j].position = AddFloat3(up, thisRing[j].position);
			thisRing[j].position = AddFloat3(prevRing[j].position, thisRing[j].position);

			thisRing[j].color = prevRing[j].color;
			thisRing[j].color.x *= 1 + 0.75f * localScale;
			thisRing[j].color.y *= 1 + 0.75f * localScale;
			thisRing[j].color.z *= 1 + 0.75f * localScale;

			float texcoord = (float)(i) / (float)(segments - 1);

			thisRing[j].texture = XMFLOAT2(texcoord, 1.0f);

			//push the previous ring again
			vertexList.push_back(prevRing[j]);
		}

		//now push the current ring
		for (int j = 0; j < leaves; j++)
		{
			vertexList.push_back(thisRing[j]);

			prevRing[j].position = thisRing[j].position;
			prevRing[j].color = thisRing[j].color;
			prevRing[j].texture = thisRing[j].texture;

			//and set up the index list

			//self, self + ring, (self + 1) % ring
			indexList.push_back(segmentsAdded);
			indexList.push_back(segmentsAdded + leaves);
			indexList.push_back((segmentsAdded + 1) % leaves);

			//and the reverse
			//indexList.push_back(segmentsAdded);
			//indexList.push_back((segmentsAdded + 1) % leaves);
			//indexList.push_back(segmentsAdded + leaves);
			segmentsAdded++;
		}

		//skip a ring for texturing purposes
		segmentsAdded += leaves;
	}

	float pitch = transform.rot.x * 0.0174532925f;
	float yaw = transform.rot.y * 0.0174532925f;
	float roll = transform.rot.z * 0.0174532925f;

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//now to duplicate the vectors
	int tempVert = vertexList.size();

	for (int i = 0; i < tempVert; i++)
	{
		vertexList.push_back(vertexList[i]);
	}

	int temp = indexList.size();

	for (int i = 0; i < temp; i+=3)
	{
		indexList.push_back(indexList[i] + tempVert);
		indexList.push_back(indexList[i + 2] + tempVert);
		indexList.push_back(indexList[i + 1] + tempVert);
	}

	for (auto it = vertexList.begin(); it != vertexList.end(); it++)
	{
		it->position = MultFloat3(it->position, transform.scale);
		XMVECTOR tempPos = XMLoadFloat3(&it->position);
		tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);
		XMStoreFloat3(&it->position, tempPos);
		it->position = AddFloat3(it->position, transform.pos);
	}

	for (int i = 0; i < indexList.size(); i += 3)
	{
		XMFLOAT3 normal = ComputeNormal(vertexList[indexList[i]].position,
			vertexList[indexList[i + 1]].position, vertexList[indexList[i + 2]].position);

		vertexList[indexList[i]].normal = normal;
		vertexList[indexList[i + 1]].normal = normal;
		vertexList[indexList[i + 2]].normal = normal;
	}

	ModelClass *newModel = new ModelClass();
	newModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), vertexList, indexList, "../Procedural-ATP/leaf.tga", leaf_tex_param);
	m_objects.push_back(newModel);
}

void GraphicsClass::CreateMoreLeaves(Transform transform, Parameters param)
{
	std::vector<ModelClass::VertexType> vertexList;
	std::vector<unsigned long> indexList;

	XMFLOAT3 newRot;
	newRot.x = 0.0f;
	newRot.y = 0.0f;
	newRot.z = 0.0f;

	int leaves = param.rings;
	int segments = param.segments;

	float localScale = 1.0f;
	ModelClass::VertexType* thisRing = new ModelClass::VertexType[leaves];
	ModelClass::VertexType* prevRing = new ModelClass::VertexType[leaves];
	ModelClass::VertexType* thisRing2 = new ModelClass::VertexType[leaves];
	ModelClass::VertexType* prevRing2 = new ModelClass::VertexType[leaves];

	float angle = (360.0f / (leaves / 4.0f)) * 0.0174532925f;

	float size = 5.0f;

	ModelClass::VertexType origin;
	origin.color = transform.color;
	origin.position = zero;

	float up = 0.2f;
	float out = 1.0f;

	//initial ring
	for (int i = 0; i < leaves; i++)
	{
		prevRing[i].position = XMFLOAT3(0.0f, up, out);

		up += 1.0f / leaves;
		out -= 1.0f / leaves;

		XMMATRIX rotMat = XMMatrixRotationY(angle * i);
		XMVECTOR tempPos = XMLoadFloat3(&prevRing[i].position);
		tempPos = XMVector3NormalizeRobust(tempPos);
		tempPos *= size;
		tempPos = XMVector3TransformCoord(tempPos, rotMat);
		XMStoreFloat3(&prevRing[i].position, tempPos);

		//prevRing[i].position = ScalarFloat3(prevRing[i].position, size);

		//prevRing[i].position = XMFLOAT3(0.0f, 0.0f, 0.0f);

		prevRing[i].color = transform.color;

		prevRing2[i] = prevRing[i];
	}

	int segmentsAdded = 0;

	for (int i = 0; i < segments; i++)
	{
		localScale = ((float)(segments - i) / (float)segments) * size;

		//set the new ring up
		for (int j = 0; j < leaves; j++)
		{
			float x = ((angle * 1.75f) / (float)segments) * (rand() % segments) / (float)segments;
			float y = ((angle * 1.0f) / (float)segments) * (rand() % segments) / (float)segments;
			float z = ((angle * 1.5f) / (float)segments) * (rand() % segments) / (float)segments;
			//x = 0.0f;
			//y = 0.0f;
			//z = 0.0f;

			XMFLOAT3 newRot = XMFLOAT3(x, y, z);

			//XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(x, y, z);
			XMMATRIX rotMat = XMMatrixRotationY(angle * j);

			XMVECTOR tempRot = XMLoadFloat3(&newRot);
			tempRot = XMVector3TransformCoord(tempRot, rotMat);
			XMStoreFloat3(&newRot, tempRot);

			rotMat = XMMatrixRotationRollPitchYaw(newRot.x, newRot.y, newRot.z);

			//XMMATRIX rotMat = XMMatrixRotationY((angle * j) * (0.5f / segments));

			thisRing[j].position = prevRing[j].position;
			thisRing[j].position.x += ((rand() % 10) - 5) / 100.0f;
			thisRing[j].position.y += ((rand() % 10) - 5) / 100.0f;
			thisRing[j].position.z += ((rand() % 10) - 5) / 100.0f;

			XMVECTOR tempPos = XMLoadFloat3(&thisRing[j].position);
			tempPos = XMVector3NormalizeRobust(tempPos);
			tempPos *= localScale;
			tempPos = XMVector3TransformCoord(tempPos, rotMat);
			XMStoreFloat3(&thisRing[j].position, tempPos);

			thisRing2[j].position = prevRing2[j].position;
			thisRing2[j].position.x -= ((rand() % 10) - 5) / 100.0f;
			thisRing2[j].position.y -= ((rand() % 10) - 5) / 100.0f;
			thisRing2[j].position.z -= ((rand() % 10) - 5) / 100.0f;

			rotMat = XMMatrixRotationRollPitchYaw(-x, -y, -z);

			tempPos = XMLoadFloat3(&thisRing2[j].position);
			tempPos = XMVector3NormalizeRobust(tempPos);
			tempPos *= localScale;
			tempPos = XMVector3TransformCoord(tempPos, rotMat);
			XMStoreFloat3(&thisRing2[j].position, tempPos);

			//thisRing[j].position = ScalarFloat3(thisRing[j].position, localScale);

			thisRing[j].color = prevRing[j].color;
			thisRing[j].color.x *= (size / localScale) * 0.2f + 0.5f;
			thisRing[j].color.y *= (size / localScale) * 0.2f + 0.5f;
			thisRing[j].color.z *= (size / localScale) * 0.2f + 0.5f;

			thisRing2[j].color = prevRing2[j].color;
			thisRing2[j].color.x *= (size / localScale) * 0.2f + 0.5f;
			thisRing2[j].color.y *= (size / localScale) * 0.2f + 0.5f;
			thisRing2[j].color.z *= (size / localScale) * 0.2f + 0.5f;

			origin.texture = XMFLOAT2(0.0f, 0.0f);
			prevRing[j].texture = XMFLOAT2(0.5f, 1.0f);
			thisRing[j].texture = XMFLOAT2(1.0f, 0.0f);

			prevRing2[j].texture = XMFLOAT2(0.5f, 1.0f);
			thisRing2[j].texture = XMFLOAT2(1.0f, 0.0f);

			vertexList.push_back(origin); //0
			vertexList.push_back(prevRing[j]); //1
			vertexList.push_back(thisRing[j]); //2

			vertexList.push_back(origin); //3
			vertexList.push_back(prevRing2[j]); //4
			vertexList.push_back(thisRing2[j]); //5

			prevRing[j].position = thisRing[j].position;
			prevRing[j].color = thisRing[j].color;

			prevRing2[j].position = thisRing2[j].position;
			prevRing2[j].color = thisRing2[j].color;

			indexList.push_back(segmentsAdded);
			indexList.push_back(segmentsAdded + 1);
			indexList.push_back(segmentsAdded + 2);

			//and the reverse
			indexList.push_back(segmentsAdded);
			indexList.push_back(segmentsAdded + 2);
			indexList.push_back(segmentsAdded + 1);
			segmentsAdded+=3;

			indexList.push_back(segmentsAdded);
			indexList.push_back(segmentsAdded + 1);
			indexList.push_back(segmentsAdded + 2);

			//and the reverse
			indexList.push_back(segmentsAdded);
			indexList.push_back(segmentsAdded + 2);
			indexList.push_back(segmentsAdded + 1);
			segmentsAdded += 3;
		}

		//skip a ring for texturing purposes
		//segmentsAdded += leaves;
	}

	float pitch = transform.rot.x * 0.0174532925f;
	float yaw = transform.rot.y * 0.0174532925f;
	float roll = transform.rot.z * 0.0174532925f;

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//for texturing purposes
	float max_x = 0.0f;
	float max_y = 0.0f;
	float min_x = 0.0f;
	float min_y = 0.0f;
	float min_z = 0.0f;
	float max_z = 0.0f;

	//first, get the bounds of the model
	for (auto it = vertexList.begin(); it != vertexList.end(); it++)
	{
		if (it->position.y > max_y)
		{
			max_y = it->position.y;
		}

		if (it->position.y < min_y)
		{
			min_y = it->position.y;
		}

		if (it->position.x > max_x)
		{
			max_x = it->position.x;
		}

		if (it->position.x < min_x)
		{
			min_x = it->position.x;
		}

		if (it->position.z > max_z)
		{
			max_z = it->position.z;
		}

		if (it->position.z < min_z)
		{
			min_z = it->position.z;
		}
	}

	float total_y = max_y - min_y;
	float total_x = max_x - min_x;
	float total_z = max_z - min_z;

	min_x *= -1;
	min_z *= -1;
	min_y *= -1;

	for (int i = 0; i < indexList.size(); i += 3)
	{
		XMFLOAT3 normal = ComputeNormal(vertexList[indexList[i]].position,
			vertexList[indexList[i + 1]].position, vertexList[indexList[i + 2]].position);

		vertexList[indexList[i]].normal = normal;
		vertexList[indexList[i + 1]].normal = normal;
		vertexList[indexList[i + 2]].normal = normal;
	}

	for (auto it = vertexList.begin(); it != vertexList.end(); it++)
	{
		float tex_x = it->position.x + min_x;
		float tex_y = it->position.y + min_y;
		float tex_z = it->position.z + min_z;

		tex_x /= total_x;
		tex_y /= total_y;
		tex_z /= total_y;

		//it->texture = XMFLOAT2((tex_x + tex_z) / 2.0f, tex_y);

		it->position = MultFloat3(it->position, transform.scale);
		XMVECTOR tempPos = XMLoadFloat3(&it->position);
		tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);
		XMStoreFloat3(&it->position, tempPos);
		it->position = AddFloat3(it->position, transform.pos);
	}

	ModelClass *newModel = new ModelClass();
	newModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), vertexList, indexList, "../Procedural-ATP/tileleaf.tga", leaf_tex_param);
	m_objects.push_back(newModel);
}

void GraphicsClass::CreateRose(Transform transform, Parameters param)
{
	std::vector<ModelClass::VertexType> vertexList;
	std::vector<unsigned long> indexList;

	XMFLOAT3 newRot;
	newRot.x = 0.0f;
	newRot.y = 0.0f;
	newRot.z = 0.0f;

	int leaves = param.rings;
	int segments = param.segments;

	float localScale = 1.0f;
	ModelClass::VertexType* thisRing = new ModelClass::VertexType[leaves];
	ModelClass::VertexType* prevRing = new ModelClass::VertexType[leaves];
	ModelClass::VertexType* thisRing2 = new ModelClass::VertexType[leaves];
	ModelClass::VertexType* prevRing2 = new ModelClass::VertexType[leaves];

	float angle = (360.0f / (leaves / 3.0f)) * 0.0174532925f;

	float size = 5.0f;

	ModelClass::VertexType origin;
	origin.color = transform.color;
	origin.position = zero;

	float up = 0.25f;
	float out = 1.0f;

	//initial ring
	for (int i = 0; i < leaves; i++)
	{
		prevRing[i].position = XMFLOAT3(0.0f, up, out);

		up += 1.0f / leaves;
		out -= 1.0f / leaves;

		XMMATRIX rotMat = XMMatrixRotationY(angle * i);
		XMVECTOR tempPos = XMLoadFloat3(&prevRing[i].position);
		tempPos = XMVector3NormalizeRobust(tempPos);
		tempPos *= size;
		tempPos = XMVector3TransformCoord(tempPos, rotMat);
		XMStoreFloat3(&prevRing[i].position, tempPos);

		//prevRing[i].position = ScalarFloat3(prevRing[i].position, size);

		//prevRing[i].position = XMFLOAT3(0.0f, 0.0f, 0.0f);

		prevRing[i].color = transform.color;

		prevRing2[i] = prevRing[i];
	}

	int segmentsAdded = 0;

	for (int i = 0; i < segments; i++)
	{
		localScale = ((float)(segments - i) / (float)segments) * size;

		//set the new ring up
		for (int j = 0; j < leaves; j++)
		{
			float x = ((angle * 1.5f) / (float)segments) * (rand() % segments) / (float)segments;
			float y = ((angle * 1.0f) / (float)segments) * (rand() % segments) / (float)segments;
			float z = ((angle * 1.5f) / (float)segments) * (rand() % segments) / (float)segments;
			//XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(x, y, z);
			XMMATRIX rotMat = XMMatrixRotationY((angle * j) * (0.5f / segments));

			thisRing[j].position = prevRing[j].position;
			thisRing[j].position.x += ((rand() % 10) - 5) / 25.0f;
			thisRing[j].position.y += ((rand() % 10) - 5) / 25.0f;
			thisRing[j].position.z += ((rand() % 10) - 5) / 25.0f;

			XMVECTOR tempPos = XMLoadFloat3(&thisRing[j].position);
			tempPos = XMVector3NormalizeRobust(tempPos);
			tempPos *= localScale;
			tempPos = XMVector3TransformCoord(tempPos, rotMat);
			XMStoreFloat3(&thisRing[j].position, tempPos);

			thisRing2[j].position = prevRing2[j].position;

			rotMat = XMMatrixRotationRollPitchYaw(-x, -y, -z);

			tempPos = XMLoadFloat3(&thisRing2[j].position);
			tempPos = XMVector3NormalizeRobust(tempPos);
			tempPos *= localScale;
			tempPos = XMVector3TransformCoord(tempPos, rotMat);
			XMStoreFloat3(&thisRing2[j].position, tempPos);

			//thisRing[j].position = ScalarFloat3(thisRing[j].position, localScale);

			thisRing[j].color = prevRing[j].color;
			thisRing[j].color.x *= (size / localScale) * 0.2f + 0.5f;
			thisRing[j].color.y *= (size / localScale) * 0.2f + 0.5f;
			thisRing[j].color.z *= (size / localScale) * 0.2f + 0.5f;

			thisRing2[j].color = prevRing2[j].color;
			thisRing2[j].color.x *= (size / localScale) * 0.2f + 0.5f;
			thisRing2[j].color.y *= (size / localScale) * 0.2f + 0.5f;
			thisRing2[j].color.z *= (size / localScale) * 0.2f + 0.5f;

			vertexList.push_back(origin); //0
			vertexList.push_back(prevRing[j]); //1
			vertexList.push_back(thisRing[j]); //2

			vertexList.push_back(origin); //3
			vertexList.push_back(prevRing2[j]); //4
			vertexList.push_back(thisRing2[j]); //5

			prevRing[j].position = thisRing[j].position;
			prevRing[j].color = thisRing[j].color;

			prevRing2[j].position = thisRing2[j].position;
			prevRing2[j].color = thisRing2[j].color;

			origin.texture = XMFLOAT2(0.0f, 0.0f);
			prevRing[j].texture = XMFLOAT2(0.5f, 1.0f);
			thisRing[j].texture = XMFLOAT2(1.0f, 0.0f);

			prevRing2[j].texture = XMFLOAT2(0.5f, 1.0f);
			thisRing2[j].texture = XMFLOAT2(1.0f, 0.0f);

			indexList.push_back(segmentsAdded);
			indexList.push_back(segmentsAdded + 1);
			indexList.push_back(segmentsAdded + 2);

			//and the reverse
			indexList.push_back(segmentsAdded);
			indexList.push_back(segmentsAdded + 2);
			indexList.push_back(segmentsAdded + 1);
			segmentsAdded += 3;

			indexList.push_back(segmentsAdded);
			indexList.push_back(segmentsAdded + 1);
			indexList.push_back(segmentsAdded + 2);

			//and the reverse
			indexList.push_back(segmentsAdded);
			indexList.push_back(segmentsAdded + 2);
			indexList.push_back(segmentsAdded + 1);
			segmentsAdded += 3;
		}

		//skip a ring for texturing purposes
		//segmentsAdded += leaves;
	}

	float pitch = transform.rot.x * 0.0174532925f;
	float yaw = transform.rot.y * 0.0174532925f;
	float roll = transform.rot.z * 0.0174532925f;

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	for (int i = 0; i < indexList.size(); i += 3)
	{
		XMFLOAT3 normal = ComputeNormal(vertexList[indexList[i]].position,
			vertexList[indexList[i + 1]].position, vertexList[indexList[i + 2]].position);

		vertexList[indexList[i]].normal = normal;
		vertexList[indexList[i + 1]].normal = normal;
		vertexList[indexList[i + 2]].normal = normal;
	}

	for (auto it = vertexList.begin(); it != vertexList.end(); it++)
	{
		it->position = MultFloat3(it->position, transform.scale);
		XMVECTOR tempPos = XMLoadFloat3(&it->position);
		tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);
		XMStoreFloat3(&it->position, tempPos);
		it->position = AddFloat3(it->position, transform.pos);
	}

	ModelClass *newModel = new ModelClass();
	newModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), vertexList, indexList, "../Procedural-ATP/texture.tga", rose_tex_param);
	m_objects.push_back(newModel);
}

Transform GraphicsClass::CreateTrunk(Transform transform, Parameters param, int recursion, bool leaves, Parameters leafParam)
{
	std::vector<ModelClass::VertexType> vertexList;
	std::vector<unsigned long> indexList;

	XMFLOAT3 newRot;
	newRot.x = 0.0f;
	newRot.y = 0.0f;
	newRot.z = 0.0f;

	XMFLOAT3 up;
	up.x = 0.0f;
	up.y = param.segHeight;
	up.z = 0.0f;

	XMFLOAT3 tempUp;
	tempUp.x = 0.0f;
	tempUp.y = 0.0f;
	tempUp.z = 0.0f;

	int ring = param.rings;
	int segments = param.segments;

	float localScale = param.startWidth;
	ModelClass::VertexType* thisRing = new ModelClass::VertexType[ring];
	ModelClass::VertexType* prevRing = new ModelClass::VertexType[ring];

	float angle = (360.0f / ring) * 0.0174532925f;

	//initial ring
	for (int i = 0; i < ring; i++)
	{
		prevRing[i].position = XMFLOAT3(param.startWidth, 0.0f, 0.0f);
		XMMATRIX rotMat = XMMatrixRotationY(angle * i);
		XMVECTOR tempPos = XMLoadFloat3(&prevRing[i].position);
		tempPos = XMVector3TransformCoord(tempPos, rotMat);
		XMStoreFloat3(&prevRing[i].position, tempPos);

		//prevRing[i].position = AddFloat3(prevRing[i].position, origin);

		//prevRing[i].position = origin;

		prevRing[i].texture = XMFLOAT2((float)i / ring / 1.0f, 0.0f);

		prevRing[i].color = transform.color;
	}

	int segmentsAdded = 0;

	for (int i = 0; i < segments; i++)
	{
		//random rotation
		newRot.x += RandFloat(param.lowerBound, param.upperBound);
		//newRot.y += ((rand() % 10) - 5) / 3.0f;
		newRot.z += RandFloat(param.lowerBound, param.upperBound);

		float pitch = newRot.x * 0.0174532925f;
		float yaw = newRot.y * 0.0174532925f;
		float roll = newRot.z * 0.0174532925f;

		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
		XMVECTOR upVector = XMLoadFloat3(&up);

		//rotate the up vector
		upVector = XMVector3TransformCoord(upVector, rotationMatrix);

		XMStoreFloat3(&up, upVector);

		tempUp = AddFloat3(tempUp, up);

		up.x = 0.0f;
		up.y = param.segHeight;
		up.z = 0.0f;

		//origin = AddFloat3(origin, up);

		localScale = param.endWidth + ((segments - (i + 1)) / (float)segments) * (param.startWidth - param.endWidth);

		//set the new ring up
		for (int j = 0; j < ring; j++)
		{
			ModelClass::VertexType nextPos;

			thisRing[j].position = XMFLOAT3(localScale, 0.0f, 0.0f);
			//thisRing[j].position = MultFloat3(thisRing[j].position, XMFLOAT3(scale, scale, scale));
			XMMATRIX rotMat = XMMatrixRotationY(angle * j);
			XMMATRIX rotMat2 = XMMatrixRotationY(angle);
			XMVECTOR tempPos = XMLoadFloat3(&thisRing[j].position);
			tempPos = XMVector3TransformCoord(tempPos, rotMat);
			//tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);
			XMStoreFloat3(&thisRing[j].position, tempPos);

			tempPos = XMVector3TransformCoord(tempPos, rotMat2);
			XMStoreFloat3(&nextPos.position, tempPos);

			thisRing[j].position = AddFloat3(tempUp, thisRing[j].position);
			nextPos.position = AddFloat3(tempUp, nextPos.position);
			//thisRing[j].position.x += ((rand() % 10) - 5) / 20;
			//thisRing[j].position.y += ((rand() % 10) - 5) / 20;
			//thisRing[j].position.z += ((rand() % 10) - 5) / 20;

			//thisRing[j].color = prevRing[j].color;
			thisRing[j].color = transform.color;
			thisRing[j].color.x *= 1 + 0.75f * localScale;
			thisRing[j].color.y *= 1 + 0.75f * localScale;
			thisRing[j].color.z *= 1 + 0.75f * localScale;

			nextPos.color = thisRing[j].color;

			thisRing[j].texture = XMFLOAT2((float)j / ring / 1.0f, (float)(i + 1) / segments);
			nextPos.texture = XMFLOAT2((float)(j + 1) / ring / 1.0f, (float)(i + 1) / segments);

			vertexList.push_back(prevRing[j]);
			vertexList.push_back(thisRing[j]);
			vertexList.push_back(nextPos);

			vertexList.push_back(prevRing[j]);
			vertexList.push_back(nextPos);

			//this bit's necessary for when it does wrap around to 0, since the texture coordinate X will be 0
			int temp = (j + 1) % ring;
			prevRing[temp].texture = XMFLOAT2((float)(j + 1) / ring / 1.0f, (float)i / segments);
			vertexList.push_back(prevRing[temp]);
			prevRing[temp].texture = XMFLOAT2((float)temp / ring / 1.0f, (float)i / segments);

			indexList.push_back(segmentsAdded);
			indexList.push_back(segmentsAdded + 2);
			indexList.push_back(segmentsAdded + 1);

			segmentsAdded += 3;

			indexList.push_back(segmentsAdded);
			indexList.push_back(segmentsAdded + 2);
			indexList.push_back(segmentsAdded + 1);

			segmentsAdded += 3;
		}

		bool toRecurse = false;
		int chance = 50;

		if (recursion > 0 && rand() % 100 < chance && i < (float)segments / 3.0f)
		{
			toRecurse = true;
			recursion--;
		}

		XMFLOAT3 average = zero;

		//update previous ring
		for (int j = 0; j < ring; j++)
		{
			prevRing[j].position = thisRing[j].position;
			prevRing[j].color = transform.color;
			prevRing[j].texture = thisRing[j].texture;

			if (toRecurse)
			{
				average = AddFloat3(average, prevRing[j].position);
			}
		}

		if (toRecurse)
		{
			average = ScalarFloat3(average, 1.0f / (float)ring);

			float pitch = transform.rot.x * 0.0174532925f;
			float yaw = transform.rot.y * 0.0174532925f;
			float roll = transform.rot.z * 0.0174532925f;

			XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

			average = MultFloat3(average, transform.scale);
			XMVECTOR tempPos = XMLoadFloat3(&average);
			tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);
			XMStoreFloat3(&average, tempPos);
			average = AddFloat3(average, transform.pos);

			Transform temp = transform;
			temp.scale = MultFloat3(temp.scale, XMFLOAT3(localScale * 0.75, localScale * 0.9, localScale * 0.75));

			temp.pos = average;
			temp.rot = AddFloat3(temp.rot, XMFLOAT3(RandFloat(0.0f, 25.0f), RandFloat(0.0f, 360.0f), RandFloat(0.0f, 20.0f)));

			temp = CreateTrunk(temp, param, recursion, leaves, leafParam);
			//CreateLeaves(temp, leaf_param);
		}
	}

	//add the top
	/*segmentsAdded -= ring;
	for (int i = 0; i < ring; i++)
	{
		vertexList.push_back(thisRing[i]);
		indexList.push_back(segmentsAdded);
		if ((segmentsAdded + 1) % ring == 0)
		{
			indexList.push_back(segmentsAdded + 1 - ring);
		}
		else
		{
			indexList.push_back(segmentsAdded + 1);
		}
		segmentsAdded++;
	}*/

	float pitch = transform.rot.x * 0.0174532925f;
	float yaw = transform.rot.y * 0.0174532925f;
	float roll = transform.rot.z * 0.0174532925f;

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	for (auto it = vertexList.begin(); it != vertexList.end(); it++)
	{
		it->position = MultFloat3(it->position, transform.scale);
		XMVECTOR tempPos = XMLoadFloat3(&it->position);
		tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);
		XMStoreFloat3(&it->position, tempPos);
		it->position = AddFloat3(it->position, transform.pos);
	}

	for (int i = 0; i < indexList.size(); i += 3)
	{
		XMFLOAT3 normal = ComputeNormal(vertexList[indexList[i]].position,
			vertexList[indexList[i + 1]].position, vertexList[indexList[i + 2]].position);

		vertexList[indexList[i]].normal = normal;
		vertexList[indexList[i + 1]].normal = normal;
		vertexList[indexList[i + 2]].normal = normal;
	}

	tempUp = MultFloat3(tempUp, transform.scale);

	XMVECTOR tempPos = XMLoadFloat3(&tempUp);
	tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);
	XMStoreFloat3(&tempUp, tempPos);

	tempUp = AddFloat3(tempUp, transform.pos);

	ModelClass *newModel = new ModelClass();
	newModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), vertexList, indexList, "../Procedural-ATP/bark.tga", trunk_tex_param);
	m_objects.push_back(newModel);

	Transform result = Transform(tempUp, transform.scale, AddFloat3(transform.rot, newRot), transform.color);

	if(leaves) CreateLeaves(result, leafParam);

	return result;
}

void GraphicsClass::CreateTree(Transform transform, Parameters param, Parameters leafParam)
{
	Transform result = CreateTrunk(transform, param, 3, true, leafParam);
	//CreateLeaves(result, param);
}

void GraphicsClass::CreateBush(Transform transform, Parameters param)
{
	int numStems = 50;
	for (int i = 0; i < numStems; i++)
	{
		Transform temp = transform;
		temp.pos.x += RandFloat(param.lowerBound, param.upperBound);
		temp.pos.z += RandFloat(param.lowerBound, param.upperBound);
		temp = CreateTrunk(temp, trunk_param, 3, true, param);
		CreateLeaves(temp, leaf_param);
	}
}

void GraphicsClass::PopModel(bool clearAll)
{
	m_objects.back()->Shutdown();
	delete m_objects.back();
	m_objects.pop_back();

	if (clearAll)
	{
		while (m_objects.size() > 1)
		{
			m_objects.back()->Shutdown();
			delete m_objects.back();
			m_objects.pop_back();
		}
	}
}

void GraphicsClass::ExportModel()
{
	//because i really don't want to deal with assimp right now

	//to-do: export to folder, export procedural texture

	ofstream obj_file, mtl_file, tex_file;
	string name = export_path;
	string timestamp = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
	int indexOffset = 0;

	//timestamp if name is empty
	if (name == "")
	{
		name = "exported_model_" + timestamp;
	}

	//write the obj first

	obj_file.open("export/" + name + ".obj");
	mtl_file.open("export/" + name + ".mtl");

	int objects = m_objects.size() - 1;

	for (int i = 0; i < objects; i++)
	{
		std::vector<ModelClass::VertexType> vertices = m_objects[objects - i]->GetVertices();
		std::vector<unsigned long> indices = m_objects[objects - i]->GetIndices();
		//string texname = m_objects[objects - i]->GetTextureName();
		string texname = "export/" + name;
		texname += std::to_string(i);
		texname += ".tga";

		//write out the tga as binary
		tex_file.open(texname, ios::out | ios::binary);

		TextureClass* tex = m_objects[objects - i]->GetTextureData();
		TextureClass::TargaHeader header;
		int h = tex->GetHeight();
		int w = tex->GetWidth();
		int size = h * w * 4;
		unsigned char* data;// = new unsigned char[size];

		header = tex->GetHeader();
		data = tex->GetData();

		tex_file.write((const char*)&header, sizeof(header));

		//now the data
		char* newData = new char[size];

		int k = (w * h * 4) - (w * 4);
		int index = 0;

		// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
		for (int j = 0; j<h; j++)
		{
			for (int i = 0; i<w; i++)
			{
				newData[index + 0] = data[k + 2];  // Red.
				newData[index + 1] = data[k + 1];  // Green.
				newData[index + 2] = data[k + 0];  // Blue
				newData[index + 3] = data[k + 3];  // Alpha

															 // Increment the indexes into the targa data.
				k += 4;
				index += 4;
			}

			// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
			k -= (w * 8);
		}

		tex_file.write(newData, size);

		tex_file.close();


		mtl_file << "#material information for object " + std::to_string(i) + "\n\n";
		mtl_file << "newmtl " + name + std::to_string(i) + "\n";
		mtl_file << "Ka 1.000 1.000 1.000\n";
		mtl_file << "Kd 1.000 1.000 1.000\n";
		mtl_file << "Ks 0.000 0.000 0.000\n";
		mtl_file << "d 1.0\n";
		mtl_file << "illum 2\n";
		mtl_file << "map_Ka " + texname + "\n";
		mtl_file << "map_Kd " + texname + "\n";
		mtl_file << "map_Ks " + texname;

		if (i < objects - 1 && objects > 1)
		{
			mtl_file << "\n\n";
		}

		obj_file << "#model information for object " + std::to_string(i) + "\n\n";

		obj_file << "#mtl file\n";
		obj_file << "mtllib " + name + ".mtl\n";
		obj_file << "usemtl " + name + std::to_string(i) + "\n\n";

		obj_file << "#vertex positions\n";

		for each(ModelClass::VertexType vertex in vertices)
		{
			string to_write = "v ";

			//coordinates
			to_write += std::to_string(vertex.position.x);
			to_write += " ";

			to_write += std::to_string(vertex.position.y);
			to_write += " ";

			to_write += std::to_string(vertex.position.z);
			to_write += " ";

			//colors
			to_write += std::to_string(vertex.color.x);
			to_write += " ";

			to_write += std::to_string(vertex.color.y);
			to_write += " ";

			to_write += std::to_string(vertex.color.z);
			to_write += "\n";

			obj_file << to_write;
		}

		obj_file << "\n#texture UVs\n";

		for each(ModelClass::VertexType vertex in vertices)
		{
			string to_write = "vt ";

			//texture UV
			to_write += std::to_string(vertex.texture.x);
			to_write += " ";

			to_write += std::to_string(1.0f - vertex.texture.y);
			to_write += "\n";

			obj_file << to_write;
		}

		obj_file << "\n#normals\n";

		for each(ModelClass::VertexType vertex in vertices)
		{
			string to_write = "vn ";

			//normals
			to_write += std::to_string(vertex.normal.x);
			to_write += " ";

			to_write += std::to_string(vertex.normal.y);
			to_write += " ";

			to_write += std::to_string(vertex.normal.z);
			to_write += "\n";

			obj_file << to_write;
		}

		obj_file << "\n#indices";

		for (int i = 0; i < indices.size(); i += 3)
		{
			string to_write = "\nf";

			for (int j = 0; j < 3; j++)
			{
				to_write += " ";
				to_write += std::to_string(indices[i + j] + 1 + indexOffset);
				to_write += "/";
				to_write += std::to_string(indices[i + j] + 1 + indexOffset);
				to_write += "/";
				to_write += std::to_string(indices[i + j] + 1 + indexOffset);
			}

			obj_file << to_write;
		}

		if (i < objects - 1 && objects > 1)
		{
			obj_file << "\n\n";
		}

		indexOffset += vertices.size();

		//PopModel(false);

	}

	obj_file.close();
	mtl_file.close();

}

void TW_CALL Trunk(void *p)
{
	GraphicsClass* parent = static_cast<GraphicsClass*>(p);
	parent->PopModel(true);
	parent->CreateTrunk(parent->new_transform, parent->trunk_param, parent->trunk_param.recursion, false, parent->leaf_param);
}

void TW_CALL Leaf(void *p)
{
	GraphicsClass* parent = static_cast<GraphicsClass*>(p);
	parent->PopModel(true);
	parent->CreateLeaves(parent->new_transform, parent->leaf_param);
}

void TW_CALL Tree(void *p)
{
	GraphicsClass* parent = static_cast<GraphicsClass*>(p);
	parent->PopModel(true);
	parent->CreateTree(parent->new_transform, parent->trunk_param, parent->leaf_param);
	//parent->CreateLeaves(leaves, parent->leaf_param);
}

void TW_CALL Bush(void *p)
{
	GraphicsClass* parent = static_cast<GraphicsClass*>(p);
	parent->PopModel(true);
	//parent->CreateMoreLeaves(parent->m_param.transform, parent->m_param.segments, parent->m_param.rings);
}

void TW_CALL AltBush(void *p)
{
	GraphicsClass* parent = static_cast<GraphicsClass*>(p);
	parent->PopModel(true);
	parent->CreateBush(parent->new_transform, parent->trunk_param);
}

void TW_CALL Rose(void *p)
{
	GraphicsClass* parent = static_cast<GraphicsClass*>(p);
	parent->PopModel(true);
	parent->CreateRose(parent->new_transform, parent->rose_param);
}

void TW_CALL CallExport(void *p)
{
	GraphicsClass* parent = static_cast<GraphicsClass*>(p);
	parent->ExportModel();
}

void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
{
	// Copy the content of souceString handled by the AntTweakBar library to destinationClientString handled by your application
	destinationClientString = sourceLibraryString;
}

void GraphicsClass::SetTweakBar(TwBar* bar)
{
	TwCopyStdStringToClientFunc(CopyStdStringToClient);

	TwAddVarRW(bar, "Light ambient", TW_TYPE_COLOR4F, &m_light->m_light.ambientColor, " group='Light settings' ");
	TwAddVarRW(bar, "Light diffuse", TW_TYPE_COLOR4F, &m_light->m_light.diffuseColor, " group='Light settings' ");
	TwAddVarRW(bar, "Light direction", TW_TYPE_DIR3F, &m_light->m_light.lightDirection, " group='Light settings' ");

	//TwAddVarRW(bar, "Model position", TW_TYPE_DIR3F, &m_param.transform.pos, NULL);
	TwAddVarRW(bar, "Model rotation", TW_TYPE_DIR3F, &new_transform.rot, " group='Spawn settings' ");
	TwAddVarRW(bar, "Model scale", TW_TYPE_DIR3F, &new_transform.scale, " group='Spawn settings' ");
	TwAddVarRW(bar, "Model color", TW_TYPE_COLOR4F, &new_transform.color, " group='Spawn settings' ");

	TwAddVarRW(bar, "L base color", TW_TYPE_COLOR3F, &leaf_tex_param.baseCol, " group='Leaf texture' ");
	TwAddVarRW(bar, "L red var", TW_TYPE_FLOAT, &leaf_tex_param.baseVar.x, " group='Leaf texture' ");
	TwAddVarRW(bar, "L blue var", TW_TYPE_FLOAT, &leaf_tex_param.baseVar.y, " group='Leaf texture' ");
	TwAddVarRW(bar, "L green var", TW_TYPE_FLOAT, &leaf_tex_param.baseVar.z, " group='Leaf texture' ");
	TwAddVarRW(bar, "L alt color", TW_TYPE_COLOR3F, &leaf_tex_param.altCol, " group='Leaf texture' ");
	TwAddVarRW(bar, "L alt red var", TW_TYPE_FLOAT, &leaf_tex_param.altVar.x, " group='Leaf texture' ");
	TwAddVarRW(bar, "L alt blue var", TW_TYPE_FLOAT, &leaf_tex_param.altVar.y, " group='Leaf texture' ");
	TwAddVarRW(bar, "L alt green var", TW_TYPE_FLOAT, &leaf_tex_param.altVar.z, " group='Leaf texture' ");
	TwAddVarRW(bar, "L texture path", TW_TYPE_STDSTRING, &leaf_tex_param.path, " group='Leaf texture' ");
	TwAddVarRW(bar, "L use texture", TW_TYPE_BOOLCPP, &leaf_tex_param.loadTex, " group='Leaf texture' ");

	TwAddVarRW(bar, "T base color", TW_TYPE_COLOR3F, &trunk_tex_param.baseCol, " group='Trunk texture' ");
	TwAddVarRW(bar, "T red var", TW_TYPE_FLOAT, &trunk_tex_param.baseVar.x, " group='Trunk texture' ");
	TwAddVarRW(bar, "T blue var", TW_TYPE_FLOAT, &trunk_tex_param.baseVar.y, " group='Trunk texture' ");
	TwAddVarRW(bar, "T green var", TW_TYPE_FLOAT, &trunk_tex_param.baseVar.z, " group='Trunk texture' ");
	TwAddVarRW(bar, "T alt color", TW_TYPE_COLOR3F, &trunk_tex_param.altCol, " group='Trunk texture' ");
	TwAddVarRW(bar, "T alt red var", TW_TYPE_FLOAT, &trunk_tex_param.altVar.x, " group='Trunk texture' ");
	TwAddVarRW(bar, "T alt blue var", TW_TYPE_FLOAT, &trunk_tex_param.altVar.y, " group='Trunk texture' ");
	TwAddVarRW(bar, "T alt green var", TW_TYPE_FLOAT, &trunk_tex_param.altVar.z, " group='Trunk texture' ");
	TwAddVarRW(bar, "T texture path", TW_TYPE_STDSTRING, &trunk_tex_param.path, " group='Trunk texture' ");
	TwAddVarRW(bar, "T use texture", TW_TYPE_BOOLCPP, &trunk_tex_param.loadTex, " group='Trunk texture' ");

	TwDefine(" Options/'Leaf texture'   group=Leaf ");
	TwDefine(" Options/'Trunk texture'   group=Trunk ");

	TwAddVarRW(bar, "T roundness", TW_TYPE_INT32, &trunk_param.rings, " min=3 max=20 group=Trunk ");
	TwAddVarRW(bar, "T height", TW_TYPE_INT32, &trunk_param.segments, " min=1 max=50 group=Trunk ");
	TwAddVarRW(bar, "T recursion", TW_TYPE_INT32, &trunk_param.recursion, " min=0 max=5 group=Trunk ");
	TwAddVarRW(bar, "T seg height", TW_TYPE_FLOAT, &trunk_param.segHeight, " min=0.05 max=5 step=0.01 group=Trunk ");
	TwAddVarRW(bar, "T start width", TW_TYPE_FLOAT, &trunk_param.startWidth, " min=0.1 max=5 step=0.01 group=Trunk ");
	TwAddVarRW(bar, "T end width", TW_TYPE_FLOAT, &trunk_param.endWidth, " min=0 max=5 step=0.01 group=Trunk ");
	TwAddVarRW(bar, "T random upper", TW_TYPE_FLOAT, &trunk_param.upperBound, " min=-10 max=10 step=0.01 group=Trunk ");
	TwAddVarRW(bar, "T random lower", TW_TYPE_FLOAT, &trunk_param.lowerBound, " min=-10 max=10 step=0.01 group=Trunk ");
	TwAddButton(bar, "Create trunk", Trunk, this, " group=Trunk ");

	TwAddVarRW(bar, "Leaves", TW_TYPE_INT32, &leaf_param.rings, " min=2 max=15 group=Leaf ");
	TwAddVarRW(bar, "L segments", TW_TYPE_INT32, &leaf_param.segments, " min=1 max=15 group=Leaf ");
	TwAddVarRW(bar, "L seg height", TW_TYPE_FLOAT, &leaf_param.segHeight, " min=0.1 max=5 step=0.01 group=Leaf ");
	//TwAddVarRW(bar, "L start width", TW_TYPE_FLOAT, &leaf_param.startWidth, " min=0 max=1 step=0.01 group=Leaf ");
	TwAddVarRW(bar, "L random upper", TW_TYPE_FLOAT, &leaf_param.upperBound, " min=-10 max=10 step=0.01 group=Leaf ");
	TwAddVarRW(bar, "L random lower", TW_TYPE_FLOAT, &leaf_param.lowerBound, " min=-10 max=10 step=0.01 group=Leaf ");
	TwAddVarRW(bar, "L upper X", TW_TYPE_FLOAT, &leaf_param.upperBoundX, " min=-10 max=10 step=0.01 group=Leaf ");
	TwAddVarRW(bar, "L lower X", TW_TYPE_FLOAT, &leaf_param.lowerBoundX, " min=-10 max=10 step=0.01 group=Leaf ");
	TwAddVarRW(bar, "L upper Y", TW_TYPE_FLOAT, &leaf_param.upperBoundY, " min=-10 max=10 step=0.01 group=Leaf ");
	TwAddVarRW(bar, "L lower Y", TW_TYPE_FLOAT, &leaf_param.lowerBoundY, " min=-10 max=10 step=0.01 group=Leaf ");
	TwAddVarRW(bar, "L upper Z", TW_TYPE_FLOAT, &leaf_param.upperBoundZ, " min=-10 max=10 step=0.01 group=Leaf ");
	TwAddVarRW(bar, "L lower Z", TW_TYPE_FLOAT, &leaf_param.lowerBoundZ, " min=-10 max=10 step=0.01 group=Leaf ");
	TwAddButton(bar, "Create leaves", Leaf, this, " group=Leaf ");
	TwAddButton(bar, "Create tree (trunk + leaves)", Tree, this, " group=Models ");
	//TwAddButton(bar, "Create bush", AltBush, this, " group=Models ");

	TwDefine(" Options/Trunk   group=Models ");
	TwDefine(" Options/Leaf   group=Models ");

	//TwAddSeparator(bar, "", NULL);

	TwAddVarRW(bar, "Export path", TW_TYPE_STDSTRING, &export_path, " group=Export ");
	TwAddButton(bar, "Export model", CallExport, this, " group=Export ");

	TwAddSeparator(bar, "", NULL);

	TwAddButton(bar, "Click and hold to pan view", NULL, NULL, NULL);
	TwAddButton(bar, "WASD to move", NULL, NULL, NULL);
	TwAddButton(bar, "SPACE/CTRL for height", NULL, NULL, NULL);

	TwDefine(" Options/'Light settings' opened=false ");
	TwDefine(" Options/'Spawn settings' opened=false ");
	TwDefine(" Options/Models opened=false ");
	TwDefine(" Options/Trunk opened=false ");
	TwDefine(" Options/Leaf opened=false ");
	TwDefine(" Options/'Trunk texture' opened=false ");
	TwDefine(" Options/'Leaf texture' opened=false ");
	TwDefine(" Options/Export opened=false ");
}