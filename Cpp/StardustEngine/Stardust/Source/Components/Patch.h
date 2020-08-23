#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Patch
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 dimension;
};

struct Patches
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> dimensions;
};