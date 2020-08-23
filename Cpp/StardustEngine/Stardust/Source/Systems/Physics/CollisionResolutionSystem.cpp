#include "CollisionResolutionSystem.h"
#include <glm/glm.hpp>
#include "../../Core/Coordinator.h"
#include "../../Components/Collider.h"
#include "../../Components/Transform.h"
#include "../../Components/Physics.h"
#include "CollisionData.h"
#include "../constantes.h"

extern Coordinator g_coordinator;

void CollisionResolutionSystem::resolve(std::vector<CollisionReport> pairs)
{
	if (!pairs.empty()) {
		for (unsigned int i = 0; i < pairs.size(); i++) {
			Entity e = pairs.at(i).getFirstEntity();
			Entity f = pairs.at(i).getLastEntity();

			const glm::vec3 & normal_e = pairs.at(i).getNormal();
			const glm::vec3 & normal_f = -pairs.at(i).getNormal();
			const float & depth = pairs.at(i).getDepth();

			const ColliderHierarchy & CHe = g_coordinator.getComponent<ColliderHierarchy>(e);
			const ColliderHierarchy & CHf = g_coordinator.getComponent<ColliderHierarchy>(f);

			if (CHe.colliders.at(pairs.at(i).getI()).type != Static && CHf.colliders.at(pairs.at(i).getJ()).type != Static) {

				g_coordinator.getComponent<Transform>(e).position -= depth / 2.0f * normal_e;
				g_coordinator.getComponent<Transform>(f).position -= depth / 2.0f * normal_f;
			}
			else if (CHe.colliders.at(pairs.at(i).getI()).type == Static) {
				g_coordinator.getComponent<Transform>(f).position -= depth * normal_f;
			}
			else if (CHf.colliders.at(pairs.at(i).getJ()).type == Static) {
				g_coordinator.getComponent<Transform>(e).position -= depth * normal_e;
			}
		}
	}
}
