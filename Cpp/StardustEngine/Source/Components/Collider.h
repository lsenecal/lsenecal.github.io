#pragma once
#include <vector>
#include <glm/glm.hpp>

enum colliderName { sphere, box, capsule, mesh };
enum colliderType { Static, Dynamic, Kinematic };

struct SphereCollider {
	glm::vec3 center = glm::vec3(0.0, 0.0, 0.0);
	float radius = 1.0f;
};

struct BoxCollider {
	glm::vec3 center = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 size = glm::vec3(1.0, 1.0, 1.0);
};

struct CapsuleCollider {
	glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 dir = glm::vec3(1.0f, 0.0f, 0.0f);
	//glm::vec3 centerUp = glm::vec3(0.0, 0.0, 0.0);
	//glm::vec3 centerDown = glm::vec3(0.0, 0.0, 0.0);
	float radius;
};

struct TriangleCollider {
	glm::vec3 pointA = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 pointB = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 pointC = glm::vec3(0.0f, 0.0f, 1.0f);
};

inline bool operator==(const TriangleCollider & t1, const TriangleCollider & t2) {
	if (t1.pointA == t2.pointA && t1.pointB == t2.pointB && t1.pointC == t2.pointC)
		return true;
	else
		return false;
};

struct MeshCollider {
	std::vector<TriangleCollider> list;
};

struct Collider {
	colliderName name;
	colliderType type = Static;

	std::shared_ptr<SphereCollider> SphereCollider;
	std::shared_ptr<BoxCollider> BoxCollider;
	std::shared_ptr<CapsuleCollider> CapsuleCollider;
	//std::shared_ptr<MeshCollider> MeshCollider;
};

struct ColliderHierarchy {
	Collider broadCollider;
	std::vector<Collider> colliders;
};