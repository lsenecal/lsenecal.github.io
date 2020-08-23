#pragma once
#include "../Camera.h"
#include "Callback.h"

#include <iostream>
#include <string>
#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window();
	~Window();

	// return the current window
	GLFWwindow * getWindow();

	// create a window using parameters
	void create_window(std::string name, int width, int height, bool fullscreen);

	// initalize callback's attributes
	void initCallback(Camera * camera);

	// set glfw callback functions using instanceCallback
	void setCallbackFunctions();

	// return dimension of the window vec2(width, height)
	glm::vec2 getDimension();

private :
	GLFWwindow* _window;
	glm::ivec2 _window_dimension;

};

