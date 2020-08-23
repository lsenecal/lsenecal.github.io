#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aMarchID;
layout (location = 2) in vec3 aColor;



out VS_OUT{
	float marchID;
	vec3 color;
} vs_out;

uniform mat4 view;
uniform mat4 projection;



void main()
{
    
	
	vs_out.marchID = aMarchID;
	vs_out.color = aColor;
	gl_Position = vec4(aPos , 1.0);
	
}
