#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<string>
#include<iostream>
#include<cerrno>

using namespace std;

std::string get_file_contents(const char* filename);

class Shader
{
public:
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile);
	//Shader(const char* vertexFile, const char* fragmentFile, const char* geometryFile);

	void Activate();		// Activates the Shader Program
	void Delete();			// Deletes the Shader Program

	GLuint ID;				// Reference ID of the Shader Program
private:
	// Checks if the different Shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);
};
#endif // SHADER_CLASS_H