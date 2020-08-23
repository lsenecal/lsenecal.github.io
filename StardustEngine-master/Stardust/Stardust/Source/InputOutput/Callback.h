#pragma once
#include <iostream>
#include <GLFW/glfw3.h>

class Callback
{
public:
	void setCamera(Camera * camera) { _camera = camera; }
	void setLastXY(float x, float y) { _lastX = x; _lastY = y; }

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
	{
		_camera->ProcessMouseScroll((float) yoffset);
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (_firstMouse)
		{
			_lastX = (float)xpos;
			_lastY = (float)ypos;
			_firstMouse = false;
		}

		float xoffset = (float)xpos - _lastX;
		float yoffset = _lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

		_lastX = (float)xpos;
		_lastY = (float)ypos;

		_camera->ProcessMouseMovement(xoffset, yoffset);
	}

	void glfw_error_callback(int error, const char* description)
	{
		std::cerr << "Glfw Error " << error << " : " << description << std::endl;
	}


private:
	Camera * _camera;
	float _lastX;
	float _lastY;
	bool _firstMouse = true;
};




static Callback instanceCallback ;
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	instanceCallback.framebuffer_size_callback(window, width, height);
}

static void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	instanceCallback.scroll_callback(window, xoffset, yoffset);
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	instanceCallback.mouse_callback(window, xpos, ypos);
}

static void glfw_error_callback(int error, const char* description)
{
	instanceCallback.glfw_error_callback(error, description);
}