#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec2 TexCoords;
	vec3 FragPos;

	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;

	mat3 TBN;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_normal3;

uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_cameraPosition;
uniform vec3 u_cameraFront;
uniform float u_time;

void main()
{    
    vec4 color = texture(texture_diffuse1, fs_in.TexCoords);
	vec3 normal = texture(texture_normal3, fs_in.TexCoords).rgb;
	normal = normalize(normal*2.0-1.0);


	if(color.a < 0.5) discard;
	//if(distance(u_cameraPosition, fs_in.FragPos) > 100) discard;
	color.a = 1.0;

	// ambient
	float ambientStrength = 0.05;
	vec3 ambient = ambientStrength * color.rgb;

	// diffuse
	vec3 lightDir = fs_in.TBN * normalize(u_lightPos - fs_in.FragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color.rgb;

	// specular
	vec3 viewDir = fs_in.TBN * normalize(u_cameraPosition - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
	vec3 specular = 0.5 * color.rgb * spec;
	

	color.xyz = ambient + (1.0 - clamp(distance(u_lightPos, fs_in.FragPos)/150.0, 0.0, 1.0)) * (diffuse + specular);

	FragColor = color;
}