#pragma once
#include <stb_image.h>

#include "../Camera.h"
#include "Shader.h"

#include <map>
#include <vector>
#include <GLFW/glfw3.h>

class Skybox
{
public:
	Skybox(){}
	Skybox(Shader * shader);
	~Skybox();

	void render(Camera * camera);
	unsigned int loadCubemap(std::vector<std::string> faces);
	void setNextThemes();
	int getIndexCurrentTheme() { return _currentTheme; }
	void setNameSky(std::string str) { nameSky = str; }
	std::string getNameSky() { return nameSky; }

private:
	GLuint _vao, _vbo;
	unsigned int _cubemapTextureID;
	std::map <std::string, unsigned int> _themes;
	int _currentTheme;
	std::string nameSky;
	Shader _shader;
};

