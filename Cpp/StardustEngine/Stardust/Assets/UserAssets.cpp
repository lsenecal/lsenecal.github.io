#include "UserAssets.h"
#include <cstdlib>


#include <iomanip>      // std::setprecision

float max(glm::vec3);

UserAssets::UserAssets(MasterManager * manager)
{
	_manager = manager;
	_manager->create_window("StarDust ENGINE", 1980, 1080, false);

	//createPlanet(glm::vec3(0.0,30.0,-100));
	//createPlanet(glm::vec3(0.0,-30.0,-100));
	//createPlanet();
}


UserAssets::~UserAssets()
{
}

void UserAssets::init()
{
	Entity entity = g_coordinator.createEntity();
	Entity grass = g_coordinator.createEntity();
	Patches area;

	float tile = 10000;
	for (float i = 0; i < tile; i += 10)
		for (float j = 0; j < tile; j += 10)
		{
			area.dimensions.push_back(glm::vec3(1.0f));
			area.normals.push_back(glm::vec3(0, 1, 0));
			double x, y, z;
			x = (i*2.0f) / (100.0f);
			z = (j*2.0f) / (100.0f);
			y = fbm(x / 30.0, z / 30.0) *50.0; // fbm(glm::vec2(x / 50.0, z / 50.0))*10.0f;

			if (Perlin3D((x) / 10.0, (y) / 10.0, (z) / 10.0) < 0.29)
				area.positions.push_back(glm::vec3(x, y, z));
		}
	Mesh cm; cm.vertices = quad_vertices;
	g_coordinator.addComponent(grass, cm);
	g_coordinator.addComponent(grass, area);

	Transform trans;
	trans.position = glm::vec3(10.0, 10.0, 10.0);


	g_coordinator.addComponent(entity, trans);

	Renderable rent;
	float dist = 10.0f;
	rent.color = glm::vec3(1 - dist, dist, /*(1.0+ sin(z)) /2 */ 0.2);
	rent.shader_name = "Cube";
	rent.withGeometryShader = false;
	g_coordinator.addComponent(entity, rent);
	CubeMesh cm2;
	g_coordinator.addComponent(entity, cm2);

	createTerrain();


	srand(0);
	for (int j = 0; j < 15; j += 1)
	{
		bool placed = false;
		do {
			int borne_maximale = 200;
			int borne_minimale = 0;
			double randomnumX = ((double)(rand() % (borne_maximale - borne_minimale) + borne_minimale)) / 200.0;
			double randomnumZ = ((double)(rand() % (borne_maximale - borne_minimale) + borne_minimale)) / 200.0;
			double randomnumS = rand() % 10 + 1;

			float x = randomnumX * 100.0;
			float z = randomnumZ * 100.0;

			float y = fbm(x / 30.0, z / 30.0) *50.0;
			//std::cout << fbm(x , z ) << std::endl;
			if (Perlin3D((x) / 10.0, (y) / 10.0, (z) / 10.0) < 0.29 && fbm(x / 30.0, z / 30.0) > 0.3)
			{
				std::cout << "j : " << j << std::endl;

				placed = true;
				Entity birch = g_coordinator.createEntity();
				Model3D m;
				m.name = "Birch";
				//m.path = "Assets/Models/Rock_9/Rock_9.fbx";
				m.path = "Assets/Models/Birch/birch_13m.obj";
				Transform t;

				RigidBody rigidbody;
				rigidbody.mass = 1.0;
				rigidbody.useGravity = false;

				t.position = glm::vec3(x, y, z);
				t.scale = glm::vec3(0.0025*randomnumS / 2.0);
				t.rotation = glm::vec3(0, 45 * randomnumS, 0);
				Renderable r;
				g_coordinator.addComponent(birch, m);
				g_coordinator.addComponent(birch, t);
				g_coordinator.addComponent(birch, r);

				std::cout << "scale : " << t.scale.x << std::endl;

				ColliderHierarchy CH;
				Collider C;
				C.name = capsule;
				C.type = Static;
				std::shared_ptr<CapsuleCollider> CC(new CapsuleCollider);
				CC->center = glm::vec3(0.0f, 1.0f, 0.0f);
				CC->radius = 100.0f * t.scale.x;
				CC->dir = glm::vec3(0.0f, 2.0f, 0.0f);
				C.CapsuleCollider = CC;
				CH.colliders.push_back(C);
				g_coordinator.addComponent(birch, rigidbody);
				g_coordinator.addComponent(birch, CH);
			}
		} while (!placed);
	}

	/*for (int j = 0; j < 10; j += 1)
	{
		int borne_maximale = 200;
		int borne_minimale = 0;
		double randomnumX = ((double)(rand() % (borne_maximale - borne_minimale) + borne_minimale)) / 200.0;
		double randomnumZ = ((double)(rand() % (borne_maximale - borne_minimale) + borne_minimale)) / 200.0;
		double randomnumS = rand() % 10 + 1;
		float x = randomnumX * 200.0;
		float z = randomnumZ * 200.0;
		float y = fbm(x / 30.0, z / 30.0) *50.0;
		if (Perlin3D((x) / 10.0, (y) / 10.0, (z) / 10.0) < 0.29)
		{
			Entity birch = g_coordinator.createEntity();
			Model3D m;
			m.name = "Rock";
			m.path = "Assets/Models/Rock_9/Rock_9.obj";
			//m.path = "Assets/Models/Birch/birch_13m.obj";
			Transform t;
			RigidBody rigidbody;
			rigidbody.mass = 1.0;
			rigidbody.useGravity = false;
			ColliderHierarchy CH;
			Collider C;
			C.name = box;
			C.type = Static;
			std::shared_ptr<BoxCollider> BC(new BoxCollider);
			//BC->center = glm::vec3(x, y, z);
			//BC->radius = 1.0f;
			//BC->dir = glm::vec3(0.0f, 1.0f, 0.0f);
			C.BoxCollider = BC;
			CH.colliders.push_back(C);
			g_coordinator.addComponent(birch, rigidbody);
			g_coordinator.addComponent(birch, CH);
			t.position = glm::vec3(x, y, z);
			t.scale = glm::vec3(0.2*randomnumS / 2.0);
			t.rotation = glm::vec3(0, 45 * randomnumS, 0);
			Renderable r;
			g_coordinator.addComponent(birch, m);
			g_coordinator.addComponent(birch, t);
			g_coordinator.addComponent(birch, r);
		}
	}*/
}

void UserAssets::createPlanet(glm::vec3 pos, float size, bool round, glm::vec3 diviseur, float precision)
{
	int nbcube = 0;
	float cubeSize = 1.0f;

	for (float x = pos.x - size / 2; x < size / 2 + pos.x; x += cubeSize) {
		for (float y = pos.y - size / 2; y < size / 2 + pos.y; y += cubeSize) {
			for (float z = pos.z - size / 2; z < size / 2 + pos.z; z += cubeSize) {
				//if (!round || (round && pow(x - pos.x, 2) + pow(y - pos.y, 2) + pow(z - pos.z, 2) <= pow(size / 2, 2))) {	
					//if (Perlin3D(x / diviseur.x, y / diviseur.y, z / diviseur.z) > precision) {
				if ((pow(x, 2) + pow(y, 2) + pow(100 + z, 2) >= pow(24 / 2 - 1, 2)) && ((pow(x, 2) + pow(y, 2) + pow(100 + z, 2) <= pow(30 / 2, 2)))) {


					Entity entity = g_coordinator.createEntity();

					Transform trans;
					trans.position = glm::vec3(x, y, z);
					//trans.rotation = glm::vec3(x, y, z);
					//trans.scale = glm::vec3(x / 50, y / 50, z / 50);
					g_coordinator.addComponent(entity, trans);

					Renderable rent;
					float dist = ((x*x) / (10 * 10) + (y*y) / (10 * 10) + ((z + 100)*(z + 100)) / (10 * 10)) / 3;
					//rent.color = glm::vec3((1.0 + sin(x)) / 2.0, (1.0 + cos(y)) / 2.0, (1.0 + sin(z)) / 2.0);
					rent.color = glm::vec3(1 - dist, dist, /*(1.0+ sin(z)) /2 */ 0.2);
					rent.shader_name = "Cube";
					rent.withGeometryShader = false;
					g_coordinator.addComponent(entity, rent);


					//Terrain t;
					//g_coordinator.addComponent(entity, t);
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
				//}
			//}
			}
		}
	}

	// 

	std::cout << "UserAssets.cpp : number of cubes = " << nbcube << std::endl;
}

void UserAssets::createTerrain()
{
	//
	int nbcube = 0;
	float cubeSize = 1.0f;
	glm::vec3 pos = glm::vec3(0.0, 15, 0.0);
	glm::vec3 color = glm::vec3(1.0, 0.0, 0.0);
	float size_x = 100.0;
	float size_y = 50.0;
	float size_z = 200.0;
	float amplitude = 50;
	int marchingID = 0;
	float pourcentage3D = 0.3;

	Entity entity;
	Terrain t;
	Octree O = Octree(256, glm::vec3(00.0, 0.0, 00.0), &t);
	//O.generate();
	//simplification QEM, 

	for (float x = 0; x <= size_x; x += cubeSize) {
		std::cout << x << std::endl;
		for (float y = -size_y / 2; y <= size_y / 2; y += cubeSize) {
			for (float z = 0; z <= size_z; z += cubeSize) {
				glm::vec3 center = glm::vec3(x, y, z);
				int nb = 0;
				bool grotte = false;
				double a(50), b(50), c(50), r(40);
				float div3D = 10.0;

				for (int i = 0; i < 8; i++) {
					pos = getCorner(i, center, cubeSize);
					if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
						bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < pourcentage3D;
						if (b) nb++;
						else grotte = true;
					}
				}


				if (nb > 0 && nb < 8) {
					int nb2(0), nb3(0), nb4(0);
					bool grotte2 = false;
					bool grotte3 = false;
					bool grotte4 = false;
					//bas
					glm::vec3 center2 = center + glm::vec3(cubeSize, 0.0, 0.0);
					glm::vec3 center3 = center + glm::vec3(cubeSize, 0.0, cubeSize);
					glm::vec3 center4 = center + glm::vec3(0.0, 0.0, cubeSize);

					for (int i = 0; i < 8; i++) {
						pos = getCorner(i, center2, cubeSize);
						if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
							bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < pourcentage3D;
							if (b) nb2++;
							else grotte2 = true;
						}
					}

					for (int i = 0; i < 8; i++) {
						pos = getCorner(i, center3, cubeSize);
						if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
							bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < pourcentage3D;
							if (b) nb3++;
							else grotte3 = true;
						}
					}

					for (int i = 0; i < 8; i++) {
						pos = getCorner(i, center4, cubeSize);
						if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
							bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < pourcentage3D;
							if (b) nb4++;
							else grotte4 = true;
						}
					}

					if (nb > 0 && nb < 8 && nb2 >0 && nb2 < 8 && nb3 >0 && nb3 < 8 && nb4 >0 && nb4 < 8) {
						glm::vec3 voxV = voxelValue(center);
						glm::vec3 voxV2 = voxelValue(center2);
						glm::vec3 voxV3 = voxelValue(center3);
						glm::vec3 voxV4 = voxelValue(center4);

						glm::vec3 normale1 = glm::cross(voxV3 - voxV2, voxV - voxV2);
						glm::vec3 normale2 = glm::cross(voxV3 - voxV4, voxV - voxV4);


						glm::vec3 test1 = voxV + glm::normalize(normale1);
						glm::vec3 test2 = voxV + glm::normalize(normale2);

						if (test1.y < fbm(test1.x / 30.0, test1.z / 30.0) * amplitude && Perlin3D(test1.x / div3D, test1.y / div3D, test1.z / div3D) < pourcentage3D)
							normale1 = -normale1;

						if (test2.y < fbm(test2.x / 30.0, test2.z / 30.0) * amplitude && Perlin3D(test2.x / div3D, test2.y / div3D, test2.z / div3D) < pourcentage3D)
							normale2 = -normale2;

						t.normale.push_back(normale1);
						t.normale.push_back(normale1);
						t.normale.push_back(normale1);
						t.normale.push_back(normale2);
						t.normale.push_back(normale2);
						t.normale.push_back(normale2);

						t.points.push_back(voxV);
						t.points.push_back(voxV2);
						t.points.push_back(voxV3);
						t.points.push_back(voxV);
						t.points.push_back(voxV3);
						t.points.push_back(voxV4);

						if (grotte) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte2) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte3) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));

						if (grotte) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte3) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte4) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
					}



					//gauche
					nb2 = 0; nb3 = 0; nb4 = 0;
					grotte2 = false; grotte3 = false; grotte4 = false;
					center2 = center + glm::vec3(cubeSize, 0.0, 0.0);
					for (int i = 0; i < 8; i++) {
						pos = getCorner(i, center2, cubeSize);
						if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
							bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < pourcentage3D;
							if (b) nb2++;
							else grotte2 = true;
						}
					}
					center3 = center + glm::vec3(cubeSize, cubeSize, 0.0);
					for (int i = 0; i < 8; i++) {
						pos = getCorner(i, center3, cubeSize);
						if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
							bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < pourcentage3D;
							if (b) nb3++;
							else grotte3 = true;
						}
					}
					center4 = center + glm::vec3(0.0, cubeSize, 0.0);
					for (int i = 0; i < 8; i++) {
						pos = getCorner(i, center4, cubeSize);
						if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
							bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < pourcentage3D;
							if (b) nb4++;
							else grotte4 = true;
						}
					}
					if (nb > 0 && nb < 8 && nb2 >0 && nb2 < 8 && nb3 >0 && nb3 < 8 && nb4 >0 && nb4 < 8) {
						glm::vec3 voxV = voxelValue(center);
						glm::vec3 voxV2 = voxelValue(center2);
						glm::vec3 voxV3 = voxelValue(center3);
						glm::vec3 voxV4 = voxelValue(center4);

						glm::vec3 moy1 = (voxV + voxV2 + voxV3) / 3.0f;
						glm::vec3 moy2 = (voxV + voxV2 + voxV3) / 3.0f;

						glm::vec3 normale1 = glm::cross(voxV - voxV3, voxV - voxV2);
						glm::vec3 normale2 = glm::cross(voxV - voxV3, voxV - voxV4);

						glm::vec3 test1 = voxV + glm::normalize(normale1);
						glm::vec3 test2 = voxV + glm::normalize(normale2);

						if (test1.y < fbm(test1.x / 30.0, test1.z / 30.0) * amplitude && Perlin3D(test1.x / div3D, test1.y / div3D, test1.z / div3D) < pourcentage3D)
							normale1 = -normale1;

						if (test2.y < fbm(test2.x / 30.0, test2.z / 30.0) * amplitude && Perlin3D(test2.x / div3D, test2.y / div3D, test2.z / div3D) < pourcentage3D)
							normale2 = -normale2;

						t.normale.push_back(normale1);
						t.normale.push_back(normale1);
						t.normale.push_back(normale1);
						t.normale.push_back(normale2);
						t.normale.push_back(normale2);
						t.normale.push_back(normale2);

						t.points.push_back(voxV);
						t.points.push_back(voxV2);
						t.points.push_back(voxV3);
						t.points.push_back(voxV);
						t.points.push_back(voxV3);
						t.points.push_back(voxV4);

						if (grotte) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte2) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte3) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));

						if (grotte) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte3) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte4) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
					}

					//droite
					nb2 = 0; nb3 = 0; nb4 = 0;
					grotte2 = false; grotte3 = false; grotte4 = false;
					center2 = center + glm::vec3(0.0, 0.0, cubeSize);
					for (int i = 0; i < 8; i++) {
						pos = getCorner(i, center2, cubeSize);
						if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
							bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < pourcentage3D;
							if (b) nb2++;
							else grotte2 = true;
						}
					}
					center3 = center + glm::vec3(0.0, cubeSize, cubeSize);
					for (int i = 0; i < 8; i++) {
						pos = getCorner(i, center3, cubeSize);
						if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
							bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < pourcentage3D;
							if (b) nb3++;
							else grotte3 = true;
						}
					}
					center4 = center + glm::vec3(0.0, cubeSize, 0.0);
					for (int i = 0; i < 8; i++) {
						pos = getCorner(i, center4, cubeSize);
						if (pos.y < fbm(pos.x / 30.0, pos.z / 30.0) * amplitude) {
							bool b = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < pourcentage3D;
							if (b) nb4++;
							else grotte4 = true;
						}
					}
					if (nb > 0 && nb < 8 && nb2 >0 && nb2 < 8 && nb3 >0 && nb3 < 8 && nb4 >0 && nb4 < 8) {
						glm::vec3 voxV = voxelValue(center);
						glm::vec3 voxV2 = voxelValue(center2);
						glm::vec3 voxV3 = voxelValue(center3);
						glm::vec3 voxV4 = voxelValue(center4);

						glm::vec3 normale1 = glm::cross(voxV3 - voxV2, voxV - voxV2);
						glm::vec3 normale2 = glm::cross(voxV3 - voxV4, voxV - voxV4);

						glm::vec3 test1 = voxV + glm::normalize(normale1);
						glm::vec3 test2 = voxV + glm::normalize(normale2);

						if (test1.y < fbm(test1.x / 30.0, test1.z / 30.0) * amplitude && Perlin3D(test1.x / div3D, test1.y / div3D, test1.z / div3D) < pourcentage3D)
							normale1 = -normale1;

						if (test2.y < fbm(test2.x / 30.0, test2.z / 30.0) * amplitude && Perlin3D(test2.x / div3D, test2.y / div3D, test2.z / div3D) < pourcentage3D)
							normale2 = -normale2;

						t.normale.push_back(normale1);
						t.normale.push_back(normale1);
						t.normale.push_back(normale1);
						t.normale.push_back(normale2);
						t.normale.push_back(normale2);
						t.normale.push_back(normale2);

						t.points.push_back(voxV);
						t.points.push_back(voxV2);
						t.points.push_back(voxV3);

						t.points.push_back(voxV);
						t.points.push_back(voxV3);
						t.points.push_back(voxV4);

						if (grotte) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte2) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte3) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));

						if (grotte) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte3) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
						if (grotte4) t.colors.push_back(glm::vec4(0.514 / 2.0, 0.494 / 2.0, 0.514 / 2.0, 1.0));
						else t.colors.push_back(glm::vec4(0.0, 0.22, 0.0, 1.0));
					}

				}
			}
		}
	}
	//*/
	entity = g_coordinator.createEntity();
	g_coordinator.addComponent(entity, t);

	/*
	for (float x = 0; x < size_x; x += cubeSize) {
		for (float z = 0; z < size_z; z += cubeSize) {
			for (float y = int(fbm(x / 30.0, z / 30.0) * amplitude) +3; y >= 0; y -= cubeSize) {
				marchingID = 0;
				int nb = 0;
				int colornb = 0;
				glm::vec4 position = glm::vec4(x, y, z, 0.0);
				glm::vec4 pos;
				for (int i = 0; i < 8; i++) {
					switch (i) {
					case 0:
						pos = position + glm::vec4(-cubeSize / 2.0f, -cubeSize / 2.0f, -cubeSize / 2.0f, 0.0);
						break;
					case 1:
						pos = position + glm::vec4(cubeSize / 2.0f, -cubeSize / 2.0f, -cubeSize / 2.0f, 0.0);
						break;
					case 2:
						pos = position + glm::vec4(cubeSize / 2.0f, -cubeSize / 2.0f, cubeSize / 2.0f, 0.0);
						break;
					case 3:
						pos = position + glm::vec4(-cubeSize / 2.0f, -cubeSize / 2.0f, cubeSize / 2.0f, 0.0);
						break;
					case 4:
						pos = position + glm::vec4(-cubeSize / 2.0f, cubeSize / 2.0f, -cubeSize / 2.0f, 0.0);
						break;
					case 5:
						pos = position + glm::vec4(cubeSize / 2.0f, cubeSize / 2.0f, -cubeSize / 2.0f, 0.0);
						break;
					case 6:
						pos = position + glm::vec4(cubeSize / 2.0f, cubeSize / 2.0f, cubeSize / 2.0f, 0.0);
						break;
					case 7:
						pos = position + glm::vec4(-cubeSize / 2.0f, cubeSize / 2.0f, cubeSize / 2.0f, 0.0);
						break;
					}
					double value = fbm(pos.x / 30.0, pos.z / 30.0) * amplitude;
					float div3D = 10.0;
					bool b = true;//(Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D) < 0.7);
					if (pos.y <= value && b) {
						nb++;
						marchingID += pow(2, i);
					}
					if (pos.y <= value) {
						colornb++;
					}
				}
				if (nb>0 && nb <8 ){
						Entity entity;
						Transform trans;
						Renderable rent;
						Terrain t;
						color = glm::vec3(0.56);
						if (colornb > 0 && colornb < 8) {
							color = glm::vec3(random(y,x), 0.54, random(x, y));
						}
						entity = g_coordinator.createEntity();
						trans.position = glm::vec3(x, y, z);
						g_coordinator.addComponent(entity, trans);
						rent.shader_name = "Cube";
						rent.withGeometryShader = false;
						rent.color = color;
						g_coordinator.addComponent(entity, rent);
						t.marchingID = 15;//marchingID;
						g_coordinator.addComponent(entity, t);
						nbcube++;
				}
			}
		}
	}
	//*/
	std::cout << "UserAssets.cpp : number of cubes = " << nbcube << std::endl;
}

glm::vec3 UserAssets::getCorner(int i, glm::vec3 position, float u_cubeSize)
{
	glm::vec3 pos;
	switch (i) {
	case 0:
		pos = position + glm::vec3(-u_cubeSize / 2.0f, -u_cubeSize / 2.0f, -u_cubeSize / 2.0f);
		break;
	case 1:
		pos = position + glm::vec3(u_cubeSize / 2.0f, -u_cubeSize / 2.0f, -u_cubeSize / 2.0f);
		break;
	case 2:
		pos = position + glm::vec3(u_cubeSize / 2.0f, -u_cubeSize / 2.0f, u_cubeSize / 2.0f);
		break;
	case 3:
		pos = position + glm::vec3(-u_cubeSize / 2.0f, -u_cubeSize / 2.0f, u_cubeSize / 2.0f);
		break;
	case 4:
		pos = position + glm::vec3(-u_cubeSize / 2.0f, u_cubeSize / 2.0f, -u_cubeSize / 2.0f);
		break;
	case 5:
		pos = position + glm::vec3(u_cubeSize / 2.0f, u_cubeSize / 2.0f, -u_cubeSize / 2.0f);
		break;
	case 6:
		pos = position + glm::vec3(u_cubeSize / 2.0f, u_cubeSize / 2.0f, u_cubeSize / 2.0f);
		break;
	case 7:
		pos = position + glm::vec3(-u_cubeSize / 2.0f, u_cubeSize / 2.0f, u_cubeSize / 2.0f);
		break;
	}
	return pos;
}

glm::vec3 UserAssets::voxelValue(glm::vec3 center)
{
	bool inside[8];
	double val[8];
	std::vector<glm::vec3> intersectionPt;
	std::vector<glm::vec3> intersectionNorm;
	double a(50), b(50), c(50), r(40);
	double cubeSize = 1.0;
	double pourcentage3D = 0.3;

	for (int i = 0; i < 8; i++) {
		glm::vec3 pos = getCorner(i, center, cubeSize);


		float div3D = 10.0;
		double p3D = Perlin3D(pos.x / div3D, pos.y / div3D, pos.z / div3D);
		val[i] = fbm(pos.x / 30.0, pos.z / 30.0) * 50.0;
		inside[i] = pos.y < val[i];
		val[i] = val[i] - pos.y;

		if (p3D > pourcentage3D) {
			inside[i] = p3D < pourcentage3D;
			val[i] = p3D - pourcentage3D;
		}
	}

	int indice1(0), indice2(1);
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).x;
		double y1 = getCorner(indice2, center, cubeSize).x;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;

		glm::vec3 pt = glm::vec3(B, getCorner(indice1, center, cubeSize).y, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 3; indice2 = 2;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).x;
		double y1 = getCorner(indice2, center, cubeSize).x;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(B, getCorner(indice1, center, cubeSize).y, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 4; indice2 = 5;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).x;
		double y1 = getCorner(indice2, center, cubeSize).x;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(B, getCorner(indice1, center, cubeSize).y, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 7; indice2 = 6;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).x;
		double y1 = getCorner(indice2, center, cubeSize).x;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(B, getCorner(indice1, center, cubeSize).y, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 0; indice2 = 4;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).y;
		double y1 = getCorner(indice2, center, cubeSize).y;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, B, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 5; indice2 = 1;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).y;
		double y1 = getCorner(indice2, center, cubeSize).y;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, B, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 6; indice2 = 2;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).y;
		double y1 = getCorner(indice2, center, cubeSize).y;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, B, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 7; indice2 = 3;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).y;
		double y1 = getCorner(indice2, center, cubeSize).y;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, B, getCorner(indice1, center, cubeSize).z);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 0; indice2 = 3;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).z;
		double y1 = getCorner(indice2, center, cubeSize).z;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, getCorner(indice1, center, cubeSize).y, B);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 1; indice2 = 2;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).z;
		double y1 = getCorner(indice2, center, cubeSize).z;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, getCorner(indice1, center, cubeSize).y, B);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 5; indice2 = 6;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).z;
		double y1 = getCorner(indice2, center, cubeSize).z;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, getCorner(indice1, center, cubeSize).y, B);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}
	indice1 = 4; indice2 = 7;
	if (inside[indice1] != inside[indice2]) {
		double y0 = getCorner(indice1, center, cubeSize).z;
		double y1 = getCorner(indice2, center, cubeSize).z;
		double x0 = val[indice1];
		double x1 = val[indice2];
		double A = (y1 - y0) / (x1 - x0);
		double B = y0 - A * x0;
		glm::vec3 pt = glm::vec3(getCorner(indice1, center, cubeSize).x, getCorner(indice1, center, cubeSize).y, B);
		intersectionPt.push_back(pt);
		intersectionNorm.push_back(glm::normalize(glm::vec3(pt.x - a, pt.y - b, pt.z - c)));
	}


	glm::vec3 retour = glm::vec3(0.0);

	glm::mat4 A = glm::mat4(0.0);
	glm::vec4 B = glm::vec4(0.0);



	for (glm::vec3 pt : intersectionPt)
		retour += pt;


	retour /= intersectionPt.size();


	if (retour.x > center.x + cubeSize || retour.x < center.x - cubeSize) retour.x = center.x;
	if (retour.z > center.z + cubeSize || retour.z < center.z - cubeSize) retour.z = center.z;
	if (retour.y > center.y + cubeSize * 1.5 || retour.y < center.y - cubeSize * 1.5) retour.y = center.y;


	return retour;
}

void UserAssets::createOrbit()
{
	float k = (float)0.017199540;

	{
		Entity entity = g_coordinator.createEntity();

		Transform transform;
		g_coordinator.addComponent(entity, transform);

		Renderable renderable;
		renderable.color = glm::vec3(1.0, 1.0, 0.0);
		renderable.shader_name = "Cube";
		renderable.withGeometryShader = false;
		g_coordinator.addComponent(entity, renderable);

		SphereMesh spheremesh;
		g_coordinator.addComponent(entity, spheremesh);

		RigidBody rigidbody;
		rigidbody.mass = 1.0;
		rigidbody.velocity = glm::vec3(0.0, 0.0, 0.0);
		g_coordinator.addComponent(entity, rigidbody);
	}

	{
		Entity entity = g_coordinator.createEntity();

		Transform transform;
		transform.position.x = 10.0;

		g_coordinator.addComponent(entity, transform);

		Renderable renderable;
		renderable.color = glm::vec3(1.0, 0.0, 0.0);
		renderable.shader_name = "Cube";
		renderable.withGeometryShader = false;
		g_coordinator.addComponent(entity, renderable);

		SphereMesh spheremesh;
		g_coordinator.addComponent(entity, spheremesh);

		RigidBody rigidbody;
		rigidbody.mass = (float)(0.000003003);
		rigidbody.velocity = glm::vec3(0.0, 0.0, -k / sqrt(transform.position.x)*time_factor);
		g_coordinator.addComponent(entity, rigidbody);
	}
}

void UserAssets::createNBox(unsigned int N)
{
	for (unsigned int i = 0; i < N; i++) {

		Entity entity = g_coordinator.createEntity();

		Transform transform;
		transform.position.x = 2.0f*i;
		transform.position.y = 2.0f*i;
		transform.position.z = 2.0f*i;
		g_coordinator.addComponent(entity, transform);

		Renderable renderable;
		renderable.color = glm::vec3(1.0, 0.0, 0.0);
		renderable.shader_name = "Cube";
		renderable.withGeometryShader = false;
		g_coordinator.addComponent(entity, renderable);

		CubeMesh cubemesh;
		g_coordinator.addComponent(entity, cubemesh);

		RigidBody rigidbody;
		rigidbody.mass = 1.0;
		rigidbody.useGravity = false;
		g_coordinator.addComponent(entity, rigidbody);

		/*Collider test;
		test.name = box;
		test.type = Static;
		std::shared_ptr<BoxCollider> BC(new BoxCollider);
		test.BoxCollider = BC;
		g_coordinator.addComponent(entity, test);*/

		ColliderHierarchy CH;
		Collider test;
		test.name = box;
		test.type = Static;
		std::shared_ptr<BoxCollider> BC(new BoxCollider);
		test.BoxCollider = BC;
		CH.colliders.push_back(test);
		g_coordinator.addComponent(entity, CH);
	}
};

void UserAssets::createNSphere(unsigned int N)
{
	for (unsigned int i = 0; i < N; i++) {

		Entity entity = g_coordinator.createEntity();

		Transform transform;
		transform.position.x = 2.0f*i;
		transform.position.y = 2.0f*i;
		transform.position.z = 2.0f*i;
		g_coordinator.addComponent(entity, transform);

		Renderable renderable;
		renderable.color = glm::vec3(1.0, 0.0, 0.0);
		renderable.shader_name = "Cube";
		renderable.withGeometryShader = false;
		g_coordinator.addComponent(entity, renderable);

		SphereMesh mesh;
		g_coordinator.addComponent(entity, mesh);

		RigidBody rigidbody;
		rigidbody.mass = 1.0;
		rigidbody.useGravity = false;
		g_coordinator.addComponent(entity, rigidbody);

		Collider test;
		test.name = sphere;
		test.type = Static;
		std::shared_ptr<SphereCollider> SC(new SphereCollider);
		test.SphereCollider = SC;
		g_coordinator.addComponent(entity, test);
	}
};

void UserAssets::test2balls()
{
	/*{
		Entity entity = g_coordinator.createEntity();
		Transform transform;
		g_coordinator.addComponent(entity, transform);
		Renderable renderable;
		renderable.color = glm::vec3(1.0, 0.0, 0.0);
		renderable.shader_name = "Cube";
		renderable.withGeometryShader = false;
		g_coordinator.addComponent(entity, renderable);
		SphereMesh spheremesh;
		g_coordinator.addComponent(entity, spheremesh);
		RigidBody rigidbody;
		rigidbody.mass = 1.0;
		rigidbody.useGravity = false;
		g_coordinator.addComponent(entity, rigidbody);
		Collider sc;
		sc.name = "SPHERE";
		sc.type = Kinematic;
		sc.size.x = 1 * max(transform.scale);
		g_coordinator.addComponent(entity, sc);
	}*/

	/*{
		Entity entity = g_coordinator.createEntity();
		Transform transform;
		transform.position.x = 5.0f;
		g_coordinator.addComponent(entity, transform);
		Renderable renderable;
		renderable.color = glm::vec3(1.0, 0.0, 0.0);
		renderable.shader_name = "Cube";
		renderable.withGeometryShader = false;
		g_coordinator.addComponent(entity, renderable);
		SphereMesh spheremesh;
		g_coordinator.addComponent(entity, spheremesh);
		RigidBody rigidbody;
		rigidbody.mass = 1.0;
		rigidbody.useGravity = false;
		g_coordinator.addComponent(entity, rigidbody);
		Collider sc;
		sc.name = sphere;
		sc.type = Kinematic;
		std::shared_ptr<SphereCollider> SC(new SphereCollider);
		sc.SphereCollider = SC;
		g_coordinator.addComponent(entity, sc);
	}*/

	{
		Entity entity = g_coordinator.createEntity();

		Transform transform;
		transform.position.x = 0.0f;
		g_coordinator.addComponent(entity, transform);

		Renderable renderable;
		renderable.color = glm::vec3(1.0, 0.0, 0.0);
		renderable.shader_name = "Cube";
		renderable.withGeometryShader = false;
		g_coordinator.addComponent(entity, renderable);

		CubeMesh cubemesh;
		g_coordinator.addComponent(entity, cubemesh);

		RigidBody rigidbody;
		rigidbody.mass = 1.0;
		rigidbody.useGravity = false;
		g_coordinator.addComponent(entity, rigidbody);

		ColliderHierarchy CH;
		Collider test;
		test.name = box;
		test.type = Kinematic;
		std::shared_ptr<BoxCollider> BC(new BoxCollider);
		test.BoxCollider = BC;
		CH.colliders.push_back(test);
		g_coordinator.addComponent(entity, CH);
	}

	{
		Entity entity = g_coordinator.createEntity();

		Transform transform;
		transform.position.y = 5.0f;
		g_coordinator.addComponent(entity, transform);

		Renderable renderable;
		renderable.color = glm::vec3(1.0, 0.0, 0.0);
		renderable.shader_name = "Cube";
		renderable.withGeometryShader = false;
		g_coordinator.addComponent(entity, renderable);

		CubeMesh cubemesh;
		g_coordinator.addComponent(entity, cubemesh);

		RigidBody rigidbody;
		rigidbody.mass = 1.0;
		rigidbody.useGravity = false;
		g_coordinator.addComponent(entity, rigidbody);

		ColliderHierarchy CH;

		Collider test;
		test.name = box;
		test.type = Static;
		std::shared_ptr<BoxCollider> BC(new BoxCollider);
		test.BoxCollider = BC;
		CH.colliders.push_back(test);

		Collider test2;
		test2.name = box;
		test2.type = Static;
		std::shared_ptr<BoxCollider> BC2(new BoxCollider);
		BC2->center = glm::vec3(0.0f, -1.0f, 0.0f);
		test2.BoxCollider = BC2;
		CH.colliders.push_back(test2);

		g_coordinator.addComponent(entity, CH);
	}
}

float max(glm::vec3 v) {
	float m = 0;

	if (v.x > m)
		m = v.x;
	if (v.y > m)
		m = v.y;
	if (v.z > m)
		m = v.z;

	return m;
}