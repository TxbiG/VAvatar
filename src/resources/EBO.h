#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include<glad/glad.h>

class EBO
{
public:
	// Constructor that generates a Elements Buffer Object and links it to indices
	EBO(GLuint* indices, GLsizeiptr size);

	void Bind();			// Binds the Elements Buffer Object
	void Unbind();			// Unbinds the Elements Buffer Object
	void Delete() const;	// Deletes the Elements Buffer Object

	GLuint ID;				// ID reference of Elements Buffer Object
};
#endif // EBO_CLASS_H
