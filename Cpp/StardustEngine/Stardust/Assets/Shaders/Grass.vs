#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aPatchDim;

out VS_OUT{
	vec3 normal;
	vec3 tangent;
	vec3 patchDimension;
}vs_out;


void main(void)
{    
	vs_out.patchDimension = aPatchDim;
	vs_out.normal = aNormal;
	vs_out.tangent = normalize(vec3(1, 0, aNormal.x/aNormal.z));
    gl_Position = vec4(aPos,1.0);
}