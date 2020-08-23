#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
	for (auto shader : _shaders)
	{
		glDeleteProgram(shader.second->ID);
		delete(shader.second);
	}
}


void ShaderManager::add(std::string name, bool withGeometryShader)
{
	Shader * shader = loadShaderFromFile(name, withGeometryShader);
	if (shader != nullptr) {
		_shaders[name] = shader;
	}
	else
	{
		std::cerr << "ShaderManager:: The shader with the name : " << name << " was not find." << std::endl;
	}
}

Shader * ShaderManager::get(std::string name)
{
	return _shaders[name];
}


Shader * ShaderManager::loadShaderFromFile(std::string name, bool withGeometryShader)
{
	Shader * shader;
	std::string vs = ("Assets/Shaders/" + name + ".vs");
	std::string gs = ("Assets/Shaders/" + name + ".gs");
	std::string fs = ("Assets/Shaders/" + name + ".fs");
	//std::cout << vs << " " << fs << std::endl;
	std::cout << withGeometryShader << std::endl;
	if (withGeometryShader)
		shader = new Shader(vs.c_str(), gs.c_str(), fs.c_str());
	else
		shader = new Shader(vs.c_str(), fs.c_str());
	return shader;
}
