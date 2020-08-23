#pragma once
// vendor
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Core
#include "Core/Coordinator.h"

// Components
#include "Components/Renderable.h"
#include "Components/Transform.h"
#include "Components/Mesh.h"

// Systems
#include "Systems/GuiSystem.h"
#include "Systems/Renderers/CubeRenderSystem.h"
#include "Systems/Renderers/QuadRenderSystem.h"
#include "Systems/Renderers/PyramidRenderSystem.h"

// other
#include "InputOutput/Window.h"
#include "InputOutput/IoManager.h"
#include "Systems/MasterRenderer.h"


// vendor
#include <string>

extern Coordinator g_coordinator;

class MasterManager
{
public:
	MasterManager();

	void render();
	void create_window(std::string name, int width, int height, bool fullscreen);
	
	bool isClosed();
	void processInput();

private:
	std::unique_ptr<Window> _window;
	std::unique_ptr<IoManager> _inputOutput;
	std::unique_ptr<MasterRenderer> _masterRenderer;

	std::shared_ptr<GuiSystem> _guiSystem = g_coordinator.registerSystem<GuiSystem>();
	
	Camera _camera;
	
	float _deltaTime, _lastFrame;
	bool _window_created, _closed;

	void registerAllExistingComponents();
};

