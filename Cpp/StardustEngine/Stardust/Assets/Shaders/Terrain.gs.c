#version 440 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3 ) out;

in vec4 gfColor[];
in vec3 gPos[];
in vec3 gNormal[];

uniform mat4 view;
uniform mat4 projection;

out vec4 fColor;
out vec3 Pos;
out vec3 Normal;

const float MAGNITUDE = 0.2;

void GenerateLine(int index)
{
	fColor = vec4(0.0,1.0,1.0,1.0);
	Pos = gPos[index];
	Normal = normalize(cross(gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz, gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz));//gNormal[index];

	gl_Position = projection * view *gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = projection * view *(gl_in[index].gl_Position + vec4(Normal, 0.0) * MAGNITUDE);
	EmitVertex();
	EndPrimitive();
}

void main()
{
	//GenerateLine(0); // first vertex normal
	//GenerateLine(1); // second vertex normal
	//GenerateLine(2); // third vertex normal


	//vec3 U = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	//vec3 V = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

	//Normal.x = U.y * V.z - U.z * V.y;
	//Normal.y = U.z * V.x - U.x * V.z;
	//Normal.z = U.x * V.y - U.y * V.x;

	Normal = normalize(gNormal[0]);

	fColor = gfColor[0];
	Pos = gPos[0];
	//Normal = gNormal[0];

	gl_Position = projection * view *gl_in[0].gl_Position;
	EmitVertex();

	Normal = normalize(gNormal[1]);

	fColor = gfColor[1];
	Pos = gPos[1];
	//Normal = gNormal[1];
	gl_Position = projection * view *gl_in[1].gl_Position;
	EmitVertex();


	Normal = normalize(gNormal[2]);

	fColor = gfColor[2];
	Pos = gPos[2];
	//Normal = gNormal[2];
	gl_Position = projection * view *gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();

}
