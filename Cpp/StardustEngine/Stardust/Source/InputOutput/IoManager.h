#pragma once

#include "../Camera.h"
#include <GLFW/glfw3.h>

class IoManager
{
public:
	IoManager() : previousTabPress(false) {}
	~IoManager() {}

	void processInput(GLFWwindow * window, Camera * camera, float deltaTime)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !previousTabPress)
		{
			previousTabPress = true;
			camera->freeze = !camera->freeze;
			if (!camera->freeze) camera->restoreMovement = true;
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
		{
			previousTabPress = false;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->ProcessKeyboard(FORWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->ProcessKeyboard(BACKWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->ProcessKeyboard(LEFT, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->ProcessKeyboard(RIGHT, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera->ProcessKeyboard(DOWN, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera->ProcessKeyboard(UP, deltaTime);

	}

private:
	bool previousTabPress;
};
