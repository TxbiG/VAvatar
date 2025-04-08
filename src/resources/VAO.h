#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include"VBO.h"

class VAO
{
public:
	VAO();		// Constructor that generates a Vertex Array Object ID

	// Links a VBO Attribute such as a position or color to the Vertex Array Object
	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset);

	void Bind();		// Binds the Vertex Array Object
	void Unbind();		// Unbinds the Vertex Array Object
	void Delete();		// Deletes the Vertex Array Object

	GLuint ID;			// ID reference for the Vertex Array Object
};
#endif // VAO_CLASS_H