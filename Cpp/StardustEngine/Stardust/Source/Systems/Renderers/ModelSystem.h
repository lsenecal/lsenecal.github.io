#pragma once
// core
#include "../../Core/System.h"
#include "../../Core/Coordinator.h"

// graphics
#include "../../Graphics/Shader.h"
#include "../../Graphics/InstancedModel.h"

// component
#include "../../Components/Mesh.h"
#include "../../Components/Transform.h"
#include "../../Components/Renderable.h"

// others
#include "VAOVBO.h"
#include "../../Camera.h"

#include <GLFW/glfw3.h>
#include <map>

extern Coordinator g_coordinator;

class ModelSystem : public System
{
public:
	void init()
	{
		first_initialisation = true;
	}

	void render(Camera * camera)
	{
		if (first_initialisation)
		{
			for (Entity entity : _entities)
			{
				std::string name = g_coordinator.getComponent<Model3D>(entity).name;

				if (_models.find(name) == _models.end()) // this model isn't in the map
				{
					std::cout << "Add " << name << " to map of models" << std::endl;
					std::string path = g_coordinator.getComponent<Model3D>(entity).path;
					_models[name] = InstancedModel(path);
				}

				Transform transform = g_coordinator.getComponent<Transform>(entity);
				_models[name].add(transform.position, transform.rotation, transform.scale);
			}

			first_initialisation = false;
		}
		for (std::map<std::string, InstancedModel>::iterator it = _models.begin(); it != _models.end(); ++it)
			it->second.render(camera);

	}

private:
	std::map<std::string, InstancedModel> _models;
	bool first_initialisation;
};

