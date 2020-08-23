#version 440 core
out vec4 FragColor;

in vec3 fColor;
in vec3 FragPos;
in vec3 Normal;

uniform float u_time;
float troncate(float x){
	return float(int((1000*x))) / 1000.0f;
}

float random (in vec2 st) {
    return troncate(float(fract(sin(dot(st.xy,
                         vec2(12.9898f,78.233f)))*
        43758.5453123f)));
}

// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0f, 0.0f));
    float c = random(i + vec2(0.0f, 1.0f));
    float d = random(i + vec2(1.0f, 1.0f));

    vec2 u = f * f * (3.0f - 2.0f * f);

    return troncate(float(mix(a, b, u.x) +
            (c - a)* u.y * (1.0f - u.x) +
            (d - b) * u.x * u.y));
}

float fbm (in vec2 st) {
    // Initial values
    float value = 0.0f;
    float amplitude = 0.5f;
    float frequency = 0.4f;
    //
    // Loop of octaves
    for (int i = 0; i < 2; i++) {
        value += troncate(amplitude *  troncate(noise(st)));
        st.x = troncate(st.x * 2.0f);
        st.y = troncate(st.y * 2.0f);
        amplitude = troncate(amplitude * 0.5f);
    }
    return troncate(value);
}

 float Perlin2D(in vec2 pos) {
	vec2 pos2;
	pos2.x = troncate(pos.x);
	pos2.y = troncate(pos.y);
		return troncate(noise(pos));
	 }


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

	if( true){
		FragColor = vec4(0.0,0.0,1.0,1.0);
	
		if(random(vec2(0.5487, 18.26984)) == 0.544){
			FragColor = vec4(1.0,0.0,1.0,1.0);
		
			if(noise(vec2(0.5487, 18.26984)) == 0.433){
				FragColor = vec4(1.0,0.0,0.0,1.0);
		
				if(Perlin2D(vec2(42.172954, 72.812)) == 0.836){
					FragColor = vec4(1.0,1.0,1.0,1.0);
				}
			}
		}
	}

    //FragColor = vec4(fColor);
}
