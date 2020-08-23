#pragma once
#include "../../Core/Types.h"
#include "../../Components/Collider.h"
#include <glm/glm.hpp>
#include <vector>
#include <limits>

class CollisionData {
private:
	bool _colliding;
	float _depth;
	std::vector<glm::vec3> _points;
	glm::vec3 _normal;

public:
	CollisionData() : _colliding(false), _depth(std::numeric_limits<float>::max()), _normal(glm::vec3(0.0f, 0.0f, 1.0f)) {};
	CollisionData(glm::vec3 normal, std::vector<glm::vec3> points) : _colliding(true), _normal(normal), _depth(std::numeric_limits<float>::max()) {
		_points.resize(points.size());
		for (unsigned int i = 0; i < _points.size(); i++) {
			_points.at(i) = points.at(i);
		}
	};
	CollisionData(glm::vec3 normal, std::vector<glm::vec3> points, float depth) : CollisionData(normal, points) {
		_depth = depth;
	};
	glm::vec3 getNormal() { return _normal; };
	void addPoint(glm::vec3 point) { _points.push_back(point); }
	float getDepth() { return _depth; };
	bool isColliding() { return _colliding; };
	void setCollide() { _colliding = true; }
	void setNormal(glm::vec3 normal) { _normal = normal; };
	void setDepth(float depth) { _depth = depth; };
	bool operator==(const CollisionData& cd) {
		if (this->_normal == cd._normal && this->_points == cd._points)
			return true;
		else
			return false;
	};
	std::vector<glm::vec3> & getContactPoints() { return _points; };
};

class CollisionReport {
private:
	Entity _a;
	Entity _b;
	CollisionData _data;
	unsigned int _i;
	unsigned int _j;
public:
	CollisionReport() {
		_data = CollisionData();
	};
	CollisionReport(Entity a, Entity b) : _a(a), _b(b) {
		_data = CollisionData();
	};
	CollisionReport(Entity a, Entity b, glm::vec3 normal, std::vector<glm::vec3> points, float depth, unsigned int i, unsigned int j) : _a(a), _b(b), _i(i), _j(j) {
		_data = CollisionData(normal, points, depth);
	};
	CollisionReport(Entity a, Entity b, CollisionData data, unsigned int i, unsigned int j) : _a(a), _b(b), _data(data), _i(i), _j(j) {};

	Entity getFirstEntity() { return _a; };
	Entity getLastEntity() { return _b; };
	glm::vec3 getNormal() { return _data.getNormal(); };
	void addPoint(glm::vec3 point) { _data.addPoint(point); }
	float getDepth() { return _data.getDepth(); };
	unsigned int getI() { return _i; };
	unsigned int getJ() { return _j; };
	bool isColliding() { return _data.isColliding(); };
	void setCollide() { _data.setCollide(); }
	void setNormal(glm::vec3 normal) { _data.setNormal(normal); };
	void setDepth(float depth) { _data.setDepth(depth); };
	bool operator==(const CollisionReport& cd) {
		if ((this->_a == cd._a && this->_b == cd._b) || (this->_a == cd._b && this->_b == cd._a))
			return true;
		else
			return false;
	};
	std::vector<glm::vec3> & getContactPoints() { return _data.getContactPoints(); };
};