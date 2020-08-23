#pragma once
// vendor
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Core
#include "../../Core/System.h"
#include "../../Core/Coordinator.h"

#include "../../Components/Terrain.h"

// Graphics 
#include "../../Graphics/Shader.h"

// other
#include "VAOVBO.h"
#include "InstanceRenderer.h"
#include "../../Camera.h"

extern Coordinator g_coordinator;

class MarchingCubeRS : public System
{
public:
	void init();
	void produce();
	void render(Camera * camera);

private:
	Shader * _shader;
	unsigned int _vao, _vbo, _vbo2, _vboColor;
	unsigned int _vaoTest, _vboTest;
	bool _firstInitialisation;

	void createVAOVBO();
};

