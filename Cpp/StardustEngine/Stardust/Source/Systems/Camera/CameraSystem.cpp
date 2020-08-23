#include "CameraSystem.h"
#include "../../Core/Coordinator.h"
#include <glm/glm.hpp>
#include "../../Components/Transform.h"
#include "../../Components/CameraComponent.h"
#include "../../Camera.h"

#include <iostream>

extern Coordinator g_coordinator;

void CameraSystem::update() {
	
	if (!_entities.empty()) {
		Entity e = *(_entities.begin());
		Camera * camera = g_coordinator.getComponent<CameraComponent>(e).camera;
		glm::vec3 & position = g_coordinator.getComponent<Transform>(e).position;
		glm::vec3 & rotation = g_coordinator.getComponent<Transform>(e).rotation;
		std::vector<glm::vec3> & orientation = g_coordinator.getComponent<Transform>(e).orientation;

		position = camera->Position;
		rotation = glm::vec3(camera->Xrotation, camera->Yrotation, camera->Zrotation);

		//std::cout << "position : " << position.x << " " << position.y << " " << position.z << std::endl;

	}
}

void CameraSystem::update2()
{
	if (!_entities.empty()) {
		Entity e = *(_entities.begin());

		Camera * camera = g_coordinator.getComponent<CameraComponent>(e).camera;
		glm::vec3 & position = g_coordinator.getComponent<Transform>(e).position;
		glm::vec3 & rotation = g_coordinator.getComponent<Transform>(e).rotation;
		std::vector<glm::vec3> & orientation = g_coordinator.getComponent<Transform>(e).orientation;

		camera->Position = position;
		camera->Xrotation = rotation.x;
		camera->Yrotation = rotation.y;
		camera->Zrotation = rotation.z;
	}
}
