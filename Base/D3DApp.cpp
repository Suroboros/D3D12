#include "D3DApp.h"

#include <cassert>
#include <dxgi1_6.h>
#include <wrl.h>

#include "D3DException.h"
#include "WindowsApp.h"

using Microsoft::WRL::ComPtr;

void D3DApp::Initialize() {
  // Enable D3D12 debug
#if defined(DEBUG) || defined(_DEBUG)
  {
    ComPtr<ID3D12Debug> debugController;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())));
    debugController->EnableDebugLayer();
    d3dDebugFlag |= DXGI_CREATE_FACTORY_DEBUG;
  }
#endif

  ThrowIfFailed(CreateDXGIFactory2(d3dDebugFlag, IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

  // Create device
  HRESULT deviceResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0,
                                           IID_PPV_ARGS(d3dDevice.GetAddressOf()));
  if (FAILED(deviceResult)) {
    ComPtr<IDXGIAdapter4> warpAdapter{};
    ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(warpAdapter.GetAddressOf())));

    ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                    IID_PPV_ARGS(d3dDevice.GetAddressOf())));
  }

  // Create fence for CPU/GPU synchronization
  ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));

  // Query descriptor sizes
  rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  dsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
  cbvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

  // Check MSAA quality support
  D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS multiSampleQualityLevels{};
  multiSampleQualityLevels.Format = backBufferFormat;
  multiSampleQualityLevels.SampleCount = 4; // 4X MASS
  multiSampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
  multiSampleQualityLevels.NumQualityLevels = 0;
  ThrowIfFailed(d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &multiSampleQualityLevels, sizeof(multiSampleQualityLevels)));
  msaaQuality = multiSampleQualityLevels.NumQualityLevels;
  assert(msaaQuality > 0 && "Unexpected MSAA quality level.");

  CreateCommandObjects();
  CreateSwapChain();
}

void D3DApp::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(d3dDevice->CreateCommandQueue1(&commandQueueDesc, __uuidof(**(&d3dDevice)), IID_PPV_ARGS(commandQueue.GetAddressOf())));
    ThrowIfFailed(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf())));
    ThrowIfFailed(d3dDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(graphicCommandList.GetAddressOf()))); // Same as CreateCommandList and close command list righ away.
}

void D3DApp::CreateSwapChain()
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = backBufferFormat;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    swapChainDesc.SampleDesc.Count = use4xMsaa ? 4 : 1;
    swapChainDesc.SampleDesc.Quality = use4xMsaa ? msaaQuality - 1 : 0;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc{};
    swapChainFullScreenDesc.RefreshRate.Numerator = 60;
    swapChainFullScreenDesc.RefreshRate.Denominator = 1;
    swapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
    swapChainFullScreenDesc.Windowed = true;

    ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        WindowsApp::GetInstance().GetWindowHandle(),
        &swapChainDesc,
        &swapChainFullScreenDesc,
        nullptr,
        reinterpret_cast<IDXGISwapChain1**>(swapChain.ReleaseAndGetAddressOf())
    ));
}

void D3DApp::CreateRTVAndDSVDescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
    rtvDescriptorHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
    rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvDescriptorHeapDesc.NodeMask = 0;
    ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(rtvDescriptorHeap.GetAddressOf())));

    D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc{};
    dsvDescriptorHeapDesc.NumDescriptors = 1;
    dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvDescriptorHeapDesc.NodeMask = 0;
    ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(dsvDescriptorHeap.GetAddressOf())));
}

Microsoft::WRL::ComPtr<ID3D12Resource> D3DApp::CurrentBackBuffer() const
{
    return Microsoft::WRL::ComPtr<ID3D12Resource>();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::CurrentBackBufferView() const
{
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::DepthStencilView() const
{
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}
