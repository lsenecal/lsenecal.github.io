#pragma once
#include <glm/glm.hpp>
#include <string>

struct Renderable
{
	glm::vec3 color;
	std::string shader_name;
	bool withGeometryShader;
};