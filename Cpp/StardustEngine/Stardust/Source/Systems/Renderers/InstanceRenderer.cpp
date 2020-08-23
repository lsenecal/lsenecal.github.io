#include "InstanceRenderer.h"



InstanceRenderer::InstanceRenderer()
{
}


InstanceRenderer::~InstanceRenderer()
{
}

void InstanceRenderer::produce()
{

}

void InstanceRenderer::createVAOVBO(VAO * vao, VBO * vbos, std::set<Entity>* entities, std::vector<glm::vec3> * vvertices, std::vector<glm::vec3> * vnormals, int * numberOfElements, Shader * shader)
{
	*numberOfElements = entities->size();

	std::cout << "Instance Renderer " << *numberOfElements << std::endl;
	
	glm::vec3 * position = new glm::vec3[*numberOfElements]; // geometric centers
	glm::vec3 * color = new glm::vec3[*numberOfElements];

	std::vector<glm::vec3> * vertices = vvertices;
	std::vector<glm::vec3> * normals = vnormals;
	//_shader = new Shader("Assets/Shaders/Cube.vs", "Assets/Shaders/Cube.fs");

	int i = 0;
	for (Entity entity : *entities) {
		position[i] = g_coordinator.getComponent<Transform>(entity).position;
		color[i] = g_coordinator.getComponent<Renderable>(entity).color;
		i++;
	}
	// set gl buffers
	
	glGenBuffers(1, &vbos->offsets);
	glBindBuffer(GL_ARRAY_BUFFER, vbos->offsets);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * *numberOfElements, &position[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbos->colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbos->colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* *numberOfElements, &color[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	free(color);
	free(position);

	// set VAO/VBO
	glGenVertexArrays(1, &vao->points);
	glBindVertexArray(vao->points);
	// Vertices
	glGenBuffers(1, &vbos->points);
	glBindBuffer(GL_ARRAY_BUFFER, vbos->points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices->size(), vertices->data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
	glEnableVertexAttribArray(0);

	// Surface normal
	glGenBuffers(1, &vbos->normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbos->normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals->size(), normals->data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
	glEnableVertexAttribArray(1);

	// offset
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbos->offsets);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);

	// colors
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, vbos->colors);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(3, 1);
}

void InstanceRenderer::render(Camera * camera, Shader * shader, VAO * vao, int * numberOfElements, int max_vertices)
{
	// for ECHAP
	if (camera->freeze) glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader->use();
	shader->setMat4("projection", camera->getProjection());
	shader->setMat4("view", camera->GetViewMatrix());
	shader->setFloat("u_time", 10.0f);
	glBindVertexArray(vao->points);
	glDrawArraysInstanced(GL_TRIANGLES, 0, max_vertices, *numberOfElements);

	glBindVertexArray(0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}
