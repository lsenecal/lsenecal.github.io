#include "TextureManager.h"



TextureManager::TextureManager()
{
}


TextureManager::~TextureManager()
{
	clear();
	for (auto texture : _textures)
		delete &texture.second;
	_textures.clear();
}

unsigned int TextureManager::add(std::string name, bool alpha)
{
	int i = contains(name);
	if (i != -1) return i;

	Texture * texture = loadTextureFromFile(name, alpha);
	if (texture != nullptr) {
		_textures.push_back(std::make_pair(name, texture));
		return texture->ID;
	}
	else
	{
		std::cerr << "TextureManager:: The texture with the name : " << name << " was not find." << std::endl;
		return -1;
	}
}

Texture * TextureManager::get(std::string name)
{
	int i = contains(name);
	if (i == -1) return NULL;
	else return _textures[i].second;
}

void TextureManager::bindAndActive(std::string name)
{
	Texture * text = get(name);
	if (text != nullptr)
	{
		text->active();
		text->bind();
	}
}

void TextureManager::bindAndActive(int index)
{
	Texture * texture = _textures.at(index).second;
	if (texture != nullptr)
	{
		texture->active();
		texture->bind();
	}
}

void TextureManager::clear()
{
	for (auto texture : _textures)
		glDeleteTextures(1, &texture.second->ID);
}


Texture * TextureManager::loadTextureFromFile(std::string name, bool alpha)
{
	Texture * texture = new Texture();
	unsigned int id;
	glGenTextures(1, &id);
	texture->ID = id;

	int width, height, nrComponents;
	unsigned char *data = stbi_load(("Assets/Textures/"+name).c_str(), &width, &height, &nrComponents, 0);
	
	texture->width = width;
	texture->height = height;

	if (data)
	{
		if (alpha)
		{
			texture->internal_Format = GL_RGBA;
			texture->image_Format = GL_RGBA;
			texture->wrap_S = GL_CLAMP_TO_EDGE;
			texture->wrap_T = GL_CLAMP_TO_EDGE;
		}
		else
		{
			texture->internal_Format = GL_RGB;
			texture->image_Format = GL_RGB;
			texture->wrap_S = GL_REPEAT;
			texture->wrap_T = GL_REPEAT;
		}
		texture->filter_Min = GL_LINEAR_MIPMAP_LINEAR;
		texture->filter_Max = GL_LINEAR;

		texture->generate(width, height, data);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << name << std::endl;
		stbi_image_free(data);
	}
	return texture;
}

unsigned int TextureManager::contains(std::string name)
{
	for (unsigned int i = 0 ; i < _textures.size(); ++i)
	{
		if (_textures.at(i).first == name) return i;
	}
	return -1;
}