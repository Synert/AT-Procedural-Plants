#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include "textureclass.h"

using namespace DirectX;

class ModelClass
{
public:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, std::vector<VertexType>, std::vector<unsigned long>, char*, TexParam);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

	TextureClass* GetTextureData();
	std::vector<VertexType> GetVertices();
	std::vector<unsigned long> GetIndices();
	std::string GetTextureName();

private:
	bool InitializeBuffers(ID3D11Device*, std::vector<VertexType>, std::vector<unsigned long>);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*, TexParam);
	void ReleaseTexture();

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	std::string m_texname;

	std::vector<VertexType> m_vertices;
	std::vector<unsigned long> m_indices;

	//not implemented yet
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;
};