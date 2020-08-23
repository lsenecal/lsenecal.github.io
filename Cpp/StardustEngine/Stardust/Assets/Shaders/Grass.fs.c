#version 440 core
out vec4 FragColor;

in GS_OUT{
	float isBlade;
	float age;
	float attenuation1;
	float attenuation2;
	vec2 textCoord;
	vec3 normal;
	vec3 FragPos;
}fs_in;

uniform sampler2D u_texture1; // grass atlas

uniform vec3 u_cameraPosition;
uniform vec3 u_cameraFront;
uniform float u_time;

float random (vec2 st) {
    return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123);
}

void main()
{
	// to uniformize
	vec3 lightPos = u_cameraPosition; // vec3(10, 5, 10);// vec3(10, 2, 10);
	vec3 viewPos = u_cameraPosition;
	vec3 lightColor = vec3(0.8f);
	
	vec4 color;
	int number_elmt_h = 2;
	int number_elmt_w = 4;
	int layerx = int(fs_in.attenuation1 * number_elmt_w);
	int layery = int(round(fs_in.attenuation2+0.25));
	vec2 uv = vec2((fs_in.textCoord.x + layerx) / number_elmt_w, (fs_in.textCoord.y + layery) / number_elmt_h);

	if(fs_in.isBlade < 0.5f){

		//if(fs_in.age+0.25 > 0.55) color =  mix(texture(t_bloom, fs_in.textCoord), vec4(0.0, 0.0, 0.0, 1.0), 0.35f);
		//else { color =  mix(texture(t_billboard, fs_in.textCoord), vec4(0.0, 0.0, 0.0, 1.0), 0.35f);
		//color =  mix(color, vec4(0.0, 0.0, 0.0, 1.0f), fs_in.age);}

		//if(fs_in.attenuation > 0.5) 
			color = texture(u_texture1, uv);//mix(texture(t_bloom, fs_in.textCoord), vec4(0.0, 0.0, 0.0, 1.0), 0.35f);
		//else color = texture(u_texture2, fs_in.textCoord);//mix(texture(t_bloom, fs_in.textCoord), vec4(0.0, 0.0, 0.0, 1.0), 0.35f);

		if(color.a < 0.65) discard;
		//color =  mix(color, vec4(147.0/255.0, 81.0/255.0, 22.0/255.0, 1.0f), (1-fs_in.age)/2.0);

		// ambient
		float ambientStrength = 0.8f;
		vec3 ambient = ambientStrength * lightColor* color.rgb;

		// diffuse
		vec3 norm = fs_in.normal;
		vec3 lightDir = normalize(lightPos - fs_in.FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor * color.rgb;
		if (layery == 1) diffuse = diffuse / 1.5;
		// specular
		/*vec3 viewDir = normalize(viewPos - fs_in.FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0f);
		vec3 specular = texture(u_texture2, uv).rgb * spec * lightColor;
		*/

		color.xyz = ambient + diffuse; // +specular / 5.0f;
	}
	else
	{
		//color = texture(t_blade, fs_in.textCoord);// , vec4(0.0, 0.0, 0.0, 1.0), 0.35f);
		//color =  vec4(18.0/255.0, 86.0/255.0, 64.0/255.0, 1.0f);
		//color = vec4(0 / 255.0, 0 / 255.0, 0 / 255.0, 1.0f);
		color =  vec4(37.0/255.0, 112.0/255.0, 17.0/255.0, 1.0f);
		//color = mix(color, vec4(0.0, 0.0, 0.0, 1.0),(1-fs_in.age)/1.75);
		color = 0.25*mix(color, vec4(0.0, 0.0, 0.0, 1.0), fs_in.age);
		color.w = 1.0f;


		// ambient
		float ambientStrength = 0.1f;
		vec3 ambient = ambientStrength * lightColor;

		// diffuse
		vec3 norm = fs_in.normal;
		vec3 lightDir = normalize(lightPos - fs_in.FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor * color.rgb;

		// specular
		vec3 viewDir = normalize(viewPos - fs_in.FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
		vec3 specular = 0.25 * spec * lightColor;


		color.xyz = ambient + diffuse;
	}
	
	
	
	FragColor = vec4(color.rgb, 1.0);
}