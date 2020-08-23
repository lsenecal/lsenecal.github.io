#pragma once

#include "util.h"

struct RenderedObject
{
	RenderedObject() = default;
	
	DirectX::XMFLOAT4X4 World = IDENTITYMATRIX();

	UINT index = -1; // constant buffer

	int changeFlag = g_FrameBufferCount;

	MeshCollection * meshCollection = nullptr;
	Material * mat = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT IndexCount = 0; // number of vertex
	UINT StartIndexLocation = 0; // start in the "big" vertex buffer
	UINT BaseVertexLocation = 0; 
};