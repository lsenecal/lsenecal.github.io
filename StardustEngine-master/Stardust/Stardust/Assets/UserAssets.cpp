#include "UserAssets.h"





UserAssets::UserAssets(MasterManager * manager)
{
	_manager = manager;
	_manager->create_window("AndouilleDust ENGINE", 1980, 1080, false);

	//createPlanet(glm::vec3(0.0,30.0,-100));
	//createPlanet(glm::vec3(0.0,-30.0,-100));
	//createPlanet();

}


UserAssets::~UserAssets()
{
}

void UserAssets::init()
{
	createPlanet();

}

void UserAssets::createPlanet(glm::vec3 pos, float size, bool round, glm::vec3 diviseur, float precision)
{
	int nbcube = 0;
	float cubeSize = 1.0f;
	for (float x = pos.x - size / 2; x < size / 2 + pos.x; x+=cubeSize) {
		for (float y = pos.y - size / 2; y < size / 2 + pos.y; y+= cubeSize) {
			for (float z = pos.z - size / 2; z < size / 2 + pos.z; z+= cubeSize) {
				if (!round || (round && pow(x - pos.x, 2) + pow(y - pos.y, 2) + pow(z - pos.z, 2) <= pow(size / 2, 2))) {	
					if (Perlin3D(x / diviseur.x, y / diviseur.y, z / diviseur.z) > precision) {
						Entity entity = g_coordinator.createEntity();

						Transform trans;
						trans.position = glm::vec3(x, y, z);
						g_coordinator.addComponent(entity, trans);

						Renderable rent;
						float dist = ((x*x) / (10 * 10) + (y*y) / (10 * 10) + ((z + 100)*(z + 100)) / (10 * 10)) / 3;
						//rent.color = glm::vec3((1.0 + sin(x)) / 2.0, (1.0 + cos(y)) / 2.0, (1.0 + sin(z)) / 2.0);
						rent.color = glm::vec3(1 - dist, dist, /*(1.0+ sin(z)) /2 */ 0.2);
						rent.shader_name = "Cube";
						rent.withGeometryShader = false;
						g_coordinator.addComponent(entity, rent);

						if (y > 0) {
							QuadMesh cm;
							g_coordinator.addComponent(entity, cm);
						}
						else if (z > -100)
						{
							PyramidMesh cm;
							g_coordinator.addComponent(entity, cm);
						}
						else {
							CubeMesh cm;
							g_coordinator.addComponent(entity, cm);
						}

						nbcube++;
					}
				}
			}
		}
	}
	std::cout << "UserAssets.cpp : number of cubes = " << nbcube << std::endl;

}


