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