#include "GrassSystem.h"

void GrassSystem::init(std::shared_ptr<TextureManager> textures)
{
	_textures = textures;
	_firstInit = true;

	// wind
	_texturesNames.push_back({ "flowmap.png", false });
	// grass
	_texturesNames.push_back({ "grass_atlas.png", true }); //_texturesNames.push_back({ "grass_atlas_specular.png", true });
}

void GrassSystem::render(Camera * camera)
{
	if (_firstInit)
	{
		createVAOVBO(camera);
		_firstInit = false;
	}
	//else updateVAOVBO(camera);

	/*glEnable(GL_MULTISAMPLE); // anti aliazing
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
	//glEnable(GL_CULL_FACE); glCullFace(GL_BACK);


	for (auto texture : _texturesNames)
		_textures->bindAndActive(texture.first);

	_geometryShader->use();
	_geometryShader->setMat4("u_projection", camera->getProjection());
	_geometryShader->setMat4("u_view", camera->GetViewMatrix());
	_geometryShader->setVec3("u_cameraPosition", camera->Position);
	_geometryShader->setVec3("u_cameraFront", camera->Front);
	_geometryShader->setMat4("u_model", glm::mat4(1.0f));
	_geometryShader->setFloat("u_time", glfwGetTime());

	// draw points
	glBindVertexArray(_vao.points);
	//glDrawArraysInstanced(GL_POINTS, 0, VERTICES_SIZE, 1);
	glDrawArrays(GL_POINTS, 0, VERTICES_SIZE);

	glBindVertexArray(0);
	glUseProgram(0);
	/*glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);*/
}

void GrassSystem::updateVAOVBO(Camera * camera)
{
	Patches * p = &g_coordinator.getComponent<Patches>(*_entities.begin());
	glBindBuffer(GL_ARRAY_BUFFER, _vbos.points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * VERTICES_SIZE, p->positions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbos.normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * VERTICES_SIZE, p->normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbos.dimensions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * VERTICES_SIZE, p->dimensions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GrassSystem::createVAOVBO(Camera * camera)
{
	_camPos = camera->Position;
	_geometryShader = std::make_unique<Shader>("Assets/Shaders/Grass.vs", "Assets/Shaders/Grass.fs.c", "Assets/Shaders/Grass.gs.c");

	Patches p = g_coordinator.getComponent<Patches>(*_entities.begin());
	VERTICES_SIZE = p.positions.size();

	_geometryShader->use();
	int i = 0;
	for (auto t : _texturesNames)
	{
		int textureID;
		textureID = _textures->add(TEXTURE2D, t.first, t.second);
		_geometryShader->setInt("u_texture" + std::to_string(i), textureID);
		i++;
	}
	glUseProgram(0);

	glGenBuffers(1, &_vbos.points);
	glBindBuffer(GL_ARRAY_BUFFER, _vbos.points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * VERTICES_SIZE, p.positions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &_vbos.normals);
	glBindBuffer(GL_ARRAY_BUFFER, _vbos.normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * VERTICES_SIZE, p.normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &_vbos.dimensions);
	glBindBuffer(GL_ARRAY_BUFFER, _vbos.dimensions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * VERTICES_SIZE, p.dimensions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &_vao.points);
	glBindVertexArray(_vao.points);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbos.points);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _vbos.normals);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, _vbos.dimensions);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}