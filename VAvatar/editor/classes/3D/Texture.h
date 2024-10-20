#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>

#include"shaderClass.h"

/*
GL_TEXTURE_1D: Images in this texture all are 1-dimensional. They have width, but no height or depth.
GL_TEXTURE_2D: Images in this texture all are 2-dimensional. They have width and height, but no depth.
GL_TEXTURE_3D: Images in this texture all are 3-dimensional. They have width, height, and depth.
GL_TEXTURE_RECTANGLE: The image in this texture (only one image. No mipmapping) is 2-dimensional. Texture coordinates used for these textures are not normalized.
GL_TEXTURE_BUFFER: The image in this texture (only one image. No mipmapping) is 1-dimensional. The storage for this data comes from a Buffer Object.
GL_TEXTURE_CUBE_MAP: There are exactly 6 distinct sets of 2D images, each image being of the same size and must be of a square size. These images act as 6 faces of a cube.
GL_TEXTURE_1D_ARRAY: Images in this texture all are 1-dimensional. However, it contains multiple sets of 1-dimensional images, all within one texture. The array length is part of the texture's size.
GL_TEXTURE_2D_ARRAY: Images in this texture all are 2-dimensional. However, it contains multiple sets of 2-dimensional images, all within one texture. The array length is part of the texture's size.
GL_TEXTURE_CUBE_MAP_ARRAY: Images in this texture are all cube maps. It contains multiple sets of cube maps, all within one texture. The array length * 6 (number of cube faces) is part of the texture size.
GL_TEXTURE_2D_MULTISAMPLE: The image in this texture (only one image. No mipmapping) is 2-dimensional. Each pixel in these images contains multiple samples instead of just one value.
GL_TEXTURE_2D_MULTISAMPLE_ARRAY: Combines 2D array and 2D multisample types. No mipmapping.
*/

class Texture
{
public:
	Texture(const char* path, GLenum texType, GLuint slot, GLenum format, GLenum pixelType);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);

	void Bind();	// Binds a texture
	void Unbind();	// Unbinds a texture
	void Delete();	// Deletes a texture
	
	GLuint ID;
	GLenum type;
};
#endif // TEXTURE_CLASS_H