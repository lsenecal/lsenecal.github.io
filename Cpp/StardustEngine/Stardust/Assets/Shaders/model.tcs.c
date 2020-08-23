#version 440 core

layout(vertices = 3) out;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_cameraPosition;

const float LOD_DISTANCE = 75.0f;

in VS_OUT
{
	vec2 TexCoords;
	vec3 FragPos;
} tcs_in[];

out TCS_OUT
{
	vec2 TexCoords;
	vec3 FragPos;
} tcs_out[];


void main(void)
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	tcs_out[gl_InvocationID].TexCoords = tcs_in[gl_InvocationID].TexCoords;
	tcs_out[gl_InvocationID].FragPos = tcs_in[gl_InvocationID].FragPos;

	if (gl_InvocationID == 0)
	{
		mat4 vp = u_projection * u_view;
		precise vec4 p0 = vp * gl_in[0].gl_Position;
		precise vec4 p1 = vp * gl_in[1].gl_Position;
		precise vec4 p2 = vp * gl_in[2].gl_Position;
		p0 /= p0.w;
		p1 /= p1.w;
		p2 /= p2.w;
		if (p0.z <= 0.0 && p1.z <= 0.0 && p2.z <= 0.0 || 
			distance(gl_in[gl_InvocationID].gl_Position.xyz, u_cameraPosition) > LOD_DISTANCE)
		{
			gl_TessLevelOuter[0] = 0.0;
			gl_TessLevelOuter[1] = 0.0;
			gl_TessLevelOuter[2] = 0.0;
			gl_TessLevelInner[0] = 0.0; // | 
		}
		else {
			gl_TessLevelOuter[0] = 1.0; // |
			gl_TessLevelOuter[1] = 1.0; // --
			gl_TessLevelOuter[2] = 1.0; //   |
			gl_TessLevelInner[0] = 1.0; // | 
		}
	}
}