#include "CollisionDetectionSystem.h"
#include "../constantes.h"
#include "../../Core/Coordinator.h"
#include "../../Components/Transform.h"
#include "../../Components/Collider.h"
#include "../../Components/Physics.h"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>

#include <bitset>

//#define BROADPHASE

bool CMP(float x, float y) {
	return fabsf(x - y) <= EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y)));
}

extern Coordinator g_coordinator;

std::vector<CollisionReport> CollisionDetectionSystem::update()
{
	// Broad phase

#ifdef BROADPHASE
	std::set<Entity> listE;

	for (auto it = _entities.begin(); it != _entities.end(); it++) {

		for (auto itt = it; itt != _entities.end(); itt++) {
			if (it != itt) {

				bool result = broadCollisionDetection(*it, *itt);

				if (result) {
					listE.insert(*it);
					listE.insert(*itt);
				}
			}
		}
	}

	std::vector<CollisionReport> pairs;
	pairs.reserve(listE.size());

	for (auto it = listE.begin(); it != listE.end(); it++) {

		for (auto itt = it; itt != listE.end(); itt++) {
			if (it != itt) {

				CollisionReport result = collisionDetection(*it, *itt);
				if (result.isColliding()) {
					if (find(pairs.begin(), pairs.end(), result) == pairs.end())
						pairs.push_back(result);
				}
			}
		}
	};

	pairs.shrink_to_fit();
	return pairs;

#else

	std::vector<CollisionReport> pairs;
	pairs.reserve(_entities.size());

	for (auto it = _entities.begin(); it != _entities.end(); it++) {

		for (auto itt = it; itt != _entities.end(); itt++) {
			if (it != itt) {

				CollisionReport result = collisionDetection(*it, *itt);
				if (result.isColliding()) {
					if (find(pairs.begin(), pairs.end(), result) == pairs.end())
						pairs.push_back(result);
				}
			}
		}
	};

	pairs.shrink_to_fit();
	return pairs;

#endif // BROADPHASE

};

bool CollisionDetectionSystem::broadCollisionDetection(Entity e, Entity f) {
	const Transform & t_e = g_coordinator.getComponent<Transform>(e);
	const Transform & t_f = g_coordinator.getComponent<Transform>(f);

	const Collider & Ce = g_coordinator.getComponent<ColliderHierarchy>(e).broadCollider;
	const Collider & Cf = g_coordinator.getComponent<ColliderHierarchy>(f).broadCollider;

	if (Ce.name == sphere) {
		if (Cf.name == sphere) {
			CollisionData test = testCollisionDetection(t_e, t_f, Ce.SphereCollider, Cf.SphereCollider);
			if (test.isColliding())
				return true;
		}
		else if (Cf.name == box) {
			CollisionData test = testCollisionDetection(t_e, t_f, Ce.SphereCollider, Cf.BoxCollider);
			if (test.isColliding())
				return true;
		}
		else if (Cf.name == capsule) {
			CollisionData test = testCollisionDetection(t_e, t_f, Ce.SphereCollider, Cf.CapsuleCollider);
			if (test.isColliding())
				return true;
		}
	}
	else if (Ce.name == box) {
		if (Cf.name == sphere) {
			CollisionData test = testCollisionDetection(t_e, t_f, Ce.BoxCollider, Cf.SphereCollider);
			if (test.isColliding())
				return true;
		}
		else if (Cf.name == box) {
			CollisionData test = testCollisionDetection(t_e, t_f, Ce.BoxCollider, Cf.BoxCollider);
			if (test.isColliding())
				return true;
		}
		else if (Cf.name == capsule) {
			CollisionData test = testCollisionDetection(t_e, t_f, Ce.BoxCollider, Cf.CapsuleCollider);
			if (test.isColliding())
				return true;
		}
	}

	return false;
}

CollisionReport CollisionDetectionSystem::collisionDetection(Entity e, Entity f)
{
	const Transform & t_e = g_coordinator.getComponent<Transform>(e);
	const Transform & t_f = g_coordinator.getComponent<Transform>(f);

	const ColliderHierarchy & CHe = g_coordinator.getComponent<ColliderHierarchy>(e);
	const ColliderHierarchy & CHf = g_coordinator.getComponent<ColliderHierarchy>(f);

	for (unsigned int i = 0; i < CHe.colliders.size(); i++) {
		for (unsigned int j = 0; j < CHf.colliders.size(); j++) {

			if (!(CHe.colliders.at(i).type == Static && CHf.colliders.at(i).type == Static)) {

				if (CHe.colliders.at(i).name == sphere) {
					if (CHf.colliders.at(j).name == sphere) {
						CollisionData test = testCollisionDetection(t_e, t_f, CHe.colliders.at(i).SphereCollider, CHf.colliders.at(j).SphereCollider);
						if (test.isColliding())
							return CollisionReport(e, f, test, i, j);
					}
					else if (CHf.colliders.at(j).name == box) {
						CollisionData test = testCollisionDetection(t_e, t_f, CHe.colliders.at(i).SphereCollider, CHf.colliders.at(j).BoxCollider);
						if (test.isColliding())
							return CollisionReport(e, f, test, i, j);
					}
					else if (CHf.colliders.at(j).name == capsule) {
						CollisionData test = testCollisionDetection(t_e, t_f, CHe.colliders.at(i).SphereCollider, CHf.colliders.at(j).CapsuleCollider);
						if (test.isColliding())
							return CollisionReport(e, f, test, i, j);
					}
				}
				else if (CHe.colliders.at(i).name == box) {
					if (CHf.colliders.at(j).name == sphere) {
						CollisionData test = testCollisionDetection(t_e, t_f, CHe.colliders.at(i).BoxCollider, CHf.colliders.at(j).SphereCollider);
						if (test.isColliding())
							return CollisionReport(e, f, test, i, j);
					}
					else if (CHf.colliders.at(j).name == box) {
						CollisionData test = testCollisionDetection(t_e, t_f, CHe.colliders.at(i).BoxCollider, CHf.colliders.at(j).BoxCollider);
						if (test.isColliding())
							return CollisionReport(e, f, test, i, j);
					}
					else if (CHf.colliders.at(j).name == capsule) {
						CollisionData test = testCollisionDetection(t_e, t_f, CHe.colliders.at(i).BoxCollider, CHf.colliders.at(j).CapsuleCollider);
						if (test.isColliding())
							return CollisionReport(e, f, test, i, j);
					}
				}
			}
		}
	}

	/*if (g_coordinator.getComponent<Collider>(e).name == sphere) {
		if (g_coordinator.getComponent<Collider>(f).name == sphere) {
			CollisionData test = testCollisionDetection(t_e, t_f, g_coordinator.getComponent<Collider>(e).SphereCollider, g_coordinator.getComponent<Collider>(f).SphereCollider);
			return CollisionReport(e, f, test);
		}
		else if (g_coordinator.getComponent<Collider>(f).name == box) {
			CollisionData test = testCollisionDetection(t_e, t_f, g_coordinator.getComponent<Collider>(e).SphereCollider, g_coordinator.getComponent<Collider>(f).BoxCollider);
			return CollisionReport(e, f, test);
		}
	}
	else if (g_coordinator.getComponent<Collider>(e).name == box) {
		if (g_coordinator.getComponent<Collider>(f).name == sphere) {
			CollisionData test = testCollisionDetection(t_e, t_f, g_coordinator.getComponent<Collider>(e).BoxCollider, g_coordinator.getComponent<Collider>(f).SphereCollider);
			return CollisionReport(e, f, test);
		}
		else if (g_coordinator.getComponent<Collider>(f).name == box) {
			CollisionData test = testCollisionDetection(t_e, t_f, g_coordinator.getComponent<Collider>(e).BoxCollider, g_coordinator.getComponent<Collider>(f).BoxCollider);
			return CollisionReport(e, f, test);
		}
	}*/

	return CollisionReport();
};

//Sphere - Sphere

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<SphereCollider> c_e, std::shared_ptr<SphereCollider> c_f) {
	Point position = t_e.position + c_e->center;
	const float & radius1 = c_e->radius;

	Point position2 = t_f.position + c_f->center;
	const float & radius2 = c_f->radius;

	float r2 = glm::dot(position2 - position, position2 - position);
	float p_rad = (radius1 + radius2)*(radius1 + radius2);
	glm::vec3 dir = normalize(position2 - position);

	if (r2 <= p_rad) {
		std::vector<Point> contactPoints(2);
		contactPoints.at(0) = position + radius1 * dir;
		contactPoints.at(1) = position2 - radius2 * dir;
		return CollisionData(dir, contactPoints, sqrt(p_rad) - sqrt(r2));
	}
	else
		return CollisionData();
}

// Sphere - Boite

Point CollisionDetectionSystem::closestPointOOB(const Point& p, const Transform & t, std::shared_ptr<BoxCollider> bc) {

	Point centerB = t.position + bc->center;
	glm::vec3 d = p - centerB;

	Point q = centerB;

	std::vector<glm::vec3> u(3);
	glm::mat4 rot = glm::mat4(1.0f);
	rot = glm::rotate(rot, glm::radians(t.rotation.x), glm::vec3(1, 0, 0));
	rot = glm::rotate(rot, glm::radians(t.rotation.y), glm::vec3(0, 1, 0));
	rot = glm::rotate(rot, glm::radians(t.rotation.z), glm::vec3(0, 0, 1));

	u.at(0) = glm::vec3(glm::vec4(1, 0, 0, 1) * rot);
	u.at(1) = glm::vec3(glm::vec4(0, 1, 0, 1) * rot);
	u.at(2) = glm::vec3(glm::vec4(0, 0, 1, 1) * rot);

	for (int i = 0; i < 3; i++) {
		float dist = glm::dot(d, u.at(i));
		if (dist > bc->size[i]) dist = bc->size[i];
		if (dist < -bc->size[i]) dist = -bc->size[i];

		q += dist * u.at(i);
	}

	return q;
};

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<SphereCollider> c_e, std::shared_ptr<BoxCollider> c_f) {
	Point centerS = t_e.position + c_e->center;
	const float & radius = c_e->radius;

	Point contactPoint = closestPointOOB(centerS, t_f, c_f);

	float dist = glm::dot(centerS - contactPoint, centerS - contactPoint); // Distance between sphere center and contact point on cube

	if (dist <= radius * radius) {
		glm::vec3 normS = glm::normalize(contactPoint - centerS); // normal on contact point for the sphere
		Point pointS = centerS + normS * radius; // contact point on the sphere

		glm::vec3 normB = -normS; // normal on contact point for the box
		Point pointB = contactPoint; // contact point on the box

		std::vector<Point> contactPoints(2);
		contactPoints.at(0) = pointS;
		contactPoints.at(1) = pointB;

		return CollisionData(normS, contactPoints, radius - sqrt(dist));
	}
	else
		return CollisionData();
}

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<BoxCollider> c_e, std::shared_ptr<SphereCollider> c_f) {
	return testCollisionDetection(t_f, t_e, c_f, c_e);
};

// Boite - Boite

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<BoxCollider> c_e, std::shared_ptr<BoxCollider> c_f) {
	CollisionData CD = FindCollisionFeatures(t_e, c_e, t_f, c_f);

	if (CD.isColliding())
		return CD;
	else
		return CollisionData();
};

Point CollisionDetectionSystem::closestPointPlane(const Point& p, const Plane & plane) {

	float dot = glm::dot(plane.normal, p);
	float distance = dot - plane.distance;
	return p - plane.normal*distance;
}

Point CollisionDetectionSystem::closestPointLine(const Point& p, const Line & line) {
	glm::vec3 lVec = line.second - line.first;
	float t = glm::dot(p - line.first, lVec) / glm::dot(lVec, lVec);
	t = fmaxf(t, 0.0f);
	t = fminf(t, 1.0f);

	return line.first + lVec * t;
};

bool CollisionDetectionSystem::PointInOBB(const Point & point, const Transform & t, std::shared_ptr<BoxCollider> c)
{
	glm::vec3 dir = point - (t.position + c->center);

	for (int i = 0; i < 3; ++i) {

		glm::vec3 axis;// (orientation[0], orientation[1], orientation[2]);

		axis = t.orientation.at(i);

		float distance = glm::dot(dir, axis);
		if (distance > c->size[i]) {
			return false;
		}
		if (distance < -c->size[i]) {
			return false;
		}
	}

	return true;
}

std::vector<Point> CollisionDetectionSystem::GetVerticesFromOBB(const Transform & t, std::shared_ptr<BoxCollider> c)
{
	std::vector<glm::vec3> v(8);

	Point C = t.position + c->center;  // OBB Center    
	const glm::vec3 & E = c->size;      // OBB Extents       
	const std::vector<glm::vec3> & A = t.orientation; //OBB Axis

	v[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	v[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
	v[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
	v[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	v[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	v[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
	v[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
	v[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

	return v;
}

Interval CollisionDetectionSystem::GetIntervalOBB(const Transform & t, std::shared_ptr<BoxCollider> c, const glm::vec3 & axis)
{
	std::vector<glm::vec3> vertex = GetVerticesFromOBB(t, c);

	Interval result;
	result.min = result.max = glm::dot(axis, vertex[0]);
	for (int i = 1; i < 8; ++i) {
		float projection = glm::dot(axis, vertex[i]);
		result.min = (projection < result.min) ? projection : result.min;
		result.max = (projection > result.max) ? projection : result.max;
	}

	return result;
}

std::vector<Line> CollisionDetectionSystem::GetEdges(const Transform & t, std::shared_ptr<BoxCollider> c)
{
	std::vector<Line> result;
	result.reserve(12);

	std::vector<Point> v = GetVerticesFromOBB(t, c);

	int index[][2] = { // Indices of edge-vertices
		{6,1},{6,3},{6,4},{2,7},{2,5},{2,0},
		{0,1},{0,3},{7,1},{7,4},{4,5},{5,3} };

	for (int j = 0; j < 12; ++j) {
		result.push_back(Line(v[index[j][0]], v[index[j][1]]));
	}

	return result;
}

std::vector<Plane> CollisionDetectionSystem::GetPlanes(const Transform & t, std::shared_ptr<BoxCollider> c)
{
	Point C = t.position + c->center;  // OBB Center    
	glm::vec3 & E = c->size;      // OBB Extents       
	const std::vector<glm::vec3> & A = t.orientation; // OBB Axis

	std::vector<Plane> result;
	result.resize(6);

	result[0] = Plane(A[0], glm::dot(A[0], (C + E[0] * E.x)));
	result[1] = Plane(A[0] * -1.0f, -glm::dot(A[0], (C - A[0] * E.x)));
	result[2] = Plane(A[1], glm::dot(A[1], (C + A[1] * E.y)));
	result[3] = Plane(A[1] * -1.0f, -glm::dot(A[1], (C - A[1] * E.y)));
	result[4] = Plane(A[2], glm::dot(A[2], (C + A[2] * E.z)));
	result[5] = Plane(A[2] * -1.0f, -glm::dot(A[2], (C - A[2] * E.z)));

	return result;
}

bool CollisionDetectionSystem::ClipToPlane(const Plane & plane, const Line & line, Point * outPoint)
{
	glm::vec3 ab = line.second - line.first;
	float nAB = glm::dot(plane.normal, ab);
	if (CMP(nAB, 0)) {
		return false;
	}

	float nA = glm::dot(plane.normal, line.first);
	float t = (plane.distance - nA) / nAB;

	if (t >= 0.0f && t <= 1.0f) {
		if (outPoint != 0) {
			*outPoint = line.first + ab * t;
		}
		return true;
	}

	return false;
}

std::vector<Point> CollisionDetectionSystem::ClipEdgesToOBB(const std::vector<Line>& edges, const Transform & t, std::shared_ptr<BoxCollider> c)
{
	std::vector<Point> result;
	result.reserve(edges.size());
	Point intersection;

	std::vector<Plane> planes = GetPlanes(t, c);

	for (unsigned int i = 0; i < planes.size(); ++i) {
		for (unsigned int j = 0; j < edges.size(); ++j) {
			if (ClipToPlane(planes[i], edges[j], &intersection)) {
				if (PointInOBB(intersection, t, c)) {
					result.push_back(intersection);
				}
			}
		}
	}

	return result;
}

float CollisionDetectionSystem::PenetrationDepth(const Transform & t1, std::shared_ptr<BoxCollider> c1, const Transform & t2, std::shared_ptr<BoxCollider> c2, const glm::vec3 & axis, bool * outShouldFlip)
{
	Interval i1 = GetIntervalOBB(t1, c1, glm::normalize(axis));
	Interval i2 = GetIntervalOBB(t2, c2, glm::normalize(axis));

	if (!((i2.min <= i1.max) && (i1.min <= i2.max))) {
		return 0.0f; // No penerattion    
	}

	float len1 = i1.max - i1.min;
	float len2 = i2.max - i2.min;

	float min = fminf(i1.min, i2.min);
	float max = fmaxf(i1.max, i2.max);

	float length = max - min;

	if (outShouldFlip != 0) { *outShouldFlip = (i2.min < i1.min); }

	return (len1 + len2) - length;
}

CollisionData CollisionDetectionSystem::FindCollisionFeatures(const Transform & tA, std::shared_ptr<BoxCollider> cA, const Transform & tB, std::shared_ptr<BoxCollider> cB)
{
	CollisionData result = CollisionData();

	std::vector<glm::vec3> o1 = tA.orientation;
	std::vector<glm::vec3> o2 = tB.orientation;

	glm::vec3 test[15] = { // Face axis
		o1[0], o1[1], o1[2],
		o2[0], o2[1], o2[2],
	};

	for (int i = 0; i < 3; ++i) { // Fill out rest of axis        
		test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
		test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
		test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
	}

	glm::vec3 hitNormal = glm::vec3(0.0f, 0.0f, 0.0f);
	bool shouldFlip;

	for (int i = 0; i < 15; ++i) {
		if (glm::dot(test[i], test[i]) < 0.001f) {
			continue;
		}

		float depth = PenetrationDepth(tA, cA, tB, cB, test[i], &shouldFlip);

		if (depth <= 0.0f) {
			return result;
		}
		else if (depth < result.getDepth()) {
			if (shouldFlip) {
				test[i] = test[i] * -1.0f;
			}
			result.setDepth(depth);
			hitNormal = test[i];
		}
	}

	if (hitNormal == glm::vec3(0.0f, 0.0f, 0.0f)) {
		return result;
	}

	glm::vec3 axis = glm::normalize(hitNormal);

	std::vector<Point> c1 = ClipEdgesToOBB(GetEdges(tB, cB), tA, cA);
	std::vector<Point> c2 = ClipEdgesToOBB(GetEdges(tA, cA), tB, cB);

	for (unsigned int i = 0; i < c1.size(); i++) {
		result.addPoint(c1.at(i));
	}

	for (unsigned int i = 0; i < c2.size(); i++) {
		result.addPoint(c2.at(i));
	}

	Interval i = GetIntervalOBB(tA, cA, axis);

	float distance = (float)((i.max - i.min) * 0.5f - result.getDepth() * 0.5f);

	Point pointOnPlane = (tA.position + cA->center) + axis * distance;

	for (unsigned int k = 0; k < result.getContactPoints().size(); k++) {
		glm::vec3 contact = result.getContactPoints().at(k);

		result.getContactPoints().at(k) = contact + (axis * glm::dot(axis, pointOnPlane - contact));

		for (unsigned int l = 0; l < k; l++) {
			if (dot(result.getContactPoints().at(l) - result.getContactPoints().at(l), result.getContactPoints().at(l) - result.getContactPoints().at(l)) < 0.0001f) {
				result.getContactPoints().erase(result.getContactPoints().begin() + l);
				break;
			}
		}
	}

	result.setCollide();
	result.setNormal(axis);

	return result;
};

// Triangle

bool CollisionDetectionSystem::PointInTriangle(const Point& point, const TriangleCollider & e) {
	Point A = e.pointA - point;
	Point B = e.pointB - point;
	Point C = e.pointC - point;

	glm::vec3 normPBC = glm::cross(B, C);
	glm::vec3 normPCA = glm::cross(C, A);
	glm::vec3 normPAB = glm::cross(A, B);

	if (glm::dot(normPBC, normPCA) < 0.0f) {
		return false;
	}
	else if (glm::dot(normPBC, normPAB) < 0.0f) {
		return false;
	}

	return true;
};

Plane CollisionDetectionSystem::fromTriangle(const TriangleCollider & e) {
	Plane result;

	Point A = e.pointA;
	Point B = e.pointB;
	Point C = e.pointC;

	result.normal = glm::normalize(glm::cross(B - A, C - A));
	result.distance = glm::dot(result.normal, A);

	return result;
}

Point CollisionDetectionSystem::closestPointTriangle(const Point& p, const TriangleCollider & e) {
	Plane plane = fromTriangle(e);

	Point closest = closestPointPlane(p, plane);

	if (PointInTriangle(closest, e))
		return closest;

	Point c1 = closestPointLine(p, Line(e.pointA, e.pointB));
	Point c2 = closestPointLine(p, Line(e.pointB, e.pointC));
	Point c3 = closestPointLine(p, Line(e.pointC, e.pointA));

	float magSq1 = glm::dot(p - c1, p - c1);
	float magSq2 = glm::dot(p - c2, p - c2);
	float magSq3 = glm::dot(p - c3, p - c3);

	if (magSq1 < magSq2 && magSq1 < magSq3) {
		return c1;
	}
	else if (magSq2 < magSq1 && magSq2 < magSq3) {
		return c2;
	}

	return c3;
}

CollisionData CollisionDetectionSystem::sphereToTriangleCollisionDetection(const Transform & t_e, std::shared_ptr<SphereCollider> sc_e, const TriangleCollider & f) {
	Point center = t_e.position + sc_e->center;
	float radius = sc_e->radius;

	Point closest = closestPointTriangle(center, f);

	float magSq = glm::dot(closest - center, closest - center);

	// vertex may need to be better dealt with

	if (magSq < radius * radius) {
		return CollisionData(closest - center, { closest }, sqrt(magSq) - radius);
	}
	else
		return CollisionData();
};

CollisionData CollisionDetectionSystem::triangleToSphereCollisionDetection(const TriangleCollider & e, const Transform & t, std::shared_ptr<SphereCollider> sc) {
	return sphereToTriangleCollisionDetection(t, sc, e);
};

Interval CollisionDetectionSystem::getIntervalTriangle(const TriangleCollider & e, const glm::vec3& axis) {
	Interval result;

	//result.min = glm::dot(axis, g_coordinator.getComponent<Collider>(e).TriangleCollider->pointA);
	result.min = glm::dot(axis, e.pointA);
	result.max = result.min;

	//float value = glm::dot(axis, g_coordinator.getComponent<Collider>(e).TriangleCollider->pointB);
	float value = glm::dot(axis, e.pointB);
	result.min = fminf(result.min, value);
	result.max = fmaxf(result.max, value);

	//value = glm::dot(axis, g_coordinator.getComponent<Collider>(e).TriangleCollider->pointC);
	value = glm::dot(axis, e.pointC);
	result.min = fminf(result.min, value);
	result.max = fmaxf(result.max, value);

	return result;
};

std::pair<bool, float> CollisionDetectionSystem::OverlapOnAxis(const Transform & t, std::shared_ptr<BoxCollider> bc, const TriangleCollider & triangle, const glm::vec3 & axis) {
	Interval a = GetIntervalOBB(t, bc, axis);
	Interval b = getIntervalTriangle(triangle, axis);

	float value = std::max(abs(a.max - b.min),
		std::max(abs(a.max - b.max),
			std::max(abs(b.max - a.min), abs(b.max - a.max)))) - (abs(b.max - b.min) + abs(a.max - a.min));

	return std::make_pair((b.min <= a.max) && (a.min <= b.max), value);
}

CollisionData CollisionDetectionSystem::boxToTriangleCollisionDetection(const Transform & t, std::shared_ptr<BoxCollider> bc, const TriangleCollider & triangle)
{
	glm::vec3 AB = triangle.pointB + triangle.pointB;
	glm::vec3 BC = triangle.pointC + triangle.pointB;
	glm::vec3 CA = triangle.pointC + triangle.pointA;

	glm::vec3 u0 = t.orientation.at(0);
	glm::vec3 u1 = t.orientation.at(1);
	glm::vec3 u2 = t.orientation.at(2);

	glm::vec3 test[13] = {
		u0, u1, u2,
		glm::cross(AB, BC),
		glm::cross(u0, AB), glm::cross(u0, BC), glm::cross(u0, CA),
		glm::cross(u1, AB), glm::cross(u1, BC), glm::cross(u1, CA),
		glm::cross(u2, AB), glm::cross(u2, BC), glm::cross(u2, CA)
	};

	std::pair<float, glm::vec3> normal = std::make_pair(0.0f, glm::vec3(0.0f, 0.0f, 0.0f));

	for (int i = 0; i < 13; i++) {

		std::pair<bool, float> truc = OverlapOnAxis(t, bc, triangle, test[i]);

		if (!(truc.first)) {
			return CollisionData();
		}
		else {
			if (truc.second < normal.first) {
				normal.first = truc.second;
				normal.second = test[i];
			}
		}
	}

	//TODO : get points

	return CollisionData(normal.second, {}, normal.first);
};

CollisionData CollisionDetectionSystem::triangleToBoxCollisionDetection(TriangleCollider e, const Transform & t, std::shared_ptr<BoxCollider> bc) {
	return boxToTriangleCollisionDetection(t, bc, e);
};

// Mesh - Sphere

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<MeshCollider> mc_e, std::shared_ptr<SphereCollider> sc_f) {

	int size = mc_e->list.size();

	for (int i = 0; i < size; i++) {
		CollisionData data = triangleToSphereCollisionDetection(mc_e->list.at(i), t_f, sc_f);

		if (data.isColliding())
			return data;
	}
	return CollisionData();
};

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<SphereCollider> sc_e, std::shared_ptr<MeshCollider> mc_f) {
	return testCollisionDetection(t_f, t_e, mc_f, sc_e);
};

// Mesh - Box

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<MeshCollider> mc_e, std::shared_ptr<BoxCollider> bc_f) {

	int size = mc_e->list.size();

	for (int i = 0; i < size; i++) {
		CollisionData data = triangleToBoxCollisionDetection(mc_e->list.at(i), t_f, bc_f);

		if (data.isColliding())
			return data;
	}

	return CollisionData();
};

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<BoxCollider> bc_e, std::shared_ptr<MeshCollider> mc_f) {
	return testCollisionDetection(t_f, t_e, mc_f, bc_e);
};

// Capsule - Sphere

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<SphereCollider> sc_e, std::shared_ptr<CapsuleCollider> cc_f) {

	Line axe = { t_f.position + cc_f->center - 0.5f * cc_f->dir, t_f.position + cc_f->center + 0.5f * cc_f->dir };
	Point center = closestPointLine(t_e.position + sc_e->center, axe);

	//std::cout << "axe : " << axe.first.x << " " << axe.first.y << " " << axe.first.z << std::endl;
	//std::cout << "axe : " << axe.second.x << " " << axe.second.y << " " << axe.second.z << std::endl;

	//std::cout << "Center : " << center.x << " " << center.y << " " << center.z << std::endl;

	//std::cout << glm::dot(t_f.position - center, t_f.position - center)/ glm::dot(cc_f->dir, cc_f->dir) << std::endl;

	std::shared_ptr<SphereCollider> sc(new SphereCollider({ center - t_f.position, cc_f->radius }));

	return testCollisionDetection(t_e, t_f, sc_e, sc);
};

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<CapsuleCollider> cc_e, std::shared_ptr<SphereCollider> sc_f) {
	return testCollisionDetection(t_f, t_e, sc_f, cc_e);
}

//Capsule - Boite

float clamp(float value, float min, float max) {
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

std::pair<Point, Point> CollisionDetectionSystem::closestPointsOnLines(const Line & axe1, const Line & axe2) {

	glm::vec3 d1 = axe1.second - axe1.first;
	glm::vec3 d2 = axe2.second - axe1.first;
	glm::vec3 r = axe1.first - axe2.first;

	float a = glm::dot(d1, d1);
	float b = glm::dot(d1, d2);
	float c = glm::dot(d1, r);
	float e = glm::dot(d2, d2);
	float f = glm::dot(d2, r);
	float d = a * e - b * b;

	float s;

	if (d != 0.0f)
		s = clamp((b*f - c * e) / d, 0.0f, 1.0f);
	else
		s = 0.0f;

	float t = (b*s + f) / e;

	if (t < 0.0f) {
		t = 0.0f;
		s = clamp(-c / a, 0.0f, 1.0f);
	}
	else if (t > 1.0f) {
		t = 1.0f;
		s = clamp((b - c) / a, 0.0f, 1.0f);
	}

	glm::vec3 L1 = axe1.first + s * d1;
	glm::vec3 L2 = axe2.first + t * d2;

	return std::make_pair(L1, L2);
};

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<BoxCollider> bc_e, std::shared_ptr<CapsuleCollider> cc_f) {

	Line axe = { t_f.position + cc_f->center - 0.5f * cc_f->dir, t_f.position + cc_f->center + 0.5f * cc_f->dir };

	std::vector<Line> lines = GetEdges(t_e, bc_e);

	float dist = std::numeric_limits<float>::max();
	Point center;

	for (unsigned int i = 0; i < lines.size(); i++) {

		std::pair<Point, Point> points = closestPointsOnLines(axe, lines.at(i));

		if (glm::dot(points.second - points.first, points.second - points.first) < dist) {
			dist = glm::dot(points.second - points.first, points.second - points.first);
			center = points.first;
		}
	}

	//std::cout << dist << std::endl;

	std::shared_ptr<SphereCollider> sc(new SphereCollider({ center - t_f.position, cc_f->radius }));

	return testCollisionDetection(t_e, t_f, bc_e, sc);
};

CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<CapsuleCollider> cc_e, std::shared_ptr<BoxCollider> bc_f) {
	return testCollisionDetection(t_f, t_e, bc_f, cc_e);
};


CollisionData CollisionDetectionSystem::testCollisionDetection(const Transform & t_e, const Transform & t_f, std::shared_ptr<CapsuleCollider> cc_e, std::shared_ptr<CapsuleCollider> cc_f) {

	Line axe_e = { cc_e->center - 0.5f * cc_e->dir, cc_e->center + 0.5f * cc_e->dir };
	Line axe_f = { cc_f->center - 0.5f * cc_f->dir, cc_f->center + 0.5f * cc_f->dir };

	std::pair<Point, Point> points = closestPointsOnLines(axe_e, axe_f);

	std::shared_ptr<SphereCollider> sc_e(new SphereCollider({ points.first, cc_e->radius }));
	std::shared_ptr<SphereCollider> sc_f(new SphereCollider({ points.second, cc_f->radius }));

	return testCollisionDetection(t_e, t_f, sc_e, sc_f);
};