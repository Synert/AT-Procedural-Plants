#include "textureclass.h"
#include <algorithm>

//perlin noise implementation is from https://github.com/sol-prog/Perlin_Noise
//GPL V3

int Blend(int a, int b)
{
	float a2 = (float)a / 255.0f;
	float b2 = (float)b / 255.0f;

	float result;

	if (b2 < 0.5f)
	{
		result = (2.0f * a2 * b2) + (a2 * a2) * (1.0f - (2.0f * b2));
	}
	else
	{
		result = (2.0f * a2) * (1.0f - b2) + sqrt(a2) * ((2.0f * b2) - 1.0f);
	}
	
	result *= 255.0f;
	return (int)result;
}

int RandIntTex(int min, int max)
{
	int result = min;
	int dif = max - min;

	if (dif == 0)
	{
		return result;
	}

	result += (rand() % dif);

	return result;
}

TextureClass::TextureClass()
{
	m_texture = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, TexParam param)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	//should the texture be generated?
	if (param.type != GenType::NONE && !param.loadTex)
	{
		m_param = param;
		CreateTarga(height, width);
	}
	else
	{
		// Load the targa image data into memory.
		result = LoadTarga((char*)param.path.c_str(), height, width);
		if (!result)
		{
			return false;
		}
	}

	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// Generate mipmaps for this texture.
	deviceContext->GenerateMips(m_textureView);

	// Release the targa image data now that the image data has been loaded into the texture.
	//delete[] m_targaData;
	//m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture view resource.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// Release the texture.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// Release the targa data.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}

	return;
}

bool TextureClass::LoadTarga(char* filename, int& height, int& width)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (j = 0; j<height; j++)
	{
		for (i = 0; i<width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

														 // Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;
	m_header = targaFileHeader;
	m_width = width;
	m_height = height;

	return true;
}

void TextureClass::CreateTarga(int& height, int& width)
{

	switch ((int)m_param.type)
	{
	case (int)GenType::LEAF:
		CreateLeaf(height, width);
		break;
	case (int)GenType::BARK:
		CreateBark(height, width);
		break;
	}

}


void TextureClass::CreateLeaf(int& height, int& width)
{
	int imageSize, index, i, j, k;

	m_width = 256;
	m_height = 256;

	m_header.bpp = (char)32;
	m_header.height = m_height;
	m_header.width = m_width;

	for (int a = 0; a < 12; a++)
	{
		m_header.data1[a] = (char)0;
	}

	m_header.data1[2] = (char)2;
	m_header.data2 = (char)8;

	// Get the important information from the header.
	height = m_height;
	width = m_width;

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];

	unsigned char* tempData = new unsigned char[imageSize];

	//now the part where i attempt to make textures

	//first, set up the temp data to be read in
	//this is mostly for my own benefit so i can make it the right way up
	//used to be bytes but that was causing weird issues
	int r, g, b, a;
	int Nr, Ng, Nb, Na;


	//starting out with a plain green to test
	g = 0;
	b = 0;
	r = 0;
	a = 255;

	Nr = 0;
	Nb = 0;
	Ng = 0;
	Na = 255;

	int thickness = m_param.thickness;

	//convert parameters
	int baseRed = (int)(m_param.baseCol.x * 255.0f);
	int baseGreen = (int)(m_param.baseCol.y * 255.0f);
	int baseBlue = (int)(m_param.baseCol.z * 255.0f);

	int baseRedVar = (int)(m_param.baseVar.x);
	int baseGreenVar = (int)(m_param.baseVar.y);
	int baseBlueVar = (int)(m_param.baseVar.z);

	int altRed = (int)(m_param.altCol.x * 255.0f);
	int altGreen = (int)(m_param.altCol.y * 255.0f);
	int altBlue = (int)(m_param.altCol.z * 255.0f);

	int altRedVar = (int)(m_param.altVar.x);
	int altGreenVar = (int)(m_param.altVar.y);
	int altBlueVar = (int)(m_param.altVar.z);

	//generate the base
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int below = abs((width / 2) - x) * 2;

			//outline
			if (abs(y - below) < thickness || y > (height - thickness * 2))
			{
				Nr = RandIntTex(altRed - altRedVar, altRed + altRedVar);
				Nb = RandIntTex(altBlue - altBlueVar, altBlue + altBlueVar);
				Ng = RandIntTex(altGreen - altGreenVar, altGreen + altGreenVar);
				Na = 0;
			}
			//everything else
			else if ((y + thickness) < below)
			{
				Nr = RandIntTex(altRed - altRedVar, altRed + altRedVar);
				Nb = RandIntTex(altBlue - altBlueVar, altBlue + altBlueVar);
				Ng = RandIntTex(altGreen - altGreenVar, altGreen + altGreenVar);
				Na = 0;
			}
			//inside
			else
			{
				Nr = RandIntTex(baseRed - baseRedVar, baseRed + baseRedVar);
				Nb = RandIntTex(baseBlue - baseBlueVar, baseBlue + baseBlueVar);
				Ng = RandIntTex(baseGreen - baseGreenVar, baseGreen + baseGreenVar);
				Na = 255;
			}

			int greenDif = (Ng - g) / 25;
			int blueDif = (Nb - b) / 25;
			int redDif = (Nr - r) / 25;
			int alphaDif = (Na - a) / 25;

			g += greenDif;
			b += blueDif;
			r += redDif;
			a += alphaDif;

			if (g > 255) g = 255;
			if (g < 0) g = 0;
			if (r > 255) r = 255;
			if (r < 0) r = 0;
			if (b > 255) b = 255;
			if (b < 0) b = 0;

			int index = (x + y * width) * 4;

			tempData[index + 0] = (char)b;
			tempData[index + 1] = (char)g;
			tempData[index + 2] = (char)r;
			tempData[index + 3] = (char)a;
		}
	}

	//now generate 'spots'
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if ((x + y) % 5 < 2 || x % 2 == 0)
			{
				int below = abs((width / 2) - x) * 2;

				if (abs(y - x) < thickness + RandIntTex(-5, 7))
				{
					//skip
				}

				else
				{
					int index = (x + y * width) * 4;

					b = (int)tempData[index + 2];
					g = (int)tempData[index + 1];
					r = (int)tempData[index + 0];
					a = (int)tempData[index + 3];

					b *= 1.1;
					g *= 1.1;
					r *= 1.1;

					if (g > 255) g = 255;
					if (g < 0) g = 0;
					if (r > 255) r = 255;
					if (r < 0) r = 0;
					if (b > 255) b = 255;
					if (b < 0) b = 0;

					tempData[index + 0] = (char)b;
					tempData[index + 1] = (char)g;
					tempData[index + 2] = (char)r;
					tempData[index + 3] = (char)a;
				}
			}
		}
	}

	//noise pass
	for (int x = width - 1; x >= 0; x--)
	{
		for (int y = 0; y < height; y++)
		{
			if (((x + y) % RandIntTex(5, 7) < RandIntTex(2, 7) || x % RandIntTex(2, 4) == 0))
			{
				int below = abs((width / 2) - x) * 2;

				if (abs(y - x) < thickness + RandIntTex(-8, 10))
				{
					//skip
				}

				else
				{

					//outline
					if (abs(y - below) < thickness || y > (height - thickness * 2))
					{
						Nr = RandIntTex(altRed - altRedVar, altRed + altRedVar);
						Nb = RandIntTex(altBlue - altBlueVar, altBlue + altBlueVar);
						Ng = RandIntTex(altGreen - altGreenVar, altGreen + altGreenVar);
						Na = 0;
					}
					//everything else
					else if ((y + thickness) < below)
					{
						Nr = RandIntTex(altRed - altRedVar, altRed + altRedVar);
						Nb = RandIntTex(altBlue - altBlueVar, altBlue + altBlueVar);
						Ng = RandIntTex(altGreen - altGreenVar, altGreen + altGreenVar);
						Na = 0;
					}
					//inside
					else
					{
						Nr = RandIntTex(baseRed - baseRedVar, baseRed + baseRedVar);
						Nb = RandIntTex(baseBlue - baseBlueVar, baseBlue + baseBlueVar);
						Ng = RandIntTex(baseGreen - baseGreenVar, baseGreen + baseGreenVar);
						Na = 255;
					}

					if (abs(y - x) < thickness)
					{
						Nr = RandIntTex(altRed - altRedVar, altRed + altRedVar);
						Nb = RandIntTex(altBlue - altBlueVar, altBlue + altBlueVar);
						Ng = RandIntTex(altGreen - altGreenVar, altGreen + altGreenVar);
						Na = 255;
					}

					int greenDif = (Ng - g) / 5;
					int blueDif = (Nb - b) / 5;
					int redDif = (Nr - r) / 5;
					int alphaDif = (Na - a) / 5;

					g += greenDif;
					b += blueDif;
					r += redDif;
					a += alphaDif;

					if (g > 255) g = 255;
					if (g < 0) g = 0;
					if (r > 255) r = 255;
					if (r < 0) r = 0;
					if (b > 255) b = 255;
					if (b < 0) b = 0;

					int index = (x + y * width) * 4;

					tempData[index + 0] = (char)b;
					tempData[index + 1] = (char)g;
					tempData[index + 2] = (char)r;
					tempData[index + 3] = (char)a;
				}
			}
		}
	}

	//darkened line pass

	int lineFreq = RandIntTex(7, 20);
	for (int x = width - 1; x >= 0; x--)
	{
		for (int y = 0; y < height; y++)
		{
			if ((x + y) % lineFreq < RandIntTex(1, 3))
			{

				if (abs(y - x) < thickness + RandIntTex(-5, 17))
				{
					//skip
				}

				else
				{

					int index = (x + y * width) * 4;

					b = (int)tempData[index + 0];
					g = (int)tempData[index + 1];
					r = (int)tempData[index + 2];
					a = (int)tempData[index + 3];

					b *= 1.1;
					g *= 1.1;
					r *= 1.1;

					if (g > 255) g = 255;
					if (g < 0) g = 0;
					if (r > 255) r = 255;
					if (r < 0) r = 0;
					if (b > 255) b = 255;
					if (b < 0) b = 0;

					tempData[index + 0] = (char)b;
					tempData[index + 1] = (char)g;
					tempData[index + 2] = (char)r;
					tempData[index + 3] = (char)a;
				}
			}
		}
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	for (j = 0; j<height; j++)
	{
		for (i = 0; i<width; i++)
		{
			m_targaData[index + 0] = tempData[index + 2];  // Red.
			m_targaData[index + 1] = tempData[index + 1];  // Green.
			m_targaData[index + 2] = tempData[index + 0];  // Blue
			m_targaData[index + 3] = tempData[index + 3];  // Alpha

														 // Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	delete[] tempData;
	tempData = 0;
}

void TextureClass::CreateBark(int& height, int& width)
{
	int imageSize, index, i, j, k;

	m_width = 128;
	m_height = 128;

	m_header.bpp = (char)32;
	m_header.height = m_height;
	m_header.width = m_width;

	for (int a = 0; a < 12; a++)
	{
		m_header.data1[a] = (char)0;
	}

	m_header.data1[2] = (char)2;
	m_header.data2 = (char)8;

	// Get the important information from the header.
	height = m_height;
	width = m_width;

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];

	unsigned char* tempData = new unsigned char[imageSize];

	//now the part where i attempt to make textures

	//first, set up the temp data to be read in
	//this is mostly for my own benefit so i can make it the right way up
	//used to be bytes but that was causing weird issues
	int r, g, b, a;
	int Nr, Ng, Nb, Na;


	//starting out with a plain green to test
	g = 0;
	b = 0;
	r = 0;
	a = 255;

	Nr = 0;
	Nb = 0;
	Ng = 0;
	Na = 255;

	int thickness = m_param.thickness;

	//convert parameters
	int baseRed = (int)(m_param.baseCol.x * 255.0f);
	int baseGreen = (int)(m_param.baseCol.y * 255.0f);
	int baseBlue = (int)(m_param.baseCol.z * 255.0f);

	int baseRedVar = (int)(m_param.baseVar.x);
	int baseGreenVar = (int)(m_param.baseVar.y);
	int baseBlueVar = (int)(m_param.baseVar.z);

	int altRed = (int)(m_param.altCol.x * 255.0f);
	int altGreen = (int)(m_param.altCol.y * 255.0f);
	int altBlue = (int)(m_param.altCol.z * 255.0f);

	int altRedVar = (int)(m_param.altVar.x);
	int altGreenVar = (int)(m_param.altVar.y);
	int altBlueVar = (int)(m_param.altVar.z);

	PerlinNoise pn = PerlinNoise(rand());
	int lineFreq = RandIntTex(7, 20);

	//generate the base
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			double n = 80 * pn.noise((double)x / (double)width, (double)y / (double)height, 0.0);
			n = n - floor(n);
			n *= 0.25;
			n += 0.75;

			n -= (double)sin(XM_PI * 16 * x);

			Nr = RandIntTex(baseRed - baseRedVar, baseRed + baseRedVar);
			Nb = RandIntTex(baseBlue - baseBlueVar, baseBlue + baseBlueVar);
			Ng = RandIntTex(baseGreen - baseGreenVar, baseGreen + baseGreenVar);
			Na = 255;

			int greenDif = (Ng - g) / 5;
			int blueDif = (Nb - b) / 5;
			int redDif = (Nr - r) / 5;
			int alphaDif = (Na - a) / 5;

			g += greenDif;
			b += blueDif;
			r += redDif;
			a += alphaDif;

			int index = (x + y * width) * 4;

			int newb = (int)floor(b * n);
			int newr = (int)floor(r * n);
			int newg = (int)floor(g * n);

			n = 1 * pn.noise((double)x / (double)(width / 6), (double)y / (double)(height / 6), 0.0);

			Nr = RandIntTex(altRed - altRedVar, altRed + altRedVar);
			Nb = RandIntTex(altBlue - altBlueVar, altBlue + altBlueVar);
			Ng = RandIntTex(altGreen - altGreenVar, altGreen + altGreenVar);
			Na = 255;

			Nr = (int)floor(Nr * n);
			Nb = (int)floor(Nb * n);
			Ng = (int)floor(Ng * n);

			newg = Blend(newg, Ng);
			newb = Blend(newb, Nb);
			newr = Blend(newr, Nr);

			if ((x % lineFreq < thickness))
			{
				newb *= 0.75;
				newg *= 0.75;
				newr *= 0.75;
			}

			n = 1 * pn.noise((double)x / (double)(width / 120), (double)y / (double)(height / 120), 0.0);

			newg *= n;
			newb *= n;
			newr *= n;

			if (newg > 255) newg = 255;
			if (newg < 0) newg = 0;
			if (newr > 255) newr = 255;
			if (newr < 0) newr = 0;
			if (newb > 255) newb = 255;
			if (newb < 0) newb = 0;

			tempData[index + 0] = (char)newb;
			tempData[index + 1] = (char)newg;
			tempData[index + 2] = (char)newr;
			tempData[index + 3] = (char)a;
		}
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	for (j = 0; j<height; j++)
	{
		for (i = 0; i<width; i++)
		{
			m_targaData[index + 0] = tempData[index + 2];  // Red.
			m_targaData[index + 1] = tempData[index + 1];  // Green.
			m_targaData[index + 2] = tempData[index + 0];  // Blue
			m_targaData[index + 3] = tempData[index + 3];  // Alpha

														   // Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	delete[] tempData;
	tempData = 0;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

TextureClass::TargaHeader TextureClass::GetHeader()
{
	return m_header;
}

unsigned char* TextureClass::GetData()
{
	return m_targaData;
}

int TextureClass::GetWidth()
{
	return m_width;
}

int TextureClass::GetHeight()
{
	return m_height;
}