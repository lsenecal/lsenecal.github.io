#include "application.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

Application * Application::m_App = nullptr;

Application::Application(HINSTANCE hInstance) : m_hInst(hInstance)
{
	assert(m_App == nullptr);
	m_App = this;
}

Application::~Application()
{
	if (m_Device != nullptr)
		WaitPreviousFrame();
}

Application * Application::GetApplication()
{
	return m_App;
}

HINSTANCE Application::ApplicationInstance() const
{
	return m_hInst;
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
	wcMain.hInstance = m_hInst;
	wcMain.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcMain.hCursor = LoadCursor(0, IDC_ARROW);
	wcMain.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcMain.lpszMenuName = 0;
	wcMain.lpszClassName = L"MainWindowClass";

	if (!RegisterClass(&wcMain)) {
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
		return false;
	}

	WNDCLASS wcAdd;

	wcAdd.style = CS_HREDRAW | CS_VREDRAW;
	wcAdd.lpfnWndProc = AddWindowProc;
	wcAdd.cbClsExtra = 0;
	wcAdd.cbWndExtra = 0;
	wcAdd.hInstance = m_hInst;
	wcAdd.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcAdd.hCursor = LoadCursor(0, IDC_ARROW);
	wcAdd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcAdd.lpszMenuName = 0;
	wcAdd.lpszClassName = L"AddWindowClass";

	if (!RegisterClass(&wcAdd)) {
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
		m_hInst,
		0);

	if (!m_hMainWindow) {
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	/*{
		HMENU hMenu = CreateMenu(); // All menus
		HMENU hMenuFile = CreateMenu();
		HMENU hMenuNew = CreateMenu();
		HMENU hMenuSelect = CreateMenu();

		// File
		AppendMenuW(hMenuFile, MF_STRING, IDM_FILE_NEW, L"&New");
		AppendMenuW(hMenuFile, MF_STRING, IDM_FILE_OPEN, L"&Open");
		AppendMenuW(hMenuFile, MF_SEPARATOR, 0, NULL);
		AppendMenuW(hMenuFile, MF_STRING, IDM_FILE_QUIT, L"&Quit");

		// New Objects
		AppendMenuW(hMenuNew, MF_STRING, IDM_ADD_BOX, L"&Box");
		AppendMenuW(hMenuNew, MF_STRING, IDM_ADD_SPHERE, L"&Sphere");
		AppendMenuW(hMenuNew, MF_STRING, IDM_ADD_PLANE, L"&Plane");

		// Select
		AppendMenuW(hMenuSelect, MF_STRING, IDM_SELECT_SELECT, L"&Select");

		AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hMenuFile, L"&File");
		AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hMenuNew, L"&New");
		AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hMenuSelect, L"&Select");
		SetMenu(m_hMainWindow, hMenu);
	}*/

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
	qualityLevels.Format = m_buffersFormat;
	qualityLevels.SampleCount = 4;
	qualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	qualityLevels.NumQualityLevels = 0;
	DX::ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(qualityLevels)));

	m_4xMsaaQuality = qualityLevels.NumQualityLevels;

	//DX::ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf())));

	UpdateViewportScissors();

	CreateCommandObjects();
	CreateSwapChain();
	CreateRTVDescriptorHeap();
	CreateDSVDescriptorHeap();
	CreateCBVDescriptorHeap();

	DX::ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_PipelineStateObject.Get()));	
	CreateRenderTargetView();
	CreateDepthStencilView();
	DX::ThrowIfFailed(m_commandList->Close());

	ID3D12CommandList* cLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cLists), cLists);

	CreateConstantBuffer();

	CreateRootSignature();
	CreateShaders();
	CreateInputLayout();

	CreatePipelineStateObject();

	/*Vertex triangleVertices[] = 
	{
		{ { 0.0f,   0.25f, 0.0f}, { 1.0f, 0.0f, 0.0f, 1.0f }},
		{ { 0.25f , -0.25f, 0.0f}, { 0.0f, 1.0f, 0.0f, 1.0f }},
		{ {-0.25f , -0.25f, 0.0f}, { 0.0f, 0.0f, 1.0f, 1.0f }}
	};*/

	Vertex triangleVertices[] =
	{
		{ { -0.25f, -0.25f, 0.0f}, { 1.0f, 0.0f, 0.0f, 1.0f }}, // bottom-left
		{ {	-0.25f , 0.25f, 0.0f}, { 0.0f, 1.0f, 0.0f, 1.0f }}, // top-left
		{ {  0.25f , -0.25f, 0.0f}, { 0.0f, 0.0f, 1.0f, 1.0f }}, // bottom-right
		{ {	-0.25f , 0.25f, 0.0f}, { 0.0f, 1.0f, 0.0f, 1.0f }}, // top-left
		{ {	 0.25f , 0.25f, 0.0f}, { 1.0f, 0.0f, 0.0f, 1.0f }}, // top-right
		{ {  0.25f , -0.25f, 0.0f}, { 0.0f, 0.0f, 1.0f, 1.0f }} // bottom-right
	};

	const UINT vertexBufferSize = sizeof(triangleVertices);

	DX::ThrowIfFailed(m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_VertexBuffer)));

	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);
	DX::ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
	m_VertexBuffer->Unmap(0, nullptr);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(Vertex);
	m_VertexBufferView.SizeInBytes = vertexBufferSize;

	/*XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX WorldViewProj = world * view * proj;

	ObjectConstants objConst;
	XMStoreFloat4x4(&objConst.WorldViewProjMat, XMMatrixTranspose(WorldViewProj));
	m_ConstantBuffer->CopyData(0, objConst);*/

	WaitPreviousFrame();

	return true;
}

void Application::UpdateViewportScissors()
{
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Height = static_cast<float>(m_ApplicationHeight);
	m_Viewport.Width = static_cast<float>(m_ApplicationWidth);
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_scissorRect = { 0, 0, m_ApplicationWidth, m_ApplicationHeight };

	XMMATRIX proj = XMMatrixPerspectiveFovLH(0.25f*M_PI, AspectRatio(), 1.0f, 100.0f);
	XMStoreFloat4x4(&m_Proj, proj);
}

void Application::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	DX::ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

	DX::ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

	DX::ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_PipelineStateObject.Get(), IID_PPV_ARGS(&m_commandList)));

	m_commandList->Close();
}

void Application::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = m_ApplicationWidth;
	swapChainDesc.BufferDesc.Height = m_ApplicationHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = m_buffersFormat;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = m_4xMsaaUse ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = m_4xMsaaUse ? (m_4xMsaaQuality - 1) : 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount; 
	swapChainDesc.OutputWindow = m_hMainWindow;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DX::ThrowIfFailed(m_Factory->CreateSwapChain(m_commandQueue.Get(), &swapChainDesc, m_SwapChain.GetAddressOf()));
}

void Application::CreateRTVDescriptorHeap() 
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;

	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NumDescriptors = bufferCount;
	rtvHeapDesc.NodeMask = 0;

	DX::ThrowIfFailed(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvDescriptorHeap.GetAddressOf())));

	m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void Application::CreateDSVDescriptorHeap() 
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.NodeMask = 0;

	DX::ThrowIfFailed(m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvDescriptorHeap.GetAddressOf())));

	m_DsvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void Application::CreateRenderTargetView()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

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
	dsDesc.SampleDesc.Count = m_4xMsaaUse ? 4 : 1;
	dsDesc.SampleDesc.Quality = m_4xMsaaUse ? (m_4xMsaaQuality - 1) : 0;
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
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		
}

void Application::CreateCBVDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvDesc;
	cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvDesc.NumDescriptors = 1;
	cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvDesc.NodeMask = 0;
	DX::ThrowIfFailed(m_Device->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(m_cbvDescriptorHeap.GetAddressOf())));
}

void Application::CreateConstantBuffer()
{
	m_ConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(m_Device.Get(), 1, true);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbDesc;
	UINT offset = UploadBuffer<ObjectConstants>::computeConstantBufferSize(sizeof(ObjectConstants));
	D3D12_GPU_VIRTUAL_ADDRESS address = m_ConstantBuffer->GetResource()->GetGPUVirtualAddress();
	int index = 0;

	address += offset * index;

	cbDesc.BufferLocation = address;
	cbDesc.SizeInBytes = offset;

	m_Device->CreateConstantBufferView(&cbDesc, m_cbvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void Application::CreateRootSignature()
{
	CD3DX12_ROOT_PARAMETER rootParameter[1];

	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	rootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignDesc(1, rootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSign = nullptr;
	ComPtr<ID3DBlob> errorReport = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSign.GetAddressOf(), errorReport.GetAddressOf());

	if (errorReport != nullptr)
	{
		OutputDebugStringA((char*)errorReport->GetBufferPointer());
	}

	DX::ThrowIfFailed(hr);

	DX::ThrowIfFailed(m_Device->CreateRootSignature(0, serializedRootSign->GetBufferPointer(), serializedRootSign->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));

}

void Application::CreateShaders()
{
#if defined(DEBUG) || defined(_DEBUG)  
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	ComPtr<ID3DBlob> errorsVS;
	ComPtr<ID3DBlob> errorsPS;

	HRESULT hr = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "VS", "vs_5_0", compileFlags, 0, &m_VertexShaderCode, &errorsVS);
	if (errorsVS != nullptr)
	{
		OutputDebugStringA((char*)errorsVS->GetBufferPointer());
	}
	
	DX::ThrowIfFailed(hr);

	hr = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "PS", "ps_5_0", compileFlags, 0, &m_PixelShaderCode, &errorsPS);
	if (errorsPS != nullptr)
	{
		OutputDebugStringA((char*)errorsPS->GetBufferPointer());
	}

	DX::ThrowIfFailed(hr);

}

void Application::CreateInputLayout()
{
	m_InputElementDescs = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void Application::CreatePipelineStateObject()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = m_RootSignature.Get();
	psoDesc.VS = { reinterpret_cast<UINT8*>(m_VertexShaderCode->GetBufferPointer()), m_VertexShaderCode->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<UINT8*>(m_PixelShaderCode->GetBufferPointer()), m_PixelShaderCode->GetBufferSize() };
	psoDesc.InputLayout = { m_InputElementDescs.data(), (UINT)m_InputElementDescs.size() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = m_DepthStencilFormat;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_buffersFormat;
	psoDesc.SampleDesc.Count = m_4xMsaaUse ? 4 : 1;
	psoDesc.SampleDesc.Quality = m_4xMsaaUse ? (m_4xMsaaQuality - 1) : 0;

	DX::ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineStateObject)));
}

void Application::FillCommandList()
{
	DX::ThrowIfFailed(m_commandAllocator->Reset());

	DX::ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_PipelineStateObject.Get()));

	m_commandList->SetGraphicsRootSignature(m_RootSignature.Get());
	m_commandList->RSSetViewports(1, &m_Viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargetBuffer[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
	m_commandList->OMSetRenderTargets(1, &rtvHandle, true, &m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_commandList->ClearDepthStencilView(m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvDescriptorHeap.Get() };
	m_commandList->SetDescriptorHeaps(1, descriptorHeaps);

	m_commandList->SetGraphicsRootDescriptorTable(0, m_cbvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_commandList->DrawInstanced(6, 1, 0, 0);

	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargetBuffer[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	DX::ThrowIfFailed(m_commandList->Close());
}

void Application::Draw(const Timer& m_Timer)
{
	FillCommandList();
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	DX::ThrowIfFailed(m_SwapChain->Present(1, 0));

	m_FrameIndex = (m_FrameIndex + 1) % bufferCount;

	WaitPreviousFrame();
}

void Application::Update(const Timer& m_Timer)
{
	XMVECTOR pos = XMLoadFloat4(&m_cameraPos);
	XMVECTOR target = XMVectorZero(); // origin
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, view);

	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX WorldViewProj = world * view * proj;

	ObjectConstants objConst;
	XMStoreFloat4x4(&objConst.WorldViewProjMat, XMMatrixTranspose(WorldViewProj));
	m_ConstantBuffer->CopyData(0, objConst);
}

void Application::WaitPreviousFrame()
{
	m_currentFence++;
	DX::ThrowIfFailed(m_commandQueue->Signal(m_Fence.Get(), m_currentFence));

	HANDLE fenceEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

	if (m_Fence->GetCompletedValue() < m_currentFence) {
		DX::ThrowIfFailed(m_Fence->SetEventOnCompletion(m_currentFence, fenceEvent));
		WaitForSingleObject(fenceEvent, INFINITE);
		CloseHandle(fenceEvent);
	}
}

void Application::OnResize()
{
	assert(m_Device);
	assert(m_SwapChain);
	assert(m_commandAllocator);

	WaitPreviousFrame();

	DX::ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_PipelineStateObject.Get()));

	for (int i = 0; i < bufferCount; i++)
		m_RenderTargetBuffer[i].Reset();
	m_DepthStencilBuffer.Reset();

	DX::ThrowIfFailed(m_SwapChain->ResizeBuffers(bufferCount, m_ApplicationWidth, m_ApplicationHeight, m_buffersFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	m_FrameIndex = 0;

	CreateRenderTargetView();
	CreateDepthStencilView();

	DX::ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	WaitPreviousFrame();

	UpdateViewportScissors();
}

void Application::OnMouseUp(WPARAM, int, int)
{
	
}

void Application::OnMouseDown(WPARAM, int, int)
{
	//OutputDebugString(std::to_wstring(1).c_str()); // Debug
}

void Application::OnMouseMove(WPARAM, int, int)
{

}

int Application::Run() {
	MSG msg = { 0 };

	m_Timer.Reset();

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
				Update(m_Timer);
				Draw(m_Timer);	
			}
		}
	}

	return (int)msg.wParam;
}

LRESULT Application::ApplicationProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;

	switch (msg) {
	case WM_CREATE:
	{
		UINT w = GetSystemMetrics(SM_CXSCREEN);
		UINT h = GetSystemMetrics(SM_CYSCREEN);
		SR = screenRatio(w, h);
		return 0;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDM_FILE_NEW:
			//MessageBox(hWnd, L"Create a new file? You will loose any unsaved progress to the current scene.", L"New?", MB_YESNO);
			MessageBeep(MB_ICONINFORMATION); 
			MessageBox(hWnd, L"NYI", 0, MB_OK);
			return 0;
		case IDM_FILE_OPEN:
			MessageBeep(MB_ICONINFORMATION);
			MessageBox(hWnd, L"NYI", 0, MB_OK);
			return 0;
		case IDM_FILE_QUIT:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			return 0;
		case IDM_ADD_BOX:
			GetWindowRect(hWnd, &rcClient);

			//OutputDebugString(std::to_wstring(rcClient.left).c_str());

			m_hAddWindow = CreateWindow(L"AddWindowClass", L"Box Configuration",
				WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
				(int)(rcClient.top), (int)(rcClient.left), (int)(rcClient.right / 4.0), (int)(rcClient.right / 4.0),
				hWnd,
				0,
				m_hInst,
				0);

			if (!m_hAddWindow) {
				MessageBox(0, L"CreateWindow FAILED", 0, 0);
				return false;
			}
			return 0;
		}
		return 0;
	case WM_SIZE:
	{
		/*RECT r1, r2;
		GetWindowRect(m_hMainWindow, &r1);
		GetClientRect(m_hMainWindow, &r2);
		UINT taskBarHeight = r1.bottom - r1.top - r2.bottom;
		UINT border = (r1.right - r1.left) - r2.right;

		m_ApplicationWidth = LOWORD(lParam) + border;
		m_ApplicationHeight = HIWORD(lParam) + taskBarHeight;
		OutputDebugString((std::to_wstring(AspectRatio()) + L"\n").c_str());*/
		m_ApplicationWidth = LOWORD(lParam);
		m_ApplicationHeight = HIWORD(lParam);
		if (m_Device)
			if (wParam != SIZE_MINIMIZED)
				OnResize();
		return 0;
	}
	/*case WM_SIZING:
	{
		RECT* rect = reinterpret_cast<RECT*>(lParam);

		RECT r1, r2;

		GetWindowRect(m_hMainWindow, &r1);
		GetClientRect(m_hMainWindow, &r2); // true drawing zone

		UINT taskBarHeight = r1.bottom - r1.top - r2.bottom;
		UINT border = (r1.right - r1.left) - r2.right;

		if (wParam == WMSZ_RIGHT || wParam == WMSZ_BOTTOMRIGHT || wParam == WMSZ_LEFT || wParam == WMSZ_BOTTOMLEFT)
		{
			UINT w = rect->right - rect->left;
			UINT h = static_cast<UINT>(w * static_cast<float>(SR.GetH()) / SR.GetW());
			rect->bottom = h + rect->top;
			return 0;
		}
		else if (wParam == WMSZ_TOPRIGHT || wParam == WMSZ_TOPLEFT)
		{
			UINT w = rect->right - rect->left;
			UINT h = static_cast<UINT>(w * static_cast<float>(SR.GetH()) / SR.GetW());
			rect->top = rect->bottom - h;
			return 0;
		}
		else if ( wParam == WMSZ_BOTTOM || wParam == WMSZ_TOP)
		{
			UINT h = rect->bottom - rect->top;
			UINT w = static_cast<UINT>(h * static_cast<float>(SR.GetW()) / SR.GetH());
			rect->right = rect->left + w;
			return 0;
		}
		
		return 0;
	}*/
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 500;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = static_cast<UINT>(500.0f * SR.GetH() / SR.GetW());
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
		if (MessageBox(0, L"Are you sure you want to quit?", L"Confirm", MB_YESNO) == IDYES) {
			//WaitPreviousFrame();
			DestroyWindow(hWnd);
		}
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Application::AddProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int length;
	RECT windowRect;
	std::wstring wstName = L"Name";
	std::vector<wchar_t> vName(wstName.begin(), wstName.end());
	std::wstring wstSize = L"Size";
	std::vector<wchar_t> vSize(wstSize.begin(), wstSize.end());
	std::wstring wstPos = L"Position";
	std::vector<wchar_t> vPos(wstPos.begin(), wstPos.end());

	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_EDITBOX_BUT:
			switch (HIWORD(wParam)) {
			case BN_CLICKED:
				length = GetWindowTextLength(m_hNameEditCtrl) + 1;
				std::vector<wchar_t> buffer1(length);
				GetWindowText(m_hNameEditCtrl, &buffer1[0], length);
				OutputDebugString(&buffer1[0]);
				OutputDebugString(L"\n");

				length = GetWindowTextLength(m_hSizeEditCtrl) + 1;
				std::vector<wchar_t> buffer2(length);
				GetWindowText(m_hSizeEditCtrl, &buffer2[0], length);
				OutputDebugString(&buffer2[0]);
				OutputDebugString(L"\n");

				length = GetWindowTextLength(m_hPosEditCtrl1) + GetWindowTextLength(m_hPosEditCtrl2) + GetWindowTextLength(m_hPosEditCtrl3) + 3;
				std::vector<wchar_t> buffer3(length);
				GetWindowText(m_hPosEditCtrl1, &buffer3[0], GetWindowTextLength(m_hPosEditCtrl1) + 1);
				GetWindowText(m_hPosEditCtrl2, &buffer3[GetWindowTextLength(m_hPosEditCtrl1)], GetWindowTextLength(m_hPosEditCtrl2) + 1);
				GetWindowText(m_hPosEditCtrl3, &buffer3[GetWindowTextLength(m_hPosEditCtrl2) + 1], GetWindowTextLength(m_hPosEditCtrl3) + 1);
				OutputDebugString(&buffer3[0]);
				OutputDebugString(L"\n");

				SendMessage(hWnd, WM_CLOSE, wParam, lParam);
				break;
			}
			return 0;
		}
		return 0;
	case WM_PAINT:
	{
		RECT rToPaint;
		if (!GetUpdateRect(hWnd, &rToPaint, FALSE))
			return 0;
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &windowRect);
		windowRect.left = windowRect.right / 10;
		windowRect.top = windowRect.bottom / 9;
		DrawText(ps.hdc, &vName[0], vName.size(), &windowRect, 0);

		windowRect.left = windowRect.right / 10;
		windowRect.top = 3 * windowRect.bottom / 9;
		DrawText(ps.hdc, &vSize[0], vSize.size(), &windowRect, 0);

		windowRect.left = windowRect.right / 10;
		windowRect.top = 5 * windowRect.bottom / 9;
		DrawText(ps.hdc, &vPos[0], vPos.size(), &windowRect, 0);

		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_CREATE:
		GetClientRect(hWnd, &windowRect);
		m_hNameEditCtrl = CreateWindow(L"EDIT", 0, 
			WS_VISIBLE | WS_CHILD | WS_BORDER, 
			windowRect.right / 2, windowRect.bottom / 9, windowRect.right / 4, windowRect.bottom / 9, 
			hWnd, (HMENU)IDC_EDITBOX_TEXT, m_hInst, 0);

		if (!m_hNameEditCtrl) {
			MessageBox(0, L"CreateWindow FAILED", 0, 0);
			return false;
		}

		m_hSizeEditCtrl = CreateWindow(L"EDIT", 0, 
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 
			windowRect.right / 2, 3 * windowRect.bottom / 9, windowRect.right / 4, windowRect.bottom / 9, 
			hWnd, 0, m_hInst, 0);

		if (!m_hSizeEditCtrl) {
			MessageBox(0, L"CreateWindow FAILED", 0, 0);
			return false;
		}

		m_hPosEditCtrl1 = CreateWindow(L"EDIT", 0, 
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 
			windowRect.right / 2, 5 * windowRect.bottom / 9, windowRect.right / 10, windowRect.bottom / 9, 
			hWnd, 0, m_hInst, 0);

		if (!m_hPosEditCtrl1) {
			MessageBox(0, L"CreateWindow FAILED", 0, 0);
			return false;
		}

		m_hPosEditCtrl2 = CreateWindow(L"EDIT", 0,
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 
			windowRect.right / 2 + windowRect.right / 10 + windowRect.right / 20, 5 * windowRect.bottom / 9, windowRect.right / 10, windowRect.bottom / 9, 
			hWnd, 0, m_hInst, 0);

		if (!m_hPosEditCtrl2) {
			MessageBox(0, L"CreateWindow FAILED", 0, 0);
			return false;
		}

		m_hPosEditCtrl3 = CreateWindow(L"EDIT", 0, 
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 
			windowRect.right / 2 + 2 * windowRect.right / 10 + 2 * windowRect.right / 20, 5 * windowRect.bottom / 9, windowRect.right / 10, windowRect.bottom / 9, 
			hWnd, 0, m_hInst, 0);

		if (!m_hPosEditCtrl3) {
			MessageBox(0, L"CreateWindow FAILED", 0, 0);
			return false;
		}

		m_hValidButton = CreateWindow(L"BUTTON", L"CLICK", 
			WS_VISIBLE | WS_CHILD | WS_BORDER, 
			windowRect.right / 3, 7 * windowRect.bottom / 9, windowRect.right / 3, windowRect.bottom / 9, 
			hWnd, (HMENU)IDC_EDITBOX_BUT, m_hInst, 0);

		if (!m_hValidButton) {
			MessageBox(0, L"CreateWindow FAILED", 0, 0);
			return false;
		}
		return 0;
	case WM_CLOSE:
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

LRESULT CALLBACK AddWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return Application::GetApplication()->AddProc(hWnd, msg, wParam, lParam);
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