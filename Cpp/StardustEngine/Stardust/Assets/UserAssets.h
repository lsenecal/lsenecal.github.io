#pragma once

#include "../Source/MasterManager.h"
#include "RandomGenerator.h"
#include "Octree.h"
#include "../Source/Systems/constantes.h"

class UserAssets
{
public:
	UserAssets(MasterManager * manager);
	~UserAssets();

	void init();

private :
	MasterManager * _manager;
	void createPlanet(glm::vec3 pos = glm::vec3(0.0,0.0,-100.0), float size = 30, bool round = true, glm::vec3 diviseur = glm::vec3(3.0), float precision = 0.5f);
	void createTerrain();

	glm::vec3 getCorner(int i, glm::vec3 position, float cubeSize);
	glm::vec3 voxelValue(glm::vec3 center);

	void createOrbit();
	void createNBox(unsigned int N);
	void createNSphere(unsigned int N);
	void test2balls();

};

