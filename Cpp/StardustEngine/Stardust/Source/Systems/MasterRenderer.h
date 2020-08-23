#pragma once
// Systems
#include "../Systems/Renderers/CubeRenderSystem.h"
#include "../Systems/Renderers/QuadRenderSystem.h"
#include "../Systems/Renderers/PyramidRenderSystem.h"
#include "../Systems/Renderers/SphereRenderSystem.h"
#include "../Systems/Renderers/MarchingCubeRS.h"
#include "../Systems/Renderers/TerrainRenderer.h"
#include "../Systems/Renderers/GrassSystem.h"
#include "../Systems/Renderers/ModelSystem.h"
#include"../Components/Terrain.h"


// others
#include "../Camera.h"
#include "../Graphics/TextureManager.h"
#include "../../Assets/RandomGenerator.h"


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
	std::shared_ptr<RenderSystem> _SphereRenderSystem;
	std::shared_ptr<TerrainRenderer> _terrainRenderSystem;
	std::shared_ptr<InstanceRenderer> _instanceRenderer;
	std::shared_ptr<GrassSystem> _grass;
	std::shared_ptr<TextureManager> _textures;


	std::shared_ptr<MarchingCubeRS> _marchingCubeRS;
	std::shared_ptr<ModelSystem> _modelSystem;

	Skybox _sky;
	bool _firstInit;
};

