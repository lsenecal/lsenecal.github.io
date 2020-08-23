#include "TerrainRenderer.h"




void TerrainRenderer::init()
{
	_firstInitialisation = true;
	_nbPoints = 0;
}

void TerrainRenderer::produce()
{

}

void TerrainRenderer::render(Camera * camera)
{
	if (_firstInitialisation) {
		//std::cout << "t1" << std::endl;
		createVAOVBO();
		//std::cout << "t2" << std::endl;
		_shader = new Shader("Assets/Shaders/Terrain.vs.c", "Assets/Shaders/Terrain.fs.c", "Assets/Shaders/Terrain.gs.c");
		//std::cout << "t3" << std::endl;
		_firstInitialisation = false;
	}
	//glEnable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);

	_shader->use();
	_shader->setMat4("projection", camera->getProjection());
	_shader->setMat4("view", camera->GetViewMatrix());
	_shader->setVec3("viewPos", camera->Position);
	_shader->setVec3("lightPos", /*camera->Position +*/ glm::vec3(50.0,150.0,50.0));
	_shader->setVec3("lightColor", glm::vec3(1.0));
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, _nbPoints);
	glBindVertexArray(0);


	//glDisable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);
}

void TerrainRenderer::createVAOVBO()
{
	std::vector<glm::vec3> pos, norm;
	std::vector<glm::vec4> color;
	for (Entity entity : _entities) {
		pos = g_coordinator.getComponent<Terrain>(entity).points;
		norm = g_coordinator.getComponent<Terrain>(entity).normale;
		color = g_coordinator.getComponent<Terrain>(entity).colors;
	}
	_nbPoints = pos.size();

	glGenBuffers(1, &_vboPos);
	glBindBuffer(GL_ARRAY_BUFFER, _vboPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * pos.size(), pos.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &_vboNormal);
	glBindBuffer(GL_ARRAY_BUFFER, _vboNormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * norm.size(), norm.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &_vboColor);
	glBindBuffer(GL_ARRAY_BUFFER, _vboColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * color.size() , color.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vboPos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _vboNormal);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, _vboColor);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
