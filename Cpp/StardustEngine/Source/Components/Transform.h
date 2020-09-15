#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	std::vector<glm::vec3> orientation = { glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0) };
};
