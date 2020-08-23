#include "Source/MasterManager.h"
#include "Assets/UserAssets.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

Coordinator g_coordinator;


int main()
{
	g_coordinator.init();
	MasterManager core = MasterManager();
	UserAssets userAssets = UserAssets(&core);
	userAssets.init();
	glfwSwapInterval(0);
	while (!glfwWindowShouldClose(glfwGetCurrentContext())) 
	{
		core.processInput();
		core.render();
	}

	return 0;
}

