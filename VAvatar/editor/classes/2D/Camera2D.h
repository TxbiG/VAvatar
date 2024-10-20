#ifndef CLASS_CAMERA2D_H
#define CLASS_CAMERA2D_H

#define GLM_ENABLE_EXPERIMENTAL

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "../../core/maths/Vector2D.h"
#include "../3D/shaderClass.h"

class Camera2D
{
public:
	Camera2D(int width, int height, glm::vec3 position);
	// Updates the camera matrix to the Vertex Shader
	void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
	// Exports the camera matrix to a shader
	void Matrix(Shader& shader, const char* uniform);
	void input(GLFWwindow* window); // Handles camera inputs

	double width;
	double height;
	float speed = 5.0f;
	float zoom = -1.0f;

	glm::vec3 cam_position = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 target_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Up = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);
};
#endif // CLASS_CAMERA2D_H