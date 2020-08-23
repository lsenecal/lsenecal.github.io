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

/*
Adapted from http://www.songho.ca/opengl/gl_sphere.html
*/

void CreateSphere(std::vector<Vertex>& vSphere, std::vector<UINT>& iSphere, UINT stackCount, UINT sectorCount, float radius)
{
	float sectorStep = 2.0f * XM_PI / sectorCount;
	float stackStep = XM_PI / stackCount;
	float stackAngle, sectorAngle;

	float x, y, z, xz;
	float nx, ny, nz, lengthInv = 1.0f / radius;
	float s, t;
	float tx, ty, tz;

	for (size_t i = 0; i <= stackCount; ++i)
	{
		stackAngle = /*XM_PI / 2.0f -*/ i * stackStep;
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
				//iSphere.push_back(k2);
				iSphere.push_back(k1 + 1);
				iSphere.push_back(k2);
			}

			if (i != (stackCount + 1))
			{
				iSphere.push_back(k2);
				iSphere.push_back(k1 + 1);
				//iSphere.push_back(k2);
				iSphere.push_back(k2 + 1);
			}
		}
	}

}