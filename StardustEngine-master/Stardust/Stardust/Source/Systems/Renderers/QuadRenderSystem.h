#pragma once
#include "RenderSystem.h"


class QuadRenderSystem : public RenderSystem
{
public:
	// Inheranced via RenderSystem
	virtual void init(std::shared_ptr<InstanceRenderer> ir) override
	{
		initRenderSystem(ir);
		_vertices = &quad_vertices;
		_normals = &quad_normals;
		_maxVertices = _vertices->size();
	}
};

