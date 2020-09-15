#pragma once
#include <glm/glm.hpp>
#include <vector>

struct RigidBody {
	bool useGravity = true;
	float mass = 1.0;
	glm::vec3 velocity = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 angularVelocity = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 sumForce = glm::vec3(0.0, 0.0, 0.0);
};