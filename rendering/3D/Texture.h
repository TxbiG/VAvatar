#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include"shaderClass.h"

class Texture
{
public:
	GLuint ID;
	const char* type;
	int width;
	int height;
	GLuint unit;

	Texture(const char* image, const char* texType, GLuint slot);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);

	//void setTexture();
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete() const;
};
#endif // TEXTURE_CLASS_H