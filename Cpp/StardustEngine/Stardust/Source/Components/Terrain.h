#pragma once

#include "../../Vendor/include/glm/glm.hpp"
#include <vector>

struct Terrain
{
	int marchingID;
	std::vector<glm::vec3>  points;
	std::vector<glm::vec3>  normale;
	std::vector<glm::vec4>  colors;
	
};