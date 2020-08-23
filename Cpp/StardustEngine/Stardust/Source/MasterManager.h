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
#include "Components/Terrain.h"
#include "Components/Patch.h"
#include "Components/Physics.h"
#include "Components/Collider.h"
#include "Components/CameraComponent.h"


// Systems
#include "Systems/GuiSystem.h"
#include "Systems/Renderers/CubeRenderSystem.h"
#include "Systems/Renderers/QuadRenderSystem.h"
#include "Systems/Renderers/PyramidRenderSystem.h"
#include "Systems/Camera/CameraSystem.h"


//Audio
#include "Audio/AudioMaster.h"

// other
#include "InputOutput/Window.h"
#include "InputOutput/IoManager.h"
#include "Systems/MasterRenderer.h"

// Physics
#include "Systems/MasterPhysics.h"

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
	std::unique_ptr<MasterPhysics> _masterPhysics;
	std::unique_ptr<AudioMaster> _masterAudio;

	std::shared_ptr<CameraSystem> _cameraSystem;

	std::shared_ptr<GuiSystem> _guiSystem;// = g_coordinator.registerSystem<GuiSystem>();
	
	Camera _camera;
	
	float _deltaTime, _lastFrame;
	bool _window_created, _closed;

	void registerAllExistingComponents();
};

