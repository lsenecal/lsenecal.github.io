#pragma once

#include "Texture.h"

#include <unordered_map>
#include <string>
#include <iostream>
#include <stb_image.h>


class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	// load a new texture to our storage if we find it and return the ID
	unsigned int add(std::string name, bool alpha = false);

	// return the texture by name
	Texture * get(std::string name);

	// bind properly the texture and active it
	void bindAndActive(std::string name);
	void bindAndActive(int index);

	// properly de-allocates all loaded shaders
	void clear();

private:
	// Storage
	std::vector <std::pair <std::string, Texture *>> _textures;

	// load and generate a texture from file
	Texture * loadTextureFromFile(std::string name, bool alpha);
	unsigned int contains(std::string name);
};


