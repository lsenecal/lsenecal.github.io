#version 440 core

layout(triangles, equal_spacing) in;

in TCS_OUT
{	
	vec2 TexCoords;
	vec3 FragPos;
} tes_in[];

out TES_OUT
{
	vec2 TexCoords;
	vec3 FragPos;
} tes_out;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_time;


void main(void)
{
	// interpolate texture coordinates
	vec2 tc = (gl_TessCoord.x * tes_in[0].TexCoords) +
			(gl_TessCoord.y * tes_in[1].TexCoords) +
			(gl_TessCoord.z * tes_in[2].TexCoords);
	// interpolate world position 
	vec4 world_position = ((gl_TessCoord.x * gl_in[0].gl_Position) +
				 (gl_TessCoord.y * gl_in[1].gl_Position) +
				 (gl_TessCoord.z * gl_in[2].gl_Position));
	// re-upload information
	tes_out.TexCoords = tc;
	tes_out.FragPos = world_position.xyz;
	// set gl position
	gl_Position = u_projection * u_view * world_position;
}