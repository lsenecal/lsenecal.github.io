#version 440 core
out vec4 FragColor;

in vec3 fColor;
in vec3 FragPos;
in vec3 Normal;

uniform float u_time;

void main()
{
	vec3 lightColor = vec3(1.0,1.0,1.0);
	//vec3 Normal = vec3(-1.0,0.0,0.0);
	vec3 lightPos = vec3(-20 * sin(u_time), 25*sin(u_time/2) ,-100 + 20  * cos(u_time));
	// ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor * 4;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec3 result = (ambient + diffuse) * vec3(fColor);
    FragColor = vec4(result, 1.0);



    //FragColor = vec4(fColor);
}
