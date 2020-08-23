/* © Copyright 2019 - Clem'Engine par Svetlana SOLOMINA et Félix ROLLET-GIRARD - Tous droits réservés */
#include "Skybox.h"


Skybox::Skybox(Shader * shader)
{
	_shader = *shader;
	//m_shader = new Shader("Data/Shader/World/skybox.vs", "Data/Shader/World/skybox.fs");

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	_themes.insert({"Nebula",0});
	_themes.insert({"Miramar",0 });
	_themes.insert({"Starfield",0});

	nameSky = "Starfield";
	//_currentTheme = 1;

	for (std::map<std::string, unsigned int>::iterator it = _themes.begin(); it != _themes.end(); ++it)
	{
		nameSky = it->first;
		std::vector<std::string> faces
		{
			"Assets/Textures/skybox/" + nameSky + "/right.tga",
			"Assets/Textures/skybox/" + nameSky + "/left.tga",
			"Assets/Textures/skybox/" + nameSky + "/top.tga",
			"Assets/Textures/skybox/" + nameSky + "/bottom.tga",
			"Assets/Textures/skybox/" + nameSky + "/front.tga",
			"Assets/Textures/skybox/" + nameSky + "/back.tga"
		};
		it->second = loadCubemap(faces);
		//std::cout << it->first << " " << it->second << std::endl;
	}
	//_cubemapTextureID = loadCubemap(faces);


//	_shader->use();
//	_shader->setInt("skybox", 0);
}



void Skybox::render(Camera * camera) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	_cubemapTextureID = _themes.find(nameSky)->second;

	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	_shader.use();

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor()); 

	glm::mat4 model(1.0f);
	//model = glm::rotate(model, glm::radians((float)glfwGetTime()), glm::vec3(0.0, -1.0, 0.0));
	_shader.setMat4("model", model);
	glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
	_shader.setMat4("view", view);
	_shader.setMat4("projection", camera->getProjection());

	_shader.setInt("skybox", _cubemapTextureID);

	// skybox cube
	glBindVertexArray(_vao);
	glActiveTexture(GL_TEXTURE0+_cubemapTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemapTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}

Skybox::~Skybox()
{
	/*glDeleteProgram(_shader.ID);
	for (auto texture : _themes)
		glDeleteTextures(1, (const GLuint*) &texture.second);*/
}


// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int Skybox::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


void Skybox::setNextThemes()
{
	//_cubemapTextureID = loadCubemap(faces);
}
