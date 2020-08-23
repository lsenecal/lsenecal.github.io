#pragma once
#include "RenderSystem.h"


class PyramidRenderSystem : public RenderSystem
{
public:
	// Inheranced via RenderSystem
	virtual void init(std::shared_ptr<InstanceRenderer> ir) override
	{
		initRenderSystem(ir);
		_vertices = &pyramid_vertices;
		_normals = &pyramid_vertices;
		_maxVertices = _vertices->size();
	}
};

