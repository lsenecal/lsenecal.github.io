#pragma once
#include "RenderSystem.h"


class CubeRenderSystem : public RenderSystem
{
public:
	// Inheranced via RenderSystem
	virtual void init(std::shared_ptr<InstanceRenderer> ir) override
	{
		initRenderSystem(ir);
		_vertices = &cube_vertices;
		_normals = &cube_normals;
		_maxVertices = _vertices->size();
	}
};

