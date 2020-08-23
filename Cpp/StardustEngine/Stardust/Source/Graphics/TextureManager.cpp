#include "TextureManager.h"



TextureManager::TextureManager()
{
}


TextureManager::~TextureManager()
{
	clear();
	_textures.clear();
}

unsigned int TextureManager::add(int type, std::string name, bool alpha)
{
	int i = contains(name);
	if (i != -1) return i;

	Texture * texture = loadTextureFromFile(type, name, alpha);
	if (texture != nullptr) {
		_textures.push_back(std::make_pair(name, texture));
		std::cout << name << "'s id : " << texture->ID << std::endl;
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


Texture * TextureManager::loadTextureFromFile(int type, std::string name, bool alpha)
{
	Texture * texture = new Texture(type);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(("Assets/Textures/" + name).c_str(), &width, &height, &nrComponents, 0);
	stbi_set_flip_vertically_on_load(false);

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
	for (unsigned int i = 0; i < _textures.size(); ++i)
	{
		if (_textures.at(i).first == name) return i;
	}
	return -1;
}