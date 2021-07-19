#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <cassert>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <algorithm>
#include <windowsx.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include "DDSTextureLoader.h"
#include "d3dx12.h"
#include "timer.h"

const int g_FrameBufferCount = 2;

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoordinates;
	DirectX::XMFLOAT3 Tangent;
};

inline DirectX::XMFLOAT4X4 IDENTITYMATRIX() {
	return DirectX::XMFLOAT4X4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

namespace DX {
	inline std::wstring AnsiToWString(const std::string& str)
	{
		WCHAR buffer[512];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
		return std::wstring(buffer);
	}

	class Exception
	{
	public:
		Exception() = default;
		Exception(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

		std::wstring ToString()const;

		HRESULT ErrorCode = S_OK;
		std::wstring FunctionName;
		std::wstring Filename;
		int LineNumber = -1;
	};


	inline void ThrowIfFailed(HRESULT hr)
	{
		std::wstring wfn = AnsiToWString(__FILE__);
		std::wstring wfuncn = AnsiToWString(__FUNCTION__);
		if (FAILED(hr)) { throw Exception(hr, wfuncn, wfn, __LINE__); }
	}
}

struct Point
{
	LONG x;
	LONG y;
};

float clamp(float value, float min, float max);

// Informations of a mesh in an array of meshes

struct MeshOffset
{
	UINT IndexCount = 0;
	UINT IndexStart = 0;
	UINT VertexStart = 0;
};

struct MeshCollection
{
	std::string Name;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	UINT IndexCount = 0;

	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R32_UINT;
	UINT IndexBufferByteSize = 0;

	std::unordered_map<std::string, MeshOffset> IndivMesh;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.SizeInBytes = VertexBufferByteSize;
		vbv.StrideInBytes = VertexByteStride;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

struct Material
{
	std::string Name;

	UINT indexCB = -1;

	UINT indexSRV = -1;

	int changeFlag = g_FrameBufferCount;
};

struct Texture
{
	std::string Name;

	std::wstring FileName;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> Upload = nullptr;
};

class SRVCounter
{
private:
	UINT count = 0;

public:
	UINT access0()
	{
		return count;
	};
	UINT access1() 
	{
		return count++;
	};
	UINT access4()
	{
		UINT temp = count;
		count += 4;
		return temp;
	};
};

void loadTexture(
	std::string textureName,
	ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList,
	std::unordered_map<std::string,
	std::unique_ptr<Texture>> &textures,
	std::wstring albedoFileName,
	std::wstring MetallicFileName,
	std::wstring RoughnessFileName,
	std::wstring NormalFileName);

void CreateSphere(std::vector<Vertex>& vSphere, std::vector<UINT>& iSphere, UINT stackCount, UINT sectorCount, float radius);