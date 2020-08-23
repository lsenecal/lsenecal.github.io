#include "Texture.h"



Texture::Texture(int type)
	: width(0), height(0), internal_Format(GL_RGB), image_Format(GL_RGB), wrap_S(GL_REPEAT), wrap_T(GL_REPEAT), filter_Min(GL_LINEAR_MIPMAP_LINEAR), filter_Max(GL_LINEAR)
{
	_textureType = type;
	glGenTextures(1, &this->ID);
}


Texture::~Texture()
{
}

void Texture::generate(GLuint width, GLuint height, unsigned char* data)
{
	if (_textureType == TEXTURE2D)
	{
		this->width = width;
		this->height = height;
		glBindTexture(GL_TEXTURE_2D, this->ID);

		// Set Texture wrap and filter modes
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrap_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrap_T);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filter_Min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filter_Max);

		// Create Texture
		glTexImage2D(GL_TEXTURE_2D, 0, this->internal_Format, width, height, 0, this->image_Format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else if (_textureType == TEXTURE2D_ARRAY)
	{
		this->width = width;
		this->height = height;
		glBindTexture(GL_TEXTURE_2D_ARRAY, this->ID);

		// Set Texture wrap and filter modes
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, this->wrap_S);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, this->wrap_T);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, this->filter_Min);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, this->filter_Max);

		// Create Texture
		int number_width = 8;
		int number_height = 8;
		int subWidth = width / number_width;
		int subHeight = height / number_height;
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 8, GL_RGBA8, subWidth, subHeight, number_width*number_height);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, height);

		for (int iRow = 0; iRow < number_height; ++iRow) {
			for (int iCol = 0; iCol < number_width; ++iCol) {
				glTexSubImage3D(
					GL_TEXTURE_2D_ARRAY, 0,
					0, 0, number_width * iRow + iCol,
					subWidth, subHeight, 1,
					this->image_Format, GL_UNSIGNED_BYTE,
					data);
			}
		}

		// Unbind texture
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

}

void Texture::bind() const
{
	if (_textureType == TEXTURE2D) glBindTexture(GL_TEXTURE_2D, this->ID);
	if (_textureType == TEXTURE2D_ARRAY) glBindTexture(GL_TEXTURE_2D_ARRAY, this->ID);
}

void Texture::active() const
{
	glActiveTexture(GL_TEXTURE0 + this->ID);
}