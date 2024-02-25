#ifndef _D3D_APP_H_
#define _D3D_APP_H_

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "../Util/Singleton.h"

// Link necessary d3d12 libraries.
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class D3DApp : public Singleton<D3DApp> {
  friend class Singleton<D3DApp>;

 public:
  virtual ~D3DApp();

 private:
  int width = 1920;
  int height = 1080;

  static const int SWAP_CHAIN_BUFFER_COUNT{2};

  Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
  Microsoft::WRL::ComPtr<ID3D12Device10> d3dDevice;
  Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
  UINT64 currentFence = 0;

  Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> graphicCommandList;

  Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

  Microsoft::WRL::ComPtr<ID3D12Resource>
      swapChainBuffers[SWAP_CHAIN_BUFFER_COUNT];
  Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

  UINT d3dDebugFlag{};

  UINT rtvDescriptorSize{};
  UINT dsvDescriptorSize{};
  UINT cbvDescriptorSize{};

  bool use4xMsaa = false;
  UINT msaaQuality{};

  DXGI_FORMAT backBufferFormat{DXGI_FORMAT_R8G8B8A8_UNORM};

  int currentBackBufferIndex{};

  bool pause = false;

 public:
  void Initialize();
  void ActivateApp(bool active);

 private:
  void CreateD3dDevice();
  void CheckMsaaSupport();
  void QueryDescriptorSizes();
  void CreateCommandObjects();
  void CreateSwapChain();
  void CreateRTVAndDSVDescriptorHeap();
  void CreateRTV();
  void CreateDSV();
  void CreateViewport();
  void CreateScissorRect();

  void FlushCommandQueue();

  Microsoft::WRL::ComPtr<ID3D12Resource> CurrentBackBuffer() const;
  D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
  D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
};

#endif  // !_D3D_APP_H_
