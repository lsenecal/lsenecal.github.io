#include "Util.hlsl"
#include "Common.hlsl"

//L : local
struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexCoord : TEXCOORD;
	float3 TangentU : TANGENT;
};

//H : Homogeneous
//W : World
struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD;
	float3 TangentW : TANGENT;
};

VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;

	float4 posW = mul(float4(vIn.PosL, 1.0f), World);
	vOut.PosW = posW.xyz;
	
	vOut.NormalW = mul(vIn.NormalL, (float3x3)World);
	vOut.TexCoord = vIn.TexCoord;
	vOut.TangentW = mul(vIn.TangentU, (float3x3)World);

	vOut.PosH = mul(posW, ViewProj);

	return vOut;
};

float4 PS(VertexOut vOut) : SV_Target
{
	MaterialData matData = g_MaterialData[MaterialIndex];

	uint diffuseMapIndex = matData.DiffuseMapIndex;
	uint mettalicMapIndex = matData.MetallicMapIndex;
	uint roughnessMapIndex = matData.RoughnessMapIndex;
	uint normalMapIndex = matData.NormalMapIndex;

	float3 Albedo = pow(TextureMap[diffuseMapIndex].Sample(samplerLinearWrap, vOut.TexCoord).rgb, 2.2f);

	float Metallic = TextureMap[mettalicMapIndex].Sample(samplerLinearWrap, vOut.TexCoord).r;

	float Roughness = TextureMap[roughnessMapIndex].Sample(samplerLinearWrap, vOut.TexCoord).r;

	float4 NormalMapSample = TextureMap[normalMapIndex].Sample(samplerAnisotropicWrap, vOut.TexCoord);
	float3 Normal = NormalMapToWorldSpace(NormalMapSample.rgb, normalize(vOut.NormalW), normalize(vOut.TangentW));

	float3 N = normalize(Normal);
	float3 V = normalize(camPos.xyz - vOut.PosW);
	float3 L = normalize(LightPosition.xyz - vOut.PosW);
	float3 H = normalize(V + L);

	float distance = length(LightPosition.xyz - vOut.PosW);
	float attenuation = 1.0f / (distance * distance + 0.001f);

	float3 radiance = LightColor.rgb * attenuation;

	float3 F0 = { 0.04f, 0.04f, 0.04f };
	F0 = lerp(F0, Albedo, Metallic);

	float NDF = DistributionGGX(N, H, Roughness);
	float G = GeometrySmith(N, V, L, Roughness);
	float3 F = SchlickFresnel(H, V, F0);

	float3 numerator = NDF * G * F;
	float denominator = 4.0f * max(dot(N, V), 0.0) * max(dot(N, L), 0.0f);
	float3 specular = numerator / max(denominator, 0.001f);

	float3 kS = F;
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;

	kD *= 1.0 - Metallic;

	float3 Lo = (kD * Albedo / PI + specular) * radiance * max(dot(N,L), 0.0f);

	float3 ambient = float3(0.03f, 0.03f, 0.03f) * Albedo;

	float3 color = ambient + Lo;

	color = color / (color + float3(1.0f, 1.0f, 1.0f));
	color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));

	float4 result = { color, 1.0f };

	return result;
};