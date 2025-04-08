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
	void Update();


	glm::mat4 getProjectMatrix() 
	{
		float left = focus.x - width / 2.0f;
		float right = focus.x + width / 2.0f;
		float top = focus.y - height / 2.0f;
		float bottom = focus.y + height / 2.0f;

		view = glm::ortho(left, right, bottom, top, 0.01f, 100.0f);
		return view;
	}

	int width, height;
	float speed = 0.1f;
	float sensitivity = 100.0f;
	float zoom = 1.0f;
	glm::vec2 focus;

	glm::mat4 view;
	//glm::mat4 zoomMat;

};
#endif // CLASS_CAMERA2D_H