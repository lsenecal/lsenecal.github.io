#define PI 3.14159265359

struct Light
{
	float3 Strength;
	float FallOffStart;
	float3 Position;
	float FallOffEnd;
};

float3 SchlickFresnel(float3 H, float3 V, float3 F0)
{
	float cos = max(dot(H, V), 0.0f);

	return F0 + (1 - F0) * pow(1 - cos, 5.0f);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;

	float NdotH = max(dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float3 N, float3 V, float roughness)
{
	float NdotV = max(dot(N, V), 0.0f);
	float r = (roughness + 1.0f);
	float k = (r*r) / 8.0f;

	float num = NdotV;
	float denom = NdotV * (1 - k) + k;

	return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float GL = GeometrySchlickGGX(N, L, roughness);
	float GV = GeometrySchlickGGX(N, V, roughness);

	return GL * GV;
}

float3 NormalMapToWorldSpace(float3 normalMap, float3 normal, float3 tangent)
{
	float3 normalT = 2.0f * normalMap - 1.0f;

	float3 N = normal;
	float3 T = normalize(tangent - dot(tangent, N)*N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	return mul(normalT, TBN);
}