#ifndef _D3D_APP_H_
#define _D3D_APP_H_

#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>

#include "../Util/Singleton.h"

// Link necessary d3d12 libraries.
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 611; }

extern "C" { __declspec(dllexport) extern const char8_t* D3D12SDKPath = u8".\\D3D12\\"; }

class D3DApp : public Singleton<D3DApp> {
 private:
  Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
  Microsoft::WRL::ComPtr<ID3D12Device10> d3dDevice;
  Microsoft::WRL::ComPtr<ID3D12Fence1> fence;

  Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> graphicCommandList;

  Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

  UINT d3dDebugFlag{};

  UINT rtvDescriptorSize{};
  UINT dsvDescriptorSize{};
  UINT cbvDescriptorSize{};

  bool use4xMsaa = false;
  UINT msaaQuality{};

  DXGI_FORMAT backBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM };

  int width = 1920;
  int height = 1080;

  const int SWAP_CHAIN_BUFFER_COUNT{ 2 };

  int currentBackBufferIndex{};

 public:
  void Initialize();
  void CreateCommandObjects();
  void CreateSwapChain();
  void CreateRTVAndDSVDescriptorHeap();

  Microsoft::WRL::ComPtr<ID3D12Resource> CurrentBackBuffer()const;
  D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
  D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
};

#endif  // !_D3D_APP_H_
