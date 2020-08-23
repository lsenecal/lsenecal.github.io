#pragma once
#include <iostream>
#include <glad/glad.h>

#define TEXTURE2D 0
#define TEXTURE3D 1
#define TEXTURE2D_ARRAY 2

class Texture
{
public:
	//constructor
	Texture(int type);
	//destructor
	~Texture();
	//texture ID
	GLuint ID;
	//texture image dimensions (in pixels)
	GLuint width, height;
	// Texture Format
	GLuint internal_Format; // Format of texture object
	GLuint image_Format; // Format of loaded image
	// Texture configuration
	GLuint wrap_S; // Wrapping mode on S axis
	GLuint wrap_T; // Wrapping mode on T axis
	GLuint filter_Min; // Filtering mode if texture pixels < screen pixels
	GLuint filter_Max; // Filtering mode if texture pixels > screen pixels

	// Generates texture from image data
	void generate(GLuint width, GLuint height, unsigned char* data);
	// Binds the texture as the current active GL_TEXTURE_2D texture object
	void bind() const;
	// active the texture
	void active() const;

private:
	int _textureType;
};
