/*Texture2D DiffuseMap : register(t1);
Texture2D MetallicMap : register(t2);
Texture2D RoughnessMap : register(t3);
Texture2D NormalMap : register(t4);*/

struct MaterialData 
{
	uint DiffuseMapIndex;
	uint MetallicMapIndex;
	uint RoughnessMapIndex;
	uint NormalMapIndex;
};

TextureCube SkyMap : register(t0);

Texture2D TextureMap[17] : register(t1);

StructuredBuffer<MaterialData> g_MaterialData : register(t0, space1);

SamplerState samplerLinearWrap : register(s0);
SamplerState samplerAnisotropicWrap : register(s1);

cbuffer cbO : register(b0)
{
	float4x4 World;
	uint MaterialIndex;
};

cbuffer cbF : register(b1)
{
	float4x4 ViewProj;

	float4 camPos;
	float4 LightColor;
	float4 LightPosition;
}