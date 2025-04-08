#include "Camera2D.h"
#include <iostream>

Camera2D::Camera2D(int width, int height, glm::vec3 position)
{
	Camera2D::width = width;
	Camera2D::height = height;
	Camera2D::focus = position;
}

void Camera2D::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	
}

void Camera2D::Matrix(Shader& shader, const char* uniform)
{
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

		//cam_position.x = (float)mouseX;
		//cam_position.y = (float)mouseY;

		//glfwSetCursorPos(window, (width / 2), (height / 2));
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
	{
		glfwSetScrollCallback(window, ScrollCallBack);
	}
}
