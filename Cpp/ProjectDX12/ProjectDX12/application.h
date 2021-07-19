#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "util.h"
#include "FrameRingBuffer.h"
#include "RenderedObject.h"

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Application {

public:
	Application(HINSTANCE hInstance);
	Application(const Application& rhs) = delete;
	Application& operator=(const Application& rhs) = delete;
	~Application();

	static Application* GetApplication();

	HINSTANCE ApplicationInstance()const;
	HWND MainWindow()const;

	bool InitApplication();
	bool InitWindow();
	bool InitD3D12();
	int Run();
	LRESULT ApplicationProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	float AspectRatio()const;

	void AddMeshObject(
		const DirectX::XMMATRIX & World, 
		UINT IndexCB, 
		const std::string & name, 
		const std::string & subname, 
		const std::string & material_name,
		D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		);

	void InitView(std::string textureName, CD3DX12_CPU_DESCRIPTOR_HANDLE & handle);

private:
	// Window
	HWND m_hMainWindow;
	HINSTANCE m_hInstance;
	static Application* m_Application;
	int m_ApplicationWidth = 1280;
	int m_ApplicationHeight = 800;
	
	DXGI_FORMAT m_RenderBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
	Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
	UINT64 m_CurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_ScissorRect;

	static const int bufferCount = 2; // double buffering
	UINT m_FrameIndex = 0;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap;

	UINT m_RtvDescriptorSize;
	UINT m_DsvDescriptorSize;
	UINT m_CbvSrvUavDescriptorSize;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_RenderTargetBuffer[bufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;	

	std::unique_ptr<UploadBuffer<ObjectConstants>> m_ConstantBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> m_Shaders;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElementDescs;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_PipelineStateObjects;
		
	DirectX::XMFLOAT4X4 m_View = IDENTITYMATRIX();
	DirectX::XMFLOAT4X4 m_Proj = IDENTITYMATRIX();

	// Mouse controls
	DirectX::XMFLOAT4 m_CameraPos = DirectX::XMFLOAT4(0.0f, 0.0f, -2.5f, 1.0f);
	POINT m_LastMousePosition;
	float m_Theta = DirectX::XM_PI/2.0f;
	float m_Phi = DirectX::XM_PI/4.0f;
	float m_Radius = 2.5f;

	// Frames Buffer
	std::vector<std::unique_ptr<FrameRingBuffer>> m_FrameRingBuffers;
	FrameRingBuffer* m_CurrentFrameRingBuffer = nullptr;
	UINT m_CurrentFrameRingBufferIndex = 0;

	// Objects templates
	std::unordered_map<std::string, std::unique_ptr<MeshCollection>> m_Geometries;

	// Rendered Objects
	std::vector<std::unique_ptr<RenderedObject>> m_RenderedObjects;

	FrameConstants m_FrameConstants;
	UINT m_FrameConstantsBufferOffset = 0;
	UINT m_SkyBoxTexOffset = 0;

	std::unordered_map<std::string, std::unique_ptr<Material>> m_Materials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> m_Textures;

	DirectX::XMFLOAT4 m_LightPosition;
	DirectX::XMFLOAT4 m_LightColor;

	SRVCounter m_SrvCounter;

private:
	void UpdateViewportScissors();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRTVDescriptorHeap();
	void CreateDSVDescriptorHeap();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void CreateDescriptorHeaps();
	void FillCommandList();
	void WaitPreviousFrame();
	void Draw();
	void Update();
	void OnResize();

	void OnMouseUp(WPARAM, int, int);
	void OnMouseDown(WPARAM, int, int);
	void OnMouseMove(WPARAM, int, int);

	void CreateRootSignature();
	void CreateShaders();
	void CreateInputLayout();

	void CreateGeometries();
	void CreateMaterials();
	void CreateTextures();
	void CreateRenderedObjects();
	void CreateFrameBuffers();

	void CreatePipelineStateObject();
};