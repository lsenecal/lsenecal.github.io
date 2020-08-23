#pragma once
#include "Shader.h"

#include <unordered_map>
#include <string>

class ShaderManager
{
public :
	ShaderManager();

	// properly de-allocates all loaded shaders
	~ShaderManager();
	
	// load a new shader to our storage if we find it
	void add(std::string name, bool withGeometyShader = false);

	// return the shader by name
	Shader * get(std::string name);
	
private:
	// Storage
	std::unordered_map <std::string, Shader *> _shaders;

	// load and generate a shader from file
	Shader * loadShaderFromFile(std::string name, bool withGeometryShader);
};

