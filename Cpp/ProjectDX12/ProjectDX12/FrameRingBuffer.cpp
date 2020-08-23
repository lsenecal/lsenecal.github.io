#include "FrameRingBuffer.h"

using namespace DirectX;

FrameRingBuffer::FrameRingBuffer(ID3D12Device * device, UINT countFrameConstants, UINT countObjectConstants, UINT countMaterialConstants)
{
	DX::ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandListAllocator.GetAddressOf())));

	objectConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(device, countObjectConstants, true);
	frameConstantBuffer = std::make_unique<UploadBuffer<FrameConstants>>(device, countFrameConstants, true);
	matConstantBuffer = std::make_unique<UploadBuffer<MaterialConstants>>(device, countMaterialConstants, false);
}

FrameRingBuffer::~FrameRingBuffer()
{
}