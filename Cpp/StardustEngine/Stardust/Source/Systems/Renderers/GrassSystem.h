#pragma once

// core
#include "../../Core/System.h"
#include "../../Core/Coordinator.h"

// graphics
#include "../../Graphics/Shader.h"

// component
#include "../../Components/Patch.h"

// others
#include "VAOVBO.h"
#include "../../Camera.h"
#include "../../Graphics/TextureManager.h"

#include <GLFW/glfw3.h>
#include <map>
extern Coordinator g_coordinator;

class GrassSystem : public System
{
private:
	void createVAOVBO(Camera * camera);
	VAO _vao; VBO _vbos;
	int VERTICES_SIZE;
	std::unique_ptr<Shader> _geometryShader;
	bool _firstInit;

	glm::vec3 patch, _camPos;
	std::shared_ptr<TextureManager> _textures;
	std::string _nameBillboard, _nameBlade01, _nameWind, _namebloom;
	std::vector<std::pair<const char *, bool>> _texturesNames;

	void updateVAOVBO(Camera * camera);


public:
	void init(std::shared_ptr<TextureManager> textures);
	void render(Camera * camera);


};