#pragma once
#include "Model.h"
#include "../Camera.h"

#include "../Graphics/Shader.h"
#include <GLFW/glfw3.h>

class InstancedModel
{
public:
	InstancedModel(){}
	InstancedModel(std::string modelPath);
	~InstancedModel();

	int add(glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 dimension = glm::vec3(1.0f, 1.0f, 1.0f));
	void add(std::vector<glm::vec3>& position, std::vector<glm::vec3>& rotation, std::vector<glm::vec3>& dimension);
	void render(Camera * cam);

	void actualizeVAOVBO();
	void produceVAOVBO();

	void clear() { m_numberOfModel = 0; }

	std::vector<glm::vec3> & getPosition() { return tab_position; }
	std::vector<glm::vec3> & getRotation() { return tab_rotation; }
	std::vector<glm::vec3> & getDimension() { return tab_dimension; }

	void setPosition(int id, glm::vec3 p) { tab_position[id] = p; }
	void setRotation(int id, glm::vec3 r) { tab_rotation[id] = r; }
	void setDimension(int id, glm::vec3 s) { tab_dimension[id] = s; }

private:
	bool m_first_initialisation;
	int m_numberOfModel;

	Shader m_shader;
	Model m_model;

	std::vector <glm::vec3> tab_position;
	std::vector <glm::vec3> tab_rotation;
	std::vector <glm::vec3> tab_dimension;

	unsigned int positionVBO, rotationVBO, dimensionVBO;
	unsigned int modelVAO, modelVBO;
};

