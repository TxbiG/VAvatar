#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glm/glm.hpp>
#include<glad/glad.h>

class VBO
{
public:
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(GLfloat* vertices, GLsizeiptr size);

	void Bind();		// Binds the Vertex Buffer Object
	void Unbind();		// Unbinds the Vertex Buffer Object
	void Delete();		// Deletes the Vertex Buffer Object

	GLuint ID; 			// Reference ID of the Vertex Buffer Object
};
#endif // VBO_CLASS_H