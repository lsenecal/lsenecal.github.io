#include "MarchingCubeRS.h"



void MarchingCubeRS::init()
{
	_firstInitialisation = true;
}

void MarchingCubeRS::produce()
{
	if (_firstInitialisation) {
		createVAOVBO();
	}
}

void MarchingCubeRS::render(Camera * camera)
{
	if (_firstInitialisation) {
		//std::cout << "t1" << std::endl;
		createVAOVBO();
		//std::cout << "t2" << std::endl;
		_shader = new Shader("Assets/Shaders/CubeCentersH3.vs", "Assets/Shaders/CubeCentersH3.gs", "Assets/Shaders/CubeCentersH3.fs");
		//std::cout << "t3" << std::endl;
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	_shader->use();
	_shader->setMat4("projection", camera->getProjection());
	_shader->setMat4("view", camera->GetViewMatrix());
	_shader->setFloat("u_time", (float)glfwGetTime());
	_shader->setFloat("u_cubeSize", 1.0f);
	_shader->setFloat("u_diviseur", 25.0f);
	_shader->setFloat("u_freq", 0.4f);
	_shader->setInt("u_octaves", 6);
	_shader->setFloat("u_amplitude", 0.5f);
	_shader->setFloat("u_seuil", 0.5f);
	_shader->setFloat("u_noiseX", 0.0f);
	_shader->setFloat("u_noiseY", 0.0f);
	_shader->setFloat("u_noiseZ", 0.0f);

	//std::cout << _data->getCubeCenters()->getAmplitude() << std::endl;
	glBindVertexArray(_vao);
	//std::cout << _numberOfElements << std::endl;
	glDrawArrays(GL_POINTS, 0, _entities.size());
	glBindVertexArray(0);
}

void MarchingCubeRS::createVAOVBO()
{
	std::vector<glm::vec3>  position ;
	std::vector<glm::vec3> color;
	std::vector<float> marchingIDs;
	int i = 0;
	for (Entity entity : _entities) {
		position.push_back(g_coordinator.getComponent<Transform>(entity).position);
		marchingIDs.push_back(g_coordinator.getComponent<Terrain>(entity).marchingID);
		color.push_back(g_coordinator.getComponent<Renderable>(entity).color);
		i++;
	}
	
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) *_entities.size(),position.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &_vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _entities.size(), marchingIDs.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &_vboColor);
	glBindBuffer(GL_ARRAY_BUFFER, _vboColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * _entities.size(), color.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo2);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, _vboColor);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	_firstInitialisation = false;

	//free(position);
}