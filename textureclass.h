#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <stdio.h>
#include <vector>
#include "PerlinNoise.h"

using namespace DirectX;

enum GenType
{
	NONE,
	LEAF,
	BARK,
	STEM,
	ROSE
};

struct TexParam
{
	XMFLOAT4 baseCol;
	XMFLOAT4 baseVar;
	XMFLOAT4 altCol;
	XMFLOAT4 altVar;
	int thickness;
	GenType type;
	std::string path;
	bool loadTex;
};

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class TextureClass
{
public:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, TexParam);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	TargaHeader GetHeader();
	unsigned char* GetData();
	int GetWidth();
	int GetHeight();

private:
	bool LoadTarga(char*, int&, int&);
	void CreateTarga(int&, int&);
	void CreateLeaf(int&, int&);
	void CreateBark(int&, int&);
	//void CreateRose(int&, int&);

private:
	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	TargaHeader m_header;
	ID3D11ShaderResourceView* m_textureView;

	int m_width;
	int m_height;
	TexParam m_param;
};