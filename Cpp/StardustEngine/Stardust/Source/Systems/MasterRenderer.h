#pragma once
// Systems
#include "../Systems/Renderers/CubeRenderSystem.h"
#include "../Systems/Renderers/QuadRenderSystem.h"
#include "../Systems/Renderers/PyramidRenderSystem.h"

// others
#include "../Camera.h"

class MasterRenderer
{
public:
	MasterRenderer();
	~MasterRenderer();

	void render(Camera * camera);
	void prepare();
	void reset();

private :

	std::shared_ptr<RenderSystem> _cubeRenderSystem;
	std::shared_ptr<RenderSystem> _quadRenderSystem;
	std::shared_ptr<RenderSystem> _pyramidRenderSystem;

	std::shared_ptr<InstanceRenderer> _instanceRenderer;
};

