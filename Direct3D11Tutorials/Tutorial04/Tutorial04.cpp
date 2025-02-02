//--------------------------------------------------------------------------------------
// File: Tutorial04.cpp
//
// This application displays a 3D cube using Direct3D 11
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729721.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include <vector>

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
    XMFLOAT3 Normal;
};


struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
    XMVECTOR lightPos;
    XMVECTOR cameraPos;
};



//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = nullptr;
HWND                    g_hWnd = nullptr;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pd3dDevice = nullptr;
ID3D11Device1*          g_pd3dDevice1 = nullptr;
ID3D11DeviceContext*    g_pImmediateContext = nullptr;
ID3D11DeviceContext1*   g_pImmediateContext1 = nullptr;
IDXGISwapChain*         g_pSwapChain = nullptr;
IDXGISwapChain1*        g_pSwapChain1 = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11VertexShader*     g_pVertexShader = nullptr;
ID3D11VertexShader*     g_pVertexShader1 = nullptr;
ID3D11VertexShader*     g_pVertexShader2 = nullptr;
ID3D11PixelShader*      g_pPixelShader = nullptr;
ID3D11PixelShader*      g_pPixelShader1 = nullptr;
ID3D11PixelShader*      g_pPixelShader2 = nullptr;
ID3D11InputLayout*      g_pVertexLayout = nullptr;
ID3D11Buffer*           g_pVertexBuffer = nullptr;
ID3D11Buffer*           g_pIndexBuffer = nullptr;
ID3D11Buffer*           g_pConstantBuffer = nullptr;
ID3D11Texture2D*        g_pDepthStencil = nullptr;
ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
XMMATRIX                g_World;
XMMATRIX                g_World1;
XMMATRIX                g_World2;
XMMATRIX                g_View;
XMMATRIX                g_Projection;
XMVECTOR			    g_lightPos;   


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 1920, 1080 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"Direct3D 11 Tutorial 4: 3D Spaces",
                           WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                           nullptr );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile( szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob );
    if( FAILED(hr) )
    {
        if( pErrorBlob )
        {
            OutputDebugStringA( reinterpret_cast<const char*>( pErrorBlob->GetBufferPointer() ) );
            pErrorBlob->Release();
        }
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );

        if ( hr == E_INVALIDARG )
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                                    D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        }

        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = g_pd3dDevice->QueryInterface( __uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice) );
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent( __uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory) );
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface( __uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2) );
    if ( dxgiFactory2 )
    {
        // DirectX 11.1 or later
        hr = g_pd3dDevice->QueryInterface( __uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1) );
        if (SUCCEEDED(hr))
        {
            (void) g_pImmediateContext->QueryInterface( __uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1) );
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd( g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1 );
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface( __uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain) );
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain( g_pd3dDevice, &sd, &g_pSwapChain );
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation( g_hWnd, DXGI_MWA_NO_ALT_ENTER );

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &pBackBuffer ) );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

  
    D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil);

    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil);
    if (FAILED(hr))
        return hr;
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	//
    


    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile( L"VS2.hlsl", "VS_main", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( nullptr,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.???????", L"Error", MB_OK );
        return hr;
    }

    ID3DBlob* pVSBlob1 = nullptr;
    hr = CompileShaderFromFile(L"VS3.hlsl", "VS_main", "vs_4_0", &pVSBlob1);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

    ID3DBlob* pVSBlob2 = nullptr;
    hr = CompileShaderFromFile(L"VS4.hlsl", "VS_main", "vs_4_0", &pVSBlob2);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader );
	if( FAILED( hr ) )
	{	
		pVSBlob->Release();
        return hr;
	}

    hr = g_pd3dDevice->CreateVertexShader(pVSBlob1->GetBufferPointer(), pVSBlob1->GetBufferSize(), nullptr, &g_pVertexShader1);
    if (FAILED(hr))
    {
        pVSBlob1->Release();
        return hr;
    }

    hr = g_pd3dDevice->CreateVertexShader(pVSBlob2->GetBufferPointer(), pVSBlob2->GetBufferSize(), nullptr, &g_pVertexShader2);
    if (FAILED(hr))
    {
        pVSBlob2->Release();
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
	hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
	pVSBlob->Release();
	if( FAILED( hr ) )
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout( g_pVertexLayout );

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile( L"PS2.hlsl", "PS_main", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( nullptr,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    ID3DBlob* pPSBlob1 = nullptr;
    hr = CompileShaderFromFile(L"PS3.hlsl", "PS_main", "ps_4_0", &pPSBlob1);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

    ID3DBlob* pPSBlob2 = nullptr;
    hr = CompileShaderFromFile(L"PS4.hlsl", "PS_main", "ps_4_0", &pPSBlob2);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader );
	pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreatePixelShader(pPSBlob1->GetBufferPointer(), pPSBlob1->GetBufferSize(), nullptr, &g_pPixelShader1);
    pPSBlob1->Release();
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreatePixelShader(pPSBlob2->GetBufferPointer(), pPSBlob2->GetBufferSize(), nullptr, &g_pPixelShader2);
    pPSBlob2->Release();
    if (FAILED(hr))
        return hr;
    
   


    // Create vertex buffer
    
	SimpleVertex vertices[]
    {
        // Lower Hexagon (z = 0, y = 0)
           //{ XMFLOAT3(1.0f, 0.0f,  0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // Vertex 0
           // { XMFLOAT3(0.5f, 0.0f,  0.866f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, // Vertex 1
           // { XMFLOAT3(-0.5f, 0.0f,  0.866f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // Vertex 2
           // { XMFLOAT3(-1.0f, 0.0f,  0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) }, // Vertex 3
           // { XMFLOAT3(-0.5f, 0.0f, -0.866f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }, // Vertex 4
           // { XMFLOAT3(0.5f, 0.0f, -0.866f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, // Vertex 5
           // { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }, // Center Vertex 6

           // // Upper Hexagon (z = 0, y = 2)
           // { XMFLOAT3(1.0f, 2.0f,  0.0f), XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f) }, // Vertex 7
           // { XMFLOAT3(0.5f, 2.0f,  0.866f), XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f) }, // Vertex 8
           // { XMFLOAT3(-0.5f, 2.0f,  0.866f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f) }, // Vertex 9
           // { XMFLOAT3(-1.0f, 2.0f,  0.0f), XMFLOAT4(1.0f, 1.0f, 0.5f, 1.0f) }, // Vertex 10
           // { XMFLOAT3(-0.5f, 2.0f, -0.866f), XMFLOAT4(1.0f, 0.5f, 1.0f, 1.0f) }, // Vertex 11
           // { XMFLOAT3(0.5f, 2.0f, -0.866f), XMFLOAT4(0.5f, 1.0f, 1.0f, 1.0f) }, // Vertex 12
           // { XMFLOAT3(0.0f, 2.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }  // Center Vertex 13

        /*for (double x = 0.0; x <= 2.0; x += 0.5)
        {
			for (double z = 0.0; z <= 2.0; z += 0.5)
			{
				vertices.push_back({ XMFLOAT3(x, 0.0f, z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) });
			}

        }*/

        //add a bunch more indices
       
        //top face
            { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f)}, // Vertex 0
            { XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f)}, // Vertex 1
            { XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f)}, // Vertex 2
            { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f)}, // Vertex 3

        //front face
            { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) , XMFLOAT3(0.0f, 0.0f, -1.0f)}, //Vertex 4
            { XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) , XMFLOAT3(0.0f, 0.0f, -1.0f)},  //Vertex 5
            { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) , XMFLOAT3(0.0f, 0.0f, -1.0f)}, // Vertex 6
            { XMFLOAT3( 1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) , XMFLOAT3(0.0f, 0.0f, -1.0f)}, // Vertex 7

        //left face
            { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f)}, //Vertex 8
            { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f)}, //Vertex 9
            { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f)}, //Vertex 10
            { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f)}, // Vertex 11

        //right face
            { XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f)}, //Vertex 12
            { XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f)}, //Vertex 13
            { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f)}, // Vertex 14
            { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) , XMFLOAT3(-1.0f, 0.0f, 0.0f)}, // Vertex 15

        //back face
            { XMFLOAT3(1.0f,  1.0f,1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) , XMFLOAT3(0.0f, 0.0f, 1.0f)}, //Vertex 16
            { XMFLOAT3(-1.0f, 1.0f,1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) , XMFLOAT3(0.0f, 0.0f, 1.0f)}, //Vertex 17
            { XMFLOAT3(1.0f, -1.0f,1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) , XMFLOAT3(0.0f, 0.0f, 1.0f)}, //Vertex 18
            { XMFLOAT3(-1.0f,-1.0f,1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) , XMFLOAT3(0.0f, 0.0f, 1.0f)}, //Vertex 19

        //bottom face
            { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) , XMFLOAT3(0.0f, -1.0f, 0.0f)}, // Vertex 20
            { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) , XMFLOAT3(0.0f, -1.0f, 0.0f)}, //Vertex 21
            { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) , XMFLOAT3(0.0f, -1.0f, 0.0f)}, // Vertex 22
            { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) , XMFLOAT3(0.0f, -1.0f, 0.0f)}, // Vertex 23


    
    };
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 20;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set vertex buffer
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    // Create index buffer
    WORD indices[] =
    {
        //top face
        3,1,0,
        2,1,3,

        ////front face
        4,5,7,
        7,6,4,

        //////left face
        8,9,11,
        11,10,8,

        ////right face
        12,13,15,
        15,14,12,

        //back face
        16,17,19,
        19,18,16,

        //bottom face
        20,21,23,
        23,22,20,
    };

    //four walls
   /* WORD indices[] =
    {
        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,
      
        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6
    };*/

    //one face of the cube
   /* WORD indices[] =
    {
        3,4,7,
        0,4,3
    };*/

    //triangle indeces
   /* WORD indices[] =
    {
        1,5,0,
        2,7,6
    };*/

    //WORD indices[] =
    //{
        //0,1,
        //1,2,
        /*2,3,
        3,0,
        4,5,
        5,6,
        6,7,
        7,4,
        0,4,
        1,5,
        2,6,
        3,7*/
    //};

    //cube triangle strip indices
    /*WORD indices[] =
    {
        0, 4 , 1, 5, 2, 6, 3, 7, 0, 4, -1, 7, 4, 6, 5, -1, 0, 3, 2, 1,-1
    };*/

    //hexagon indices
    /*WORD indices[] =
    {
        4,11,5,12,0,7,1,8,2,9,3,10,4,11,-1,3,2,6,1,0,-1,3,4,6,5,0,-1,10,9,13,8,7,-1,10,11,13,12,7,-1
    };*/

	/*std::vector<WORD> indices;
    for (int i = 0; i <= 4; i++)
    {
		for (int j = 0; j <= 4; j++)
		{
			indices.push_back(i*4);
			indices.push_back(i* 4 +5+ j);
		}
        indices.push_back(-1);
        break;
    }*/

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( WORD ) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set index buffer
    g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bd, nullptr, &g_pConstantBuffer );
    if( FAILED( hr ) )
        return hr;

    // Initialize the world matrix
	g_World = XMMatrixIdentity();
	g_World1 = XMMatrixIdentity();
	g_World2 = XMMatrixIdentity();

    // Initialize the view matrix
	XMVECTOR Eye = XMVectorSet( -0.5f, 0.5f, -1.5f, 0.0f );
	XMVECTOR At = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	g_View = XMMatrixLookAtLH( Eye, At, Up);

    // Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH( XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f );

	//Initialize the light position
    //g_lightPos = XMVectorSet(-2.0f, -5.0f, 0.0f, 1.0f);

    ID3D11RasterizerState* m_rasterState = 0;

    D3D11_RASTERIZER_DESC rasterDesc;
    //rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.ScissorEnable = false;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    hr = g_pd3dDevice->CreateRasterizerState(&rasterDesc, &m_rasterState);

    g_pImmediateContext->RSSetState(m_rasterState);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    if( g_pConstantBuffer ) g_pConstantBuffer->Release();
    if( g_pVertexBuffer ) g_pVertexBuffer->Release();
    if( g_pIndexBuffer ) g_pIndexBuffer->Release();
    if( g_pVertexLayout ) g_pVertexLayout->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain1 ) g_pSwapChain1->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext1 ) g_pImmediateContext1->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice1 ) g_pd3dDevice1->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
    case WM_PAINT:
        hdc = BeginPaint( hWnd, &ps );
        EndPaint( hWnd, &ps );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

        // Note that this tutorial does not handle resizing (WM_SIZE) requests,
        // so we created the window without the resize border.

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
    // Update our time
    static float t = 0.0f;
    if( g_driverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        t += ( float )XM_PI * 0.0125f;
    }
    else
    {
        static ULONGLONG timeStart = 0;
        ULONGLONG timeCur = GetTickCount64();
        if( timeStart == 0 )
            timeStart = timeCur;
        t = ( timeCur - timeStart ) / 1000.0f;
    }

    //
    // Animate the cube
    //


    
    

    //THE SUN
    
    //apply scaling
    //g_World = XMMatrixRotationY( 0.5f); //rotation for the cube on the right
	//XMMATRIX mscale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
    //g_World *= mscale; // Combine translation and orbit

 //   //THE EARTH
 //   XMMATRIX mSpin = XMMatrixRotationY(-t * 2);
 //   XMMATRIX mTranslate = XMMatrixTranslation(-6.0f, 0.0f, 0.0f);
 //   XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	//XMMATRIX mOrbit = XMMatrixRotationY(t);

 //   g_World1 = mScale * mSpin * mTranslate * mOrbit; //Rotation, scale and translation for the cube on the left

	////THE MOON
	//XMMATRIX mMoonSpin = XMMatrixRotationY(-t * 8);
	//XMMATRIX mMoonTranslate = XMMatrixTranslation(-3.0f, 0.0f, 0.0f);
	//XMMATRIX mMoonScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	//XMMATRIX mMoonOrbit = XMMatrixRotationY(t * 6);
 //   
	//g_World2 = mMoonScale * mMoonSpin * mMoonTranslate * mMoonOrbit * g_World1; //Rotation, scale and translation for the cube on the left

    //
    // Clear the back buffer
    //
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, Colors::MidnightBlue );

    //
    // Update variables
    //
    ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose( g_World );
	cb.mView = XMMatrixTranspose( g_View );
	cb.mProjection = XMMatrixTranspose( g_Projection );
	cb.lightPos = XMVectorSet(cos(t) * 3, 2.0f, sin(t) * 3, 1.0f);
	g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, nullptr, &cb, 0, 0 );
    
	g_pImmediateContext->VSSetShader( g_pVertexShader, nullptr, 0 );
	g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pConstantBuffer );
    g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pImmediateContext->PSSetShader( g_pPixelShader, nullptr, 0 );
	g_pImmediateContext->DrawIndexed( 72, 0, 0 );        // 36 vertices needed for 12 triangles in a triangle list

   /* g_pImmediateContext->VSSetShader(g_pVertexShader1, nullptr, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pImmediateContext->PSSetShader(g_pPixelShader1, nullptr, 0);
    g_pImmediateContext->DrawIndexed(72, 0, 0);

    g_pImmediateContext->VSSetShader(g_pVertexShader2, nullptr, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pImmediateContext->PSSetShader(g_pPixelShader2, nullptr, 0);
    g_pImmediateContext->DrawIndexed(72, 0, 0);*/
    
 //   //rendering the second cube
 //   ConstantBuffer cb2;
 //   cb2.mWorld = XMMatrixTranspose(g_World1);
 //   cb2.mView = XMMatrixTranspose(g_View);
 //   cb2.mProjection = XMMatrixTranspose(g_Projection);
 //   g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb2, 0, 0);

 //   //
 //   // Render the second cube
 //   //
 //   g_pImmediateContext->DrawIndexed(36, 0, 0);

 //   ConstantBuffer cb3;
	//cb3.mWorld = XMMatrixTranspose(g_World2);
	//cb3.mView = XMMatrixTranspose(g_View);
	//cb3.mProjection = XMMatrixTranspose(g_Projection);
	//g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb3, 0, 0);

	//g_pImmediateContext->DrawIndexed(36, 0, 0);
    

    /*g_World *= XMMatrixTranslation(0.0f, 2.0f, 3.0f);
    cb.mWorld = XMMatrixTranspose(g_World);

    g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    g_pImmediateContext->DrawIndexed(36, 0, 0);*/

    /*
    g_World *= XMMatrixTranslation(-2.8f, 2.0f, 3.0f);
    cb.mWorld = XMMatrixTranspose(g_World);

    g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    g_pImmediateContext->DrawIndexed(36, 0, 0);*/
    //
    // Present our back buffer to our front buffer
    //
    g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    g_pSwapChain->Present( 0, 0 );
}

