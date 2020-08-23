#include "InstancedModel.h"



InstancedModel::InstancedModel(std::string modelPath)
{
	m_shader = Shader("Assets/Shaders/model.vs.c", "Assets/Shaders/model.fs.c");
	m_model = Model(modelPath);

	m_numberOfModel = 0;

	tab_position = std::vector<glm::vec3>();
	tab_rotation = std::vector<glm::vec3>();
	tab_dimension = std::vector<glm::vec3>();

	m_first_initialisation = true;
}


InstancedModel::~InstancedModel()
{
}

int InstancedModel::add(glm::vec3 position, glm::vec3 rotation, glm::vec3 dimension)
{
	tab_position.push_back(position);
	tab_rotation.push_back(rotation);
	tab_dimension.push_back(dimension);

	m_numberOfModel++;

	return m_numberOfModel - 1;
}

void InstancedModel::add(std::vector<glm::vec3> & position, std::vector<glm::vec3> & rotation, std::vector<glm::vec3> & dimension)
{
	tab_position = position;
	tab_rotation = rotation;
	tab_dimension = dimension;

	m_numberOfModel = position.size();
}

void InstancedModel::render(Camera * cam)
{
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (m_first_initialisation) {
		produceVAOVBO();
		m_first_initialisation = false;
	}

	m_shader.use(); // /!\ assurer de ne pas juste remetre le même shader !
	m_shader.setMat4("u_view", cam->GetViewMatrix());
	m_shader.setMat4("u_projection", cam->getProjection());
	m_shader.setVec3("u_lightPos", cam->Position);
	m_shader.setVec3("u_lightColor", glm::vec3(1));
	m_shader.setVec3("u_cameraPosition", cam->Position);
	m_shader.setVec3("u_cameraFront", cam->Front);
	m_shader.setFloat("u_time", glfwGetTime());

	m_shader.setInt("texture_diffuse1", 0);
	/*if (m_model.meshes[i].textures.size() > 1)
		m_shader.setInt("texture_specular2", 1);
	if (m_model.meshes[i].textures.size() > 2)
	m_shader.setInt("texture_normal3", 2);*/

	//m_shader->setFloat("time", glfwGetTime());
	for (unsigned int i = 0; i < m_model.meshes.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_model.meshes[i].textures[0].id);

		if (m_model.meshes[i].textures.size() > 1)
		{
			m_shader.setInt("texture_specular2", 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_model.meshes[i].textures[1].id);
		}

		if (m_model.meshes[i].textures.size() > 2)
		{
			m_shader.setInt("texture_normal3", 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_model.meshes[i].textures[2].id);
		}

	    //if(m_model->meshes[i].textures[0].id == m_model->textures_loaded.at(3).id) m_shader->setBool("needles", true);
		//else m_shader->setBool("needles", false);

		glBindVertexArray(m_model.meshes[i].VAO);
		//glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElementsInstanced(GL_TRIANGLES, m_model.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, m_numberOfModel);
		glBindVertexArray(0);
	}

	glBindVertexArray(0);
	glUseProgram(0);
	//glDisable(GL_BLEND);
}


void InstancedModel::produceVAOVBO()
{
	glGenBuffers(1, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_numberOfModel, tab_position.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &rotationVBO);
	glBindBuffer(GL_ARRAY_BUFFER, rotationVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_numberOfModel, tab_rotation.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &dimensionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, dimensionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_numberOfModel, tab_dimension.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (unsigned int i = 0; i < m_model.meshes.size(); i++)
	{
		unsigned int VAO = m_model.meshes[i].VAO;
		glBindVertexArray(VAO);

		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(5, 1);

		glEnableVertexAttribArray(6);
		glBindBuffer(GL_ARRAY_BUFFER, rotationVBO);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(6, 1);

		glEnableVertexAttribArray(7);
		glBindBuffer(GL_ARRAY_BUFFER, dimensionVBO);
		glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(7, 1);

		glBindVertexArray(0);
	}
}
