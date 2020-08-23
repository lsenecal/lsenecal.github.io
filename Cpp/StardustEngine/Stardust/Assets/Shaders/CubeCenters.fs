#version 460 core
out vec4 FragColor;

in vec3 fcolor;

void main()
{
    //FragColor = vec4((cos(pow(fcolor.x,2))+1)/2.0,(cos(fcolor.y*10.0)+1)/2.0,(sin(fcolor.z)+1)/2.0, 1.0);   
    FragColor = vec4(sin(fcolor.x * 30), sin(fcolor.y * 30), 0.5, 1.0);   
	//FragColor = vec4((sin(fcolor.x)+1.0 )/ 2.0, (sin(fcolor.y)+1.0)/2.0, (sin(fcolor.z)+1.0)/2.0, 1.0);   
}