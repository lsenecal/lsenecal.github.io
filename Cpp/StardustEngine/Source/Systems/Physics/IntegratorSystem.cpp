#include "IntegratorSystem.h"
#include "../constantes.h"
#include "../../Components/Transform.h"
#include "../../Components/Physics.h"
#include "../../Components/Collider.h"
#include <glm/glm.hpp>

void IntegratorSystem::update()
{
	float h = (float)physicsInterval; // pas d'integration

	for (Entity e : _entities) {

		if (g_coordinator.getComponent<Collider>(e).type == Dynamic) {

			g_coordinator.getComponent<Transform>(e).position += h * g_coordinator.getComponent<RigidBody>(e).velocity;
			g_coordinator.getComponent<RigidBody>(e).velocity += h * g_coordinator.getComponent<RigidBody>(e).sumForce;

			g_coordinator.getComponent<Transform>(e).rotation += h * g_coordinator.getComponent<RigidBody>(e).angularVelocity;
		}
	}
}
