#include "GravitySystem.h"
#include "../../Components/Transform.h"
#include "../../Components/Physics.h"
#include "../../Core/Coordinator.h"
#include "../constantes.h"
#include <GLFW/glfw3.h>

extern Coordinator g_coordinator;

void GravitySystem::update()
{
	float h = (float)physicsInterval; // pas d'integration

	//std::vector<glm::vec3> y(2 * _entities.size());
	//std::vector<glm::vec3> gt(2 * _entities.size());
	
	int indice = 0;

	for (Entity e : _entities) {
		/*Transform & transform = g_coordinator.getComponent<Transform>(e);
			float mass = g_coordinator.getComponent<RigidBody>(e).mass;

			glm::vec3 ForceSum = glm::vec3(0.0, 0.0, 0.0);

			float & y = transform.position.y;

			y = (float)(-0.5*g*t*t);*/
		
			//y.at(indice) = g_coordinator.getComponent<Transform>(e).position;
			//y.at(indice + _entities.size()) = g_coordinator.getComponent<RigidBody>(e).velocity;

			//gt.at(indice) = y.at(indice + _entities.size());

			glm::vec3 forceSum = glm::vec3(0.0, 0.0, 0.0);

			// Calcul de la sommes des forces
			for (Entity e2 : _entities) {
				if (e2 != e) {
					//glm::vec3 r = y.at(indice) - g_coordinator.getComponent<Transform>(e2).position;
					glm::vec3 r = g_coordinator.getComponent<Transform>(e).position - g_coordinator.getComponent<Transform>(e2).position;
					float r2 = glm::dot(r, r);
					glm::vec3 dir = glm::normalize(r);
					float forceNorm = 0;
					if (g_coordinator.getComponent<RigidBody>(e).useGravity) {
						forceNorm += (float)(-k2 * time_factor * time_factor * g_coordinator.getComponent<RigidBody>(e2).mass / (r2 - EPSILON));
					}
					dir *= forceNorm;
					forceSum += dir;
				}
			}
			//gt.at(indice + _entities.size()) = forceSum;

			g_coordinator.getComponent<RigidBody>(e).sumForce = forceSum;


			//Euler
			/*Transform & transform = g_coordinator.getComponent<Transform>(e);
			RigidBody & rigidbody = g_coordinator.getComponent<RigidBody>(e);
			transform.position = y.at(indice) + h * gt.at(indice);
			rigidbody.velocity = y.at(indice + _entities.size()) + h * gt.at(indice + _entities.size());*/

			indice++;
		
	}
}

