#pragma once

#include "util.h"
#include "UploadBuffer.h"

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoordinates;
	DirectX::XMFLOAT3 Tangent;
};

struct MaterialConstants
{
	UINT DiffuseMapIndex;
	UINT MetallicMapIndex;
	UINT RoughnessMapIndex;
	UINT NormalMapIndex;
};

struct ObjectConstants // Constants per objects
{
	DirectX::XMFLOAT4X4 World = IDENTITYMATRIX();
	UINT MaterialIndex = 0;
};

struct FrameConstants // Constants per render call for all objects
{
	DirectX::XMFLOAT4X4 ViewProj = IDENTITYMATRIX();
	DirectX::XMFLOAT4 CameraPosition = { 0.0f , 0.0f, 0.0f, 1.0f };
	
	DirectX::XMFLOAT4 LightColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 LightPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
};

struct FrameRingBuffer {

	FrameRingBuffer(ID3D12Device* device, UINT countFrameConstants, UINT countObjectConstants, UINT countMaterialConstants);
	FrameRingBuffer(const FrameRingBuffer&) = delete;
	FrameRingBuffer& operator=(const FrameRingBuffer&) = delete;
	~FrameRingBuffer();

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandListAllocator;

	std::unique_ptr<UploadBuffer<ObjectConstants>> objectConstantBuffer;
	std::unique_ptr<UploadBuffer<MaterialConstants>> matConstantBuffer;
	std::unique_ptr<UploadBuffer<FrameConstants>> frameConstantBuffer;

	UINT64 fence = 0;
};

void CreateSphere(
	std::vector<Vertex> & v,
	std::vector<UINT> & i,
	UINT stackCount = 24,
	UINT sectorCount = 48, 
	float radius = 1.0f
);
