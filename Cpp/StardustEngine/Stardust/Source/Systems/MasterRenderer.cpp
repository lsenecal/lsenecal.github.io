#include "MasterRenderer.h"



MasterRenderer::MasterRenderer()
{
	// create renderer 
	_instanceRenderer = std::make_unique<InstanceRenderer>(); // used by some Renderers System to render in an Instancied way
	_textures = std::make_shared<TextureManager>();


	// register renderers systems
	_cubeRenderSystem = g_coordinator.registerSystem<CubeRenderSystem>();
	_quadRenderSystem = g_coordinator.registerSystem<QuadRenderSystem>();
	_pyramidRenderSystem = g_coordinator.registerSystem<PyramidRenderSystem>();
	_marchingCubeRS = g_coordinator.registerSystem<MarchingCubeRS>();
	_terrainRenderSystem = g_coordinator.registerSystem<TerrainRenderer>();
	_grass = g_coordinator.registerSystem<GrassSystem>();
	_SphereRenderSystem = g_coordinator.registerSystem<SphereRenderSystem>();
	_modelSystem = g_coordinator.registerSystem<ModelSystem>();

	// create renderers systems signatures
	Signature cubeRendererSignature;
	cubeRendererSignature.set(g_coordinator.getComponentType<Renderable>());
	cubeRendererSignature.set(g_coordinator.getComponentType<Transform>());
	cubeRendererSignature.set(g_coordinator.getComponentType<CubeMesh>());

	Signature quadRendererSignature;
	quadRendererSignature.set(g_coordinator.getComponentType<Renderable>());
	quadRendererSignature.set(g_coordinator.getComponentType<Transform>());
	quadRendererSignature.set(g_coordinator.getComponentType<QuadMesh>());

	Signature pyramidRendererSignature;
	pyramidRendererSignature.set(g_coordinator.getComponentType<Renderable>());
	pyramidRendererSignature.set(g_coordinator.getComponentType<Transform>());
	pyramidRendererSignature.set(g_coordinator.getComponentType<PyramidMesh>());

	Signature marchingCubeRendererSignature;
	marchingCubeRendererSignature.set(g_coordinator.getComponentType<Renderable>());
	marchingCubeRendererSignature.set(g_coordinator.getComponentType<Transform>());
	marchingCubeRendererSignature.set(g_coordinator.getComponentType<Terrain>());

	Signature terrainRendererSignature;
	terrainRendererSignature.set(g_coordinator.getComponentType<Terrain>());
	
	Signature grassRendererSignature;
	grassRendererSignature.set(g_coordinator.getComponentType<Patches>());

	Signature modelSignature;
	modelSignature.set(g_coordinator.getComponentType<Renderable>());
	modelSignature.set(g_coordinator.getComponentType<Transform>());
	modelSignature.set(g_coordinator.getComponentType<Model3D>());

	// attribuate signature to system
	g_coordinator.setSystemSignature<CubeRenderSystem>(cubeRendererSignature);
	g_coordinator.setSystemSignature<QuadRenderSystem>(quadRendererSignature);
	g_coordinator.setSystemSignature<PyramidRenderSystem>(pyramidRendererSignature);
	g_coordinator.setSystemSignature<MarchingCubeRS>(marchingCubeRendererSignature);
	g_coordinator.setSystemSignature<TerrainRenderer>(terrainRendererSignature);
	g_coordinator.setSystemSignature<GrassSystem>(grassRendererSignature);
	g_coordinator.setSystemSignature<ModelSystem>(modelSignature);

	Signature sphereRendererSignature;
	sphereRendererSignature.set(g_coordinator.getComponentType<Renderable>());
	sphereRendererSignature.set(g_coordinator.getComponentType<Transform>());
	sphereRendererSignature.set(g_coordinator.getComponentType<SphereMesh>());
	g_coordinator.setSystemSignature<SphereRenderSystem>(sphereRendererSignature);

	// initialize renderers systems
	_cubeRenderSystem->init(_instanceRenderer);
	_quadRenderSystem->init(_instanceRenderer);
	_pyramidRenderSystem->init(_instanceRenderer);
	_marchingCubeRS->init();
	_terrainRenderSystem->init();
	_grass->init(_textures);

	_SphereRenderSystem->init(_instanceRenderer);
	_modelSystem->init();

	_firstInit = true;


}


MasterRenderer::~MasterRenderer()
{
}

void MasterRenderer::render(Camera * camera)
{
	if (_firstInit) {
		_sky = Skybox(new Shader("Assets/Shaders/skybox.vs", "Assets/Shaders/skybox.fs"));
		_firstInit = false;
	}
	//glEnable(GL_MULTISAMPLE); // anti aliazing
	glEnable(GL_DEPTH_TEST);
	////glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera->Position.y = 2.0+fbm(camera->Position.x / 30.0, camera->Position.z / 30.0) * 50.0;


	_sky.render(camera);
	_cubeRenderSystem->render(camera);
	_terrainRenderSystem->render(camera);
	_grass->render(camera);

	_SphereRenderSystem->render(camera);

	//_quadRenderSystem->render(camera);
	//_pyramidRenderSystem->render(camera);

	_modelSystem->render(camera);

	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	//glDisable(GL_MULTISAMPLE); // anti aliazing

}

void MasterRenderer::prepare()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MasterRenderer::reset()
{
	glfwSwapBuffers(glfwGetCurrentContext());
	glfwPollEvents();
}
