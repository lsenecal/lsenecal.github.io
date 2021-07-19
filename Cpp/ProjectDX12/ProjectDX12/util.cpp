#include "util.h"
#include <comdef.h>

using Microsoft::WRL::ComPtr;

DX::Exception::Exception(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
	ErrorCode(hr),
	FunctionName(functionName),
	Filename(filename),
	LineNumber(lineNumber)
{
}

std::wstring DX::Exception::ToString()const
{
	// Get the string description of the error code.
	_com_error err(ErrorCode);
	std::wstring msg = err.ErrorMessage();

	return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
}

float clamp(float value, float min, float max)
{
	return value < min ? min : (value > max ? max : value);
}

void loadTexture(
	std::string textureName,
	ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList,
	std::unordered_map<std::string, std::unique_ptr<Texture>> &textures,
	std::wstring albedoFileName,
	std::wstring MetallicFileName,
	std::wstring RoughnessFileName,
	std::wstring NormalFileName) 
{

	auto Albedo = std::make_unique<Texture>();
	auto Normal = std::make_unique<Texture>();
	auto Metallic = std::make_unique<Texture>();
	auto Roughness = std::make_unique<Texture>();

	Albedo->FileName = albedoFileName;
	Albedo->Name = textureName + "Albedo";
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
		device,
		commandList, Albedo->FileName.c_str(),
		Albedo->Resource,
		Albedo->Upload
	));


	Normal->FileName = NormalFileName;
	Normal->Name = textureName + "Normal";
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
		device,
		commandList, Normal->FileName.c_str(),
		Normal->Resource,
		Normal->Upload
	));


	Metallic->FileName = MetallicFileName;
	Metallic->Name = textureName + "Metallic";
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
		device,
		commandList, Metallic->FileName.c_str(),
		Metallic->Resource,
		Metallic->Upload
	));

	Roughness->FileName = NormalFileName;
	Roughness->Name = textureName + "Roughness";
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
		device,
		commandList, Roughness->FileName.c_str(),
		Roughness->Resource,
		Roughness->Upload
	));

	textures.insert({ Albedo->Name, std::move(Albedo) });
	textures.insert({ Normal->Name, std::move(Normal) });
	textures.insert({ Metallic->Name, std::move(Metallic) });
	textures.insert({ Roughness->Name, std::move(Roughness) });
}

/*
Adapted from http://www.songho.ca/opengl/gl_sphere.html
*/

void CreateSphere(std::vector<Vertex>& vSphere, std::vector<UINT>& iSphere, UINT stackCount, UINT sectorCount, float radius)
{
	float sectorStep = 2.0f * DirectX::XM_PI / sectorCount;
	float stackStep = DirectX::XM_PI / stackCount;
	float stackAngle, sectorAngle;

	float x, y, z, xz;
	float nx, ny, nz, lengthInv = 1.0f / radius;
	float s, t;
	float tx, ty, tz;

	for (size_t i = 0; i <= stackCount; ++i)
	{
		stackAngle = i * stackStep;
		xz = radius * sinf(stackAngle);
		y = radius * cosf(stackAngle);

		for (size_t j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;

			x = xz * cosf(sectorAngle);
			z = xz * sinf(sectorAngle);

			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			s = (float)j / sectorCount;
			t = (float)i / stackCount;

			tx = -radius * sinf(stackAngle) * sinf(sectorAngle);
			tz = radius * sinf(stackAngle) * cosf(sectorAngle);
			ty = 0.0f;

			Vertex v;
			v.Pos = { x, y, z };
			v.Normal = { nx, ny, nz };
			v.TexCoordinates = { s, t };
			v.Tangent = { tx, ty, tz };

			vSphere.push_back(v);
		}
	}

	int k1, k2;

	for (size_t i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;

		for (size_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				iSphere.push_back(k1);
				iSphere.push_back(k1 + 1);
				iSphere.push_back(k2);
			}

			if (i != (stackCount + 1))
			{
				iSphere.push_back(k2);
				iSphere.push_back(k1 + 1);
				iSphere.push_back(k2 + 1);
			}
		}
	}

}