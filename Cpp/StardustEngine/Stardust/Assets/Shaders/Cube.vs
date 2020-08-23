#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in vec3 aColor;
layout (location = 4) in vec3 aRotation;
layout (location = 5) in vec3 aDimension;

out vec3 fColor;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;

mat4 rotationX( in float angle ) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

mat4 rotationY( in float angle ) {
	return mat4(	cos(angle),		0,		sin(angle),	0,
			 				0,		1.0,			 0,	0,
					-sin(angle),	0,		cos(angle),	0,
							0, 		0,				0,	1);
}

mat4 rotationZ( in float angle ) {
	return mat4(	cos(angle),		-sin(angle),	0,	0,
			 		sin(angle),		cos(angle),		0,	0,
							0,				0,		1,	0,
							0,				0,		0,	1);
}

mat4 scale( in vec3 sc){
	return mat4(	sc.x,		0,		0,		0,
			 		   0,	 sc.y,		0,		0,
					   0,		0,		sc.z,	0,
					   0,		0,		0,		1);

}

void main()
{

    fColor = aColor;
	
	mat4 model = scale(aDimension) * rotationX(radians(aRotation.x)) * rotationY(radians(aRotation.y)) * rotationZ(radians(aRotation.z));

	//mat4 model = mat4(1.0);
	model[3] = vec4(aOffset, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = aNormal;
}
