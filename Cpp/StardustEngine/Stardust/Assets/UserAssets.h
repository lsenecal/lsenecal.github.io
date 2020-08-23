#pragma once

#include "../Source/MasterManager.h"
#include "RandomGenerator.h"
class UserAssets
{
public:
	UserAssets(MasterManager * manager);
	~UserAssets();

	void init();

private :
	MasterManager * _manager;
	void createPlanet(glm::vec3 pos = glm::vec3(0.0,0.0,-100.0), float size = 30, bool round = true, glm::vec3 diviseur = glm::vec3(3.0), float precision = 0.5f);
};

