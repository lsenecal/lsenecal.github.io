#include "MasterManager.h"



MasterManager::MasterManager()
{
	registerAllExistingComponents();

	_camera = Camera(glm::vec3(0.0, 0.0, 10.0));
	_inputOutput = std::make_unique<IoManager>();
	_masterRenderer = std::make_unique<MasterRenderer>();

	_window_created = false; _closed = false;
	_deltaTime = 0.0f; _lastFrame = 0.0f;

	
	// CREATE GUI SYSTEM
	// -----------------
	Signature signature;
	signature.set(g_coordinator.getComponentType<Renderable>());
	signature.set(g_coordinator.getComponentType<Transform>());
	signature.set(g_coordinator.getComponentType<Mesh>());
	
	// set gui's signature
	g_coordinator.setSystemSignature<GuiSystem>(signature);
	// -----------------
}


void MasterManager::render()
{
	_masterRenderer->prepare();

	float currentFrame = (float)glfwGetTime();
	_deltaTime = currentFrame - _lastFrame;
	_lastFrame = currentFrame;

	_masterRenderer->render(&_camera);
	_guiSystem->update();

	_masterRenderer->reset();
}

void MasterManager::create_window(std::string name, int width, int height, bool fullscreen)
{
	if (!_window_created) {
		_window = std::make_unique<Window>();
		_window->initCallback(&_camera);
		_window->create_window(name, width, height, fullscreen);
		_window->setCallbackFunctions();
		_window_created = true;
		_camera.setProjection(glm::perspective(glm::radians(_camera.Zoom), (float)_window->getDimension().x / (float)_window->getDimension().y, 0.1f, 1000.0f));
		
		// init system
		_guiSystem->init();
	}
}

bool MasterManager::isClosed()
{
	return _closed;
}

void MasterManager::processInput()
{
	_inputOutput->processInput(_window->getWindow(), &_camera, _deltaTime);
}




void MasterManager::registerAllExistingComponents()
{
	// Add all components to ComponentManager :
	g_coordinator.registerComponent<Renderable>();
	g_coordinator.registerComponent<Transform>();
	g_coordinator.registerComponent<Mesh>();
	g_coordinator.registerComponent<CubeMesh>();
	g_coordinator.registerComponent<QuadMesh>();
	g_coordinator.registerComponent<PyramidMesh>();
}






