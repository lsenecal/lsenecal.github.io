#include "MasterManager.h"



MasterManager::MasterManager()
{
	registerAllExistingComponents();

	_camera = Camera(glm::vec3(0.0, 0.0, 10.0));

	_inputOutput = std::make_unique<IoManager>();
	_masterRenderer = std::make_unique<MasterRenderer>();
	_masterPhysics = std::make_unique<MasterPhysics>();
	_masterAudio = std::make_unique<AudioMaster>();

	_cameraSystem = g_coordinator.registerSystem<CameraSystem>();
	Signature cameraSystemSignature;
	cameraSystemSignature.set(g_coordinator.getComponentType<Transform>());
	cameraSystemSignature.set(g_coordinator.getComponentType<CameraComponent>());
	cameraSystemSignature.set(g_coordinator.getComponentType<ColliderHierarchy>());
	g_coordinator.setSystemSignature<CameraSystem>(cameraSystemSignature);

	_guiSystem = g_coordinator.registerSystem<GuiSystem>();

	_window_created = false; _closed = false;
	_deltaTime = 0.0f; _lastFrame = 0.0f;

	
	// CREATE GUI SYSTEM
	// -----------------
	Signature signature;
	signature.set(g_coordinator.getComponentType<Renderable>());
	signature.set(g_coordinator.getComponentType<Transform>());
	//signature.set(g_coordinator.getComponentType<Mesh>());
	
	// set gui's signature
	g_coordinator.setSystemSignature<GuiSystem>(signature);
	// -----------------

	{
		Entity camera_entity = g_coordinator.createEntity();
		Transform transform;
		transform.position = _camera.Position;
		g_coordinator.addComponent(camera_entity, transform);

		CameraComponent cp;
		cp.camera = &_camera;
		g_coordinator.addComponent(camera_entity, cp);

		RigidBody rigidbody;
		rigidbody.mass = 1.0;
		rigidbody.useGravity = false;
		g_coordinator.addComponent(camera_entity, rigidbody);

		ColliderHierarchy CH;
		Collider test;
		test.name = sphere;
		test.type = Kinematic;
		std::shared_ptr<SphereCollider> SC(new SphereCollider);
		SC->radius = 0.2f;
		test.SphereCollider = SC;
		CH.broadCollider = test;
		CH.colliders.push_back(test);
		g_coordinator.addComponent(camera_entity, CH);
	}
}


void MasterManager::render()
{

	_cameraSystem->update();
	_masterPhysics->update();
	_cameraSystem->update2();

	_masterRenderer->prepare();

	float currentFrame = (float)glfwGetTime();
	_deltaTime = currentFrame - _lastFrame;
	_lastFrame = currentFrame;

	_masterRenderer->render(&_camera);
	_guiSystem->update();

	_masterAudio->Update(_camera.Position.x, _camera.Position.y, _camera.Position.z, _camera.Front.x, _camera.Front.y, _camera.Front.z, _camera.Up.x, _camera.Up.y, _camera.Up.z);


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
		
		_masterAudio->SetForestAmbience();


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
	g_coordinator.registerComponent<RigidBody>();
	//g_coordinator.registerComponent<Collider>();
	g_coordinator.registerComponent<ColliderHierarchy>();
	g_coordinator.registerComponent<CubeMesh>();
	g_coordinator.registerComponent<QuadMesh>();
	g_coordinator.registerComponent<PyramidMesh>();

	g_coordinator.registerComponent<Terrain>();
	g_coordinator.registerComponent<Patches>();
	g_coordinator.registerComponent<Model3D>();

	g_coordinator.registerComponent<SphereMesh>();

	g_coordinator.registerComponent<CameraComponent>();
}