#pragma once
#include"../Vendor/include/glm/glm.hpp"
#include "../Source/MasterManager.h"
#include "RandomGenerator.h"



class Octree
{
public:
	Octree(int size, glm::vec3 pos, Terrain * t);
	~Octree();

	int generate();
	
	void produceGeometry(Terrain * t);

	glm::vec3 getCorner(int i, glm::vec3 position, float u_cubeSize);

	glm::vec3 voxelValue(glm::vec3 center);

private:
	bool _haveChilds;
	std::shared_ptr<Octree> _enfants[8];
	int _size;
	glm::vec3 _position;
	Terrain * _terrain;

};

