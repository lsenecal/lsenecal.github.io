#pragma once
// vendor
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Core
#include "../../Core/System.h"
#include "../../Core/Coordinator.h"

// Graphics 
#include "../../Graphics/Shader.h"

// other
#include "VAOVBO.h"
#include "InstanceRenderer.h"
#include "../../Camera.h"

extern Coordinator g_coordinator;

class RenderSystem : public System
{
public:

	void render(Camera * camera);

	virtual void init(std::shared_ptr<InstanceRenderer> ir) = 0;

protected:
	int _maxVertices;
	bool _firstInitialisation;
	
	Shader * _shader;
	int _numberOfElements;

	VAO _vao;
	VBO _vbos;

	std::vector <glm::vec3> * _vertices;
	std::vector <glm::vec3> * _normals;

	std::shared_ptr<InstanceRenderer> _instanceRenderer;

	void initRenderSystem(std::shared_ptr<InstanceRenderer> ir);

};


