#include "MasterRenderer.h"



MasterRenderer::MasterRenderer()
{
	// create renderer
	_instanceRenderer = std::make_unique<InstanceRenderer>();


	// register renderers systems
	_cubeRenderSystem = g_coordinator.registerSystem<CubeRenderSystem>();
	_quadRenderSystem = g_coordinator.registerSystem<QuadRenderSystem>();
	_pyramidRenderSystem = g_coordinator.registerSystem<PyramidRenderSystem>();

	// set renderers systems signatures
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
	
	g_coordinator.setSystemSignature<CubeRenderSystem>(cubeRendererSignature);
	g_coordinator.setSystemSignature<QuadRenderSystem>(quadRendererSignature);
	g_coordinator.setSystemSignature<PyramidRenderSystem>(pyramidRendererSignature);


	// initialize renderers systems
	_cubeRenderSystem->init(_instanceRenderer);
	_quadRenderSystem->init(_instanceRenderer);
	_pyramidRenderSystem->init(_instanceRenderer);

}


MasterRenderer::~MasterRenderer()
{
}

void MasterRenderer::render(Camera * camera)
{
	_cubeRenderSystem->render(camera);
	_quadRenderSystem->render(camera);
	_pyramidRenderSystem->render(camera);
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
