#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;

out vec4 gfColor;
out vec3 gPos;
out vec3 gNormal;

uniform mat4 view;
uniform mat4 projection;



void main()
{
    gfColor = aColor;
	gPos = aPos;
	
	//gl_Position = projection * view * vec4(aPos, 1.0);
	gl_Position =  vec4(aPos, 1.0);
	gNormal = normalize(aNormal);
}
