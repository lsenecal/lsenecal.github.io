#pragma once
#include "../../Core/System.h"
#include "../../Components/Transform.h"
#include "CollisionData.h"
#include <vector>

#define Point glm::vec3
#define Line std::pair<glm::vec3, glm::vec3>

struct Plane {
	glm::vec3 normal;
	float distance;

	inline Plane() : normal(1, 0, 0) {};
	inline Plane(const glm::vec3& n, float d) : normal(n), distance(d) {};
};

typedef struct Interval { 
	float min;   
	float max; 
} Interval;

class CollisionDetectionSystem : public System
{	
public:
	std::vector<CollisionReport> update();
private:
	CollisionReport collisionDetection(Entity e, Entity f);
	bool broadCollisionDetection(Entity e, Entity f);

	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<SphereCollider>, std::shared_ptr<SphereCollider>);
	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<SphereCollider>, std::shared_ptr<BoxCollider>);
	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<BoxCollider>, std::shared_ptr<SphereCollider>);
	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<BoxCollider>, std::shared_ptr<BoxCollider>);

	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<SphereCollider>, std::shared_ptr<CapsuleCollider>);
	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<CapsuleCollider>, std::shared_ptr<SphereCollider>);
	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<BoxCollider>, std::shared_ptr<CapsuleCollider>);
	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<CapsuleCollider>, std::shared_ptr<BoxCollider>);
	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<CapsuleCollider>, std::shared_ptr<CapsuleCollider>);

	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<MeshCollider>, std::shared_ptr<SphereCollider>);
	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<SphereCollider>, std::shared_ptr<MeshCollider>);

	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<MeshCollider>, std::shared_ptr<BoxCollider>);
	CollisionData testCollisionDetection(const Transform &, const Transform &, std::shared_ptr<BoxCollider>, std::shared_ptr<MeshCollider>);

	Point closestPointLine(const Point& p, const Line & line);
	Point closestPointPlane(const Point& p, const Plane & plane);
	bool ClipToPlane(const Plane& plane, const Line& line, Point* outPoint);
	bool PointInOBB(const Point& point, const Transform & t, std::shared_ptr<BoxCollider> c);
	Interval GetIntervalOBB(const Transform & t, std::shared_ptr<BoxCollider> c, const glm::vec3& axis);
	Point closestPointOOB(const Point& p, const Transform & t, std::shared_ptr<BoxCollider> c_A);
	std::vector<Point> GetVerticesFromOBB(const Transform & t, std::shared_ptr<BoxCollider> c);
	std::vector<Line> GetEdges(const Transform & t, std::shared_ptr<BoxCollider> c);
	std::vector<Plane> GetPlanes(const Transform & t, std::shared_ptr<BoxCollider> c);
	std::vector<Point> ClipEdgesToOBB(const std::vector<Line>& edges, const Transform & t, std::shared_ptr<BoxCollider> c);
	float PenetrationDepth(const Transform & tA, std::shared_ptr<BoxCollider> cA, const Transform & tB, std::shared_ptr<BoxCollider> cB, const glm::vec3& axis, bool* outShouldFlip);
	CollisionData FindCollisionFeatures(const Transform & tA, std::shared_ptr<BoxCollider> cA, const Transform & tB, std::shared_ptr<BoxCollider> cB);
	std::pair<Point, Point> closestPointsOnLines(const Line & axe1, const Line & axe2);

	// Triangle
	bool PointInTriangle(const Point& point, const TriangleCollider & e);
	Plane fromTriangle(const TriangleCollider & e);
	Point closestPointTriangle(const Point& p, const TriangleCollider & e);
	CollisionData sphereToTriangleCollisionDetection(const Transform & t, std::shared_ptr<SphereCollider> sc, const TriangleCollider & f);
	CollisionData triangleToSphereCollisionDetection(const TriangleCollider & e, const Transform & t, std::shared_ptr<SphereCollider> sc);
	Interval getIntervalTriangle(const TriangleCollider & e, const glm::vec3& axis);
	std::pair<bool, float> OverlapOnAxis(const Transform & t, std::shared_ptr<BoxCollider> sc, const TriangleCollider & triangle, const glm::vec3 & axis);
	CollisionData boxToTriangleCollisionDetection(const Transform & t, std::shared_ptr<BoxCollider> sc, const TriangleCollider & triangle);
	CollisionData triangleToBoxCollisionDetection(TriangleCollider e, const Transform & t, std::shared_ptr<BoxCollider> bc);
};