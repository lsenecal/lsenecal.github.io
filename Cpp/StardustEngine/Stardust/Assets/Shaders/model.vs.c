#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec3 aOffset;
layout (location = 6) in vec3 aRotation;
layout (location = 7) in vec3 aScale;

out VS_OUT{
	vec2 TexCoords;
	vec3 FragPos;

	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;

	mat3 TBN;
} vs_out;

uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_lightPos;
uniform vec3 u_cameraPosition;

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

void main()
{
	mat4 m = mat4(1.0f);
	m[0][0] *= aScale.x;
	m[1][1] *= aScale.y;
	m[2][2] *= aScale.z;
	m[3] = vec4(aOffset, 1.0);
	m *= rotationX(radians(aRotation.x)) * rotationY(radians(aRotation.y)) * rotationZ(radians(aRotation.z));

	vec4 position = m * vec4(aPos, 1.0);

	mat3 normalMatrix = transpose(inverse(mat3(m)));
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(cross(N, T));
	
	mat3 TBN = transpose(mat3(T, B, N));
	vs_out.TangentLightPos = TBN * u_lightPos;
	vs_out.TangentViewPos = TBN * u_cameraPosition;
	vs_out.TangentFragPos = TBN * position.xyz;

	vs_out.TexCoords = aTexCoords;
	vs_out.FragPos = position.xyz;
	//vs_out.TangentLightPos = normalize(vec3(m * vec4(aNormal, 0.0)));
	/*vec3 T = normalize(vec3(u_view * m * vec4(aTangent, 0.0)));
	vec3 N = normalize(vec3(u_view * m * vec4(aNormal, 0.0)));
	vec3 B = normalize(vec3(u_view * m * vec4(aBitangent, 0.0)));*/
	vs_out.TBN = TBN;

	gl_Position = u_projection * u_view * position;
}