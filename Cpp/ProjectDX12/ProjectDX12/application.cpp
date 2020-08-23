#include "application.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

Application * Application::m_Application = nullptr;

Application::Application(HINSTANCE hInstance) : m_hInstance(hInstance)
{
	assert(m_Application == nullptr);
	m_Application = this;
}

Application::~Application()
{
	if (m_Device != nullptr)
		WaitPreviousFrame();
}

Application * Application::GetApplication()
{
	return m_Application;
}

HINSTANCE Application::ApplicationInstance() const
{
	return m_hInstance;
}

HWND Application::MainWindow() const
{
	return m_hMainWindow;
}

bool Application::InitApplication()
{
	if (!InitWindow())
		return false;

	if (!InitD3D12())
		return false;

	return true;
}

bool Application::InitWindow() {

	WNDCLASS wcMain;

	wcMain.style = CS_HREDRAW | CS_VREDRAW;
	wcMain.lpfnWndProc = MainWindowProc;
	wcMain.cbClsExtra = 0;
	wcMain.cbWndExtra = 0;
	wcMain.hInstance = m_hInstance;
	wcMain.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcMain.hCursor = LoadCursor(0, IDC_ARROW);
	wcMain.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcMain.lpszMenuName = 0;
	wcMain.lpszClassName = L"MainWindowClass";

	if (!RegisterClass(&wcMain)) {
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
		return false;
	}

	m_hMainWindow = CreateWindow(L"MainWindowClass",
		L"ProjectDX12",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_ApplicationWidth,
		m_ApplicationHeight,
		0,
		0,
		m_hInstance,
		0);

	if (!m_hMainWindow) {
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWindow, SW_SHOW);
	UpdateWindow(m_hMainWindow);

	return true;
}

bool Application::InitD3D12()
{
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		DX::ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_Factory)));

	HRESULT deviceResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));

	if (FAILED(deviceResult)) {
		ComPtr<IDXGIAdapter> warpAdapter;
		DX::ThrowIfFailed(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

		DX::ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
	}

	DX::ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
	qualityLevels.Format = m_RenderBufferFormat;
	qualityLevels.SampleCount = 4;
	qualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	qualityLevels.NumQualityLevels = 0;
	DX::ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(qualityLevels)));

	UpdateViewportScissors();

	CreateCommandObjects();
	CreateSwapChain();
	CreateRTVDescriptorHeap();
	CreateDSVDescriptorHeap();

	DX::ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_PipelineStateObjects["Objects"].Get()));	
	CreateRenderTargetView();
	CreateDepthStencilView();
	
	CreateTextures();
	CreateMaterials();
	CreateDescriptorHeaps();
	CreateRootSignature();
	CreateShaders();
	CreateInputLayout();

	CreateGeometries();

	m_LightPosition = { 0.0f, 10.0f, 0.0f, 1.0f };

	CreateRenderedObjects();
	CreateFrameBuffers();

	CreatePipelineStateObject();

	DX::ThrowIfFailed(m_CommandList->Close());

	ID3D12CommandList* cLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cLists), cLists);

	WaitPreviousFrame();

	m_Geometries.at("BASIC")->DisposeUploaders();

	return true;
}

// Helper function to easily add an object to the scene

void Application::AddMeshObject(
	const XMMATRIX & world,
	UINT IndexCB,
	const std::string & name,
	const std::string & subname,
	const std::string & material_name,
	D3D_PRIMITIVE_TOPOLOGY topology
	)
{
	auto m = std::make_unique<RenderedObject>();
	XMStoreFloat4x4(&m->World, world);
	m->index = IndexCB;
	m->meshCollection = m_Geometries.at(name).get();
	m->topology = topology;
	m->IndexCount = m_Geometries.at(name).get()->IndivMesh[subname].IndexCount;
	m->StartIndexLocation = m_Geometries.at(name).get()->IndivMesh[subname].IndexStart;
	m->BaseVertexLocation = m_Geometries.at(name).get()->IndivMesh[subname].VertexStart;
	m->mat = m_Materials.at(material_name).get();
	m_RenderedObjects.push_back(std::move(m));
}

void Application::InitView(std::string textureName, CD3DX12_CPU_DESCRIPTOR_HANDLE & handle)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	auto A = m_Textures[textureName + "Albedo"]->Resource;
	srvDesc.Format = A->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = A->GetDesc().MipLevels;
	m_Device->CreateShaderResourceView(A.Get(), &srvDesc, handle);

	handle.Offset(1, m_CbvSrvUavDescriptorSize);
	   
	auto M = m_Textures[textureName + "Metallic"]->Resource;
	srvDesc.Format = M->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = M->GetDesc().MipLevels;
	m_Device->CreateShaderResourceView(M.Get(), &srvDesc, handle);

	handle.Offset(1, m_CbvSrvUavDescriptorSize);

	auto R = m_Textures[textureName + "Roughness"]->Resource;
	srvDesc.Format = R->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = R->GetDesc().MipLevels;
	m_Device->CreateShaderResourceView(R.Get(), &srvDesc, handle);

	handle.Offset(1, m_CbvSrvUavDescriptorSize);

	auto N = m_Textures[textureName + "Normal"]->Resource;
	srvDesc.Format = N->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = N->GetDesc().MipLevels;
	m_Device->CreateShaderResourceView(N.Get(), &srvDesc, handle);

	handle.Offset(1, m_CbvSrvUavDescriptorSize);
}

void Application::UpdateViewportScissors()
{
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Height = static_cast<float>(m_ApplicationHeight);
	m_Viewport.Width = static_cast<float>(m_ApplicationWidth);
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_ScissorRect = { 0, 0, m_ApplicationWidth, m_ApplicationHeight };

	XMMATRIX proj = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, proj);
}

void Application::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	DX::ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

	DX::ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));

	DX::ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), m_PipelineStateObjects["Objects"].Get(), IID_PPV_ARGS(&m_CommandList)));

	m_CommandList->Close();
}

void Application::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = m_ApplicationWidth;
	swapChainDesc.BufferDesc.Height = m_ApplicationHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = m_RenderBufferFormat;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount; 
	swapChainDesc.OutputWindow = m_hMainWindow;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DX::ThrowIfFailed(m_Factory->CreateSwapChain(m_CommandQueue.Get(), &swapChainDesc, m_SwapChain.GetAddressOf()));
}

void Application::CreateRTVDescriptorHeap() 
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;

	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NumDescriptors = bufferCount;
	rtvHeapDesc.NodeMask = 0;

	DX::ThrowIfFailed(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_RtvDescriptorHeap.GetAddressOf())));

	m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void Application::CreateDSVDescriptorHeap() 
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.NodeMask = 0;

	DX::ThrowIfFailed(m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_DsvDescriptorHeap.GetAddressOf())));

	m_DsvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void Application::CreateRenderTargetView()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < bufferCount; i++) {
		DX::ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargetBuffer[i])));
		m_Device->CreateRenderTargetView(m_RenderTargetBuffer[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_RtvDescriptorSize);
	}
}

void Application::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC dsDesc;
	dsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsDesc.Alignment = 0;
	dsDesc.Width = m_ApplicationWidth;
	dsDesc.Height = m_ApplicationHeight;
	dsDesc.DepthOrArraySize = 1;
	dsDesc.MipLevels = 1;
	dsDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.SampleDesc.Quality = 0;
	dsDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	dsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE cv;
	cv.Format = m_DepthStencilFormat;
	cv.DepthStencil.Depth = 1.0f;
	cv.DepthStencil.Stencil = 0;

	DX::ThrowIfFailed(m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&dsDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&cv,
		IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = m_DepthStencilFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.Texture2D.MipSlice = 0;
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

void Application::CreateDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = m_Textures.size();
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	DX::ThrowIfFailed(m_Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(m_SrvDescriptorHeap.GetAddressOf())));

	m_CbvSrvUavDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	InitView("rust", handle);
	InitView("celtic", handle);
	InitView("wood", handle);
	InitView("stone", handle);
	InitView("rock", handle);
	InitView("marble", handle);

	////////////

	auto skybox = m_Textures["skyBox"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDescSkyBox = {};
	srvDescSkyBox.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDescSkyBox.Format = skybox->GetDesc().Format;
	srvDescSkyBox.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDescSkyBox.TextureCube.MipLevels = skybox->GetDesc().MipLevels;
	srvDescSkyBox.TextureCube.MostDetailedMip = 0;
	srvDescSkyBox.TextureCube.ResourceMinLODClamp = 0.0f;

	m_Device->CreateShaderResourceView(skybox.Get(), &srvDescSkyBox, handle);

	m_SkyBoxTexOffset = m_SrvCounter.access0() - 1;
}

void Application::CreateRootSignature()
{
	CD3DX12_ROOT_PARAMETER rootParameter[5];

	rootParameter[0].InitAsConstantBufferView(0);
	rootParameter[1].InitAsConstantBufferView(1);
	rootParameter[2].InitAsShaderResourceView(0, 1);

	CD3DX12_DESCRIPTOR_RANGE texTable0;
	texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
	rootParameter[3].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 17, 1, 0);
	rootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		1,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.0f,
		8
	);

	std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplers = { linearWrap, anisotropicWrap };

	CD3DX12_ROOT_SIGNATURE_DESC rootSignDesc(5, rootParameter, samplers.size(), samplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSign = nullptr;
	ComPtr<ID3DBlob> errorReport = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSign.GetAddressOf(), errorReport.GetAddressOf());

	if (errorReport != nullptr)
	{
		OutputDebugStringA((char*)errorReport->GetBufferPointer());
	}

	DX::ThrowIfFailed(hr);

	DX::ThrowIfFailed(m_Device->CreateRootSignature(
		0, 
		serializedRootSign->GetBufferPointer(), 
		serializedRootSign->GetBufferSize(), 
		IID_PPV_ARGS(m_RootSignature.GetAddressOf())
	));
}

void Application::CreateShaders()
{
#if defined(DEBUG) || defined(_DEBUG)  
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	
	{
		ComPtr<ID3DBlob> errorsVS;
		HRESULT hr = D3DCompileFromFile(L"Shaders\\shader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_1", compileFlags, 0, m_Shaders["VS_Objects"].GetAddressOf(), &errorsVS);
		if (errorsVS != nullptr)
		{
			OutputDebugStringA((char*)errorsVS->GetBufferPointer());
		}

		DX::ThrowIfFailed(hr);
	}
	
	{
		ComPtr<ID3DBlob> errorsPS;
		HRESULT hr = D3DCompileFromFile(L"Shaders\\shader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_1", compileFlags, 0, m_Shaders["PS_Objects"].GetAddressOf(), &errorsPS);
		if (errorsPS != nullptr)
		{
			OutputDebugStringA((char*)errorsPS->GetBufferPointer());
		}

		DX::ThrowIfFailed(hr);
	}

	{
		ComPtr<ID3DBlob> errorsVS;
		HRESULT hr = D3DCompileFromFile(L"Shaders\\Sky.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_1", compileFlags, 0, m_Shaders["VS_SkyBox"].GetAddressOf(), &errorsVS);
		if (errorsVS != nullptr)
		{
			OutputDebugStringA((char*)errorsVS->GetBufferPointer());
		}

		DX::ThrowIfFailed(hr);
	}
	
	{
		ComPtr<ID3DBlob> errorsPS;
		HRESULT hr = D3DCompileFromFile(L"Shaders\\Sky.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_1", compileFlags, 0, m_Shaders["PS_SkyBox"].GetAddressOf(), &errorsPS);
		if (errorsPS != nullptr)
		{
			OutputDebugStringA((char*)errorsPS->GetBufferPointer());
		}

		DX::ThrowIfFailed(hr);
	}
}

void Application::CreateInputLayout()
{
	m_InputElementDescs = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void Application::CreateGeometries()
{
	std::unique_ptr<MeshCollection> meshes = std::make_unique<MeshCollection>();

	std::vector<Vertex> vBox = {
		{ { -0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, { 0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f } }, // front-bottom-left
		{ { -0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, { 0.0f, 0.0f }, {  1.0f, 0.0f,  0.0f } }, // front-top-left
		{ {  0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, { 1.0f, 0.0f }, {  1.0f, 0.0f,  0.0f } }, // front-top-right
		{ {  0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, { 1.0f, 1.0f }, {  1.0f, 0.0f,  0.0f } }, // front-bottom-right

		{ { -0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f } }, // up-bottom-left
		{ { -0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 0.0f }, {  1.0f, 0.0f,  0.0f } }, // up-top-left
		{ {  0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, { 1.0f, 0.0f }, {  1.0f, 0.0f,  0.0f } }, // up-top-right
		{ {  0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, { 1.0f, 1.0f }, {  1.0f, 0.0f,  0.0f } }, // up-bottom-right

		{ { -0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, 1.0f }, { -1.0f, 0.0f,  0.0f } }, // down-bottom-left
		{ { -0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, 0.0f }, { -1.0f, 0.0f,  0.0f } }, // down-top-left
		{ {  0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, { 1.0f, 0.0f }, { -1.0f, 0.0f,  0.0f } }, // down-top-right
		{ {  0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, { 1.0f, 1.0f }, { -1.0f, 0.0f,  0.0f } }, // down-bottom-right

		{ { -0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f }, {  0.0f, 0.0f, -1.0f } }, // left-bottom-left
		{ { -0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, { 0.0f, 0.0f }, {  0.0f, 0.0f, -1.0f } }, // left-top-left
		{ { -0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f }, {  0.0f, 0.0f, -1.0f } }, // left-top-right
		{ { -0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, { 1.0f, 1.0f }, {  0.0f, 0.0f, -1.0f } }, // left-bottom-right

		{ {  0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f }, {  0.0f, 0.0f,  1.0f } }, // right-bottom-left
		{ {	 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, { 0.0f, 0.0f }, {  0.0f, 0.0f,  1.0f } }, // right-top-left
		{ {	 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f }, {  0.0f, 0.0f,  1.0f } }, // right-top-right
		{ {  0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, { 1.0f, 1.0f }, {  0.0f, 0.0f,  1.0f } }, // right-bottom-right

		{ {  0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, { 0.0f, 1.0f }, { -1.0f, 0.0f,  0.0f } }, // back-bottom-left
		{ {  0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, { 0.0f, 0.0f }, { -1.0f, 0.0f,  0.0f } }, // back-top-left
		{ { -0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, { 1.0f, 0.0f }, { -1.0f, 0.0f,  0.0f } }, // back-top-right
		{ { -0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, { 1.0f, 1.0f }, { -1.0f, 0.0f,  0.0f } }  // back-bottom-right
	};

	std::vector<UINT> iBox = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		20, 21, 22,
		20, 22, 23,

		// top face
		4, 5, 6,
		4, 6, 7,

		// bottom face
		8, 9, 10,
		8, 10, 11,

		// left face
		12, 13, 14,
		12, 14, 15,

		//right face
		16, 17, 18,
		16, 18, 19
	};

	std::vector<Vertex> vSquare = {
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } }, // bottom-left
		{ {	-0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }}, // top-left
		{ {	 0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } }, // top-right
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } } // bottom-right
	};

	std::vector<UINT> iSquare = {
		// front face
		0, 1, 2,
		0, 2, 3
	};

	std::vector<Vertex> vSphere;
	std::vector<UINT> iSphere;

	CreateSphere(vSphere, iSphere);
	
	UINT vertexSize = vBox.size() + vSquare.size() + vSphere.size();
	UINT indexSize = iBox.size() + iSquare.size() + iSphere.size();

	std::vector<Vertex> vertices(vertexSize);
	std::vector<UINT> indices(indexSize);

	for (size_t i = 0; i < vBox.size(); i++)
		vertices.at(i) = vBox.at(i);

	for (size_t i = 0; i < vSquare.size(); i++)
		vertices.at(vBox.size() + i) = vSquare.at(i);

	for (size_t i = 0; i < vSphere.size(); i++)
		vertices.at(vBox.size() + vSquare.size() + i) = vSphere.at(i);

	for (size_t i = 0; i < iBox.size(); i++)
		indices.at(i) = iBox.at(i);

	for (size_t i = 0; i < iSquare.size(); i++)
		indices.at(iBox.size() + i) = iSquare.at(i);

	for (size_t i = 0; i < iSphere.size(); i++)
		indices.at(iBox.size() + iSquare.size() + i) = iSphere.at(i);

	meshes->VertexBufferByteSize = (UINT)vertices.size() * sizeof(Vertex);
	meshes->IndexBufferByteSize = (UINT)indices.size() * sizeof(UINT);
	
	meshes->IndexCount = indices.size();

	DX::ThrowIfFailed(m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(meshes->VertexBufferByteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&meshes->VertexBufferUploader)
	));

	DX::ThrowIfFailed(m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(meshes->IndexBufferByteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&meshes->IndexBufferUploader)
	));

	DX::ThrowIfFailed(m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(meshes->VertexBufferByteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&meshes->VertexBufferGPU)
	));

	DX::ThrowIfFailed(m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(meshes->IndexBufferByteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&meshes->IndexBufferGPU)
	));

	meshes->VertexByteStride = sizeof(Vertex);

	MeshOffset boxOffset;
	boxOffset.IndexCount = iBox.size();
	boxOffset.IndexStart = 0;
	boxOffset.VertexStart = 0;

	MeshOffset squareOffset;
	squareOffset.IndexCount = iSquare.size();
	squareOffset.IndexStart = iBox.size();
	squareOffset.VertexStart = vBox.size();

	MeshOffset sphereOffset;
	sphereOffset.IndexCount = iSphere.size();
	sphereOffset.IndexStart = iBox.size() + iSquare.size();
	sphereOffset.VertexStart = vBox.size() + vSquare.size();

	meshes->IndivMesh.insert({ "Box", boxOffset });
	meshes->IndivMesh.insert({ "Square", squareOffset });
	meshes->IndivMesh.insert({ "Sphere", sphereOffset });

	D3D12_SUBRESOURCE_DATA subV = {};
	subV.pData = vertices.data();
	subV.RowPitch = meshes->VertexBufferByteSize;
	subV.SlicePitch = subV.RowPitch;

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(meshes->VertexBufferGPU.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources<1>(m_CommandList.Get(), meshes->VertexBufferGPU.Get(), meshes->VertexBufferUploader.Get(), 0, 0, 1, &subV);
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(meshes->VertexBufferGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	D3D12_SUBRESOURCE_DATA subI = {};
	subI.pData = indices.data();
	subI.RowPitch = meshes->IndexBufferByteSize;
	subI.SlicePitch = subI.RowPitch;

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(meshes->IndexBufferGPU.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources<1>(m_CommandList.Get(), meshes->IndexBufferGPU.Get(), meshes->IndexBufferUploader.Get(), 0, 0, 1, &subI);
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(meshes->IndexBufferGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

	m_Geometries.insert({"BASIC", std::move(meshes) });
}

void Application::CreateMaterials()
{
	auto rust = std::make_unique<Material>();

	rust->Name = "Rust";
	rust->indexCB = 0;
	rust->indexSRV = m_SrvCounter.access4();

	m_Materials.insert({ rust->Name, std::move(rust) });

	auto celtic = std::make_unique<Material>();
	celtic->Name = "Celtic";
	celtic->indexCB = 1;
	celtic->indexSRV = m_SrvCounter.access4();

	m_Materials.insert({ celtic->Name, std::move(celtic) });

	auto wood = std::make_unique<Material>();
	wood->Name = "Wood";
	wood->indexCB = 2;
	wood->indexSRV = m_SrvCounter.access4();

	m_Materials.insert({ wood->Name, std::move(wood) });

	auto stone = std::make_unique<Material>();
	stone->Name = "Stone";
	stone->indexCB = 3;
	stone->indexSRV = m_SrvCounter.access4();

	m_Materials.insert({ stone->Name, std::move(stone) });

	auto rock = std::make_unique<Material>();
	rock->Name = "Rock";
	rock->indexCB = 4;
	rock->indexSRV = m_SrvCounter.access4();

	m_Materials.insert({ rock->Name, std::move(rock) });

	auto marble = std::make_unique<Material>();
	marble->Name = "Marble";
	marble->indexCB = 5;
	marble->indexSRV = m_SrvCounter.access4();

	m_Materials.insert({ marble->Name, std::move(marble) });

	auto sky = std::make_unique<Material>();
	sky->Name = "SkyBox";
	sky->indexCB = 6;
	sky->indexSRV = m_SrvCounter.access1();

	m_Materials.insert({ sky->Name, std::move(sky) });
}

void Application::CreateTextures()
{
	loadTexture("rust", m_Device.Get(), m_CommandList.Get(), m_Textures,
		L"Textures/Rust/rustediron2_basecolor.dds", 
		L"Textures/Rust/rustediron2_metallic.dds", 
		L"Textures/Rust/rustediron2_roughness.dds", 
		L"Textures/Rust/rustediron2_normal.dds");

	//////////////////////

	loadTexture("celtic", m_Device.Get(), m_CommandList.Get(), m_Textures,
		L"Textures/Celtic/ornate-celtic-gold-albedo.dds", 
		L"Textures/Celtic/ornate-celtic-gold-metallic.dds", 
		L"Textures/Celtic/ornate-celtic-gold-roughness.dds", 
		L"Textures/Celtic/ornate-celtic-gold-normal-ogl.dds");

	////////////
	
	loadTexture("wood", m_Device.Get(), m_CommandList.Get(), m_Textures,
		L"Textures/Wood/hardwood-brown-planks-albedo.dds",
		L"Textures/Wood/hardwood-brown-planks-metallic.dds",
		L"Textures/Wood/hardwood-brown-planks-roughness.dds",
		L"Textures/Wood/hardwood-brown-planks-normal-ogl.dds");

	///////////////

	loadTexture("stone", m_Device.Get(), m_CommandList.Get(), m_Textures,
		L"Textures/Stone/cobblestone-curved-albedo.dds",
		L"Textures/Stone/cobblestone-curved-metallic.dds",
		L"Textures/Stone/cobblestone-curved-roughness.dds",
		L"Textures/Stone/cobblestone-curved-normal-ogl.dds");

	//////////////

	loadTexture("rock", m_Device.Get(), m_CommandList.Get(), m_Textures, 
		L"Textures/Rock/slate2-tiled-albedo2.dds", 
		L"Textures/Rock/slate2-tiled-metalness.dds", 
		L"Textures/Rock/slate2-tiled-rough.dds", 
		L"Textures/Rock/slate2-tiled-ogl.dds");

	//////////////

	loadTexture("marble", m_Device.Get(), m_CommandList.Get(), m_Textures,
		L"Textures/Marble/marble-albedo.dds",
		L"Textures/Marble/marble-Metallic.dds",
		L"Textures/Marble/marble-Roughness.dds",
		L"Textures/Marble/marble-Normal-ogl.dds");

	/////////////

	auto SkyBox = std::make_unique<Texture>();

	SkyBox->Name = "skyBox";
	SkyBox->FileName = L"Textures/skybox/cubemap.dds";
	DX::ThrowIfFailed(CreateDDSTextureFromFile12(
		m_Device.Get(),
		m_CommandList.Get(),
		SkyBox->FileName.c_str(),
		SkyBox->Resource,
		SkyBox->Upload
	));

	m_Textures.insert({ SkyBox->Name, std::move(SkyBox) });

}

void Application::CreateRenderedObjects()
{
	AddMeshObject(XMMatrixTranslation( 2.0f, -2.0f,  2.0f), 0, "BASIC", "Sphere", "Rust");
	AddMeshObject(XMMatrixTranslation( 2.0f, -2.0f, -2.0f), 1, "BASIC", "Sphere", "Celtic");	
	AddMeshObject(XMMatrixTranslation(-2.0f, -2.0f,  2.0f), 2, "BASIC", "Sphere", "Wood");
	AddMeshObject(XMMatrixTranslation(-2.0f, -2.0f, -2.0f), 3, "BASIC", "Sphere", "Stone");
	AddMeshObject(XMMatrixTranslation(-2.0f,  2.0f, -2.0f), 4, "BASIC", "Sphere", "Rock");
	AddMeshObject(XMMatrixTranslation( 2.0f,  2.0f, -2.0f), 5, "BASIC", "Sphere", "Marble");

	AddMeshObject(XMMatrixScaling(1000.0f, 1000.0f, 1000.0f), 6, "BASIC", "Sphere", "SkyBox");

	//AddMeshObject(XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixRotationAxis(XMVectorSet(1.0, 0.0f, 0.0f, 0.0f), 90.0f), 0, "BASIC", "Square", "Celtic");
}

void Application::CreateFrameBuffers()
{
	for (UINT i = 0; i < g_FrameBufferCount; i++)
	{
		m_FrameRingBuffers.push_back(std::make_unique<FrameRingBuffer>(m_Device.Get(), 1, (UINT)m_RenderedObjects.size(), (UINT)m_Materials.size()));
	}
}

void Application::CreatePipelineStateObject()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = m_RootSignature.Get();
	psoDesc.VS = { reinterpret_cast<UINT8*>(m_Shaders["VS_Objects"]->GetBufferPointer()), m_Shaders["VS_Objects"]->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<UINT8*>(m_Shaders["PS_Objects"]->GetBufferPointer()), m_Shaders["PS_Objects"]->GetBufferSize() };
	psoDesc.InputLayout = { m_InputElementDescs.data(), (UINT)m_InputElementDescs.size() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = m_DepthStencilFormat;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_RenderBufferFormat;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;

	DX::ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineStateObjects["Objects"])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDescSky = {};
	psoDescSky.pRootSignature = m_RootSignature.Get();
	psoDescSky.VS = { reinterpret_cast<UINT8*>(m_Shaders["VS_SkyBox"]->GetBufferPointer()), m_Shaders["VS_SkyBox"]->GetBufferSize() };
	psoDescSky.PS = { reinterpret_cast<UINT8*>(m_Shaders["PS_SkyBox"]->GetBufferPointer()), m_Shaders["PS_SkyBox"]->GetBufferSize() };
	psoDescSky.InputLayout = { m_InputElementDescs.data(), (UINT)m_InputElementDescs.size() };
	psoDescSky.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDescSky.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDescSky.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDescSky.DSVFormat = m_DepthStencilFormat;
	psoDescSky.SampleMask = UINT_MAX;
	psoDescSky.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDescSky.NumRenderTargets = 1;
	psoDescSky.RTVFormats[0] = m_RenderBufferFormat;
	psoDescSky.SampleDesc.Count = 1;
	psoDescSky.SampleDesc.Quality = 0;
	psoDescSky.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDescSky.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	DX::ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&psoDescSky, IID_PPV_ARGS(m_PipelineStateObjects["SkyBox"].GetAddressOf())));
}

void Application::FillCommandList()
{
	auto commandAllocator = m_CurrentFrameRingBuffer->commandListAllocator;

	DX::ThrowIfFailed(commandAllocator->Reset());

	DX::ThrowIfFailed(m_CommandList->Reset(commandAllocator.Get(), m_PipelineStateObjects["Objects"].Get()));

	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargetBuffer[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
	
	m_CommandList->ClearRenderTargetView(rtvHandle, Colors::LightGray, 0, nullptr);
	m_CommandList->ClearDepthStencilView(m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, true, &m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_SrvDescriptorHeap.Get() };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	D3D12_GPU_VIRTUAL_ADDRESS frameCBAddress = m_CurrentFrameRingBuffer->frameConstantBuffer->GetResource()->GetGPUVirtualAddress();
	m_CommandList->SetGraphicsRootConstantBufferView(1, frameCBAddress);

	D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = m_CurrentFrameRingBuffer->matConstantBuffer->GetResource()->GetGPUVirtualAddress();
	m_CommandList->SetGraphicsRootShaderResourceView(2, matCBAddress);

	CD3DX12_GPU_DESCRIPTOR_HANDLE skyHandle(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	skyHandle.Offset(m_SkyBoxTexOffset, m_CbvSrvUavDescriptorSize);
	m_CommandList->SetGraphicsRootDescriptorTable(3, skyHandle);

	m_CommandList->SetGraphicsRootDescriptorTable(4, m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	UINT objSize = UploadBuffer<ObjectConstants>::computeConstantBufferSize(sizeof(ObjectConstants));

	auto objectConstants = m_CurrentFrameRingBuffer->objectConstantBuffer->GetResource();

	for (UINT i = 0; i < m_RenderedObjects.size() - 1; i++)
	{	
		m_CommandList->IASetVertexBuffers(0, 1, &m_RenderedObjects[i]->meshCollection->VertexBufferView());
		m_CommandList->IASetIndexBuffer(&m_RenderedObjects[i]->meshCollection->IndexBufferView());
		m_CommandList->IASetPrimitiveTopology(m_RenderedObjects[i]->topology);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectConstants->GetGPUVirtualAddress() + m_RenderedObjects[i]->index * objSize;

		m_CommandList->SetGraphicsRootConstantBufferView(0, objCBAddress);

		m_CommandList->SetPipelineState(m_PipelineStateObjects["Objects"].Get());
		m_CommandList->DrawIndexedInstanced(m_RenderedObjects[i]->IndexCount, 1, m_RenderedObjects[i]->StartIndexLocation, m_RenderedObjects[i]->BaseVertexLocation, 0);		
	}

	{
		UINT s = m_RenderedObjects.size() - 1;

		m_CommandList->IASetVertexBuffers(0, 1, &m_RenderedObjects[s]->meshCollection->VertexBufferView());
		m_CommandList->IASetIndexBuffer(&m_RenderedObjects[s]->meshCollection->IndexBufferView());
		m_CommandList->IASetPrimitiveTopology(m_RenderedObjects[s]->topology);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectConstants->GetGPUVirtualAddress() + m_RenderedObjects[s]->index * objSize;

		m_CommandList->SetGraphicsRootConstantBufferView(0, objCBAddress);

		m_CommandList->SetPipelineState(m_PipelineStateObjects["SkyBox"].Get());
		m_CommandList->DrawIndexedInstanced(m_RenderedObjects[s]->IndexCount, 1, m_RenderedObjects[s]->StartIndexLocation, m_RenderedObjects[s]->BaseVertexLocation, 0);
	}
	
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargetBuffer[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	DX::ThrowIfFailed(m_CommandList->Close());
}

void Application::Draw()
{
	FillCommandList();
	ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	DX::ThrowIfFailed(m_SwapChain->Present(0, 0));

	m_FrameIndex = (m_FrameIndex + 1) % bufferCount;

	m_CurrentFrameRingBuffer->fence = ++m_CurrentFence;

	m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence);
}

void Application::Update()
{	
	float x = m_Radius * sinf(m_Phi) * cosf(m_Theta);
	float z = m_Radius * sinf(m_Phi) * sinf(m_Theta);
	float y = m_Radius * cosf(m_Phi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMStoreFloat4(&m_CameraPos, pos);
	XMVECTOR target = XMVectorZero(); // origin
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, view);

	m_CurrentFrameRingBufferIndex = (m_CurrentFrameRingBufferIndex + 1) % g_FrameBufferCount;
	m_CurrentFrameRingBuffer = m_FrameRingBuffers[m_CurrentFrameRingBufferIndex].get();

	if (m_CurrentFrameRingBuffer->fence != 0 && m_Fence->GetCompletedValue() < m_CurrentFrameRingBuffer->fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		DX::ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFrameRingBuffer->fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	{
		auto currentObject = m_CurrentFrameRingBuffer->objectConstantBuffer.get();

		for (auto& o : m_RenderedObjects)
		{
			if (o->changeFlag > 0)
			{
				XMMATRIX World = XMLoadFloat4x4(&o->World);

				ObjectConstants obj;
				DirectX::XMStoreFloat4x4(&obj.World, XMMatrixTranspose(World));
				obj.MaterialIndex = o->index;

				currentObject->CopyData(o->index, obj);

				o->changeFlag--;
			}
		}
	}

	{
		auto currentMaterial = m_CurrentFrameRingBuffer->matConstantBuffer.get(); 

		for (auto& m : m_Materials)
		{
			Material* mat = m.second.get();
			if (mat->changeFlag > 0)
			{
				MaterialConstants matC;

				matC.DiffuseMapIndex = mat->indexSRV;
				matC.MetallicMapIndex = mat->indexSRV + 1;
				matC.RoughnessMapIndex = mat->indexSRV + 2;
				matC.NormalMapIndex = mat->indexSRV + 3;

				currentMaterial->CopyData(mat->indexCB, matC);

				mat->changeFlag--;
			}
		}
	}

	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMStoreFloat4x4(&m_FrameConstants.ViewProj, XMMatrixTranspose(viewProj));

	m_FrameConstants.CameraPosition = { x, y, z, 1.0f };
	m_FrameConstants.LightColor = XMFLOAT4(300.0f, 300.0f, 300.0f, 300.0f);
	m_FrameConstants.LightPosition = m_LightPosition;

	auto currentFrameConstants = m_CurrentFrameRingBuffer->frameConstantBuffer.get();
	currentFrameConstants->CopyData(0, m_FrameConstants);
}

void Application::WaitPreviousFrame()
{
	m_CurrentFence++;
	DX::ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence));

	HANDLE fenceEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

	if (m_Fence->GetCompletedValue() < m_CurrentFence) {
		DX::ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFence, fenceEvent));
		WaitForSingleObject(fenceEvent, INFINITE);
		CloseHandle(fenceEvent);
	}
}

void Application::OnResize()
{
	assert(m_Device);
	assert(m_SwapChain);
	assert(m_CommandAllocator);

	WaitPreviousFrame();

	DX::ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_PipelineStateObjects["Objects"].Get()));

	for (int i = 0; i < bufferCount; i++)
		m_RenderTargetBuffer[i].Reset();
	m_DepthStencilBuffer.Reset();

	DX::ThrowIfFailed(m_SwapChain->ResizeBuffers(bufferCount, m_ApplicationWidth, m_ApplicationHeight, m_RenderBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	m_FrameIndex = 0;

	CreateRenderTargetView();
	CreateDepthStencilView();

	DX::ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	WaitPreviousFrame();

	UpdateViewportScissors();
}

void Application::OnMouseUp(WPARAM wParam, int x, int y)
{
	ReleaseCapture();
}

void Application::OnMouseDown(WPARAM wParam, int x, int y)
{
	m_LastMousePosition.x = x;
	m_LastMousePosition.y = y;

	SetCapture(m_hMainWindow);
}

void Application::OnMouseMove(WPARAM wParam, int x, int y)
{
	if (wParam == MK_LBUTTON)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(m_LastMousePosition.x - x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(m_LastMousePosition.y - y));

		m_Theta += dx;
		m_Phi += dy;

		m_Phi = clamp(m_Phi, 0.001f, XM_PI - 0.001f);
	}
	else if (wParam == MK_RBUTTON)
	{
		float dx = 0.05f * static_cast<float>(x - m_LastMousePosition.x);
		float dy = 0.05f * static_cast<float>(y - m_LastMousePosition.y);

		m_Radius += dy - dx;

		m_Radius = clamp(m_Radius, 2.5f, 100.0f);
	}

	m_LastMousePosition.x = x;
	m_LastMousePosition.y = y;
}

int Application::Run() {
	MSG msg = { 0 };

	while (msg.message != WM_QUIT) {
		if (msg.message == -1) {
			MessageBox(0, L"GetMessage FAILED", L"Error", MB_OK);
			break;
		}
		else {
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				Update();
				Draw();	
			}
		}
	}

	return (int)msg.wParam;
}

LRESULT Application::ApplicationProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_SIZE:
	{
		m_ApplicationWidth = LOWORD(lParam);
		m_ApplicationHeight = HIWORD(lParam);
		if (m_Device)
			if (wParam != SIZE_MINIMIZED)
				OnResize();
		return 0;
	}
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 500;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 280;
		return 0;
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
			WaitPreviousFrame();
			DestroyWindow(hWnd);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

float Application::AspectRatio() const
{
	return static_cast<float>(m_ApplicationWidth) / m_ApplicationHeight;
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return Application::GetApplication()->ApplicationProc(hWnd, msg, wParam, lParam);
}

std::ostream& XM_CALLCONV operator<< (std::ostream& os, DirectX::FXMVECTOR v)
{
	DirectX::XMFLOAT4 dest;
	XMStoreFloat4(&dest, v);
	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ", " << dest.w << ")";
	return os;
}

std::ostream& XM_CALLCONV operator<< (std::ostream& os, DirectX::FXMMATRIX m)
{
	for (int i = 0; i < 4; i++)
	{
		os << DirectX::XMVectorGetX(m.r[i]) << "\t";
		os << DirectX::XMVectorGetY(m.r[i]) << "\t";
		os << DirectX::XMVectorGetZ(m.r[i]) << "\t";
		os << DirectX::XMVectorGetW(m.r[i]);
	}
	return os;
}