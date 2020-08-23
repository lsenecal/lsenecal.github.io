#include "Common.hlsl"

//L : local
struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexCoord : TEXCOORD;
};

//H : Homogeneous

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;

	vOut.PosL = vIn.PosL;

	float4 posW = mul(float4(vIn.PosL, 1.0f), World);

	posW.xyz += camPos.xyz;

	vOut.PosH = mul(posW, ViewProj);

	return vOut;
};

float4 PS(VertexOut vOut) : SV_Target
{
	return SkyMap.Sample(samplerLinearWrap, vOut.PosL);
}