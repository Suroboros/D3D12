#include "D3DApp.h"

#include <dxgi1_6.h>
#include <wrl.h>

#include <cassert>

#include "D3DException.h"
#include "WindowsApp.h"

using Microsoft::WRL::ComPtr;

D3DApp::~D3DApp() {
  if (d3dDevice != nullptr) {
    // Wait until the GPU is done processing the commands in the queue before
    // destroying any resources the GPU is still referencing. Otherwise, the GPU
    // might crash when the application exits
    FlushCommandQueue();
  }
}

void D3DApp::Initialize() {
  // Enable D3D12 debug
#if defined(DEBUG) || defined(_DEBUG)
  {
    ComPtr<ID3D12Debug> debugController;
    ThrowIfFailed(
        D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())));
    debugController->EnableDebugLayer();
    d3dDebugFlag |= DXGI_CREATE_FACTORY_DEBUG;
  }
#endif

  CreateD3dDevice();
  CheckMsaaSupport();
  QueryDescriptorSizes();
  CreateCommandObjects();
  CreateSwapChain();
}

void D3DApp::CreateD3dDevice() {
  ThrowIfFailed(CreateDXGIFactory2(d3dDebugFlag,
                                   IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

  // Create device
  HRESULT deviceResult = D3D12CreateDevice(
      nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(d3dDevice.GetAddressOf()));
  if (FAILED(deviceResult)) {
    ComPtr<IDXGIAdapter4> warpAdapter{};
    ThrowIfFailed(
        dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(warpAdapter.GetAddressOf())));

    ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                    IID_PPV_ARGS(d3dDevice.GetAddressOf())));
  }

  // Create fence for CPU/GPU synchronization
  ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
                                       IID_PPV_ARGS(fence.GetAddressOf())));
}

void D3DApp::CheckMsaaSupport() {
  assert(d3dDevice);

  D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS multiSampleQualityLevels{};
  multiSampleQualityLevels.Format = backBufferFormat;
  multiSampleQualityLevels.SampleCount = 4;  // 4X MASS
  multiSampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
  multiSampleQualityLevels.NumQualityLevels = 0;
  ThrowIfFailed(d3dDevice->CheckFeatureSupport(
      D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &multiSampleQualityLevels,
      sizeof(multiSampleQualityLevels)));
  msaaQuality = multiSampleQualityLevels.NumQualityLevels;
  assert(msaaQuality > 0 && "Unexpected MSAA quality level.");
}

void D3DApp::QueryDescriptorSizes() {
  assert(d3dDevice);

  rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  dsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
  cbvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void D3DApp::CreateCommandObjects() {
  assert(d3dDevice);

  D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
  commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  ThrowIfFailed(d3dDevice->CreateCommandQueue1(
      &commandQueueDesc, __uuidof(**(&d3dDevice)),
      IID_PPV_ARGS(commandQueue.GetAddressOf())));
  ThrowIfFailed(d3dDevice->CreateCommandAllocator(
      D3D12_COMMAND_LIST_TYPE_DIRECT,
      IID_PPV_ARGS(commandAllocator.GetAddressOf())));
  ThrowIfFailed(d3dDevice->CreateCommandList1(
      0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE,
      IID_PPV_ARGS(graphicCommandList
                       .GetAddressOf())));  // Same as CreateCommandList and
                                            // close command list righ away.
}

void D3DApp::CreateSwapChain() {
  assert(dxgiFactory);
  assert(commandQueue);

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
  swapChainFullScreenDesc.ScanlineOrdering =
      DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
  swapChainFullScreenDesc.Windowed = true;

  ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
      commandQueue.Get(), WindowsApp::GetInstance().GetWindowHandle(),
      &swapChainDesc, &swapChainFullScreenDesc, nullptr,
      reinterpret_cast<IDXGISwapChain1**>(swapChain.ReleaseAndGetAddressOf())));
}

void D3DApp::ActivateApp(bool active) { pause = !active; }

void D3DApp::CreateRTVAndDSVDescriptorHeap() {
  assert(d3dDevice);

  D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
  rtvDescriptorHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
  rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  rtvDescriptorHeapDesc.NodeMask = 0;
  ThrowIfFailed(d3dDevice->CreateDescriptorHeap(
      &rtvDescriptorHeapDesc, IID_PPV_ARGS(rtvDescriptorHeap.GetAddressOf())));

  D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc{};
  dsvDescriptorHeapDesc.NumDescriptors = 1;
  dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
  dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  dsvDescriptorHeapDesc.NodeMask = 0;
  ThrowIfFailed(d3dDevice->CreateDescriptorHeap(
      &dsvDescriptorHeapDesc, IID_PPV_ARGS(dsvDescriptorHeap.GetAddressOf())));
}

void D3DApp::CreateRTV() {
  assert(rtvDescriptorHeap);
  assert(swapChain);

  CD3DX12_CPU_DESCRIPTOR_HANDLE trvDescriptorHandle(
      rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
  for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i) {
    ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainBuffers[i])));
    d3dDevice->CreateRenderTargetView(
        swapChainBuffers[i].Get(),
        //  create a view to the first mipmap level of this resource(the back
        //  buffer only has one mipmap level) with the format the resource was
        //  created with
        nullptr, trvDescriptorHandle);
    trvDescriptorHandle.Offset(1, rtvDescriptorSize);
  }
}

void D3DApp::CreateDSV() {
  assert(d3dDevice);
  assert(graphicCommandList);

  D3D12_RESOURCE_DESC depthStencilDesc;
  depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
  depthStencilDesc.Alignment = 0;
  depthStencilDesc.Width = width;
  depthStencilDesc.Height = height;
  depthStencilDesc.DepthOrArraySize = 1;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
  depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
  depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

  depthStencilDesc.SampleDesc.Count = use4xMsaa ? 4 : 1;
  depthStencilDesc.SampleDesc.Quality = use4xMsaa ? (msaaQuality - 1) : 1;

  D3D12_CLEAR_VALUE optClear;
  optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  optClear.DepthStencil.Depth = 1.0f;
  optClear.DepthStencil.Stencil = 0;

  auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
  ThrowIfFailed(d3dDevice->CreateCommittedResource(
      &heapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilDesc,
      D3D12_RESOURCE_STATE_COMMON, &optClear,
      IID_PPV_ARGS(depthStencilBuffer.GetAddressOf())));

  d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr,
                                    DepthStencilView());

  auto resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
      depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
      D3D12_RESOURCE_STATE_DEPTH_WRITE);
  graphicCommandList->ResourceBarrier(1, &resourceBarrier);
}

void D3DApp::CreateViewport() {
  D3D12_VIEWPORT viewportDesc;
  viewportDesc.TopLeftX = 0;
  viewportDesc.TopLeftY = 0;
  viewportDesc.Width = static_cast<float>(width);
  viewportDesc.Height = static_cast<float>(height);
  viewportDesc.MinDepth = 0.0f;
  viewportDesc.MaxDepth = 1.0f;

  graphicCommandList->RSSetViewports(1, &viewportDesc);
}

void D3DApp::CreateScissorRect() {
  D3D12_RECT scissorRectDesc{0, 0, width, height};
  graphicCommandList->RSSetScissorRects(1, &scissorRectDesc);
}

void D3DApp::FlushCommandQueue() {
  ++currentFence;
  // Since we are on GPU timeline, the new fence point won`t be set until the
  // GPU finishes procesing all te comands prior to this signal
  ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFence));

  // Wait until the GPU has completed the commands up to this fence point.
  if (fence->GetCompletedValue() < currentFence) {
    HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
    ThrowIfFailed(fence->SetEventOnCompletion(currentFence, eventHandle));
    // Wait until the GPU hits current fence event is firec.
    WaitForSingleObject(eventHandle, INFINITE);
    CloseHandle(eventHandle);
  }
}

Microsoft::WRL::ComPtr<ID3D12Resource> D3DApp::CurrentBackBuffer() const {
  return Microsoft::WRL::ComPtr<ID3D12Resource>();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::CurrentBackBufferView() const {
  return CD3DX12_CPU_DESCRIPTOR_HANDLE(
      rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
      currentBackBufferIndex, rtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::DepthStencilView() const {
  return dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}
