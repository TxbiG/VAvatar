#include "Camera2D.h"
#include <iostream>

Camera2D::Camera2D(int width, int height, glm::vec3 position)
{
	Camera2D::width = width;
	Camera2D::height = height;
	cam_position = position;
}

void Camera2D::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(cam_position, target_position, Up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / (float)height, nearPlane, farPlane);

	cameraMatrix = projection * view; // Sets new camera matrix
}

void Camera2D::Matrix(Shader& shader, const char* uniform)
{
	// Exports camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	std::cout << xoffset << ": " << yoffset << ": " << std::endl;
}

void Camera2D::input(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glfwSetCursorPos(window, (width / 2), (height / 2));

		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		cam_position.x = (float)mouseX;
		cam_position.y = (float)mouseY;

		glfwSetCursorPos(window, (width / 2), (height / 2));
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
	{
		glfwSetScrollCallback(window, ScrollCallBack);
	}
}
