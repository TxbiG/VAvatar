#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>

#include "VAO.h"
#include "EBO.h"
#include "Texture.h"
#include "Camera3D.h"

using namespace std;

/*
class Mesh
{
public:
	Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures); // Initialiate the mesh

	//void input();
	//void update();
	void render(Shader& shader, Camera3D& camera, glm::mat4 matrix, glm::vec3 translation, glm::quat rotation, glm::vec3 scale); // Render the mesh


	void set_matrix() {  }
	void set_translation(float x, float y, float z) { translation.x = x; translation.y = y; translation.z = z; }
	void set_rotation(float x, float y, float z) { rotation.x = x; rotation.y = y; rotation.z = z; }
	void set_scale(float x, float y, float z) { scale.x = x; scale.y = y; scale.z = z; }

	glm::mat4 get_matrix() const { return matrix; }

	glm::vec3 get_translation() const { return translation; }
	float get_translationX() const { return translation.x; }
	float get_translationY() const { return translation.y; }
	float get_translationZ() const { return translation.z; }

	glm::quat get_rotation() const { return rotation; }
	float get_rotationX() const { return rotation.x; }
	float get_rotationY() const { return rotation.y; }
	float get_rotationZ() const { return rotation.z; }

	glm::vec3 get_scale() const { return scale; }
	float get_scaleX() const { return scale.x; }
	float get_scaleY() const { return scale.y; }
	float get_scaleZ() const { return scale.z; }

private:
	//std::vector<Vertex> vertices;
	//std::vector<unsigned int> indices;
	//std::vector<Texture> textures;

	glm::mat4 matrix;
	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale = glm::vec3(0.0,0.0,0.0);

	VAO VAO; // in public so it can be used in render functuons
};*/
#endif // MESH_CLASS_H