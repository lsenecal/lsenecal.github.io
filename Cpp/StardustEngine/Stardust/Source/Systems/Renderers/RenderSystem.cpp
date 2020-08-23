#include "RenderSystem.h"



void RenderSystem::initRenderSystem(std::shared_ptr<InstanceRenderer> ir)
{
	_instanceRenderer = ir;
	_firstInitialisation = true;
}


void RenderSystem::render(Camera * camera)
{
	if (_firstInitialisation) {

		_shader = new Shader("Assets/Shaders/Cube.vs", "Assets/Shaders/Cube.fs");
		_instanceRenderer->createVAOVBO(&_vao, &_vbos, &_entities, _vertices, _normals, &_numberOfElements, _shader);

		_firstInitialisation = false;
	}
	else
		_instanceRenderer->updateVAOVBO(&_vao, &_vbos, &_entities, _vertices, _normals, &_numberOfElements);

	_instanceRenderer->render(camera, _shader, &_vao, &_numberOfElements, _maxVertices);
}