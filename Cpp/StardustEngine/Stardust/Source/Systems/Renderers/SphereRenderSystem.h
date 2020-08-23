#pragma once
#include "RenderSystem.h"
#define PI 3.1415926535f

class SphereRenderSystem : public RenderSystem 
{
public:
	virtual void init(std::shared_ptr<InstanceRenderer> ir) override
	{
		initRenderSystem(ir);
		_vertices = &sphere_vertices;
		_normals = &sphere_normals;
		generateVertices(_vertices, _normals);
		_maxVertices = _vertices->size();
	}
private:
	void generateVertices(std::vector <glm::vec3> * v, std::vector <glm::vec3> * n) {

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		float radius = 1.0;
		unsigned int sectorsNumber = 33;
		unsigned int stacksNumber = 33;
		
		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal

		float sectorStep = 2.0f * PI / sectorsNumber;
		float stackStep = PI / stacksNumber;
		float sectorAngle, stackAngle;

		for (unsigned int i = 0; i <= stacksNumber; ++i)
		{
			stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xy = radius * cosf(stackAngle);             // r * cos(u)
			z = radius * sinf(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (unsigned int j = 0; j <= sectorsNumber; ++j)
			{
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// vertex position (x, y, z)
				x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
				vertices.push_back(glm::vec3(x, y, z));

				// normalized vertex normal (nx, ny, nz)
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				normals.push_back(glm::vec3(nx, ny, nz));
			}
		}

		std::vector<unsigned int> indices;
		int k1, k2;
		for (unsigned int i = 0; i < stacksNumber; ++i)
		{
			k1 = i * (sectorsNumber + 1);     // beginning of current stack
			k2 = k1 + sectorsNumber + 1;      // beginning of next stack

			for (unsigned int j = 0; j < sectorsNumber; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (stacksNumber - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}

		v->resize(indices.size());
		n->resize(indices.size());

		for (unsigned int i = 0; i < indices.size(); i++) {
			v->at(i) = vertices.at(indices.at(i));
			n->at(i) = normals.at(indices.at(i));
		}

	}
};

