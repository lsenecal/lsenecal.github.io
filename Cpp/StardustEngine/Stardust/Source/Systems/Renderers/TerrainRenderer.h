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
class TerrainRenderer : public System
{
public:
	// Public Methodes
	void init();
	void produce();
	void render(Camera * camera);

private :
	// Private Variables
	Shader * _shader;
	bool _firstInitialisation;

	unsigned int _vao, _vboPos, _vboNormal, _vboColor;
	unsigned int _nbPoints;

	// Private Methodes
	void createVAOVBO();

};

