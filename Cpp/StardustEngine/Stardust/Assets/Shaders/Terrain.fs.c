#version 440 core
out vec4 FragColor;

in vec4 fColor;
in vec3 Pos;
in vec3 Normal;


uniform float u_time;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
//uniform vec3 objectColor;

void main()
{
	//lightColor = vec3(1.0, 1.0, 1.0);
	// ambient
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * lightColor * fColor.xyz;

	// diffuse 
	vec3 norm = normalize(Normal);
	//if(norm.y < 0.0) norm = -norm;
	vec3 lightDir = normalize(lightPos - Pos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor * fColor.xyz;

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - Pos);
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
	vec3 specular = specularStrength * spec * lightColor * fColor.xyz;

	vec3 result = (ambient + diffuse + specular);
	FragColor = vec4(result, 1.0);
	
	//FragColor = vec4((sin(Pos.x) + 1.0) / 2.0, (cos(Pos.y) + 1.0) / 2.0, (cos(Pos.z) + 1.0) / 2.0, 1.0);
	//FragColor = fColor;
    //FragColor = vec4(smoothstep(100, 0, Pos.x), smoothstep(50,0,Pos.y), smoothstep(100, 0, Pos.z),1.0);
	//FragColor = vec4(0.0, 84.0/255, 0.0, 1.0);

}
