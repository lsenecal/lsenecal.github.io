#pragma once
#include "../Core/System.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

class GuiSystem : public System
{
private:
	void initializeNewFrame();
	void configure();
	void configureMenuBar();

public:
	void init();
	void update();
	void end();
};

