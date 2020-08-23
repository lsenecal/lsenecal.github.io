#include "Texture.h"



Texture::Texture()
	: width(0), height(0), internal_Format(GL_RGB), image_Format(GL_RGB), wrap_S(GL_REPEAT), wrap_T(GL_REPEAT), filter_Min(GL_LINEAR), filter_Max(GL_LINEAR)
{
	glGenTextures(1, &this->ID);
}


Texture::~Texture()
{
}

void Texture::generate(GLuint width, GLuint height, unsigned char* data)
{
	this->width = width;
	this->height = height;
	// Create Texture
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, this->internal_Format, width, height, 0, this->image_Format, GL_UNSIGNED_BYTE, data);
	// Set Texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filter_Max);
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}

void Texture::active() const
{
	glActiveTexture(GL_TEXTURE0 + this->ID);
}