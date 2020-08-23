#pragma once

#include "util.h"

// Helper class to manage resource in UPLOAD state (Constant Buffers for instance)

template <typename T>
class UploadBuffer {
public:
	static UINT computeConstantBufferSize(UINT size) 
	{
		return (size + 255) & ~255;
	}

	UploadBuffer(ID3D12Device* device, UINT count, bool isConstantBuffer) : m_isConstantBuffer(isConstantBuffer)
	{
		m_elementByteSize = m_isConstantBuffer ? computeConstantBufferSize(sizeof(T)) : sizeof(T);

		DX::ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_elementByteSize * count),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_UploadBuffer)));

		DX::ThrowIfFailed(m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedData)));
	}
	
	UploadBuffer(const UploadBuffer& UB) = delete;
	UploadBuffer& operator=(const UploadBuffer& UB) = delete;
	~UploadBuffer()
	{
		if (m_UploadBuffer != nullptr)
			m_UploadBuffer->Unmap(0, nullptr);
		
		m_MappedData = nullptr;
	}

	ID3D12Resource* GetResource()
	{
		return m_UploadBuffer.Get();
	}

	void CopyData(int index, const T& data)
	{
		memcpy(&m_MappedData[index * m_elementByteSize], &data, sizeof(T));
	}
private:
	UINT m_elementByteSize;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadBuffer;

	BYTE* m_MappedData = nullptr;;
	bool m_isConstantBuffer;
};