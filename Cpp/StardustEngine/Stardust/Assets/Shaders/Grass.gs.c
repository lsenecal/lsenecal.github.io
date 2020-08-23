#version 440 core
layout(points) in;
layout(triangle_strip, max_vertices = 16) out;

in VS_OUT{
	vec3 normal;
	vec3 tangent;
	vec3 patchDimension;
} gs_in[];

out GS_OUT{
	float isBlade;
	float age;
	float attenuation1;
	float attenuation2;
	vec2 textCoord;
	vec3 normal;
	vec3 FragPos;
} gs_out;

// *******************************************************************
//                            UNIFORM
// *******************************************************************

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform vec3 u_cameraPosition;
uniform vec3 u_cameraFront;

uniform float u_time;

uniform sampler2D u_texture0;

// *******************************************************************
//                             CONST
// *******************************************************************

#define PI 3.141592653589793

// color shade
const float shade[7] = float[7](0.575, 0.575, 0.675, 0.675, 0.750, 0.750, 0.875);

// texture coordinate
const vec2 textCoord[7] = vec2[7](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 0.5), vec2(1.0, 0.5), vec2(0.0, 0.5), vec2(1.0, 0.5), vec2(0.5, 1.0));

// edges of LOD
const float LOD1 = 500.0f;
const float LOD2 = 1000.0f;
const float LOD3 = 1500.0f;

// *******************************************************************
//                            UTILS
// *******************************************************************

mat4 rotationX( in float angle ) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

mat4 rotationY( in float angle )
{
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

float random (vec2 st) {
    return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123);
}


float noise (in vec2 _st) {
    vec2 i = floor(_st);
    vec2 f = fract(_st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

// *******************************************************************
//                            BILLBOARD
// *******************************************************************
void createBillboards(vec4 position, float less_details, vec4 wind)
{
	vec4 vertexPosition[4];
	vec2 texture[4];

	// down left
	vertexPosition[0] = vec4(-gs_in[0].patchDimension.x/2.0, 0.0, 0.0, 0.0);
	texture[0] = vec2(0.0, 0.0);

	// down right
	vertexPosition[1] = vec4(gs_in[0].patchDimension.x/2.0, 0.0, 0.0, 0.0);
	texture[1] = vec2(1.0, 0.0);

	// up left
	vertexPosition[2] = vec4(-gs_in[0].patchDimension.x/2.0, gs_in[0].patchDimension.y/2.0, 0.0, 0.0);
	texture[2] = vec2(0.0, 1.0);

	// up right
	vertexPosition[3] = vec4(gs_in[0].patchDimension.x/2.0, gs_in[0].patchDimension.y/2.0, 0.0, 0.0);
	texture[3] = vec2(1.0, 1.0);

	mat4 modelGlobal = rotationY(random(position.zx)*PI);
	mat4 modelQuad =  (rotationX(wind.x*PI*0.75f - PI*0.25f) * rotationZ(wind.y*PI*0.75f - PI*0.25f)); 
	float size = random(position.xz) * 0.66f + 0.34f; // for random size

	mat4 model = mat4(1.0f);
	gs_out.attenuation1 = noise(position.xz);
	gs_out.attenuation2 = noise(position.zx);

	// emit vertices
	for(int k = 0; k < less_details; ++k)
	{
		if(k == 1) model = rotationY(radians(45));
		if(k == 2) model = rotationY(-radians(45));
		for(int i = 0; i < 4; ++i)
		{
			vec4 offset;
			if(i==2 || i==3) offset = position + modelGlobal*modelQuad*model*(vertexPosition[i]*size); 
			else offset = position + modelGlobal*model*vertexPosition[i]*size; 

			gl_Position = u_projection * u_view * u_model * offset;
			gs_out.textCoord = texture[i];
			gs_out.FragPos = offset.xyz;
			gs_out.isBlade = 0.0f;
			EmitVertex();
		}
		EndPrimitive();
	}
}

// *******************************************************************
//                             SOIL
// *******************************************************************
void createSoil(vec4 position)
{
	vec4 vertexPosition[4];
	vec2 texture[4];
	// down left
	vertexPosition[0] = vec4(-gs_in[0].patchDimension.x/2.0, 0, -gs_in[0].patchDimension.z/2.0, 0.0);
	texture[0] = vec2(0.0, 0.0);

	// down right
	vertexPosition[1] = vec4(gs_in[0].patchDimension.x/2.0, 0, -gs_in[0].patchDimension.z/2.0, 0.0);
	texture[1] = vec2(1.0, 0.0);

	// up left
	vertexPosition[2] = vec4(-gs_in[0].patchDimension.x/2., 0, +gs_in[0].patchDimension.z/2.0, 0.0);
	texture[2] = vec2(0.0, 1.0);

	// up right
	vertexPosition[3] = vec4(gs_in[0].patchDimension.x/2.0, 0, +gs_in[0].patchDimension.z/2.0, 0.0);
	texture[3] = vec2(1.0, 1.0);

	for(int i = 0; i < 4; ++i)
	{
		vec4 offset = position + (vertexPosition[i]*gs_in[0].patchDimension.x/2.0);
		gl_Position = u_projection * u_view * u_model * offset;
		gs_out.textCoord = texture[i];
		gs_out.FragPos = offset.xyz;
		gs_out.isBlade = 1.0f;
		EmitVertex();
	}
	EndPrimitive();
}


// *******************************************************************
//                             MAIN
// *******************************************************************
void main()
{
	vec4 position = gl_in[0].gl_Position;
	vec3 distance_with_camera = position.xyz - u_cameraPosition;
	
	// angle
	vec3 u = normalize(u_cameraFront.xyz);
	vec3 v = normalize(distance_with_camera.xyz);
	float angle = acos(dot(u, v))*180.0f;

	//if (angle > -125 && angle < 125) {

		// wind
		vec2 windDirection = vec2(1.0, 0.0); float windStrength = 0.15f;
		vec2 uv = position.xz / 10.0f + windDirection * windStrength * u_time;
		vec4 wind = texture(u_texture0, uv);

		// initialize out variables
		gs_out.age = windStrength * (wind.x*windDirection.x + windDirection.y*wind.y);
		gs_out.normal = gs_in[0].normal;

		// distance of position to camera
		float dist_length = length(distance_with_camera);
		float t = 6.0f; if (dist_length > LOD2) t *= 1.5f;
		dist_length += (random(position.xz)*t - t / 2.0f);

		// change number of quad function of distance
		float lessDetails = 3;
		if (dist_length > LOD1) lessDetails = 2;
		if (dist_length > LOD2) lessDetails = 1;
		if (dist_length > LOD3) lessDetails = 0;

		// create grass
		if (lessDetails != 1
			|| (lessDetails == 1 && (int(position.x * 10) % 1) == 0 || (int(position.z * 10) % 1) == 0)
			|| (lessDetails == 2 && (int(position.x * 5) % 1) == 0 || (int(position.z * 5) % 1) == 0)
			)
			createBillboards(position, lessDetails, wind);

		//gs_out.FragPos = position.xyz;

		// create soil
		//createSoil(position);
	//}
}