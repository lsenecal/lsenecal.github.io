#pragma once
// vendor
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// other
#include "VAOVBO.h"
#include "../../Core/Types.h"
#include "../../Graphics/Shader.h"
#include "../../Camera.h"

// component
#include "../../Components/Renderable.h"
#include "../../Components/Transform.h"
#include "../../Components/Mesh.h"

// core
#include "../../Core/Coordinator.h"

// vendor
#include <set>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>


extern Coordinator g_coordinator;

class InstanceRenderer
{
public:
	InstanceRenderer();
	~InstanceRenderer();

	void produce();

	void createVAOVBO(VAO * vao, VBO * vbos, std::set<Entity>* entities, std::vector<glm::vec3> * vertices, std::vector<glm::vec3> * normals, int * numberOfElements, Shader * shader);
	void updateVAOVBO(VAO * vao, VBO * vbos, std::set<Entity>* entities, std::vector<glm::vec3> * vvertices, std::vector<glm::vec3> * vnormals, int * numberOfElements);

	void render(Camera * camera, Shader * shader, VAO * vao, int * numberOfElements, int max_vertices);
private:

};
