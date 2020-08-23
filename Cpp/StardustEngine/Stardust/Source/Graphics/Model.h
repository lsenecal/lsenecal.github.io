#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "MeshType.h"

#include <stb_image.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <map>
#include <vector>
using namespace std;


unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model
{
public:
	/*  Model Data */
	vector<TextureType> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<MeshType> meshes;
	string directory;
	bool gammaCorrection;

	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model() {}

	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}

	// draws the model, and thus all its meshes
	void Draw(Shader shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

private:
	/*  Functions   */
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const &path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene)
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}

	void generateTangentsBitangents(MeshType * mesh)
	{
		for (int i = 0; i < mesh->indices.size(); i += 3)
		{
			glm::vec3 v0 = mesh->vertices[mesh->indices[i]].Position;		// A
			glm::vec3 v1 = mesh->vertices[mesh->indices[i + 1]].Position;		// B
			glm::vec3 v2 = mesh->vertices[mesh->indices[i + 2]].Position;		// C

			/*
			glm::vec3 normal = glm::vec3(0, 0, 0);
			glm::vec3 AB = v1 - v0;
			glm::vec3 BC = v2 - v1;
			// calcul du produit scalaire
			normal.x = AB.y * BC.z - AB.z * BC.y;
			normal.y = AB.z * BC.x - AB.x * BC.z;
			normal.z = AB.x * BC.y - AB.y * BC.x;
			normal = glm::normalize(normal);
			normal = -normal;
			*/

			glm::vec2 uv0 = mesh->vertices[mesh->indices[i]].TexCoords;
			glm::vec2 uv1 = mesh->vertices[mesh->indices[i + 1]].TexCoords;
			glm::vec2 uv2 = mesh->vertices[mesh->indices[i + 2]].TexCoords;

			glm::vec3 e1 = v1 - v0;
			glm::vec3 e2 = v2 - v0;

			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = (float) 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

			//glm::vec3 tangent = (e1 * (deltaUV2.y - (e2 * deltaUV1.y))) * r;
			//glm::vec3 bitangent = (e2 * (deltaUV1.x - (e1 * deltaUV2.x))) * r;
			glm::vec3 tangent = r * (deltaUV2.y * e1 - deltaUV1.y * e2);
			tangent = glm::normalize(tangent);
			glm::vec3 bitangent = r * (-deltaUV2.x * e1 + deltaUV1.x * e2);
			bitangent = glm::normalize(bitangent);

			mesh->vertices[mesh->indices[i]].Tangent = tangent;
			mesh->vertices[mesh->indices[i]].Bitangent = bitangent;
			//mesh->vertices[mesh->indices[i]].Normal = normal;

			mesh->vertices[mesh->indices[i + 1]].Tangent = tangent;
			mesh->vertices[mesh->indices[i + 1]].Bitangent = bitangent;
			//mesh->vertices[mesh->indices[i+1]].Normal = normal;

			mesh->vertices[mesh->indices[i + 2]].Tangent = tangent;
			mesh->vertices[mesh->indices[i + 2]].Bitangent = bitangent;
			//mesh->vertices[mesh->indices[i+2]].Normal = normal;
		}
	}

	MeshType processMesh(aiMesh *mesh, const aiScene *scene)
	{
		// Data to fill
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<TextureType> textures;

		// Walk through each of the mesh's vertices
		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

			// Positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			// Normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			//bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;

			// Texture Coordinates
			if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
			{
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}
			vertices.push_back(vertex);
		}


		// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// Retrieve all indices of the face and store them in the indices vector
			for (GLuint j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		// Process materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
			// Same applies to other texture as the following list summarizes:
			// Diffuse: texture_diffuseN
			// Specular: texture_specularN
			// Normal: texture_normalN

			// 1. diffuse maps
			vector<TextureType> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			vector<TextureType> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			std::vector<TextureType> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. height maps
			std::vector<TextureType> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			// return a mesh object created from the extracted mesh data
			return MeshType(vertices, indices, textures);
		}
		// Return a mesh object created from the extracted mesh data
		MeshType tmp = MeshType(vertices, indices, textures);
		//generateTangentsBitangents(&tmp);
		return tmp;
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a TextureType struct.
	vector<TextureType> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{
		vector<TextureType> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				TextureType texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};



#endif