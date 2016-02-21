/*	Direct3D9 Device */

#pragma warning(disable : 4995) // for errors in cstring, cwchar and cstdio

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <strsafe.h>
#include <stdexcept> // std::out_of_range

#include "main.h"
#include "logging.h"
#include "Ini.h"   // Read ini file
#include "cLTFX.h" // D3D9 menus

//-----------------------------------------------------------------------------
// defines
//-----------------------------------------------------------------------------

#ifndef SAFE_RELEASE
#define _SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

// Resource Manager
CD3DManager *m_pManager;

// Settings from ini file
CIni cIniObject; 
// General Settings
BOOL Stereo_Enabled = TRUE; // TODO : implement use of this variable in the code
BOOL LaserSight = FALSE;
int DominantEye = 0; // 0 : move both eyes, 1 : move right eye, 2 : move left eye
BOOL ShowFPS = TRUE;

// fps counter
ID3DXFont*          g_pFont = NULL;
TCHAR               g_strFont[LF_FACESIZE];
int                 g_nFontSize = 24;
char pstrFPS[MAX_PATH] = "0 fps";

// variable declaration
IDirect3DSurface9 *			m_pRenderSurfBackBuffer;

IDirect3DSurface9 *			m_pRenderSurfBackBufferDepthLeft;
IDirect3DSurface9 *			m_pRenderSurfBackBufferDepthRight;

IDirect3DSurface9 *			pSurfLeft;
IDirect3DSurface9 *			pSurfRight;

D3DPRESENT_PARAMETERS m_PresentParam; // PresentationParameters
UINT m_refCount; // reference counter

char BufPast[50000];
int shaderCount;
int pixelShaderCount;
float separation;
float convergence;

std::unordered_map<IDirect3DPixelShader9*, IDirect3DPixelShader9*> pPixelShader; // Old shader, Stereo shader

// variable initialization
bool renderLeft = true;
bool firstTime1 = true;
const int primHoldStill = 2;
int shaderCounter = 0;
LARGE_INTEGER qwTicksPerSec = { 0 };
LARGE_INTEGER qwTime = { 0 };
float fAbsTime = 0;
float fLastTime = 0;
DWORD dwFrames = 0;

// Menu
const int MAX_VS = 5000; // Maximum number of vertex shaders
const int MAX_GROUP = 1;
const int MAX_DRAW = 25;
cMenu menu = cMenu(MAX_VS, MAX_GROUP, MAX_DRAW);
int GID[MAX_GROUP] = { 0 };

settingsVS sVS = settingsVS(); // Vertex shader settings

//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------

HRESULT CD3DManager::Initialize()
{
	ADD_LOG("CD3DManager::Initialize()");
	/*
	initialize Resources such as textures 
	(managed and unmanaged [D3DPOOL]), 
	vertex buffers, and other D3D rendering resources
	...
	oCreateTexture(..., ..., &m_pD3Dtexture);
	*/

	if (ShowFPS)
	{
		//	ADD_LOG("pD3Ddev = %d", pD3Ddev);
		if (m_pD3Ddev != NULL)
		{
			HDC hDC = GetDC(NULL);
			int nLogPixelsY = ::GetDeviceCaps(hDC, LOGPIXELSY);
			ReleaseDC(NULL, hDC);

			int nHeight = -g_nFontSize * nLogPixelsY / 72;
			//StringCchCopy( g_strFont, 32, "Arial" );
			HRESULT hr = D3DXCreateFont(m_pD3Ddev,            // D3D device
				nHeight,               // Height
				0,                     // Width
				FW_BOLD,               // Weight
				1,                     // MipLevels, 0 = autogen mipmaps
				FALSE,                 // Italic
				DEFAULT_CHARSET,       // CharSet
				OUT_DEFAULT_PRECIS,    // OutputPrecision
				DEFAULT_QUALITY,       // Quality
				DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
				"Arial",              // pFaceName
				&g_pFont);              // ppFont

			if (FAILED(hr)) // pour font
			{
				//	return hr;
				if (hr == D3DERR_INVALIDCALL) {
					ADD_LOG("D3DXCreateFont failed : D3DERR_INVALIDCALL");
				}
				else if (hr == D3DXERR_INVALIDDATA) {
					ADD_LOG("D3DXCreateFont failed : D3DXERR_INVALIDDATA");
				}
				else if (hr == E_OUTOFMEMORY) {
					ADD_LOG("D3DXCreateFont failed : E_OUTOFMEMORY");
				}
			}
			else
				ADD_LOG("D3DXCreateFont succeeded");
		}
	}

	// Menu
	menu.InitalizeFont("Arial", 24, m_pD3Ddev);
	menu.SetInitialCoords(20, 50);
	//menu.SetSettingFile(INI_SETTINGS);
	menu.SetColor(C_SELECTED,    D3DCOLOR_ARGB(250, 250, 240,  70));
	menu.SetColor(C_VALUES,     D3DCOLOR_ARGB(250, 225, 140,  70));
	menu.SetColor(C_DEFAULT,  D3DCOLOR_ARGB(250, 220,  60,  55));
	menu.SetColor(C_ACTIVATED,   D3DCOLOR_ARGB(250, 20, 240,  50));
	menu.SetColor(C_MOUSEOVER,  D3DCOLOR_ARGB(250, 180,  80,  70));
	menu.SetColor(C_BOX_BORDER, D3DCOLOR_ARGB(150,  51,  51,  51));
	menu.SetColor(C_BOX,        D3DCOLOR_ARGB(250,  25, 220, 251));
	menu.SetColor(C_GROUP,      D3DCOLOR_ARGB(250,  70, 194, 240));
	menu.InitializeMenu();

	//InitKeyHook();//KeyInPut for menu, you can change to GetAsyncKeyState too
	AddMenuItems();

	return S_OK;
}

HRESULT CD3DManager::PreReset()
{
	ADD_LOG("CD3DManager::PreReset()");
	/*
	release all UNMANAGED [D3DPOOL_DEFAULT] 
	textures, vertex buffers, and other 
	volitile resources
	...
	_SAFE_RELEASE(m_pD3Dtexture);
	*/

	if (ShowFPS)
	{
		if (g_pFont)
		{
			g_pFont->OnLostDevice();
			g_pFont->OnResetDevice();
		}
	}

	if (!firstTime1)
	{
		firstTime1 = true;
		// release surfaces
		_SAFE_RELEASE(m_pRenderSurfBackBuffer);
#ifdef OVERUNDER
		_SAFE_RELEASE(m_pRenderSurfBackBufferDepthLeft);
		_SAFE_RELEASE(m_pRenderSurfBackBufferDepthRight);
		_SAFE_RELEASE(pSurfLeft);
		_SAFE_RELEASE(pSurfRight);
#endif	
	}
	
	//if (menu.FontCreated && menu.firstInit)
	//{
	//	menu.PreReset();
	//}
	menu.PreReset();

	return S_OK;
}

HRESULT CD3DManager::PostReset()
{
	ADD_LOG("CD3DManager::PostReset()");
	/*
	re-initialize all UNMANAGED [D3DPOOL_DEFAULT]
	textures, vertex buffers, and other volitile 
	resources
	...
	oCreateTexture(..., ..., &m_pD3Dtexture);
	*/

	// TODO : add missing code

	//if (!menu.FontCreated && menu.firstInit)
	//{
	//	menu.PostReset("Verdana", 8, m_pD3Ddev);
	//}

	return S_OK;
}

HRESULT CD3DManager::Release()
{
	ADD_LOG("CD3DManager::Release()");
	/*
	Release all textures, vertex buffers, and 
	other resources
	...
	_SAFE_RELEASE(m_pD3Dtexture);
	*/

	if (ShowFPS) {
		_SAFE_RELEASE(g_pFont);
	}

	// Release all the orgiginal vertex shaders and stereo verter shaders
	for (auto it = sVS.stereoVS.begin(); it != sVS.stereoVS.end(); ++it)
	{
		_SAFE_RELEASE(const_cast<IDirect3DVertexShader9*>(it->first)); // Not sure if this is right
		//_SAFE_RELEASE(it->second); // The stereo vertex shaders were returned to the game so the game will release them
	}

	// release surfaces
	_SAFE_RELEASE(m_pRenderSurfBackBuffer);
#ifdef OVERUNDER
	_SAFE_RELEASE(m_pRenderSurfBackBufferDepthLeft);
	_SAFE_RELEASE(m_pRenderSurfBackBufferDepthRight);
	_SAFE_RELEASE(pSurfLeft);
	_SAFE_RELEASE(pSurfRight);
#endif	

	return S_OK;
}

//-----------------------------------------------------------------------------

// Initialize once
void hkIDirect3DDevice9_IDirect3DDevice9(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_IDirect3DDevice9()");

	m_pManager = new CD3DManager(pD3Ddev);
	m_pManager->Initialize();

	static char path[MAX_PATH];
	HMODULE hm = NULL;
	if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		"Triditizer",
		&hm))
	{
		int ret = GetLastError();
		fprintf(stderr, "GetModuleHandle returned %d\n", ret);
	}
	GetModuleFileNameA(hm, path, sizeof(path));
	// find the \ in the pathname  
	char* backslash = strrchr(path, '\\');
	if (backslash != NULL)
	{
		strcpy(backslash, "\\Settings.ini");
	}
	cIniObject.SetPathName(path);
	readIni(); // load settings

	m_refCount = 1;

	shaderCount = 0;
	pixelShaderCount = 0;
	separation = 0.1f; //0.5f;
	convergence = 1.0f;
}

// IDirect3DDevice9 functions

// 0
HRESULT APIENTRY hkIDirect3DDevice9_QueryInterface(LPDIRECT3DDEVICE9 pD3Ddev, REFIID riid, LPVOID *ppvObj)
{
	ADD_LOG("hkIDirect3DDevice9_QueryInterface()");
	return oQueryInterface(pD3Ddev, riid, ppvObj);
}

// 1
ULONG APIENTRY hkIDirect3DDevice9_AddRef(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_AddRef() : m_refCount = %d", m_refCount);
	m_refCount++;
	return oAddRef(pD3Ddev);
}

// 2
ULONG APIENTRY hkIDirect3DDevice9_Release(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_Release() : m_refCount = %d", m_refCount);
	if (--m_refCount == 0)
		m_pManager->Release();

	return oRelease(pD3Ddev);
}

// 3
HRESULT APIENTRY hkIDirect3DDevice9_TestCooperativeLevel(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_TestCooperativeLevel()");
	return oTestCooperativeLevel(pD3Ddev);
}

// 4
UINT APIENTRY hkIDirect3DDevice9_GetAvailableTextureMem(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_GetAvailableTextureMem()");
	return oGetAvailableTextureMem(pD3Ddev);
}

// 5
HRESULT APIENTRY hkIDirect3DDevice9_EvictManagedResources(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_EvictManagedResources()");
	return oEvictManagedResources(pD3Ddev);
}

// 6
HRESULT APIENTRY hkIDirect3DDevice9_GetDirect3D(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3D9 **ppD3D9)
{
	ADD_LOG("hkIDirect3DDevice9_GetDirect3D()");
	return oGetDirect3D(pD3Ddev, ppD3D9);
}

// 7
HRESULT APIENTRY hkIDirect3DDevice9_GetDeviceCaps(LPDIRECT3DDEVICE9 pD3Ddev, D3DCAPS9 *pCaps)
{
	ADD_LOG("hkIDirect3DDevice9_GetDeviceCaps()");
	return oGetDeviceCaps(pD3Ddev, pCaps);
}

// 8
HRESULT APIENTRY hkIDirect3DDevice9_GetDisplayMode(LPDIRECT3DDEVICE9 pD3Ddev, UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
	ADD_LOG("hkIDirect3DDevice9_GetDisplayMode()");
	return oGetDisplayMode(pD3Ddev, iSwapChain, pMode);
}

// 9
HRESULT APIENTRY hkIDirect3DDevice9_GetCreationParameters(LPDIRECT3DDEVICE9 pD3Ddev, D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	ADD_LOG("hkIDirect3DDevice9_GetCreationParameters()");
	return oGetCreationParameters(pD3Ddev, pParameters);
}

// 10
HRESULT APIENTRY hkIDirect3DDevice9_SetCursorProperties(LPDIRECT3DDEVICE9 pD3Ddev, UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9 *pCursorBitmap)
{
	ADD_LOG("hkIDirect3DDevice9_SetCursorProperties()");
	return oSetCursorProperties(pD3Ddev, XHotSpot, YHotSpot, pCursorBitmap);
}

// 11
void APIENTRY hkIDirect3DDevice9_SetCursorPosition(LPDIRECT3DDEVICE9 pD3Ddev, int X, int Y, DWORD Flags)
{
	ADD_LOG("hkIDirect3DDevice9_SetCursorPosition()");
	oSetCursorPosition(pD3Ddev, X, Y, Flags);
}

// 12
BOOL APIENTRY hkIDirect3DDevice9_ShowCursor(LPDIRECT3DDEVICE9 pD3Ddev, BOOL bShow)
{
	ADD_LOG("hkIDirect3DDevice9_ShowCursor()");
	return oShowCursor(pD3Ddev, bShow);
}

// 13
HRESULT APIENTRY hkIDirect3DDevice9_CreateAdditionalSwapChain(LPDIRECT3DDEVICE9 pD3Ddev, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain)
{
	ADD_LOG("hkIDirect3DDevice9_CreateAdditionalSwapChain()");
	return oCreateAdditionalSwapChain(pD3Ddev, pPresentationParameters, ppSwapChain);
}

// 14
HRESULT APIENTRY hkIDirect3DDevice9_GetSwapChain(LPDIRECT3DDEVICE9 pD3Ddev, UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
{
	ADD_LOG("hkIDirect3DDevice9_GetSwapChain()");
	return oGetSwapChain(pD3Ddev, iSwapChain, pSwapChain);
}

// 15
unsigned int APIENTRY hkIDirect3DDevice9_GetNumberOfSwapChains(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_GetNumberOfSwapChains()");
	return oGetNumberOfSwapChains(pD3Ddev);
}

// 16
HRESULT APIENTRY hkIDirect3DDevice9_Reset(LPDIRECT3DDEVICE9 pD3Ddev, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	ADD_LOG("hkIDirect3DDevice9_Reset()"); // It's called when going from windowed to fullscreen and other cases

	D3DPRESENT_PARAMETERS	presentParameters;
	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.BackBufferWidth = pPresentationParameters->BackBufferWidth;
	presentParameters.BackBufferHeight = pPresentationParameters->BackBufferHeight;
	presentParameters.BackBufferFormat = pPresentationParameters->BackBufferFormat;
	presentParameters.BackBufferCount = pPresentationParameters->BackBufferCount;

	presentParameters.MultiSampleType = pPresentationParameters->MultiSampleType;
	presentParameters.MultiSampleQuality = pPresentationParameters->MultiSampleQuality;

	presentParameters.SwapEffect = pPresentationParameters->SwapEffect; //D3DSWAPEFFECT_DISCARD;
	presentParameters.hDeviceWindow = pPresentationParameters->hDeviceWindow;
	presentParameters.Windowed = pPresentationParameters->Windowed;
	presentParameters.EnableAutoDepthStencil = false;
	presentParameters.AutoDepthStencilFormat = pPresentationParameters->AutoDepthStencilFormat;// D3DFMT_D24S8;//D3DFMT_D16;
	presentParameters.Flags = pPresentationParameters->Flags & ~(D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL);
	presentParameters.FullScreen_RefreshRateInHz = pPresentationParameters->FullScreen_RefreshRateInHz;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE; //pPresentationParameters->PresentationInterval;// D3DPRESENT_INTERVAL_IMMEDIATE;

	m_pManager->PreReset();

	HRESULT hRet = oReset(pD3Ddev, &presentParameters);

	if (SUCCEEDED(hRet))
	{
		m_PresentParam = presentParameters;
		m_pManager->PostReset();
	}

	return hRet;
}

// 17
HRESULT APIENTRY hkIDirect3DDevice9_Present(LPDIRECT3DDEVICE9 pD3Ddev, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion)
{
	ADD_LOG("hkIDirect3DDevice9_Present()");
	return oPresent(pD3Ddev, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

// 18
HRESULT APIENTRY hkIDirect3DDevice9_GetBackBuffer(LPDIRECT3DDEVICE9 pD3Ddev, UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
	ADD_LOG("hkIDirect3DDevice9_GetBackBuffer()");
	return oGetBackBuffer(pD3Ddev, iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

// 19
HRESULT APIENTRY hkIDirect3DDevice9_GetRasterStatus(LPDIRECT3DDEVICE9 pD3Ddev, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
	ADD_LOG("hkIDirect3DDevice9_GetRasterStatus()");
	return oGetRasterStatus(pD3Ddev, iSwapChain, pRasterStatus);
}

// 20
HRESULT APIENTRY hkIDirect3DDevice9_SetDialogBoxMode(LPDIRECT3DDEVICE9 pD3Ddev, BOOL bEnableDialogs)
{
	ADD_LOG("hkIDirect3DDevice9_SetDialogBoxMode()");
	return oSetDialogBoxMode(pD3Ddev, bEnableDialogs);
}

// 21
void APIENTRY hkIDirect3DDevice9_SetGammaRamp(LPDIRECT3DDEVICE9 pD3Ddev, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
	ADD_LOG("hkIDirect3DDevice9_SetGammaRamp()");
	oSetGammaRamp(pD3Ddev, iSwapChain, Flags, pRamp);
}

// 22
void APIENTRY hkIDirect3DDevice9_GetGammaRamp(LPDIRECT3DDEVICE9 pD3Ddev, UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
	ADD_LOG("hkIDirect3DDevice9_GetGammaRamp()");
	oGetGammaRamp(pD3Ddev, iSwapChain, pRamp);
}

// 23
HRESULT APIENTRY hkIDirect3DDevice9_CreateTexture(LPDIRECT3DDEVICE9 pD3Ddev, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	ADD_LOG("hkIDirect3DDevice9_CreateTexture()");
	return oCreateTexture(pD3Ddev, Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);;
}

// 24
HRESULT APIENTRY hkIDirect3DDevice9_CreateVolumeTexture(LPDIRECT3DDEVICE9 pD3Ddev, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
	ADD_LOG("hkIDirect3DDevice9_CreateVolumeTexture()");
	return oCreateVolumeTexture(pD3Ddev, Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

// 25
HRESULT APIENTRY hkIDirect3DDevice9_CreateCubeTexture(LPDIRECT3DDEVICE9 pD3Ddev, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
	ADD_LOG("hkIDirect3DDevice9_CreateCubeTexture()");
	return oCreateCubeTexture(pD3Ddev, EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

// 26
HRESULT APIENTRY hkIDirect3DDevice9_CreateVertexBuffer(LPDIRECT3DDEVICE9 pD3Ddev, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	ADD_LOG("hkIDirect3DDevice9_CreateVertexBuffer()");
	return oCreateVertexBuffer(pD3Ddev, Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

// 27
HRESULT APIENTRY hkIDirect3DDevice9_CreateIndexBuffer(LPDIRECT3DDEVICE9 pD3Ddev, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
	ADD_LOG("hkIDirect3DDevice9_CreateIndexBuffer()");
	return oCreateIndexBuffer(pD3Ddev, Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

// 28
HRESULT APIENTRY hkIDirect3DDevice9_CreateRenderTarget(LPDIRECT3DDEVICE9 pD3Ddev, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	ADD_LOG("hkIDirect3DDevice9_CreateRenderTarget()");
	return oCreateRenderTarget(pD3Ddev, Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

// 29
HRESULT APIENTRY hkIDirect3DDevice9_CreateDepthStencilSurface(LPDIRECT3DDEVICE9 pD3Ddev, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	ADD_LOG("hkIDirect3DDevice9_CreateDepthStencilSurface()");
	return oCreateDepthStencilSurface(pD3Ddev, Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

// 30
HRESULT APIENTRY hkIDirect3DDevice9_UpdateSurface(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
	ADD_LOG("hkIDirect3DDevice9_UpdateSurface()");
	return oUpdateSurface(pD3Ddev, pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

// 31
HRESULT APIENTRY hkIDirect3DDevice9_UpdateTexture(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DBaseTexture9 *pSourceTexture, IDirect3DBaseTexture9 *pDestinationTexture)
{
	ADD_LOG("hkIDirect3DDevice9_UpdateTexture()");
	return oUpdateTexture(pD3Ddev, pSourceTexture, pDestinationTexture);
}

// 32
HRESULT APIENTRY hkIDirect3DDevice9_GetRenderTargetData(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
	ADD_LOG("hkIDirect3DDevice9_GetRenderTargetData()");
	return oGetRenderTargetData(pD3Ddev, pRenderTarget, pDestSurface);
}

// 33
HRESULT APIENTRY hkIDirect3DDevice9_GetFrontBufferData(LPDIRECT3DDEVICE9 pD3Ddev, UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
	ADD_LOG("hkIDirect3DDevice9_GetFrontBufferData()");
	return oGetFrontBufferData(pD3Ddev, iSwapChain, pDestSurface);
}

// 34
HRESULT APIENTRY hkIDirect3DDevice9_StretchRect(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	ADD_LOG("hkIDirect3DDevice9_StretchRect()");
	return oStretchRect(pD3Ddev, pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

// 35
HRESULT APIENTRY hkIDirect3DDevice9_ColorFill(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
	ADD_LOG("hkIDirect3DDevice9_ColorFill()");
	return oColorFill(pD3Ddev, pSurface, pRect, color);
}

// 36
HRESULT APIENTRY hkIDirect3DDevice9_CreateOffscreenPlainSurface(LPDIRECT3DDEVICE9 pD3Ddev, UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	ADD_LOG("hkIDirect3DDevice9_CreateOffscreenPlainSurface()");
	return oCreateOffscreenPlainSurface(pD3Ddev, Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

// 37
HRESULT APIENTRY hkIDirect3DDevice9_SetRenderTarget(LPDIRECT3DDEVICE9 pD3Ddev, DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	ADD_LOG("hkIDirect3DDevice9_SetRenderTarget()");

#ifdef OVERUNDER
	IDirect3DSurface9* pMyRenderTarget = pRenderTarget;

	if (firstTime1)
	{
		firstTime1 = false;
		initSurfaces(pD3Ddev);
	}

	if ((pRenderTarget == m_pRenderSurfBackBuffer) || (pRenderTarget == pSurfLeft) || (pRenderTarget == pSurfRight))
	{
		if (renderLeft)
		{
			// ADD_LOG("renderLeft");
			pMyRenderTarget = pSurfLeft;
		}
		else
		{
			// ADD_LOG("renderRight");
			pMyRenderTarget = pSurfRight;
		}
	}

	HRESULT hr = oSetRenderTarget(pD3Ddev, RenderTargetIndex, pMyRenderTarget);

	D3DSURFACE_DESC desc;
	pMyRenderTarget->GetDesc(&desc);

	ADD_LOG("newly set render target : %dx%d", desc.Width, desc.Height);

	if ((desc.Width == 1) && (desc.Height == 1))
	{
		// Need to set the viewport to the depth buffer size
		IDirect3DSurface9 *		renderSurfTempDepth;
		oGetDepthStencilSurface(pD3Ddev, &renderSurfTempDepth);
		ADD_LOG("Calling hkIDirect3DDevice9_SetDepthStencilSurface from hkIDirect3DDevice9_SetRenderTarget");
		hkIDirect3DDevice9_SetDepthStencilSurface(pD3Ddev, renderSurfTempDepth);
		renderSurfTempDepth->Release();

		D3DVIEWPORT9 vp;
		vp.X = 0;
		vp.Y = 0;
		vp.Width = desc.Width;
		vp.Height = desc.Height;
		vp.MinZ = 0.0f;
		vp.MaxZ = 1.0f;
		oSetViewport(pD3Ddev, &vp);
	}
	return hr;
#endif

#ifndef OVERUNDER
	return oSetRenderTarget(pD3Ddev, RenderTargetIndex, pRenderTarget);
#endif

}

// 38
HRESULT APIENTRY hkIDirect3DDevice9_GetRenderTarget(LPDIRECT3DDEVICE9 pD3Ddev, DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	ADD_LOG("hkIDirect3DDevice9_GetRenderTarget()");
	return oGetRenderTarget(pD3Ddev, RenderTargetIndex, ppRenderTarget);
}

// 39
HRESULT APIENTRY hkIDirect3DDevice9_SetDepthStencilSurface(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DSurface9* pNewZStencil)
{
	ADD_LOG("hkIDirect3DDevice9_SetDepthStencilSurface()");

#ifdef OVERUNDER

	if ((pNewZStencil == m_pRenderSurfBackBufferDepthLeft) || (pNewZStencil == m_pRenderSurfBackBufferDepthRight))
	{
		if (renderLeft)
		{
			// ADD_LOG("hkIDirect3DDevice9_SetDepthStencilSurface -> renderLeft");
			return oSetDepthStencilSurface(pD3Ddev, m_pRenderSurfBackBufferDepthLeft);
		}
		else
		{
			// ADD_LOG("hkIDirect3DDevice9_SetDepthStencilSurface -> renderRight"); 
			return oSetDepthStencilSurface(pD3Ddev, m_pRenderSurfBackBufferDepthRight);
		}
	}
	
	if (pNewZStencil == NULL)
		return oSetDepthStencilSurface(pD3Ddev, m_pRenderSurfBackBufferDepthLeft); // Fixed a crash in a game

	return oSetDepthStencilSurface(pD3Ddev, pNewZStencil);
#endif

#ifndef OVERUNDER
	return oSetDepthStencilSurface(pNewZStencil);
#endif
}

// 40
HRESULT APIENTRY hkIDirect3DDevice9_GetDepthStencilSurface(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DSurface9 **ppZStencilSurface)
{
	ADD_LOG("hkIDirect3DDevice9_GetDepthStencilSurface()");
	if (firstTime1)
	{
		firstTime1 = false;
		initSurfaces(pD3Ddev);
	}

	return oGetDepthStencilSurface(pD3Ddev, ppZStencilSurface);
}

// 41
HRESULT APIENTRY hkIDirect3DDevice9_BeginScene(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("#################################################################################hkIDirect3DDevice9_BeginScene()");

	if (firstTime1)
	{
		firstTime1 = false;
		initSurfaces(pD3Ddev);
	}

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		if (GetAsyncKeyState(VK_F3) & 0x8000)
			decreaseSeparation();
		else if (GetAsyncKeyState(VK_F4) & 0x8000)
			increaseSeparation();
		else if (GetAsyncKeyState(VK_F5) & 0x8000)
			decreaseConvergence();
		else if (GetAsyncKeyState(VK_F6) & 0x8000)
			increaseConvergence();
		else if (GetAsyncKeyState(VK_PRIOR) & 0x8000) // disable all stereo vertex shaders
		{
			for (auto it = sVS.enabled.begin(); it != sVS.enabled.end(); ++it)
			{
				it->second = 0.0f;
			}
		}
		else if (GetAsyncKeyState(VK_NEXT) & 0x8000) // enable all stereo vertex shaders
		{
			for (auto it = sVS.enabled.begin(); it != sVS.enabled.end(); ++it)
			{
				it->second = 1.0f;
			}
		}
	}

	menu.HandleKeyNavigation(0);
	//menu.HandleMouseNavigation(0);
	menu.HandleItemHotkeys(0);
	menu.DragMenuWindow();
	
	HRESULT hr;
	hr = oBeginScene(pD3Ddev);

	if (renderLeft)
		move_view(pD3Ddev, LEFT);
	else
		move_view(pD3Ddev, RIGHT);

	return hr;
}

// 42
HRESULT APIENTRY hkIDirect3DDevice9_EndScene(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("---------------------------------------------------------------------------------hkIDirect3DDevice9_EndScene()");

	if ((ShowFPS == TRUE) && (g_pFont != 0))
	{
		QueryPerformanceFrequency(&qwTicksPerSec);
		LONGLONG m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
		QueryPerformanceCounter(&qwTime);
		fAbsTime = qwTime.QuadPart / (double)m_llQPFTicksPerSec;
		dwFrames += 1;

		// Update the scene stats once per second
		if (fAbsTime - fLastTime > 1.0f)
		{
			float fFPS = (float)(dwFrames / (fAbsTime - fLastTime));
			fLastTime = fAbsTime;
			sprintf(pstrFPS, "%0.2f fps, Number of Vertex shaders : %d", fFPS, menu.getItemsCount());
			dwFrames = 0;
		}

		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = 1.0;
		rc.bottom = 1.0;
		HRESULT hr1 = g_pFont->DrawText(NULL, pstrFPS, -1, &rc, DT_NOCLIP, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	}

	// Menu
	//menu.DrawText1(1, 1, 0x00000000, "");//Fix the Render, on some games this is needed
	menu.DrawMenu("Settings menu", pD3Ddev);

	if (menu.bMenu)
	{
		menu.DrawMouse(pD3Ddev);
	}

#ifdef OVERUNDER

	D3DSURFACE_DESC desc;
	m_pRenderSurfBackBuffer->GetDesc(&desc);

	// Calculate the rect of left and right frames
	// TODO : Later, do this only once and use the precalculated rect
	int heightMiddle = desc.Height / 2;

	RECT tempRectTop;
	RECT tempRectBottom;

	tempRectTop.left = 0;
	tempRectTop.top = 0;
	tempRectTop.right = desc.Width;
	tempRectTop.bottom = heightMiddle;

	tempRectBottom.left = 0;
	tempRectBottom.top = desc.Height - heightMiddle;
	tempRectBottom.right = desc.Width;
	tempRectBottom.bottom = desc.Height;

	// TODO : look into what's the best filter type to use here
	oStretchRect(pD3Ddev, pSurfLeft, NULL, m_pRenderSurfBackBuffer, &tempRectTop, D3DTEXF_NONE);
	oStretchRect(pD3Ddev, pSurfRight, NULL, m_pRenderSurfBackBuffer, &tempRectBottom, D3DTEXF_NONE);
#endif

	if (GetAsyncKeyState(0x2C) & 0x8000) // print screen key
		hkIDirect3DDevice9_takeScreenshot(pD3Ddev);

	HRESULT hr = oEndScene(pD3Ddev);

#ifdef OVERUNDER
	if (renderLeft)
	{
		renderLeft = false;
		if (oSetRenderTarget(pD3Ddev, 0, pSurfRight) != D3D_OK) {
			ADD_LOG("oSetRenderTarget(0, pSurfRight) failed");
		}
	}
	else
	{
		renderLeft = true;
		if (oSetRenderTarget(pD3Ddev, 0, pSurfLeft) != D3D_OK) {
			ADD_LOG("oSetRenderTarget(0, pSurfLeft) failed");
		}
	}
#endif

	return hr;
}

// 43
HRESULT APIENTRY hkIDirect3DDevice9_Clear(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	ADD_LOG("hkIDirect3DDevice9_Clear()");
	if (firstTime1)
	{
		firstTime1 = false;
		initSurfaces(pD3Ddev);
	}

	return oClear(pD3Ddev, Count, pRects, Flags, Color, Z, Stencil);
}

// 44
HRESULT APIENTRY hkIDirect3DDevice9_SetTransform(LPDIRECT3DDEVICE9 pD3Ddev, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
{
	ADD_LOG("hkIDirect3DDevice9_SetTransform()");
	return oSetTransform(pD3Ddev, State, pMatrix);
}

// 45
HRESULT APIENTRY hkIDirect3DDevice9_GetTransform(LPDIRECT3DDEVICE9 pD3Ddev, D3DTRANSFORMSTATETYPE State, D3DMATRIX *pMatrix)
{
	ADD_LOG("hkIDirect3DDevice9_GetTransform()");
	return oGetTransform(pD3Ddev, State, pMatrix);
}

// 46
HRESULT APIENTRY hkIDirect3DDevice9_MultiplyTransform(LPDIRECT3DDEVICE9 pD3Ddev, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
{
	ADD_LOG("hkIDirect3DDevice9_MultiplyTransform()");
	return oMultiplyTransform(pD3Ddev, State, pMatrix);
}

// 47
HRESULT APIENTRY hkIDirect3DDevice9_SetViewport(LPDIRECT3DDEVICE9 pD3Ddev, CONST D3DVIEWPORT9 *pViewport)
{
	ADD_LOG("hkIDirect3DDevice9_SetViewport()");
	return oSetViewport(pD3Ddev, pViewport);
}

// 48
HRESULT APIENTRY hkIDirect3DDevice9_GetViewport(LPDIRECT3DDEVICE9 pD3Ddev, D3DVIEWPORT9 *pViewport)
{
	ADD_LOG("hkIDirect3DDevice9_GetViewport()");
	return oGetViewport(pD3Ddev, pViewport);
}

// 49
HRESULT APIENTRY hkIDirect3DDevice9_SetMaterial(LPDIRECT3DDEVICE9 pD3Ddev, CONST D3DMATERIAL9 *pMaterial)
{
	ADD_LOG("hkIDirect3DDevice9_SetMaterial()");
	return oSetMaterial(pD3Ddev, pMaterial);
}

// 50
HRESULT APIENTRY hkIDirect3DDevice9_GetMaterial(LPDIRECT3DDEVICE9 pD3Ddev, D3DMATERIAL9 *pMaterial)
{
	ADD_LOG("hkIDirect3DDevice9_GetMaterial()");
	return oGetMaterial(pD3Ddev, pMaterial);
}

// 51
HRESULT APIENTRY hkIDirect3DDevice9_SetLight(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Index, CONST D3DLIGHT9 *pLight)
{
	ADD_LOG("hkIDirect3DDevice9_SetLight()");
	return oSetLight(pD3Ddev, Index, pLight);
}

// 52
HRESULT APIENTRY hkIDirect3DDevice9_GetLight(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Index, D3DLIGHT9 *pLight)
{
	ADD_LOG("hkIDirect3DDevice9_GetLight()");
	return oGetLight(pD3Ddev, Index, pLight);
}

// 53
HRESULT APIENTRY hkIDirect3DDevice9_LightEnable(LPDIRECT3DDEVICE9 pD3Ddev, DWORD LightIndex, BOOL bEnable)
{
	ADD_LOG("hkIDirect3DDevice9_LightEnable()");
	return oLightEnable(pD3Ddev, LightIndex, bEnable);
}

// 54
HRESULT APIENTRY hkIDirect3DDevice9_GetLightEnable(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Index, BOOL *pEnable)
{
	ADD_LOG("hkIDirect3DDevice9_GetLightEnable()");
	return oGetLightEnable(pD3Ddev, Index, pEnable);
}

// 55
HRESULT APIENTRY hkIDirect3DDevice9_SetClipPlane(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Index, CONST float *pPlane)
{
	ADD_LOG("hkIDirect3DDevice9_SetClipPlane()");
	return oSetClipPlane(pD3Ddev, Index, pPlane);
}

// 56
HRESULT APIENTRY hkIDirect3DDevice9_GetClipPlane(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Index, float *pPlane)
{
	ADD_LOG("hkIDirect3DDevice9_GetClipPlane()");
	return oGetClipPlane(pD3Ddev, Index, pPlane);
}

// 57
HRESULT APIENTRY hkIDirect3DDevice9_SetRenderState(LPDIRECT3DDEVICE9 pD3Ddev, D3DRENDERSTATETYPE State, DWORD Value)
{
	ADD_LOG("hkIDirect3DDevice9_SetRenderState()");
	return oSetRenderState(pD3Ddev, State, Value);
}

// 58
HRESULT APIENTRY hkIDirect3DDevice9_GetRenderState(LPDIRECT3DDEVICE9 pD3Ddev, D3DRENDERSTATETYPE State, DWORD *pValue)
{
	ADD_LOG("hkIDirect3DDevice9_GetRenderState()");
	return oGetRenderState(pD3Ddev, State, pValue);
}

// 59
HRESULT APIENTRY hkIDirect3DDevice9_CreateStateBlock(LPDIRECT3DDEVICE9 pD3Ddev, D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB)
{
	ADD_LOG("hkIDirect3DDevice9_CreateStateBlock()");
	return oCreateStateBlock(pD3Ddev, Type, ppSB);
}

// 60
HRESULT APIENTRY hkIDirect3DDevice9_BeginStateBlock(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_BeginStateBlock() ");
	return oBeginStateBlock(pD3Ddev);
}

// 61
HRESULT APIENTRY hkIDirect3DDevice9_EndStateBlock(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DStateBlock9** ppSB)
{
	ADD_LOG("hkIDirect3DDevice9_EndStateBlock()");
	return oEndStateBlock(pD3Ddev, ppSB);
}

// 62
HRESULT APIENTRY hkIDirect3DDevice9_SetClipStatus(LPDIRECT3DDEVICE9 pD3Ddev, CONST D3DCLIPSTATUS9 *pClipStatus)
{
	ADD_LOG("hkIDirect3DDevice9_SetClipStatus()");
	return oSetClipStatus(pD3Ddev, pClipStatus);
}

// 63
HRESULT APIENTRY hkIDirect3DDevice9_GetClipStatus(LPDIRECT3DDEVICE9 pD3Ddev, D3DCLIPSTATUS9 *pClipStatus)
{
	ADD_LOG("hkIDirect3DDevice9_GetClipStatus()");
	return oGetClipStatus(pD3Ddev, pClipStatus);
}

// 64
HRESULT APIENTRY hkIDirect3DDevice9_GetTexture(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Stage, IDirect3DBaseTexture9 **ppTexture)
{
	ADD_LOG("hkIDirect3DDevice9_GetTexture()");
	return oGetTexture(pD3Ddev, Stage, ppTexture);
}

// 65
HRESULT APIENTRY hkIDirect3DDevice9_SetTexture(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Stage, IDirect3DBaseTexture9 *pTexture)
{
	ADD_LOG("hkIDirect3DDevice9_SetTexture()");
	return oSetTexture(pD3Ddev, Stage, pTexture);
}

// 66
HRESULT APIENTRY hkIDirect3DDevice9_GetTextureStageState(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue)
{
	ADD_LOG("hkIDirect3DDevice9_GetTextureStageState()");
	return oGetTextureStageState(pD3Ddev, Stage, Type, pValue);
}

// 67
HRESULT APIENTRY hkIDirect3DDevice9_SetTextureStageState(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	ADD_LOG("hkIDirect3DDevice9_SetTextureStageState()");
	return oSetTextureStageState(pD3Ddev, Stage, Type, Value);
}

// 68
HRESULT APIENTRY hkIDirect3DDevice9_GetSamplerState(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
	ADD_LOG("hkIDirect3DDevice9_GetSamplerState()");
	return oGetSamplerState(pD3Ddev, Sampler, Type, pValue);
}

// 69
HRESULT APIENTRY hkIDirect3DDevice9_SetSamplerState(LPDIRECT3DDEVICE9 pD3Ddev, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	ADD_LOG("hkIDirect3DDevice9_SetSamplerState()");
	return oSetSamplerState(pD3Ddev, Sampler, Type, Value);
}

// 70
HRESULT APIENTRY hkIDirect3DDevice9_ValidateDevice(LPDIRECT3DDEVICE9 pD3Ddev, DWORD *pNumPasses)
{
	ADD_LOG("hkIDirect3DDevice9_ValidateDevice()");
	return oValidateDevice(pD3Ddev, pNumPasses);
}

// 71
HRESULT APIENTRY hkIDirect3DDevice9_SetPaletteEntries(LPDIRECT3DDEVICE9 pD3Ddev, UINT PaletteNumber, CONST PALETTEENTRY *pEntries)
{
	ADD_LOG("hkIDirect3DDevice9_SetPaletteEntries()");
	return oSetPaletteEntries(pD3Ddev, PaletteNumber, pEntries);
}

// 72
HRESULT APIENTRY hkIDirect3DDevice9_GetPaletteEntries(LPDIRECT3DDEVICE9 pD3Ddev, UINT PaletteNumber, PALETTEENTRY *pEntries)
{
	ADD_LOG("hkIDirect3DDevice9_GetPaletteEntries()");
	return oGetPaletteEntries(pD3Ddev, PaletteNumber, pEntries);
}

// 73
HRESULT APIENTRY hkIDirect3DDevice9_SetCurrentTexturePalette(LPDIRECT3DDEVICE9 pD3Ddev, UINT PaletteNumber)
{
	ADD_LOG("hkIDirect3DDevice9_SetCurrentTexturePalette()");
	return oSetCurrentTexturePalette(pD3Ddev, PaletteNumber);
}

// 74
HRESULT APIENTRY hkIDirect3DDevice9_GetCurrentTexturePalette(LPDIRECT3DDEVICE9 pD3Ddev, UINT *pPaletteNumber)
{
	ADD_LOG("hkIDirect3DDevice9_GetCurrentTexturePalette()");
	return oGetCurrentTexturePalette(pD3Ddev, pPaletteNumber);
}

// 75
HRESULT APIENTRY hkIDirect3DDevice9_SetScissorRect(LPDIRECT3DDEVICE9 pD3Ddev, CONST RECT* pRect)
{
	ADD_LOG("hkIDirect3DDevice9_SetScissorRect()");
	return oSetScissorRect(pD3Ddev, pRect);
}

// 76
HRESULT APIENTRY hkIDirect3DDevice9_GetScissorRect(LPDIRECT3DDEVICE9 pD3Ddev, RECT* pRect)
{
	ADD_LOG("hkIDirect3DDevice9_GetScissorRect()");
	return oGetScissorRect(pD3Ddev, pRect);
}

// 77
HRESULT APIENTRY hkIDirect3DDevice9_SetSoftwareVertexProcessing(LPDIRECT3DDEVICE9 pD3Ddev, BOOL bSoftware)
{
	ADD_LOG("hkIDirect3DDevice9_SetSoftwareVertexProcessing()");
	return oSetSoftwareVertexProcessing(pD3Ddev, bSoftware);
}

// 78
BOOL APIENTRY hkIDirect3DDevice9_GetSoftwareVertexProcessing(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_GetSoftwareVertexProcessing()");
	return oGetSoftwareVertexProcessing(pD3Ddev);
}

// 79
HRESULT APIENTRY hkIDirect3DDevice9_SetNPatchMode(LPDIRECT3DDEVICE9 pD3Ddev, float nSegments)
{
	ADD_LOG("hkIDirect3DDevice9_SetNPatchMode()");
	return oSetNPatchMode(pD3Ddev, nSegments);
}

// 80
float APIENTRY hkIDirect3DDevice9_GetNPatchMode(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_GetNPatchMode()");
	return oGetNPatchMode(pD3Ddev);
}

// 81
HRESULT APIENTRY hkIDirect3DDevice9_DrawPrimitive(LPDIRECT3DDEVICE9 pD3Ddev, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	ADD_LOG("hkIDirect3DDevice9_DrawPrimitive()");
	if ((PrimitiveCount <= primHoldStill) || (separation == 0)) // fixes post-processing in some games
	{
		move_view(pD3Ddev, DO_NOT_MOVE);
	}
	else
	{
		if (renderLeft)
			move_view(pD3Ddev, LEFT);
		else
			move_view(pD3Ddev, RIGHT);
	}

	return oDrawPrimitive(pD3Ddev, PrimitiveType, StartVertex, PrimitiveCount);
}

// 82
HRESULT APIENTRY hkIDirect3DDevice9_DrawIndexedPrimitive(LPDIRECT3DDEVICE9 pD3Ddev, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	ADD_LOG("hkIDirect3DDevice9_DrawIndexedPrimitive()");
	return oDrawIndexedPrimitive(pD3Ddev, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

// 83
HRESULT APIENTRY hkIDirect3DDevice9_DrawPrimitiveUP(LPDIRECT3DDEVICE9 pD3Ddev, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	ADD_LOG("hkIDirect3DDevice9_DrawPrimitiveUP()");
	move_view(pD3Ddev, DO_NOT_MOVE); // for some games shadows

	return oDrawPrimitiveUP(pD3Ddev, PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

// 84
HRESULT APIENTRY hkIDirect3DDevice9_DrawIndexedPrimitiveUP(LPDIRECT3DDEVICE9 pD3Ddev, D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	ADD_LOG("hkIDirect3DDevice9_DrawIndexedPrimitiveUP()");
	//move_view(pD3Ddev, DO_NOT_MOVE); // Some games might need this

	return oDrawIndexedPrimitiveUP(pD3Ddev, PrimitiveType, MinIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}


// 85
HRESULT APIENTRY hkIDirect3DDevice9_ProcessVertices(LPDIRECT3DDEVICE9 pD3Ddev, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
	ADD_LOG("hkIDirect3DDevice9_ProcessVertices()");
	return oProcessVertices(pD3Ddev, SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

// 86
HRESULT APIENTRY hkIDirect3DDevice9_CreateVertexDeclaration(LPDIRECT3DDEVICE9 pD3Ddev, CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl)
{
	ADD_LOG("hkIDirect3DDevice9_CreateVertexDeclaration()");
	return oCreateVertexDeclaration(pD3Ddev, pVertexElements,ppDecl);
}

// 87
HRESULT APIENTRY hkIDirect3DDevice9_SetVertexDeclaration(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DVertexDeclaration9* pDecl)
{
	ADD_LOG("hkIDirect3DDevice9_SetVertexDeclaration()");
	return oSetVertexDeclaration(pD3Ddev, pDecl);
}

// 88
HRESULT APIENTRY hkIDirect3DDevice9_GetVertexDeclaration(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DVertexDeclaration9** ppDecl)
{
	ADD_LOG("hkIDirect3DDevice9_GetVertexDeclaration()");
	return oGetVertexDeclaration(pD3Ddev, ppDecl);
}

// 89
HRESULT APIENTRY hkIDirect3DDevice9_SetFVF(LPDIRECT3DDEVICE9 pD3Ddev, DWORD FVF)
{
	ADD_LOG("hkIDirect3DDevice9_SetFVF()");
	return oSetFVF(pD3Ddev, FVF);
}

// 90
HRESULT APIENTRY hkIDirect3DDevice9_GetFVF(LPDIRECT3DDEVICE9 pD3Ddev, DWORD* pFVF)
{
	ADD_LOG("hkIDirect3DDevice9_GetFVF()");
	return oGetFVF(pD3Ddev, pFVF);
}

// 91
HRESULT APIENTRY hkIDirect3DDevice9_CreateVertexShader(LPDIRECT3DDEVICE9 pD3Ddev, CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader)
{
	ADD_LOG("hkIDirect3DDevice9_CreateVertexShader()");
	// Call function to create a modified vertex shader
	return CreateStereoVertexShader(pD3Ddev, pFunction, ppShader);
}

// 92
HRESULT APIENTRY hkIDirect3DDevice9_SetVertexShader(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DVertexShader9* pShader)
{
	ADD_LOG("hkIDirect3DDevice9_SetVertexShader()");
	HRESULT hr;

	if (pShader != 0)
	{
		// Find the stereo Vertex shader and call SetVertexShader on it
		try {
			if (sVS.enabled.at(pShader))
				hr = oSetVertexShader(pD3Ddev, sVS.stereoVS.at(pShader));
			else
				hr = oSetVertexShader(pD3Ddev, pShader);
		}
		catch (const std::out_of_range& oor) { 
			// Not found, use original vertex shader instead and print a message to the log file
			ADD_LOG("hkIDirect3DDevice9_SetVertexShader() : no stereo vertex shader found for original vertex shader %p", pShader);
			hr = oSetVertexShader(pD3Ddev, pShader);
		}
	}
	else
	{
		hr = oSetVertexShader(pD3Ddev, NULL); // Not supposed to happen
	}

	return hr;
}

// 93
HRESULT APIENTRY hkIDirect3DDevice9_GetVertexShader(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DVertexShader9** ppShader)
{
	ADD_LOG("hkIDirect3DDevice9_GetVertexShader()");
	return oGetVertexShader(pD3Ddev, ppShader);
}

// 94
HRESULT APIENTRY hkIDirect3DDevice9_SetVertexShaderConstantF(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	ADD_LOG("hkIDirect3DDevice9_SetVertexShaderConstantF()");
	return oSetVertexShaderConstantF(pD3Ddev, StartRegister, pConstantData, Vector4fCount);
}

// 95
HRESULT APIENTRY hkIDirect3DDevice9_GetVertexShaderConstantF(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	ADD_LOG("hkIDirect3DDevice9_GetVertexShaderConstantF()");
	return oGetVertexShaderConstantF(pD3Ddev, StartRegister, pConstantData, Vector4fCount);
}

// 96
HRESULT APIENTRY hkIDirect3DDevice9_SetVertexShaderConstantI(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	ADD_LOG("hkIDirect3DDevice9_SetVertexShaderConstantI()");
	return oSetVertexShaderConstantI(pD3Ddev, StartRegister, pConstantData, Vector4iCount);
}

// 97
HRESULT APIENTRY hkIDirect3DDevice9_GetVertexShaderConstantI(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	ADD_LOG("hkIDirect3DDevice9_GetVertexShaderConstantI(");
	return oGetVertexShaderConstantI(pD3Ddev, StartRegister, pConstantData, Vector4iCount);
}

// 98
HRESULT APIENTRY hkIDirect3DDevice9_SetVertexShaderConstantB(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
	ADD_LOG("hkIDirect3DDevice9_SetVertexShaderConstantB()");
	return oSetVertexShaderConstantB(pD3Ddev, StartRegister, pConstantData, BoolCount);
}

// 99
HRESULT APIENTRY hkIDirect3DDevice9_GetVertexShaderConstantB(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	ADD_LOG("hkIDirect3DDevice9_GetVertexShaderConstantB()");
	return oGetVertexShaderConstantB(pD3Ddev, StartRegister, pConstantData, BoolCount);
}

// 100
HRESULT APIENTRY hkIDirect3DDevice9_SetStreamSource(LPDIRECT3DDEVICE9 pD3Ddev, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	ADD_LOG("hkIDirect3DDevice9_SetStreamSource()");
	return oSetStreamSource(pD3Ddev, StreamNumber, pStreamData, OffsetInBytes, Stride);
}

// 101
HRESULT APIENTRY hkIDirect3DDevice9_GetStreamSource(LPDIRECT3DDEVICE9 pD3Ddev, UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride)
{
	ADD_LOG("hkIDirect3DDevice9_GetStreamSource()");
	return oGetStreamSource(pD3Ddev, StreamNumber, ppStreamData, OffsetInBytes, pStride);
}

// 102
HRESULT APIENTRY hkIDirect3DDevice9_SetStreamSourceFreq(LPDIRECT3DDEVICE9 pD3Ddev, UINT StreamNumber, UINT Divider)
{
	ADD_LOG("hkIDirect3DDevice9_SetStreamSourceFreq()");
	return oSetStreamSourceFreq(pD3Ddev, StreamNumber, Divider);
}

// 103
HRESULT APIENTRY hkIDirect3DDevice9_GetStreamSourceFreq(LPDIRECT3DDEVICE9 pD3Ddev, UINT StreamNumber, UINT* Divider)
{
	ADD_LOG("hkIDirect3DDevice9_GetStreamSourceFreq()");
	return oGetStreamSourceFreq(pD3Ddev, StreamNumber, Divider);
}

// 104
HRESULT APIENTRY hkIDirect3DDevice9_SetIndices(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DIndexBuffer9* pIndexData)
{
	ADD_LOG("hkIDirect3DDevice9_SetIndices()");
	return oSetIndices(pD3Ddev, pIndexData);
}

// 105
HRESULT APIENTRY hkIDirect3DDevice9_GetIndices(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DIndexBuffer9** ppIndexData)
{
	ADD_LOG("hkIDirect3DDevice9_GetIndices()");
	return oGetIndices(pD3Ddev, ppIndexData);
}

// 106
HRESULT APIENTRY hkIDirect3DDevice9_CreatePixelShader(LPDIRECT3DDEVICE9 pD3Ddev, CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
	ADD_LOG("hkIDirect3DDevice9_CreatePixelShader()");
	// Call function to create a modified pixel shader (to add a lasersight)
	return CreateModifiedPixelShader(pD3Ddev, pFunction, ppShader);
}

// 107
HRESULT APIENTRY hkIDirect3DDevice9_SetPixelShader(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DPixelShader9* pShader)
{
	ADD_LOG("hkIDirect3DDevice9_SetPixelShader()");
	HRESULT hr;

	if ((LaserSight == 1) || (LaserSight == 2))
	{

		if (pShader != 0)
		{
			// Find the modified pixel shader and call SetPixelShader on it
			try {
				hr = oSetPixelShader(pD3Ddev, pPixelShader.at(pShader));
			}
			catch (const std::out_of_range& oor) {
				// Not found, use original pixel shader instead and print a message to the log file
				ADD_LOG("hkIDirect3DDevice9_SetPixelShader() : no modified pixel shader found for original pixel shader %p", pShader);
				hr = oSetPixelShader(pD3Ddev, pShader);
			}
		}
		else
		{
			hr = oSetPixelShader(pD3Ddev, NULL); // Not supposed to happen
		}
	}
	else // LaserSight == 0
	{
		hr = oSetPixelShader(pD3Ddev, pShader);
	}

	return hr;
}

// 108
HRESULT APIENTRY hkIDirect3DDevice9_GetPixelShader(LPDIRECT3DDEVICE9 pD3Ddev, IDirect3DPixelShader9** ppShader)
{
	ADD_LOG("hkIDirect3DDevice9_GetPixelShader()");
	return oGetPixelShader(pD3Ddev, ppShader);
}

// 109
HRESULT APIENTRY hkIDirect3DDevice9_SetPixelShaderConstantF(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	ADD_LOG("hkIDirect3DDevice9_SetPixelShaderConstantF()");
	return oSetPixelShaderConstantF(pD3Ddev, StartRegister, pConstantData, Vector4fCount);
}

// 110
HRESULT APIENTRY hkIDirect3DDevice9_GetPixelShaderConstantF(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	ADD_LOG("hkIDirect3DDevice9_GetPixelShaderConstantF()");
	return oGetPixelShaderConstantF(pD3Ddev, StartRegister, pConstantData, Vector4fCount);
}

// 111
HRESULT APIENTRY hkIDirect3DDevice9_SetPixelShaderConstantI(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	ADD_LOG("hkIDirect3DDevice9_SetPixelShaderConstantI()");
	return oSetPixelShaderConstantI(pD3Ddev, StartRegister, pConstantData, Vector4iCount);
}

// 112
HRESULT APIENTRY hkIDirect3DDevice9_GetPixelShaderConstantI(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	ADD_LOG("hkIDirect3DDevice9_GetPixelShaderConstantI()");
	return oGetPixelShaderConstantI(pD3Ddev, StartRegister, pConstantData, Vector4iCount);
}

// 113
HRESULT APIENTRY hkIDirect3DDevice9_SetPixelShaderConstantB(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
	ADD_LOG("hkIDirect3DDevice9_SetPixelShaderConstantB()");
	return oSetPixelShaderConstantB(pD3Ddev, StartRegister, pConstantData, BoolCount);
}

// 114
HRESULT APIENTRY hkIDirect3DDevice9_GetPixelShaderConstantB(LPDIRECT3DDEVICE9 pD3Ddev, UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	ADD_LOG("hkIDirect3DDevice9_GetPixelShaderConstantB()");
	return oGetPixelShaderConstantB(pD3Ddev, StartRegister, pConstantData, BoolCount);
}

// 115
HRESULT APIENTRY hkIDirect3DDevice9_DrawRectPatch(LPDIRECT3DDEVICE9 pD3Ddev, UINT Handle, CONST float *pNumSegs, CONST D3DRECTPATCH_INFO *pRectPatchInfo)
{
	ADD_LOG("hkIDirect3DDevice9_DrawRectPatch()");
	return oDrawRectPatch(pD3Ddev, Handle, pNumSegs, pRectPatchInfo);
}

// 116
HRESULT APIENTRY hkIDirect3DDevice9_DrawTriPatch(LPDIRECT3DDEVICE9 pD3Ddev, UINT Handle, CONST float *pNumSegs, CONST D3DTRIPATCH_INFO *pTriPatchInfo)
{
	ADD_LOG("hkIDirect3DDevice9_DrawTriPatch()");
	return oDrawTriPatch(pD3Ddev, Handle, pNumSegs, pTriPatchInfo);
}

// 117
HRESULT APIENTRY hkIDirect3DDevice9_DeletePatch(LPDIRECT3DDEVICE9 pD3Ddev, UINT Handle)
{
	ADD_LOG("hkIDirect3DDevice9_DeletePatch()");
	return oDeletePatch(pD3Ddev, Handle);
}

// 118
HRESULT APIENTRY hkIDirect3DDevice9_CreateQuery(LPDIRECT3DDEVICE9 pD3Ddev, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
	ADD_LOG("hkIDirect3DDevice9_CreateQuery()");
	return oCreateQuery(pD3Ddev, Type, ppQuery);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Custom functions added below

void readIni(void)
{
	Stereo_Enabled = cIniObject.GetBool("General Settings", "Stereo_Enabled", TRUE);
	LaserSight = cIniObject.GetInt("General Settings", "LaserSight", FALSE);
	if ((LaserSight != 0) && (LaserSight != 1) && (LaserSight != 2))
		LaserSight = 0;
	if (LaserSight == 2) // Deactivate ls2 because it's not implemented yet
		LaserSight = 0;

	DominantEye = cIniObject.GetInt("General Settings", "DominantEye", 0);
	if ((DominantEye != 0) && (DominantEye != 1) && (DominantEye != 2))
		DominantEye = 0;

	ShowFPS = cIniObject.GetBool("General Settings", "ShowFPS", TRUE);
}

#ifdef OVERUNDER
HRESULT hkIDirect3DDevice9_takeScreenshot(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_takeScreenshot()");
	IDirect3DSurface9 * tempSurfOne;

	D3DSURFACE_DESC desc;
	RECT tempRectOne;
	RECT tempRectTwo;

	m_pRenderSurfBackBuffer->GetDesc(&desc);

	tempRectOne.left = 0;
	tempRectOne.top = 0;
	tempRectOne.right = desc.Width;
	tempRectOne.bottom = desc.Height;

	tempRectTwo.left = desc.Width;
	tempRectTwo.top = 0;
	tempRectTwo.right = desc.Width * 2;
	tempRectTwo.bottom = desc.Height;

	oCreateRenderTarget(pD3Ddev, desc.Width * 2, desc.Height, desc.Format, desc.MultiSampleType, desc.MultiSampleQuality, FALSE, &tempSurfOne, NULL);
	oStretchRect(pD3Ddev, pSurfLeft, NULL, tempSurfOne, &tempRectOne, D3DTEXF_NONE);
	oStretchRect(pD3Ddev, pSurfRight, NULL, tempSurfOne, &tempRectTwo, D3DTEXF_NONE);

	int number = 0;
	TCHAR screenshot[MAX_PATH];
	CreateDirectory("C:\\StereoScreenshots", NULL);
	sprintf(screenshot, "C:\\StereoScreenshots\\stereoScreenShot%.4d.jps", number);

	while ((_access(screenshot, 0)) != -1) // Check for existence
	{
		number += 1;
		sprintf(screenshot, "C:\\StereoScreenshots\\stereoScreenShot%.4d.jps", number);
	}

	HRESULT hr = D3DXSaveSurfaceToFile(screenshot, D3DXIFF_JPG, tempSurfOne, NULL, NULL);
	tempSurfOne->Release();

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}
#endif

// Credits for the stereo projection equations : http://paulbourke.net/stereographics/stereorender/
void move_view(LPDIRECT3DDEVICE9 pD3Ddev, MOVE_DIRECTION move_dir)
{
	ADD_LOG("hkIDirect3DDevice9_move_view() : move_dir = %d", move_dir);
	const float FOV = D3DXToRadian(90.0f); // radians
	const float zNear = 0.05f;//0.1f;
	const float zFar = 1000.0f;
	const float ratio = 16.0f / 9.0f; // TODO : change depending on resolution

	// Send aspect ratio to shader inverse projection calculation
	D3DXVECTOR4 c230(ratio, 0.0f, 0.0f, 0.0f);
	oSetVertexShaderConstantF(pD3Ddev, 230, (float*)&c230, 1); 

	// Projection
	const float wd2 = zNear * tan(FOV / 2);
	float ndfl = zNear / convergence;
	const float bottom = -wd2;
	const float top = wd2;
	D3DXMATRIX matTrans;
	float left;
	float right;

	if (DominantEye == 0)
	{
		if (move_dir == LEFT)
		{		
			left = -ratio * wd2 - 0.5f * separation * ndfl;
			right = ratio * wd2 - 0.5f * separation * ndfl;
			D3DXMatrixTranslation(&matTrans, separation * 0.5f, 0.0f, 0.0f);
		}
		else if (move_dir == RIGHT)
		{
			left = -ratio * wd2 + 0.5f * separation * ndfl;
			right = ratio * wd2 + 0.5f * separation * ndfl;
			D3DXMatrixTranslation(&matTrans, -separation * 0.5f, 0.0f, 0.0f);
		}
		else if (move_dir == DO_NOT_MOVE)
		{	
			left = -ratio * wd2;
			right = ratio * wd2;
		}
	}
	else if (DominantEye == 1)
	{
		if (move_dir == LEFT)
		{
			left = -ratio * wd2 - 0.5f * separation * ndfl;
			right = ratio * wd2 - 0.5f * separation * ndfl;
			D3DXMatrixTranslation(&matTrans, 0.0f, 0.0f, 0.0f);
		}
		else if (move_dir == RIGHT)
		{
			left = -ratio * wd2 + 0.5f * separation * ndfl;
			right = ratio * wd2 + 0.5f * separation * ndfl;
			D3DXMatrixTranslation(&matTrans, -separation, 0.0f, 0.0f);
		}
		else if (move_dir == DO_NOT_MOVE)
		{		
			left = -ratio * wd2;
			right = ratio * wd2;
		}
	}
	else if (DominantEye == 2)
	{
		if (move_dir == LEFT)
		{
			left = -ratio * wd2 - 0.5f * separation * ndfl;
			right = ratio * wd2 - 0.5f * separation * ndfl;
			D3DXMatrixTranslation(&matTrans, separation, 0.0f, 0.0f);
		}
		else if (move_dir == RIGHT)
		{
			left = -ratio * wd2 + 0.5f * separation * ndfl;
			right = ratio * wd2 + 0.5f * separation * ndfl;
			D3DXMatrixTranslation(&matTrans, 0.0f, 0.0f, 0.0f);
		}
		else if (move_dir == DO_NOT_MOVE) // usually HUD and effects rendered in 2D
		{		
			left = -ratio * wd2;
			right = ratio * wd2;
		}
	}

	D3DXMATRIX matProj(2 * zNear / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
		0.0f, 2 * zNear / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
		0.0f, 0.0f, (zFar + zNear) / (zFar - zNear), -2 * zFar*zNear / (zFar - zNear),
		0.0f, 0.0f, 1.0f, 0.0f);

	D3DXMatrixTranspose(&matProj, &matProj);
	matProj = matTrans * matProj;
	D3DXMatrixTranspose(&matProj, &matProj);

	oSetVertexShaderConstantF(pD3Ddev, 234, (float*)&matProj, 4);
}

void decreaseSeparation()
{
	ADD_LOG("hkIDirect3DDevice9_decreaseSeparation()");
	float temp = separation;

	temp -= 0.0025f;

	if (temp < 0.0f)
		separation = 0.0f;
	else
		separation = temp;
}

void increaseSeparation()
{
	ADD_LOG("hkIDirect3DDevice9_increaseSeparation()");

	separation += 0.0025f;
}

void decreaseConvergence()
{
	ADD_LOG("hkIDirect3DDevice9_decreaseConvergence()");

	convergence += convergence * 0.05;

	if (convergence > 1000.0f)
		convergence = 1000.0f;
}

void increaseConvergence()
{
	ADD_LOG("hkIDirect3DDevice9_increaseConvergence()");

	convergence -= convergence * 0.05;

	if (convergence < 1.0f)
		convergence = 1.0f;
}

BOOL IsDepthFormatOk(IDirect3D9 *pD3D9,
	D3DFORMAT DepthFormat,
	D3DFORMAT AdapterFormat,
	D3DFORMAT BackBufferFormat)
{
	ADD_LOG("IsDepthFormatOk()");
	// Verify that the depth format exists
	HRESULT hr = pD3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		AdapterFormat,
		D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE,
		DepthFormat);

	if (FAILED(hr))
		return hr;

	// Verify that the depth format is compatible
	hr = pD3D9->CheckDepthStencilMatch(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		AdapterFormat,
		BackBufferFormat,
		DepthFormat);

	return hr;
}

// Must be called only once. At the end, the surface must be released
void initSurfaces(LPDIRECT3DDEVICE9 pD3Ddev)
{
	ADD_LOG("hkIDirect3DDevice9_initSurfaces()");
	if (ShowFPS)
	{
		//	ADD_LOG("pD3Ddev = %d", pD3Ddev);
		if (pD3Ddev != NULL)
		{
			HDC hDC = GetDC(NULL);
			int nLogPixelsY = ::GetDeviceCaps(hDC, LOGPIXELSY);
			ReleaseDC(NULL, hDC);
			int nHeight = -g_nFontSize * nLogPixelsY / 72;
			HRESULT hr = D3DXCreateFont(pD3Ddev,            // D3D device
				nHeight,               // Height
				0,                     // Width
				FW_BOLD,               // Weight
				1,                     // MipLevels, 0 = autogen mipmaps
				FALSE,                 // Italic
				DEFAULT_CHARSET,       // CharSet
				OUT_DEFAULT_PRECIS,    // OutputPrecision
				DEFAULT_QUALITY,       // Quality
				DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
				"Arial",              // pFaceName
				&g_pFont);              // ppFont

			if (FAILED(hr)) // pour font
			{
				//	return hr;
				if (hr == D3DERR_INVALIDCALL) {
					ADD_LOG("D3DXCreateFont failed : D3DERR_INVALIDCALL");
				}
				else if (hr == D3DXERR_INVALIDDATA) {
					ADD_LOG("D3DXCreateFont failed : D3DXERR_INVALIDDATA");
				}
				else if (hr == E_OUTOFMEMORY) {
					ADD_LOG("D3DXCreateFont failed : E_OUTOFMEMORY");
				}
			}
			else
				ADD_LOG("D3DXCreateFont succeeded");
		}
	}

	HRESULT hrGetBackBuffer = oGetBackBuffer(pD3Ddev, 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pRenderSurfBackBuffer); 
	if (hrGetBackBuffer == D3D_OK) {
		ADD_LOG("hkIDirect3DDevice9_initSurfaces : hrGetBackBuffer == D3D_OK");
	}
	else if (hrGetBackBuffer == D3DERR_INVALIDCALL) {
		ADD_LOG("hkIDirect3DDevice9_initSurfaces : hrGetBackBuffer == D3DERR_INVALIDCALL");
	}
	
	ADD_LOG("hkIDirect3DDevice9_initSurfaces : after oGetBackBuffer");
	
#ifdef OVERUNDER
	D3DSURFACE_DESC desc;
	m_pRenderSurfBackBuffer->GetDesc(&desc);

	ADD_LOG("hkIDirect3DDevice9_initSurfaces : MultiSampleType = %d", desc.MultiSampleType);

	oCreateRenderTarget(pD3Ddev, desc.Width, desc.Height, desc.Format, desc.MultiSampleType, desc.MultiSampleQuality, FALSE, &pSurfLeft, NULL);
	oCreateRenderTarget(pD3Ddev, desc.Width, desc.Height, desc.Format, desc.MultiSampleType, desc.MultiSampleQuality, FALSE, &pSurfRight, NULL);

	IDirect3D9 *tempD3D9;
	oGetDirect3D(pD3Ddev, &tempD3D9);

	D3DFORMAT d3dDepthFormat = D3DFMT_D24S8;

	ADD_LOG("hkIDirect3DDevice9_initSurfaces, checking D3DFMT_D24S8");
	if (SUCCEEDED(IsDepthFormatOk(tempD3D9, D3DFMT_D24S8, desc.Format, desc.Format))) 
	{
		ADD_LOG("hkIDirect3DDevice9_initSurfaces, D3DFMT_D24S8 chosen");
		d3dDepthFormat = D3DFMT_D24S8;
	}
	else
	{
		ADD_LOG("hkIDirect3DDevice9_initSurfaces, D3DFMT_D24S8 not ok. Checking D3DFMT_D16");
		if (SUCCEEDED(IsDepthFormatOk(tempD3D9, D3DFMT_D16, desc.Format, desc.Format)))
		{
			ADD_LOG("hkIDirect3DDevice9_initSurfaces, D3DFMT_D16 chosen");
			d3dDepthFormat = D3DFMT_D16;
		}
		else
		{
			ADD_LOG("hkIDirect3DDevice9_initSurfaces, D3DFMT_D16 not ok, this this not work");
		}
	}

	tempD3D9->Release();

	HRESULT hrFirst = oCreateDepthStencilSurface(pD3Ddev, desc.Width, desc.Height, d3dDepthFormat, desc.MultiSampleType, desc.MultiSampleQuality, FALSE, &m_pRenderSurfBackBufferDepthLeft, NULL);
	if (hrFirst == D3D_OK) {
		ADD_LOG("CreateDepthStencilSurface() : hrFirst returned D3D_OK");
	}
	else if (hrFirst == D3DERR_NOTAVAILABLE) {
		ADD_LOG("CreateDepthStencilSurface() returned D3DERR_NOTAVAILABLE");
	}
	else if (hrFirst == D3DERR_INVALIDCALL) {
		ADD_LOG("CreateDepthStencilSurface() returned D3DERR_INVALIDCALL");
	}
	else if (hrFirst == D3DERR_OUTOFVIDEOMEMORY) {
		ADD_LOG("CreateDepthStencilSurface() returned D3DERR_OUTOFVIDEOMEMORY");
	}
	else if (hrFirst == E_OUTOFMEMORY) {
		ADD_LOG("CreateDepthStencilSurface() returned E_OUTOFMEMORY");
	}
	
	HRESULT hrSecond = oCreateDepthStencilSurface(pD3Ddev, desc.Width, desc.Height, d3dDepthFormat, desc.MultiSampleType, desc.MultiSampleQuality, FALSE, &m_pRenderSurfBackBufferDepthRight, NULL);
	if (hrSecond == D3D_OK) {
		ADD_LOG("CreateDepthStencilSurface() : hrSecond returned D3D_OK");
	}
	else if (hrSecond == D3DERR_NOTAVAILABLE) {
		ADD_LOG("CreateDepthStencilSurface() returned D3DERR_NOTAVAILABLE");
	}
	else if (hrSecond == D3DERR_INVALIDCALL) {
		ADD_LOG("CreateDepthStencilSurface() returned D3DERR_INVALIDCALL");
	}
	else if (hrSecond == D3DERR_OUTOFVIDEOMEMORY) {
		ADD_LOG("CreateDepthStencilSurface() returned D3DERR_OUTOFVIDEOMEMORY");
	}
	else if (hrSecond == E_OUTOFMEMORY) {
		ADD_LOG("CreateDepthStencilSurface() returned E_OUTOFMEMORY");
	}

	ADD_LOG("Calling hkIDirect3DDevice9_SetDepthStencilSurface from initSurfaces");
	hkIDirect3DDevice9_SetDepthStencilSurface(pD3Ddev, m_pRenderSurfBackBufferDepthLeft);
	oSetRenderState(pD3Ddev, D3DRS_ZENABLE, TRUE); // I think it's set by default to FALSE if EnableAutoDepthStencil == false
#endif

}

HRESULT APIENTRY CreateStereoVertexShader(LPDIRECT3DDEVICE9 pD3Ddev, CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
	ADD_LOG("CreateStereoVertexShader()");
	shaderCounter += 1;

	// Create original vertex shader
	HRESULT hr;
	hr = oCreateVertexShader(pD3Ddev, pFunction, ppShader);
	if (hr != D3D_OK) {
		return hr;
	}
	
	LPD3DXBUFFER pBufIn = NULL;
	D3DXCreateBuffer(50000, &pBufIn); // create a buffer object with a size that should be big enough for any normal sized shader
	D3DXDisassembleShader(pFunction, false, NULL, &pBufIn);
	
#ifdef LOG_VERTEXSHADERS
	ADD_LOG("====================================================================");
	ADD_LOG("Trying to add input log now (shader %d)", shaderCounter);
	ADD_LOG((char*)pBufIn->GetBufferPointer());
	ADD_LOG("log complete");
	ADD_LOG("====================================================================");
	ADD_LOG("\n");
#endif

	char* pIn; // Buffer read pointer
	pIn = (char*)pBufIn->GetBufferPointer();

	char BufOut[50000]; // Buffer to hold the stereo vertex shader code
	ZeroMemory(BufOut, sizeof(char) * 50000);

	char* pOut; // Buffer write pointer
	pOut = &BufOut[0];

	char* str1 = strstr(pIn, "dcl_position"); // Find the first "dcl_position"

	// Exit if we can't find the first "dcl_position". That should normally never happen.
	if (str1 == NULL)
	{
		ADD_LOG("CreateStereoVertexShader() : dcl_position not found");
		// Stereo vertex shader creation failed. use original shader instead
		sVS.enabled[*ppShader] = false;
		sVS.stereoVS[*ppShader] = *ppShader;
		addVSMenuItem(*ppShader); // Add to menu

		return hr;
	}
	
	// Find vertex shader version
	char* strVS;
	int vsVersion = 0;
	if ((strVS = strstr((char*)pBufIn->GetBufferPointer(), "vs_1_1")) != NULL)
	{
		vsVersion = 11; 
		strVS -= 4;
	}
	else if ((strVS = strstr((char*)pBufIn->GetBufferPointer(), "vs_2_0")) != NULL)
	{
		vsVersion = 20;
		strVS -= 4;
	}
	else if ((strVS = strstr((char*)pBufIn->GetBufferPointer(), "vs_2_x")) != NULL) // treat the same as v2.0
	{
		vsVersion = 20;
		strVS -= 4;
	}
	else if ((strVS = strstr((char*)pBufIn->GetBufferPointer(), "vs_3_0")) != NULL)
	{
		vsVersion = 30;
		strVS -= 4;
	}

	// Copy from input string to output string up to the beginning of the first dcl_position
	int nchar = str1 - strVS;
	strncat(pOut, strVS, nchar);
	pOut = pOut + nchar;
	pIn = strVS + nchar;
	
	// code different for specific vertex shader version and laser sight version follows
	#define V11_LS0		1
	#define V11_LS1		2
	#define V20_LS0		3
	#define V20_LS1		4
	#define V30_LS0		5
	#define V30_LS1		6

	int VSx_LSx = 0;
	if ((vsVersion == 11) && (LaserSight == 0))
		VSx_LSx = V11_LS0;
	else if ((vsVersion == 11) && (LaserSight == 1))
		VSx_LSx = V11_LS1;
	else if ((vsVersion == 20) && (LaserSight == 0))
		VSx_LSx = V20_LS0;
	else if ((vsVersion == 20) && (LaserSight == 1))
		VSx_LSx = V20_LS1;
	else if ((vsVersion == 30) && (LaserSight == 0))
		VSx_LSx = V30_LS0;
	else if ((vsVersion == 30) && (LaserSight == 1))
		VSx_LSx = V30_LS1;

	#define V11		1
	#define V20		2
	#define V30		3

	int VSx = 0;
	if (vsVersion == 11)
		VSx = V11;
	else if (vsVersion == 20)
		VSx = V20;
	else if (vsVersion == 30)
		VSx = V30;

	// Append definition
	switch (VSx_LSx)
	{
	case V11_LS0:
		VS_appendDef_V11_LS0(&pOut);
		break;
	case V11_LS1:
		VS_appendDef_V11_LS1(&pOut);
		break;
	case V20_LS0:
		VS_appendDef_V20_LS0(&pOut);
		break;
	case V20_LS1:
		VS_appendDef_V20_LS1(&pOut);
		break;
	case V30_LS0:
		VS_appendDef_V30_LS0(&pOut);
		break;
	case V30_LS1:
		VS_appendDef_V30_LS1(&pOut);
		break;
	}

	// Replace output registers
	int countOR = 0;
	char strOR[5];
	
	switch (VSx)
	{
	case V11:
		VS_replaceOR_V11(&pIn, &countOR, strOR, &pOut);
		break;
	case V20:
		VS_replaceOR_V20(&pIn, &countOR, strOR, &pOut);
		break;
	case V30:
		VS_replaceOR_V30(&pIn, &countOR, strOR, &pOut);
		break;
	}

	// Add new shader code
	// TODO : verify return code. It's pointless to continue to modify the shader if the functions below fail
	if (countOR == 1)
	{
		switch (VSx_LSx)
		{
		case V11_LS0:
			VS_addcode_ORe1_VS11_LS0(&pIn, strOR, &pOut);
			break;
		case V11_LS1:
			VS_addcode_ORe1_VS11_LS1(&pIn, strOR, &pOut);
			break;
		case V20_LS0:
			VS_addcode_ORe1_VS20_LS0(&pIn, strOR, &pOut);
			break;
		case V20_LS1:
			VS_addcode_ORe1_VS20_LS1(&pIn, strOR, &pOut);
			break;
		case V30_LS0:
			VS_addcode_ORe1_VS30_LS0(&pIn, strOR, &pOut);
			break;
		case V30_LS1:
			VS_addcode_ORe1_VS30_LS1(&pIn, strOR, &pOut);
			break;
		}
	}
	else if (countOR > 1)
	{
		switch (VSx_LSx)
		{
		case V11_LS0:
			VS_addcode_ORgt1_VS11_LS0(&pIn, strOR, &pOut);
			break;
		case V11_LS1:
			VS_addcode_ORgt1_VS11_LS1(&pIn, strOR, &pOut);
			break;
		case V20_LS0:
			VS_addcode_ORgt1_VS20_LS0(&pIn, strOR, &pOut);
			break;
		case V20_LS1:
			VS_addcode_ORgt1_VS20_LS1(&pIn, strOR, &pOut);
			break;
		case V30_LS0:
			VS_addcode_ORgt1_VS30_LS0(&pIn, strOR, &pOut);
			break;
		case V30_LS1:
			VS_addcode_ORgt1_VS30_LS1(&pIn, strOR, &pOut);
			break;
		}
	}
	
	if (countOR == 0) // Should never happen
	{
		// For some unknow reason, no output registers were found
		// log the error and use the original vertex shader
		ADD_LOG("CreateStereoVertexShader() : zero output registers found");
		// Stereo vertex shader creation failed. use original shader instead
		sVS.enabled[*ppShader] = false;
		sVS.stereoVS[*ppShader] = *ppShader;
		addVSMenuItem(*ppShader); // Add to menu
		return hr; // already created at the beginning of the function
	}

#ifdef LOG_VERTEXSHADERS
	ADD_LOG("\n");
	ADD_LOG("==== LOG_VERTEXSHADERS start ====");
	ADD_LOG(&BufOut[0]);
	ADD_LOG("==== LOG_VERTEXSHADERS end ====");
	ADD_LOG("\n\n\n");
#endif
	
	// Assemble and create the new shader
	LPD3DXBUFFER pNewFunction;
	LPD3DXBUFFER ppErrorMsgs;
	
	strcpy(BufPast, &BufOut[0]);
	hr = D3DXAssembleShader(&BufOut[0], strlen(&BufOut[0]), NULL, NULL, 0, &pNewFunction, &ppErrorMsgs); 
	// This way of modifying the shader works if the game uses D3DXCompileShaderFromFile or D3DXAssembleShader
	// It does not work if the game uses D3DXCreateEffectFromFile with a .fx file

	if (hr == D3D_OK)
	{
		ADD_LOG("CreateStereoVertexShader() : D3DXAssembleShader succeeded");
		// Create stereo vertex shader
		IDirect3DVertexShader9* pVertexShaderTemp;
		hr = oCreateVertexShader(pD3Ddev, (DWORD*)pNewFunction->GetBufferPointer(), &pVertexShaderTemp);
		if (hr == D3D_OK) { // CreateVertexShader succeeded
			sVS.enabled[*ppShader] = true; // default value here
			sVS.stereoVS[*ppShader] = pVertexShaderTemp;
			addVSMenuItem(*ppShader); // Add to menu
			ADD_LOG("CreateStereoVertexShader() : vertex shader created, original = %p, stereo shader = %p", *ppShader, pVertexShaderTemp);
			// TODO : do hashing of original shader here and store in pVertexShaderHash
		}
		else // CreateVertexShader failed
		{
			// Stereo vertex shader creation failed. use original shader instead
			sVS.enabled[*ppShader] = false;
			sVS.stereoVS[*ppShader] = *ppShader;
			addVSMenuItem(*ppShader); // Add to menu
			ADD_LOG("CreateStereoVertexShader() : vertex shader creation failed, original = %p", *ppShader);
		}
	}
	else
	{
		// There's most likely an error in the generated shader code. Look at the logs to figure it out.
		// Stereo vertex shader creation failed. use original shader instead
		sVS.enabled[*ppShader] = false;
		sVS.stereoVS[*ppShader] = *ppShader;
		addVSMenuItem(*ppShader); // Add to menu
		ADD_LOG("CreateStereoVertexShader() : D3DXAssembleShader failed"); 
		switch (hr)
		{
		case D3DERR_INVALIDCALL:
			ADD_LOG("CreateStereoVertexShader() : D3DERR_INVALIDCALL");
			break;
		case D3DXERR_INVALIDDATA:
			ADD_LOG("CreateStereoVertexShader() : D3DXERR_INVALIDDATA");
			break;
		case E_OUTOFMEMORY:
			ADD_LOG("CreateStereoVertexShader() : E_OUTOFMEMORY");
			break;
		default:
			ADD_LOG("CreateStereoVertexShader() : unknown error");
		}
		ADD_LOG("CreateStereoVertexShader() : Error log says : \n");
		ADD_LOG((char*)ppErrorMsgs->GetBufferPointer());
	}
	_SAFE_RELEASE(pNewFunction);

	pBufIn->Release();

	return hr;
}

// CreateStereoVertexShader helper functions

// append new registers definitions lines (for different vertex shader versions and laser sight options)
// No attempts is made to verify if the registers are already used. We just use the high numbered registers hoping they are free.
HRESULT APIENTRY VS_appendDef_V11_LS0(char** ppOut)
{
	ADD_LOG("VS_appendDef_V11_LS0()");
	const char* strDef =
		"def c231, 0, 1, 0, 0\n"
		"    def c232, 0, 0, 0, 1\n"
		"    def c233, 0, 0, -9.999875, 10\n"
		"    def c239, 2, 2, 0.000001, 0.5\n"
		"    ";

	UINT strDefSize = (UINT)strlen(strDef);
	strncat(*ppOut, strDef, strDefSize);
	*ppOut = *ppOut + strDefSize;

	return D3D_OK;
}

HRESULT APIENTRY VS_appendDef_V11_LS1(char** ppOut)
{
	ADD_LOG("VS_appendDef_V11_LS1()");
	const char* strDef =
		"def c231, 0, 1, 0, 0\n"
		"    def c232, 0, 0, 0, 1\n"
		"    def c233, 0, 0, -9.999875, 10\n"
		"    def c238, 90, 1, 0, 0\n" // lasersight type1
		"    def c239, 2, 2, 0.000001, 0.5\n"
		"    ";

	UINT strDefSize = (UINT)strlen(strDef);
	strncat(*ppOut, strDef, strDefSize);
	*ppOut = *ppOut + strDefSize;

	return D3D_OK;
}

//VS_appendDef_V11_LS2 // not implemented

HRESULT APIENTRY VS_appendDef_V20_LS0(char** ppOut)
{
	ADD_LOG("VS_appendDef_V20_LS0()");
	const char* strDef =
		"def c231, 0, 1, 0, 0\n"
		"    def c232, 0, 0, 0, 1\n"
		"    def c233, 0, 0, -9.999875, 10\n"
		"    def c239, 2, 2, 0.000001, 0.5\n"
		"    ";

	UINT strDefSize = (UINT)strlen(strDef);
	strncat(*ppOut, strDef, strDefSize);
	*ppOut = *ppOut + strDefSize;

	return D3D_OK;
}

HRESULT APIENTRY VS_appendDef_V20_LS1(char** ppOut)
{
	ADD_LOG("VS_appendDef_V20_LS1()");
	const char* strDef =
		"def c231, 0, 1, 0, 0\n"
		"    def c232, 0, 0, 0, 1\n"
		"    def c233, 0, 0, -9.999875, 10\n"
		"    def c238, 90, 1, 0, 0\n" // lasersight type1
		"    def c239, 2, 2, 0.000001, 0.5\n"
		"    ";

	UINT strDefSize = (UINT)strlen(strDef);
	strncat(*ppOut, strDef, strDefSize);
	*ppOut = *ppOut + strDefSize;

	return D3D_OK;
}

//VS_appendDef_V20_LS2 // not implemented

HRESULT APIENTRY VS_appendDef_V30_LS0(char** ppOut)
{
	ADD_LOG("VS_appendDef_V30_LS0()");
	const char* strDef =
		"def c231, 0, 1, 0, 0\n"
		"    def c232, 0, 0, 0, 1\n"
		"    def c233, 0, 0, -9.999875, 10\n"
		"    def c239, 2, 2, 0.000001, 0\n"
		"    ";

	UINT strDefSize = (UINT)strlen(strDef);
	strncat(*ppOut, strDef, strDefSize);
	*ppOut = *ppOut + strDefSize;

	return D3D_OK;
}

HRESULT APIENTRY VS_appendDef_V30_LS1(char** ppOut)
{
	ADD_LOG("VS_appendDef_V30_LS1()");
	const char* strDef =
		"def c230, 1.33333333, 0, 0, 0\n"
		"    def c231, 0, 1, 0, 0\n"
		"    def c232, 0, 0, 0, 1\n"
		"    def c233, 0, 0, -9.999875, 10\n"
		"    def c238, 90, 1, 0, 0\n"
		"    def c239, 2, 2, 0.000001, 0\n"
		"    dcl_texcoord9 o9\n" // lasersight
		"    ";

	UINT strDefSize = (UINT)strlen(strDef);
	strncat(*ppOut, strDef, strDefSize);
	*ppOut = *ppOut + strDefSize;

	return D3D_OK;
}

//VS_appendDef_V30_LS2 // not implemented

// Replace ouput registers by r10 or r30 register depending on vertex shader version
HRESULT APIENTRY VS_replaceOR_V11(char** ppIn, int* pcountOR, char* pOR, char** ppOut)
{
	ADD_LOG("VS_replaceOR_V11()");
	char* pFindOR = NULL;
	char * posOR[20];

	// Find out how many times the output register is used in the shader (not counting the declaration)
	//int countOR = 0;
	pFindOR = strstr(*ppIn, "oPos");
	while (pFindOR != NULL)
	{
		posOR[*pcountOR] = pFindOR;
		*pcountOR += 1;
		pFindOR = strstr(pFindOR + 1, "oPos");
	}
	pOR[0] = 'o';
	pOR[1] = 'P';
	pOR[2] = 'o';
	pOR[3] = 's';
	pOR[4] = '\0';

	if (*pcountOR > 1)
	{
		// replace all output registers by r10
		// It will be necessary to add a mov instruction at the end
		int i = 0;
		while (i < *pcountOR)
		{
			int nchar = posOR[i] - *ppIn;
			strncat(*ppOut, *ppIn, nchar);
			*ppOut = *ppOut + nchar;
			*ppIn = *ppIn + nchar;
			strncat(*ppOut, "r10", 3);
			*ppOut = *ppOut + 3;

			int ORSize = (UINT)strlen(&pOR[0]);
			*ppIn += ORSize;

			i++;
		}

		char* strFindEOL = strstr(*ppIn, "\n") + 1; // Find output register
		int nchar = strFindEOL - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		*ppOut = *ppOut + nchar;
		*ppIn = *ppIn + nchar;
	}

	return D3D_OK;
}

HRESULT APIENTRY VS_replaceOR_V20(char** ppIn, int* pcountOR, char* pOR, char** ppOut)
{
	ADD_LOG("VS_replaceOR_V20()");
	char* pFindOR = NULL;
	char * posOR[20];

	// Find out how many times the output register is used in the shader (not counting the declaration)
	//int countOR = 0;
	pFindOR = strstr(*ppIn, "oPos");
	while (pFindOR != NULL)
	{
		posOR[*pcountOR] = pFindOR;
		*pcountOR += 1;
		pFindOR = strstr(pFindOR + 1, "oPos");
	}
	pOR[0] = 'o';
	pOR[1] = 'P';
	pOR[2] = 'o';
	pOR[3] = 's';
	pOR[4] = '\0';

	if (*pcountOR > 1)
	{
		// replace all output registers by r10
		// It will be necessary to add a mov instruction at the end
		int i = 0;
		while (i < *pcountOR)
		{
			int nchar = posOR[i] - *ppIn;
			strncat(*ppOut, *ppIn, nchar);
			*ppOut = *ppOut + nchar;
			*ppIn = *ppIn + nchar;
			strncat(*ppOut, "r10", 3);
			*ppOut = *ppOut + 3;

			int ORSize = (UINT)strlen(&pOR[0]);
			*ppIn += ORSize;

			i++;
		}

		char* strFindEOL = strstr(*ppIn, "\n") + 1; // Find output register
		int nchar = strFindEOL - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		*ppOut = *ppOut + nchar;
		*ppIn = *ppIn + nchar;
	}

	return D3D_OK;
}

HRESULT APIENTRY VS_replaceOR_V30(char** ppIn, int* pcountOR, char* pOR, char** ppOut)
{
	ADD_LOG("VS_replaceOR_V30()");
	char* pFindOR = NULL;
	char * posOR[20];

	// Find "dcl_position o" and the number following
	pFindOR = strstr(*ppIn, "dcl_position o"); // Find output register

	// TODO : add error checking here
	if (pFindOR != NULL)
	{
		pOR[0] = 'o';
		pOR[1] = pFindOR[14];

		int i = 1;
		while (pFindOR[14 + i] != '\n') // maybe "\n"...
		{
			pOR[1 + i] = pFindOR[14 + i]; // End up with a buffer containing "o1" or "o2" etc
			i += 1;
		}
		pOR[1 + i] = '\0'; // Append NULL character
	}

	// Find out how many times the output register is used in the shader (not counting the declaration)
	pFindOR = strstr(pFindOR + 14, &pOR[0]);
	while (pFindOR != NULL)
	{
		posOR[*pcountOR] = pFindOR;
		*pcountOR += 1;
		pFindOR = strstr(pFindOR + 14, &pOR[0]);
	}

	if (*pcountOR > 1)
	{
		// replace all output registers by r30
		// It will be necessary to add a mov instruction at the end
		int i = 0;
		while (i < *pcountOR)
		{
			int nchar = posOR[i] - *ppIn;
			strncat(*ppOut, *ppIn, nchar);
			*ppOut = *ppOut + nchar;
			*ppIn = *ppIn + nchar;
			strncat(*ppOut, "r30", 3);
			*ppOut = *ppOut + 3;

			int ORSize = (UINT)strlen(&pOR[0]);
			*ppIn += ORSize;

			i++;
		}

		char* strFindEOL = strstr(*ppIn, "\n") + 1; // Find output register
		int nchar = strFindEOL - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		*ppOut = *ppOut + nchar;
		*ppIn = *ppIn + nchar;
	}

	return D3D_OK;
}

HRESULT APIENTRY VS_addcode_common(char** ppIn, int cond, char* pFindOR, char** ppOut, char* strSource, char* strSource2, char* strSource3)
{
	ADD_LOG("VS_addcode_common() : cond = %d", cond);

	#define COND_MOV	1
	#define COND_ADD	2
	#define COND_MAD	3
	#define COND_m4x4	4

	// To debug how it's going up to here
	//ADD_LOG("==== LOG_VERTEXSHADERS start (in VS_addcode_common) ====");
	//ADD_LOG(&BufOut[0]);
	//ADD_LOG("==== LOG_VERTEXSHADERS end (in VS_addcode_common) ====");

	if (cond == COND_MOV)
	{
		char* strComma = strstr(pFindOR, ", "); // Find the comma
		int i = 0;
		while (strComma[2 + i] != '\n')
		{
			strSource[i] = strComma[2 + i]; // buffer containing the source register of the mov instruction
			i += 1;
		}
		strSource[i] = '\0'; // Append NULL character
	}

	else if (cond == COND_ADD)
	{
		// Not sure if this is always good. Could maybe use the other input parameter. 
		// Note : I don't remember what I meant by that previous comment
		char* strComma = strstr(pFindOR, ", "); // Find the comma
		int i = 0;
		while (strComma[2 + i] != ',')
		{
			strSource[i] = strComma[2 + i]; // buffer containing the source register of the add instruction
			i += 1;
		}
		strSource[i] = '\0'; // Append NULL character

		strComma = strstr(strComma + 1, ", "); // Find the comma
		i = 0;
		while (strComma[2 + i] != '\n')
		{
			strSource2[i] = strComma[2 + i]; // buffer containing the source register of the add instruction
			i += 1;
		}
		strSource2[i] = '\0'; // Append NULL character
	}
	else if (cond == COND_MAD)
	{
		// We cannot know what to do when we encounter a mad instruction so we replace the output register
		// with a temp register and adapt the code to work with that

		// Not sure if this is always good. Could maybe use the other input parameter. 
		char* strComma = strstr(pFindOR, ", "); // Find the comma
		int i = 0;
		while (strComma[2 + i] != ',')
		{
			strSource[i] = strComma[2 + i]; // buffer containing the source register of the mad instruction
			i += 1;
		}
		strSource[i] = '\0'; // Append NULL character

		strComma = strstr(strComma + 1, ", "); // Find the comma
		i = 0;
		while (strComma[2 + i] != ',')
		{
			strSource2[i] = strComma[2 + i]; // buffer containing the source register of the mad instruction
			i += 1;
		}
		strSource2[i] = '\0'; // Append NULL character

		strComma = strstr(strComma + 1, ", "); // Find the comma
		i = 0;
		while (strComma[2 + i] != '\n')
		{
			strSource3[i] = strComma[2 + i]; // buffer containing the source register of the mad instruction
			i += 1;
		}
		strSource3[i] = '\0'; // Append NULL character

	}
	else if (cond == COND_m4x4)
	{
		// m4x4 and four dp4 are not perfectly equivalent replacement.
		// If we mix them together, we risk getting z fighting issues
		// Because of that, we replace m4x4 with four dp4
		//
		// m4x4 oPos, v0 , c0
		//
		// can be replaced by :
		//
		// dp4 oPos.x , v0 , c0
		// dp4 oPos.y , v0 , c1
		// dp4 oPos.z , v0 , c2
		// dp4 oPos.w , v0 , c3

		// Not sure if this is always good. Could maybe use the other input parameter. 
		char* strComma = strstr(pFindOR, ", "); // Find the comma
		int i = 0;
		while (strComma[2 + i] != ',')
		{
			strSource[i] = strComma[2 + i]; // buffer containing the source register of the m4x4 instruction
			i += 1;
		}
		strSource[i] = '\0'; // Append NULL character

		strComma = strstr(strComma + 1, ", "); // Find the comma
		i = 0;
		while (strComma[2 + i] != '\n')
		{
			strSource2[i] = strComma[2 + i]; // buffer containing the source register of the m4x4 instruction
			i += 1;
		}
		strSource2[i] = '\0'; // Append NULL character

		pFindOR -= 1; // Because it's an instruction with 4 characters instead of three
	}
	// Not totally sure if appending a '\0' is necessary here
	
	if ((cond == COND_MOV) || (cond == COND_ADD) || (cond == COND_MAD) || (cond == COND_m4x4))
	{
		// Write in output buffer until the line before the mov instruction on the output register
		int nchar = pFindOR - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		*ppOut = *ppOut + nchar;
		*ppIn = *ppIn + nchar;
	}

	return D3D_OK;
}

// Add new code to vertex shader that uses 1 output register (output register equal to 1)
HRESULT APIENTRY VS_addcode_ORe1_VS11_LS0(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORe1_VS11_LS0()");
	
	char* pFindOR = NULL;
	pFindOR = strstr(*ppIn, "oPos");
	pFindOR -= 4;
	
	char strSource[9];
	char strSource2[9];
	char strSource3[9];

	#define COND_MOV	1
	#define COND_ADD	2
	#define COND_MAD	3
	#define COND_m4x4	4

	int cond = 0; // condition that decides how to proceed
	if ((pFindOR[0] == 'm') && (pFindOR[1] == 'o') && (pFindOR[2] == 'v'))
		cond = COND_MOV;
	else if ((pFindOR[0] == 'a') && (pFindOR[1] == 'd') && (pFindOR[2] == 'd'))
		cond = COND_ADD;
	else if ((pFindOR[0] == 'm') && (pFindOR[1] == 'a') && (pFindOR[2] == 'd'))
		cond = COND_MAD;
	else if ((pFindOR[0] == '4') && (pFindOR[1] == 'x') && (pFindOR[2] == '4'))
		cond = COND_m4x4;
		
	if (cond == 0)
	{
		ADD_LOG("VS_addcode_ORe1_VS11_LS0() : unsupported condition")
		return D3DERR_INVALIDCALL;
	}

	VS_addcode_common(ppIn, cond, pFindOR, ppOut, strSource, strSource2, strSource3);
	

	// Insert my assembly code
	char strAsmCode[700]; // Too big but better than too small

	if (cond == COND_MOV) // mov
	{
		sprintf(&strAsmCode[0],
			"    dp4 r11.x, %s, c230\n"			// Inverse Projection
			"    dp4 r11.y, %s, c231\n"
			"    dp4 r11.z, %s, c232\n"
			"    dp4 r11.w, %s, c233\n"

			"    dp4 r10.x, r11, c234\n"		// Projection
			"    dp4 r10.y, r11, c235\n"
			"    dp4 r10.z, r11, c236\n"
			"    dp4 r10.w, r11, c237\n"

			"    mov %s, r10\n"
			"\0",
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strOR[0]);
	}
	else if (cond == COND_ADD) // add
	{
		sprintf(&strAsmCode[0],
			"add %s, %s, %s\n"

			"    dp4 r11.x, %s, c230\n"			// Inverse Projection
			"    dp4 r11.y, %s, c231\n"
			"    dp4 r11.z, %s, c232\n"
			"    dp4 r11.w, %s, c233\n"
			"    dp4 r10.x, r11, c234\n"		// Projection
			"    dp4 r10.y, r11, c235\n"
			"    dp4 r10.z, r11, c236\n"
			"    dp4 r10.w, r11, c237\n"

			"    mov %s, r10\n"
			"    \0",
			&strSource[0], &strSource[0], &strSource2[0],
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strOR[0]);
	}
	else if (cond == COND_MAD)
	{
		sprintf(&strAsmCode[0],
			"mad r10, %s, %s, %s\n"

			"    dp4 r11.x, r10, c230\n"		// Inverse Projection
			"    dp4 r11.y, r10, c231\n"
			"    dp4 r11.z, r10, c232\n"
			"    dp4 r11.w, r10, c233\n"
			"    dp4 r8.x, r11, c234\n"			// Projection
			"    dp4 r8.y, r11, c235\n"
			"    dp4 r8.z, r11, c236\n"
			"    dp4 r8.w, r11, c237\n"

			"    mov %s, r8\n"
			"    \0",
			&strSource[0], &strSource2[0], &strSource3[0],
			&strOR[0]
			);
	}
	else if (cond == COND_m4x4)
	{
		sprintf(&strAsmCode[0],
			"dp4 r10.x, %s, %s\n"			// What the game would have done, r10.x is not an error
			"    dp4 r10.y, %s, %s\n"
			"    dp4 r10.z, %s, %s\n"
			"    dp4 r10.w, %s, %s\n"

			"    dp4 r11.x, r10, c230\n"	// Inverse Projection
			"    dp4 r11.y, r10, c231\n"
			"    dp4 r11.z, r10, c232\n"
			"    dp4 r11.w, r10, c233\n"

			"    dp4 r8.x, r11, c234\n"		// Projection
			"    dp4 r8.y, r11, c235\n"
			"    dp4 r8.z, r11, c236\n"
			"    dp4 r8.w, r11, c237\n"

			"    mov %s, r8\n"
			"    \0",
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strOR[0]);
	}

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	if (cond == 1) // mov
	{ // Move past the next line containing the mov instruction
		*ppIn = strstr(*ppIn, "\n") + 1;
	}
	else if (cond == 2) // add
	{ // Move past the next line containing the add instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == 3) // mad
	{ // Move past the next line containing the mad instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == 4) // m4x4
	{ // Move past the next line containing the m4x4 instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}

	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

HRESULT APIENTRY VS_addcode_ORe1_VS11_LS1(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORe1_VS11_LS1()");

	char* pFindOR = NULL;
	pFindOR = strstr(*ppIn, "oPos");
	pFindOR -= 4;
	
	char strSource[9];
	char strSource2[9];
	char strSource3[9];

	#define COND_MOV	1
	#define COND_ADD	2
	#define COND_MAD	3
	#define COND_m4x4	4

	int cond = 0; // condition that decides how to proceed
	if ((pFindOR[0] == 'm') && (pFindOR[1] == 'o') && (pFindOR[2] == 'v'))
		cond = COND_MOV;
	else if ((pFindOR[0] == 'a') && (pFindOR[1] == 'd') && (pFindOR[2] == 'd'))
		cond = COND_ADD;
	else if ((pFindOR[0] == 'm') && (pFindOR[1] == 'a') && (pFindOR[2] == 'd'))
		cond = COND_MAD;
	else if ((pFindOR[0] == '4') && (pFindOR[1] == 'x') && (pFindOR[2] == '4'))
		cond = COND_m4x4;

	if (cond == 0)
	{
		ADD_LOG("VS_addcode_ORe1_VS11_LS1() : unsupported condition")
		return D3DERR_INVALIDCALL;
	}

	VS_addcode_common(ppIn, cond, pFindOR, ppOut, strSource, strSource2, strSource3);

	// Insert assembly code
	char strAsmCode[700]; // Too big but better than too small

	if (cond == COND_MOV)
	{
		sprintf(&strAsmCode[0],
			"    abs r11, %s\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n" // 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    sub r11.x, c230.y, r11.x\n"
			"    sub r11.y, c230.y, r11.y\n"
			"    mul r11, r11, r11\n"
			"    add r11.x, r11.x, r11.y\n"
			"    rsq r11.x, r11.x\n"
			"    sge r11.x, r11.x, c238.x\n"
			"    slt r11.y, r11.x, c238.x\n"
			"    mov oT7.xy, r11.xy\n"

			"dp4 r11.x, %s, c230\n"				// Inverse Projection
			"    dp4 r11.y, %s, c231\n"
			"    dp4 r11.z, %s, c232\n"
			"    dp4 r11.w, %s, c233\n"

			"    dp4 r10.x, r11, c234\n"		// Projection
			"    dp4 r10.y, r11, c235\n"
			"    dp4 r10.z, r11, c236\n"
			"    dp4 r10.w, r11, c237\n"

			"    mul r11, %s, r9.xxxx\n"		// fix
			"    mad %s, r10, r9.yyyy, r11\n"	// move

			"\0",
			&strSource[0],
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strSource[0],
			&strOR[0]);
	}
	else if (cond == COND_ADD)
	{
		sprintf(&strAsmCode[0],
			"add %s, %s, %s\n"

			"    abs r11, %s\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n" // 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    sub r11.x, c230.y, r11.x\n"
			"    sub r11.y, c230.y, r11.y\n"
			"    mul r11, r11, r11\n"
			"    add r11.x, r11.x, r11.y\n"
			"    rsq r11.x, r11.x\n"
			"    sge r11.x, r11.x, c238.x\n"
			"    slt r11.y, r11.x, c238.x\n"
			"    mov oT7.xy, r11.xy\n"

			"    dp4 r11.x, %s, c230\n"			// Inverse Projection
			"    dp4 r11.y, %s, c231\n"
			"    dp4 r11.z, %s, c232\n"
			"    dp4 r11.w, %s, c233\n"
			"    dp4 r10.x, r11, c234\n"		// Projection
			"    dp4 r10.y, r11, c235\n"
			"    dp4 r10.z, r11, c236\n"
			"    dp4 r10.w, r11, c237\n"

			"    mul r11, %s, r9.xxxx\n"		// fix
			"    mad %s, r10, r9.yyyy, r11\n"	// move

			"    \0",
			&strSource[0], &strSource[0], &strSource2[0],
			&strSource[0], // Lasersight
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strSource[0],
			&strOR[0]);
	}
	else if (cond == COND_MAD)
	{
		sprintf(&strAsmCode[0],
			"mad r10, %s, %s, %s\n"

			"    abs r11, r10\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n"	// 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    sub r11.x, c230.y, r10.x\n"
			"    sub r11.y, c230.y, r10.y\n"
			"    mul r11, r11, r11\n"
			"    add r11.x, r11.x, r11.y\n"
			"    rsq r11.x, r11.x\n"
			"    sge r11.x, r11.x, c238.x\n"
			"    slt r11.y, r11.x, c238.x\n"
			"    mov oT7.xy, r11.xy\n"

			"    dp4 r11.x, r10, c230\n"			// Inverse Projection
			"    dp4 r11.y, r10, c231\n"
			"    dp4 r11.z, r10, c232\n"
			"    dp4 r11.w, r10, c233\n"
			"    dp4 r8.x, r11, c234\n"				// Projection
			"    dp4 r8.y, r11, c235\n"
			"    dp4 r8.z, r11, c236\n"
			"    dp4 r8.w, r11, c237\n"

			"    mul r11, r10, r9.xxxx\n"			// fix
			"    mad %s, r8, r9.yyyy, r11\n"		// move

			"    \0",
			&strSource[0], &strSource2[0], &strSource3[0],
			&strOR[0]
			);
	}
	else if (cond == COND_m4x4)
	{
		// TODO : one parameter missing. I wrote that code 8 years ago and I don't remember what it does and what the missing parameter should be
		sprintf(&strAsmCode[0],
			"dp4 r10.x, %s, %s\n"		// What the game would have done, r10.x not an error
			"    dp4 r10.y, %s, %s\n"
			"    dp4 r10.z, %s, %s\n"
			"    dp4 r10.w, %s, %s\n"

			"    sub r11.x, c230.y, r10.x\n"
			"    sub r11.y, c230.y, r10.y\n"
			"    mul r11, r11, r11\n"
			"    add r11.x, r11.x, r11.y\n"
			"    rsq r11.x, r11.x\n"
			"    sge r11.x, r11.x, c238.x\n"
			"    slt r11.y, r11.x, c238.x\n"
			"    mov oT7.xy, r11.xy\n"

			"    abs r11, %s\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n"	// 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    dp4 r11.x, r10, c230\n"			// Inverse Projection
			"    dp4 r11.y, r10, c231\n"
			"    dp4 r11.z, r10, c232\n"
			"    dp4 r11.w, r10, c233\n"

			"    dp4 r8.x, r11, c234\n"				// Projection
			"    dp4 r8.y, r11, c235\n"
			"    dp4 r8.z, r11, c236\n"
			"    dp4 r8.w, r11, c237\n"

			"    mul r11, r10, r9.xxxx\n"			// fix
			"    mad %s, r8, r9.yyyy, r11\n"		// move

			"    \0",
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strOR[0]);
	}

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	if (cond == 1) // mov
	{ // Move past the next line containing the mov instruction
		*ppIn = strstr(*ppIn, "\n") + 1;
	}
	else if (cond == 2) // add
	{ // Move past the next line containing the add instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == 3) // mad
	{ // Move past the next line containing the mad instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == 4) // m4x4
	{ // Move past the next line containing the m4x4 instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}

	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

//VS_addcode_ORe1_VS11_LS2 // not implemented

HRESULT APIENTRY VS_addcode_ORe1_VS20_LS0(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORe1_VS20_LS0()");

	char* pFindOR = NULL;
	pFindOR = strstr(*ppIn, "oPos");
	pFindOR -= 4;
	
	char strSource[9];
	char strSource2[9];
	char strSource3[9];

	#define COND_MOV	1
	#define COND_ADD	2
	#define COND_MAD	3
	#define COND_m4x4	4

	int cond = 0; // condition that decides how to proceed
	if ((pFindOR[0] == 'm') && (pFindOR[1] == 'o') && (pFindOR[2] == 'v'))
		cond = COND_MOV;
	else if ((pFindOR[0] == 'a') && (pFindOR[1] == 'd') && (pFindOR[2] == 'd'))
		cond = COND_ADD;
	else if ((pFindOR[0] == 'm') && (pFindOR[1] == 'a') && (pFindOR[2] == 'd'))
		cond = COND_MAD;
	else if ((pFindOR[0] == '4') && (pFindOR[1] == 'x') && (pFindOR[2] == '4'))
		cond = COND_m4x4;

	if (cond == 0)
	{
		ADD_LOG("VS_addcode_ORe1_VS20_LS0() : unsupported condition")
		return D3DERR_INVALIDCALL;
	}

	VS_addcode_common(ppIn, cond, pFindOR, ppOut, strSource, strSource2, strSource3);

	// Insert my assembly code
	char strAsmCode[700]; // Too big but better than too small

	if (cond == COND_MOV)
	{
		sprintf(&strAsmCode[0],
			"    abs r11, %s\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n" // 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    dp4 r11.x, %s, c230\n"			// Inverse Projection
			"    dp4 r11.y, %s, c231\n"
			"    dp4 r11.z, %s, c232\n"
			"    dp4 r11.w, %s, c233\n"

			"    dp4 r10.x, r11, c234\n"		// Projection
			"    dp4 r10.y, r11, c235\n"
			"    dp4 r10.z, r11, c236\n"
			"    dp4 r10.w, r11, c237\n"

			"    mul r11, %s, r9.xxxx\n"		// fix
			"    mad %s, r10, r9.yyyy, r11\n"	// move

			"\0",
			&strSource[0],
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strSource[0],
			&strOR[0]);
	}
	else if (cond == COND_ADD)
	{
		sprintf(&strAsmCode[0],
			"add %s, %s, %s\n"

			"    abs r11, %s\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n"	// 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    dp4 r11.x, %s, c230\n"				// Inverse Projection
			"    dp4 r11.y, %s, c231\n"
			"    dp4 r11.z, %s, c232\n"
			"    dp4 r11.w, %s, c233\n"
			"    dp4 r10.x, r11, c234\n"			// Projection
			"    dp4 r10.y, r11, c235\n"
			"    dp4 r10.z, r11, c236\n"
			"    dp4 r10.w, r11, c237\n"

			"    mul r11, %s, r9.xxxx\n"			// fix
			"    mad %s, r10, r9.yyyy, r11\n"		// move

			"    \0",
			&strSource[0], &strSource[0], &strSource2[0],
			&strSource[0], // Lasersight
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strSource[0],
			&strOR[0]);
	}
	else if (cond == COND_MAD)
	{
		sprintf(&strAsmCode[0],
			"mad r10, %s, %s, %s\n"

			"    abs r11, r10\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n"	// 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    dp4 r11.x, r10, c230\n"			// Inverse Projection
			"    dp4 r11.y, r10, c231\n"
			"    dp4 r11.z, r10, c232\n"
			"    dp4 r11.w, r10, c233\n"
			"    dp4 r8.x, r11, c234\n"				// Projection
			"    dp4 r8.y, r11, c235\n"
			"    dp4 r8.z, r11, c236\n"
			"    dp4 r8.w, r11, c237\n"

			"    mul r11, r10, r9.xxxx\n"			// fix
			"    mad %s, r8, r9.yyyy, r11\n"		// move

			"    \0",
			&strSource[0], &strSource2[0], &strSource3[0],
			&strOR[0]
			);
	}
	else if (cond == COND_m4x4)
	{
		sprintf(&strAsmCode[0],
			"dp4 r10.x, %s, %s\n"		// What the game would have done, r10.x not an error
			"    dp4 r10.y, %s, %s\n"
			"    dp4 r10.z, %s, %s\n"
			"    dp4 r10.w, %s, %s\n"

			"    abs r11, r10\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n"	// 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    dp4 r11.x, r10, c230\n"			// Inverse Projection
			"    dp4 r11.y, r10, c231\n"
			"    dp4 r11.z, r10, c232\n"
			"    dp4 r11.w, r10, c233\n"

			"    dp4 r8.x, r11, c234\n"				// Projection
			"    dp4 r8.y, r11, c235\n"
			"    dp4 r8.z, r11, c236\n"
			"    dp4 r8.w, r11, c237\n"

			"    mul r11, r10, r9.xxxx\n"			// fix
			"    mad %s, r8, r9.yyyy, r11\n"		// move

			"    \0",
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strOR[0]);
	}

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	if (cond == COND_MOV)
	{ // Move past the next line containing the mov instruction
		*ppIn = strstr(*ppIn, "\n") + 1;
	}
	else if (cond == COND_ADD)
	{ // Move past the next line containing the add instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == COND_MAD)
	{ // Move past the next line containing the mad instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == COND_m4x4)
	{ // Move past the next line containing the m4x4 instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}

	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

HRESULT APIENTRY VS_addcode_ORe1_VS20_LS1(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORe1_VS20_LS1()");

	char* pFindOR = NULL;
	pFindOR = strstr(*ppIn, "oPos");
	pFindOR -= 4;
	
	char strSource[9];
	char strSource2[9];
	char strSource3[9];

	#define COND_MOV	1
	#define COND_ADD	2
	#define COND_MAD	3
	#define COND_m4x4	4

	int cond = 0; // condition that decides how to proceed
	if ((pFindOR[0] == 'm') && (pFindOR[1] == 'o') && (pFindOR[2] == 'v'))
		cond = COND_MOV;
	else if ((pFindOR[0] == 'a') && (pFindOR[1] == 'd') && (pFindOR[2] == 'd'))
		cond = COND_ADD;
	else if ((pFindOR[0] == 'm') && (pFindOR[1] == 'a') && (pFindOR[2] == 'd'))
		cond = COND_MAD;
	else if ((pFindOR[0] == '4') && (pFindOR[1] == 'x') && (pFindOR[2] == '4'))
		cond = COND_m4x4;

	if (cond == 0)
	{
		ADD_LOG("VS_addcode_ORe1_VS20_LS1() : unsupported condition")
		return D3DERR_INVALIDCALL;
	}

	VS_addcode_common(ppIn, cond, pFindOR, ppOut, strSource, strSource2, strSource3);

	// Insert my assembly code
	char strAsmCode[700]; // Too big but better than too small

	if (cond == COND_MOV) // mov
	{
		sprintf(&strAsmCode[0],
			"    abs r11, %s\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n" // 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    sub r11.x, c230.y, r11.x\n"
			"    sub r11.y, c230.y, r11.y\n"
			"    mul r11, r11, r11\n"
			"    add r11.x, r11.x, r11.y\n"
			"    rsq r11.x, r11.x\n"
			"    sge r11.x, r11.x, c238.x\n"
			"    slt r11.y, r11.x, c238.x\n"
			"    mov oT7.xy, r11.xy\n"

			"dp4 r11.x, %s, c230\n"				// Inverse Projection
			"    dp4 r11.y, %s, c231\n"
			"    dp4 r11.z, %s, c232\n"
			"    dp4 r11.w, %s, c233\n"

			"    dp4 r10.x, r11, c234\n"		// Projection
			"    dp4 r10.y, r11, c235\n"
			"    dp4 r10.z, r11, c236\n"
			"    dp4 r10.w, r11, c237\n"

			"    mul r11, %s, r9.xxxx\n"		// fix
			"    mad %s, r10, r9.yyyy, r11\n"	// move

			"\0",
			&strSource[0],
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strSource[0],
			&strOR[0]);
	}
	else if (cond == COND_ADD) // add
	{
		sprintf(&strAsmCode[0],
			"add %s, %s, %s\n"

			"    abs r11, %s\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n" // 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    sub r11.x, c230.y, r11.x\n"
			"    sub r11.y, c230.y, r11.y\n"
			"    mul r11, r11, r11\n"
			"    add r11.x, r11.x, r11.y\n"
			"    rsq r11.x, r11.x\n"
			"    sge r11.x, r11.x, c238.x\n"
			"    slt r11.y, r11.x, c238.x\n"
			"    mov oT7.xy, r11.xy\n"

			"    dp4 r11.x, %s, c230\n"			// Inverse Projection
			"    dp4 r11.y, %s, c231\n"
			"    dp4 r11.z, %s, c232\n"
			"    dp4 r11.w, %s, c233\n"
			"    dp4 r10.x, r11, c234\n"		// Projection
			"    dp4 r10.y, r11, c235\n"
			"    dp4 r10.z, r11, c236\n"
			"    dp4 r10.w, r11, c237\n"

			"    mul r11, %s, r9.xxxx\n"		// fix
			"    mad %s, r10, r9.yyyy, r11\n"	// move

			"    \0",
			&strSource[0], &strSource[0], &strSource2[0],
			&strSource[0], // Lasersight
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strSource[0],
			&strOR[0]);
	}
	else if (cond == COND_MAD)
	{
		sprintf(&strAsmCode[0],
			"mad r10, %s, %s, %s\n"

			"    abs r11, r10\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n" // 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    sub r11.x, c230.y, r10.x\n"
			"    sub r11.y, c230.y, r10.y\n"
			"    mul r11, r11, r11\n"
			"    add r11.x, r11.x, r11.y\n"
			"    rsq r11.x, r11.x\n"
			"    sge r11.x, r11.x, c238.x\n"
			"    slt r11.y, r11.x, c238.x\n"
			"    mov oT7.xy, r11.xy\n"

			"    dp4 r11.x, r10, c230\n"		// Inverse Projection
			"    dp4 r11.y, r10, c231\n"
			"    dp4 r11.z, r10, c232\n"
			"    dp4 r11.w, r10, c233\n"
			"    dp4 r8.x, r11, c234\n"			// Projection
			"    dp4 r8.y, r11, c235\n"
			"    dp4 r8.z, r11, c236\n"
			"    dp4 r8.w, r11, c237\n"

			"    mul r11, r10, r9.xxxx\n"		// fix
			"    mad %s, r8, r9.yyyy, r11\n"	// move

			"    \0",
			&strSource[0], &strSource2[0], &strSource3[0],
			&strOR[0]);
	}
	else if (cond == COND_m4x4)
	{
		// TODO : one parameter missing. I wrote that code 8 years ago and I don't remember what it does and what the missing parameter should be
		sprintf(&strAsmCode[0],
			"dp4 r10.x, %s, %s\n"		// What the game would have done, r10.x not an error
			"    dp4 r10.y, %s, %s\n"
			"    dp4 r10.z, %s, %s\n"
			"    dp4 r10.w, %s, %s\n"

			"    sub r11.x, c230.y, r10.x\n"
			"    sub r11.y, c230.y, r10.y\n"
			"    mul r11, r11, r11\n"
			"    add r11.x, r11.x, r11.y\n"
			"    rsq r11.x, r11.x\n"
			"    sge r11.x, r11.x, c238.x\n"
			"    slt r11.y, r11.x, c238.x\n"
			"    mov oT7.xy, r11.xy\n"

			"    abs r11, %s\n"
			"    slt r9.xyz, r11.xyz, c239.xyz\n" // 1 if view need to stay fixed
			"    mul r9.x, r9.x, r9.y\n"
			"    mul r9.x, r9.x, r9.z\n"
			"    slt r9.y, r9.x, c239.w\n"

			"    dp4 r11.x, r10, c230\n"		// Inverse Projection
			"    dp4 r11.y, r10, c231\n"
			"    dp4 r11.z, r10, c232\n"
			"    dp4 r11.w, r10, c233\n"

			"    dp4 r8.x, r11, c234\n"			// Projection
			"    dp4 r8.y, r11, c235\n"
			"    dp4 r8.z, r11, c236\n"
			"    dp4 r8.w, r11, c237\n"

			"    mul r11, r10, r9.xxxx\n"		// fix
			"    mad %s, r8, r9.yyyy, r11\n"	// move

			"    \0",
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strSource[0], &strSource2[0],
			&strOR[0]);
	}

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	if (cond == COND_MOV)
	{ // Move past the next line containing the mov instruction
		*ppIn = strstr(*ppIn, "\n") + 1;
	}
	else if (cond == COND_ADD)
	{ // Move past the next line containing the add instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == COND_MAD)
	{ // Move past the next line containing the mad instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == COND_m4x4)
	{ // Move past the next line containing the m4x4 instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}

	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

//VS_addcode_ORe1_VS20_LS2 // no implemented

HRESULT APIENTRY VS_addcode_ORe1_VS30_LS0(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORe1_VS30_LS0()");

	char* pFindOR = NULL;
	pFindOR = strstr(*ppIn, "dcl_position o"); // Find output register, assuming that it works since it did the first time
	pFindOR = strstr(pFindOR + 14, &strOR[0]);
	pFindOR -= 4;
	
	char strSource[9];
	char strSource2[9];
	char strSource3[9];

	#define COND_MOV	1
	#define COND_ADD	2
	#define COND_MAD	3
	#define COND_m4x4	4

	int cond = 0; // condition that decides how to proceed
	if ((pFindOR[0] == 'm') && (pFindOR[1] == 'o') && (pFindOR[2] == 'v'))
		cond = COND_MOV;
	else if ((pFindOR[0] == 'a') && (pFindOR[1] == 'd') && (pFindOR[2] == 'd'))
		cond = COND_ADD;
	else if ((pFindOR[0] == 'm') && (pFindOR[1] == 'a') && (pFindOR[2] == 'd'))
		cond = COND_MAD;
	else if ((pFindOR[0] == '4') && (pFindOR[1] == 'x') && (pFindOR[2] == '4'))
		cond = COND_m4x4;
	
	if (cond == 0)
	{
		ADD_LOG("VS_addcode_ORe1_VS30_LS0() : unsupported condition")
		return D3DERR_INVALIDCALL;
	}

	VS_addcode_common(ppIn, cond, pFindOR, ppOut, strSource, strSource2, strSource3);

	// Insert my assembly code
	char strAsmCode[700]; // Too big but better than too small

	if (cond == COND_MOV)
	{
		sprintf(&strAsmCode[0],
			"dp4 r31.x, %s, c230\n"				// Inverse Projection
			"    dp4 r31.y, %s, c231\n"
			"    dp4 r31.z, %s, c232\n"
			"    dp4 r31.w, %s, c233\n"

			"    dp4 %s.x, r31, c234\n"			// Projection
			"    dp4 %s.y, r31, c235\n"
			"    dp4 %s.z, r31, c236\n"
			"    dp4 %s.w, r31, c237\n"

			"    abs r31, %s\n"
			"    if_lt r31.z, c239.z\n"
			"      if_lt r31.x, c239.x\n"
			"        if_lt r31.y, c239.y\n"
			"          mov %s, %s\n"
			"        endif\n"
			"      endif\n"
			"    endif\n"
			"\0",
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strOR[0], &strOR[0], &strOR[0], &strOR[0],
			&strSource[0], // z
			&strOR[0], &strSource[0]
			); // z
	}
	else if (cond == COND_ADD)
	{
		sprintf(&strAsmCode[0],
			"add %s, %s, %s\n"

			"    dp4 r31.x, %s, c230\n"			// Inverse Projection
			"    dp4 r31.y, %s, c231\n"
			"    dp4 r31.z, %s, c232\n"
			"    dp4 r31.w, %s, c233\n"

			"    dp4 %s.x, r31, c234\n"			// Projection
			"    dp4 %s.y, r31, c235\n"
			"    dp4 %s.z, r31, c236\n"
			"    dp4 %s.w, r31, c237\n"

			"    abs r31, %s\n"
			"    if_lt r31.z, c239.z\n"
			"      if_lt r31.x, c239.x\n"
			"        if_lt r31.y, c239.y\n"
			"          mov %s, %s\n"
			"        endif\n"
			"      endif\n"
			"    endif\n"

			"    \0",
			&strSource[0], &strSource[0], &strSource2[0],
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strOR[0], &strOR[0], &strOR[0], &strOR[0],
			&strSource[0], // z
			&strOR[0], &strSource[0]); // z
	}
	else if (cond == COND_MAD)
	{
		sprintf(&strAsmCode[0],
			"mad r30, %s, %s, %s\n"

			"    dp4 r31.x, r30, c230\n"	// Inverse Projection
			"    dp4 r31.y, r30, c231\n"
			"    dp4 r31.z, r30, c232\n"
			"    dp4 r31.w, r30, c233\n"
			"    dp4 %s.x, r31, c234\n"		// Projection
			"    dp4 %s.y, r31, c235\n"
			"    dp4 %s.z, r31, c236\n"
			"    dp4 %s.w, r31, c237\n"

			"    abs r31, r30\n"
			"    if_lt r31.z, c239.z\n"
			"      if_lt r31.x, c239.x\n"
			"        if_lt r31.y, c239.y\n"
			"          mov %s, r30\n"
			"        endif\n"
			"      endif\n"
			"    endif\n"

			"    \0",
			&strSource[0], &strSource2[0], &strSource3[0],
			&strOR[0], &strOR[0], &strOR[0], &strOR[0],
			&strOR[0]);
	}
	else if (cond == COND_m4x4)
	{
		sprintf("Not implemented. Ask programmer to implement for vs%s\0", "3.0");
	}

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	if (cond == COND_MOV) // mov
	{ // Move past the next line containing the mov instruction
		*ppIn = strstr(*ppIn, "\n") + 1;
	}
	else if (cond == COND_ADD) // add
	{ // Move past the next line containing the add instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == COND_MAD) // mad
	{ // Move past the next line containing the mad instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == COND_m4x4) // m4x4
	{ // Move past the next line containing the m4x4 instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}


	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

HRESULT APIENTRY VS_addcode_ORe1_VS30_LS1(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORe1_VS30_LS1()");

	char* pFindOR = NULL;
	pFindOR = strstr(*ppIn, "dcl_position o"); // Find output register, assuming that it works since it did the first time
	pFindOR = strstr(pFindOR + 14, &strOR[0]);
	pFindOR -= 4;
	
	char strSource[9];
	char strSource2[9];
	char strSource3[9];

	#define COND_MOV	1
	#define COND_ADD	2
	#define COND_MAD	3
	#define COND_m4x4	4

	int cond = 0; // condition that decides how to proceed
	if ((pFindOR[0] == 'm') && (pFindOR[1] == 'o') && (pFindOR[2] == 'v'))
		cond = COND_MOV;
	else if ((pFindOR[0] == 'a') && (pFindOR[1] == 'd') && (pFindOR[2] == 'd'))
		cond = COND_ADD;
	else if ((pFindOR[0] == 'm') && (pFindOR[1] == 'a') && (pFindOR[2] == 'd'))
		cond = COND_MAD;
	else if ((pFindOR[0] == '4') && (pFindOR[1] == 'x') && (pFindOR[2] == '4'))
		cond = COND_m4x4;

	if (cond == 0)
	{
		ADD_LOG("VS_addcode_ORe1_VS30_LS1() : unsupported condition")
		return D3DERR_INVALIDCALL;
	}

	VS_addcode_common(ppIn, cond, pFindOR, ppOut, strSource, strSource2, strSource3);

	// Insert my assembly code
	char strAsmCode[700]; // Too big but better than too small

	if (cond == COND_MOV) // mov
	{
		sprintf(&strAsmCode[0],
			"mov r31, %s" // Lasersight, ligne pour essayer de traiter cas o・le registre serait du genre r1.xzzy
			"    sub r31.x, c230.y, r31.x\n"
			"    sub r31.y, c230.y, r31.y\n"
			"    mul r31, r31, r31\n"
			"    add r31.x, r31.x, r31.y\n"
			"    rsq r31.x, r31.x\n"
			"    mov o9.xy, c238.zz\n"
			"    if_gt r31.x, c238.x\n"
			"      mov o9.xy, c238.yz\n"
			"    endif\n"

			"dp4 r31.x, %s, c230\n"				// Inverse Projection
			"    dp4 r31.y, %s, c231\n"
			"    dp4 r31.z, %s, c232\n"
			"    dp4 r31.w, %s, c233\n"

			"    dp4 %s.x, r31, c234\n"			// Projection
			"    dp4 %s.y, r31, c235\n"
			"    dp4 %s.z, r31, c236\n"
			"    dp4 %s.w, r31, c237\n"

			"    abs r31, %s\n"
			"    if_lt r31.z, c239.z\n"
			"      if_lt r31.x, c239.x\n"
			"        if_lt r31.y, c239.y\n"
			"          mov %s, %s\n"
			"        endif\n"
			"      endif\n"
			"    endif\n"
			"\0",
			&strSource[0], // Lasersight
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strOR[0], &strOR[0], &strOR[0], &strOR[0],
			&strSource[0],
			&strOR[0], &strSource[0]
			);
	}
	else if (cond == COND_ADD) // add
	{
		sprintf(&strAsmCode[0],
			"add %s, %s, %s\n"

			"mov r31, %s" // Lasersight, test to try to handle case with o register with format r1.xzzy
			"    sub r31.x, c230.y, r31.x\n"
			"    sub r31.y, c230.y, r31.y\n"
			"    mul r31, r31, r31\n"
			"    add r31.x, r31.x, r31.y\n"
			"    rsq r31.x, r31.x\n"
			"    mov o9.xy, c238.zz\n"
			"    if_gt r31.x, c238.x\n"
			"      mov o9.xy, c238.yz\n"
			"    endif\n"

			"    dp4 r31.x, %s, c230\n"			// Inverse Projection
			"    dp4 r31.y, %s, c231\n"
			"    dp4 r31.z, %s, c232\n"
			"    dp4 r31.w, %s, c233\n"

			"    dp4 %s.x, r31, c234\n"			// Projection
			"    dp4 %s.y, r31, c235\n"
			"    dp4 %s.z, r31, c236\n"
			"    dp4 %s.w, r31, c237\n"

			"    abs r31, %s\n"
			"    if_lt r31.z, c239.z\n"
			"      if_lt r31.x, c239.x\n"
			"        if_lt r31.y, c239.y\n"
			"          mov %s, %s\n"
			"        endif\n"
			"      endif\n"
			"    endif\n"

			"    \0",
			&strSource[0], &strSource[0], &strSource2[0],
			&strSource[0], // Lasersight
			&strSource[0], &strSource[0], &strSource[0], &strSource[0],
			&strOR[0], &strOR[0], &strOR[0], &strOR[0],
			&strSource[0],
			&strOR[0], &strSource[0]);
	}
	else if (cond == COND_MAD)
	{
		sprintf(&strAsmCode[0],
			"mad r30, %s, %s, %s\n"

			"    sub r31.x, c230.y, r30.x\n"
			"    sub r31.y, c230.y, r30.y\n"
			"    mul r31, r31, r31\n"
			"    add r31.x, r31.x, r31.y\n"
			"    rsq r31.x, r31.x\n"
			"    mov o9.xy, c238.zz\n"
			"    if_gt r31.x, c238.x\n"
			"      mov o9.xy, c238.yz\n"
			"    endif\n"

			"    dp4 r31.x, r30, c230\n"	// Inverse Projection
			"    dp4 r31.y, r30, c231\n"
			"    dp4 r31.z, r30, c232\n"
			"    dp4 r31.w, r30, c233\n"
			"    dp4 %s.x, r31, c234\n"		// Projection
			"    dp4 %s.y, r31, c235\n"
			"    dp4 %s.z, r31, c236\n"
			"    dp4 %s.w, r31, c237\n"

			"    abs r31, r30\n"
			"    if_lt r31.z, c239.z\n"
			"      if_lt r31.x, c239.x\n"
			"        if_lt r31.y, c239.y\n"
			"          mov %s, r30\n"
			"        endif\n"
			"      endif\n"
			"    endif\n"

			"    \0",
			&strSource[0], &strSource2[0], &strSource3[0],
			&strOR[0], &strOR[0], &strOR[0], &strOR[0],
			&strOR[0]); // z
	}
	else if (cond == COND_m4x4)
	{
		sprintf("Not implemented. Ask programmer to implement for vs%s\0", "3.0");
	}

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	if (cond == 1) // mov
	{ // Move past the next line containing the mov instruction
		*ppIn = strstr(*ppIn, "\n") + 1;
	}
	else if (cond == 2) // add
	{ // Move past the next line containing the add instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == 3) // mad
	{ // Move past the next line containing the mad instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}
	else if (cond == 4) // m4x4
	{ // Move past the next line containing the m4x4 instruction
		*ppIn = strstr(*ppIn, "\n") + 1 + 4;
	}

	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

//VS_addcode_ORe1_VS30_LS2 // no implemented

// Add new code to vertex shader that uses more than 1 output register (output registers greater than 1)
HRESULT APIENTRY VS_addcode_ORgt1_VS11_LS0(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORgt1_VS11_LS0()");
	char strAsmCode[700]; // Too big but better than too small
	sprintf(&strAsmCode[0],
		"    dp4 r11.x, r10, c230\n"	// Inverse Projection
		"    dp4 r11.y, r10, c231\n"
		"    dp4 r11.z, r10, c232\n"
		"    dp4 r11.w, r10, c233\n"
		"    dp4 r8.x, r11, c234\n"		// Projection
		"    dp4 r8.y, r11, c235\n"
		"    dp4 r8.z, r11, c236\n"
		"    dp4 r8.w, r11, c237\n"

		"    mov %s, r8\n"

		"\0",
		&strOR[0]);

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

HRESULT APIENTRY VS_addcode_ORgt1_VS11_LS1(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORgt1_VS11_LS1()");
	char strAsmCode[700]; // Too big but better than too small
	sprintf(&strAsmCode[0],
		"    abs r11, r10\n"
		"    slt r9.xyz, r11.xyz, c239.xyz\n" // 1 if view need to stay fixed
		"    mul r9.x, r9.x, r9.y\n"
		"    mul r9.x, r9.x, r9.z\n"
		"    slt r9.y, r9.x, c239.w\n"

		"    sub r11.x, c230.y, r10.x\n"
		"    sub r11.y, c230.y, r10.y\n"
		"    mul r11, r11, r11\n"
		"    add r11.x, r11.x, r11.y\n"
		"    rsq r11.x, r11.x\n"
		"    sge r11.x, r11.x, c238.x\n"
		"    slt r11.y, r11.x, c238.x\n"
		"    mov oT7.xy, r11.xy\n"

		"    dp4 r11.x, r10, c230\n"		// Inverse Projection
		"    dp4 r11.y, r10, c231\n"
		"    dp4 r11.z, r10, c232\n"
		"    dp4 r11.w, r10, c233\n"
		"    dp4 r8.x, r11, c234\n"			// Projection
		"    dp4 r8.y, r11, c235\n"
		"    dp4 r8.z, r11, c236\n"
		"    dp4 r8.w, r11, c237\n"

		"    mul r11, r10, r9.xxxx\n"		// fix
		"    mad %s, r8, r9.yyyy, r11\n"	// move

		"\0",
		&strOR[0]);

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

//VS_addcode_ORgt1_VS11_LS2 // no implemented

HRESULT APIENTRY VS_addcode_ORgt1_VS20_LS0(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORgt1_VS20_LS0()");
	char strAsmCode[700]; // Too big but better than too small
	sprintf(&strAsmCode[0],
		"    abs r11, r10\n"
		"    slt r9.xyz, r11.xyz, c239.xyz\n" // 1 if view need to stay fixed
		"    mul r9.x, r9.x, r9.y\n"
		"    mul r9.x, r9.x, r9.z\n"
		"    slt r9.y, r9.x, c239.w\n"

		"    dp4 r11.x, r10, c230\n"		// Inverse Projection
		"    dp4 r11.y, r10, c231\n"
		"    dp4 r11.z, r10, c232\n"
		"    dp4 r11.w, r10, c233\n"
		"    dp4 r8.x, r11, c234\n"			// Projection
		"    dp4 r8.y, r11, c235\n"
		"    dp4 r8.z, r11, c236\n"
		"    dp4 r8.w, r11, c237\n"

		"    mul r11, r10, r9.xxxx\n"		// fix
		"    mad %s, r8, r9.yyyy, r11\n"	// move

		"\0",
		&strOR[0]);

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

HRESULT APIENTRY VS_addcode_ORgt1_VS20_LS1(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORgt1_VS20_LS1()");
	char strAsmCode[700]; // Too big but better than too small
	sprintf(&strAsmCode[0],
		"    abs r11, r10\n"
		"    slt r9.xyz, r11.xyz, c239.xyz\n" // 1 if view need to stay fixed
		"    mul r9.x, r9.x, r9.y\n"
		"    mul r9.x, r9.x, r9.z\n"
		"    slt r9.y, r9.x, c239.w\n"

		"    sub r11.x, c230.y, r10.x\n"
		"    sub r11.y, c230.y, r10.y\n"
		"    mul r11, r11, r11\n"
		"    add r11.x, r11.x, r11.y\n"
		"    rsq r11.x, r11.x\n"
		"    sge r11.x, r11.x, c238.x\n"
		"    slt r11.y, r11.x, c238.x\n"
		"    mov oT7.xy, r11.xy\n"

		"    dp4 r11.x, r10, c230\n"		// Inverse Projection
		"    dp4 r11.y, r10, c231\n"
		"    dp4 r11.z, r10, c232\n"
		"    dp4 r11.w, r10, c233\n"
		"    dp4 r8.x, r11, c234\n"			// Projection
		"    dp4 r8.y, r11, c235\n"
		"    dp4 r8.z, r11, c236\n"
		"    dp4 r8.w, r11, c237\n"

		"    mul r11, r10, r9.xxxx\n"		// fix
		"    mad %s, r8, r9.yyyy, r11\n"	// move

		"\0",
		&strOR[0]);

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

//VS_addcode_ORgt1_VS20_LS2 // no implemented

HRESULT APIENTRY VS_addcode_ORgt1_VS30_LS0(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORgt1_VS30_LS0()");
	char strAsmCode[700]; // Too big but better than too small
	sprintf(&strAsmCode[0],
		"    dp4 r31.x, r30, c230\n"	// Inverse Projection
		"    dp4 r31.y, r30, c231\n"
		"    dp4 r31.z, r30, c232\n"
		"    dp4 r31.w, r30, c233\n"
		"    dp4 %s.x, r31, c234\n"		// Projection
		"    dp4 %s.y, r31, c235\n"
		"    dp4 %s.z, r31, c236\n"
		"    dp4 %s.w, r31, c237\n"

		"    abs r31, r30\n"
		"    if_lt r31.z, c239.z\n"
		"      if_lt r31.x, c239.x\n"
		"        if_lt r31.y, c239.y\n"
		"          mov %s, r30\n"
		"        endif\n"
		"      endif\n"
		"    endif\n"

		"\0",
		&strOR[0], &strOR[0], &strOR[0], &strOR[0],
		&strOR[0]
		);

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

HRESULT APIENTRY VS_addcode_ORgt1_VS30_LS1(char** ppIn, char* strOR, char** ppOut)
{
	ADD_LOG("VS_addcode_ORgt1_VS30_LS1()");
	char strAsmCode[700]; // Too big but better than too small
	sprintf(&strAsmCode[0],
		"    sub r31.x, c230.y, r30.x\n"
		"    sub r31.y, c230.y, r30.y\n"
		"    mul r31, r31, r31\n"
		"    add r31.x, r31.x, r31.y\n"
		"    rsq r31.x, r31.x\n"
		"    mov o9.xy, c238.zz\n"
		"    if_gt r31.x, c238.x\n"
		"      mov o9.xy, c238.yz\n"
		"    endif\n"

		"    dp4 r31.x, r30, c230\n"	// Inverse Projection
		"    dp4 r31.y, r30, c231\n"
		"    dp4 r31.z, r30, c232\n"
		"    dp4 r31.w, r30, c233\n"
		"    dp4 %s.x, r31, c234\n"		// Projection
		"    dp4 %s.y, r31, c235\n"
		"    dp4 %s.z, r31, c236\n"
		"    dp4 %s.w, r31, c237\n"

		"    abs r31, r30\n"
		"    if_lt r31.z, c239.z\n"
		"      if_lt r31.x, c239.x\n"
		"        if_lt r31.y, c239.y\n"
		"          mov %s, r30\n"
		"        endif\n"
		"      endif\n"
		"    endif\n"

		"\0",
		&strOR[0], &strOR[0], &strOR[0], &strOR[0],
		&strOR[0]
		);

	int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
	strncat(*ppOut, &strAsmCode[0], strAsmCodeSize);
	*ppOut = *ppOut + strAsmCodeSize;

	char* strComment = strstr(*ppIn, "//");
	if (strComment != NULL)
	{
		int nchar = strComment - *ppIn;
		strncat(*ppOut, *ppIn, nchar);
		//strOut = strOut + nchar;
		//strIn = strComment + nchar;
	}

	return D3D_OK;
}

//VS_addcode_ORgt1_VS30_LS2 // no implemented

HRESULT APIENTRY CreateModifiedPixelShader(LPDIRECT3DDEVICE9 pD3Ddev, CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
	//	ADD_LOG("hkIDirect3DDevice9_CreatePixelShader");
	bool ShaderCreated = false;
	
	HRESULT hr = D3D_OK;

	// Create original pixel shader
	hr = oCreatePixelShader(pD3Ddev, pFunction, ppShader);
	if (hr != D3D_OK) {
		return hr;
	}

	if ((LaserSight == 1) || (LaserSight == 2))
	{
		LPD3DXBUFFER pBufIn;
		D3DXCreateBuffer(50000, &pBufIn);
		D3DXDisassembleShader(pFunction, false, NULL, &pBufIn); // manual size

#ifdef LOG_PIXELSHADERS
		ADD_LOG("====================================================================");
		ADD_LOG("Trying to add input log now (ps shader %d)", pixelShaderCount);
		ADD_LOG((char*)pBufIn->GetBufferPointer());
		ADD_LOG("pass input!");
		ADD_LOG("\n");
#endif

		char* strIn; // Pointer of the read address in the shader buffer
		strIn = (char*)pBufIn->GetBufferPointer();



		char BufOut[50000];
		ZeroMemory(BufOut, sizeof(char) * 50000); // It won't work if you don't do that.
		char* strOut; // Pointer of the write address in the output buffer
		strOut = &BufOut[0];

		// Can't write more than one time to oC0 in ps_2_0 but you can in ps_3_0
		char* str1 = strstr((char*)pBufIn->GetBufferPointer(), "    dcl");
		if (str1 != NULL)
		{
			char* strPS;
			int psVersion = 0;
			if ((strPS = strstr((char*)pBufIn->GetBufferPointer(), "ps_2_0")) != NULL)
			{
				psVersion = 20;
				strPS -= 4;
			}
			else if ((strPS = strstr((char*)pBufIn->GetBufferPointer(), "ps_3_0")) != NULL)
			{
				psVersion = 30;
				strPS -= 4;
			}

			if (psVersion == 20)
			{
				ADD_LOG("1");

				int nchar = str1 - strPS;
				strncat(strOut, strPS, nchar);
				strOut = strOut + nchar;
				strIn = strPS + nchar;

				const char* strDcl =
					"    def c31, 1, 0, 0, 1\n"
					"    dcl t7.xy\n";

				ADD_LOG("2");

				UINT strDclSize = (UINT)strlen(strDcl);
				strncat(strOut, strDcl, strDclSize);
				strOut = strOut + strDclSize;

				char* stroC0 = strstr(strIn, "oC0"); // write up to oC0
				while (stroC0 != NULL)
				{
					nchar = stroC0 - strIn;
					strncat(strOut, strIn, nchar);
					strIn += nchar;
					strOut += nchar;

					const char* strr11 = "r11";
					nchar = 3;
					strncat(strOut, strr11, nchar);
					strIn += nchar;
					strOut += nchar;

					char* strNL = strstr(strIn, "\n") + 1;
					if (strNL != NULL)
					{
						nchar = strNL - strIn;
						strncat(strOut, strIn, nchar);
						strIn += nchar;
						strOut += nchar;
					}

					stroC0 = strstr(strIn, "oC0"); // write up to oC0
				}
				ADD_LOG("3");

				char* strAsmCode =
					"    mul r11.xyzw, r11.xyzw, t7.yyyy\n"
					"    mad r11.xyzw, c31, t7.xxxx, r11.xyzw\n"
					"    mov oC0, r11\n"
					"\0";

				ADD_LOG("4");

				int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
				strncat(strOut, &strAsmCode[0], strAsmCodeSize);
				strOut = strOut + strAsmCodeSize;

				char* strComment = strstr(strIn, "//");
				if (strComment != NULL)
				{
					int nchar = strComment - strIn;
					strncat(strOut, strIn, nchar);
					//strOut = strOut + nchar;
					//strIn = strComment + nchar;
				}

#ifdef LOG_PIXELSHADERS
				ADD_LOG("\n");
				ADD_LOG("Trying to add output log now for the above input");
				ADD_LOG(&BufOut[0]);
				ADD_LOG("pass output!");
				ADD_LOG("\n\n\n");
#endif

				LPD3DXBUFFER pNewFunction;
				LPD3DXBUFFER ppErrorMsgs;

				hr = D3DXAssembleShader(&BufOut[0], strlen(&BufOut[0]), NULL, NULL, 0, &pNewFunction, &ppErrorMsgs);

				if (hr == D3D_OK)
				{
					ADD_LOG("CreateModifiedPixelShader() : D3DXAssembleShader succeeded");
					// Create modified pixel shader
					IDirect3DPixelShader9* pPixelShaderTemp;
					hr = oCreatePixelShader(pD3Ddev, (DWORD*)pNewFunction->GetBufferPointer(), &pPixelShaderTemp);
					if (hr == D3D_OK) { // CreatePixelShader succeeded
						ShaderCreated = true;
						pPixelShader[*ppShader] = pPixelShaderTemp;
						ADD_LOG("CreateModifiedPixelShader() : pixel shader created, original = %p, stereo shader = %p", *ppShader, pPixelShaderTemp);
						// TODO : do hashing of original shader here and store in pPixelShaderHash
					}
					else // CreatePixelShader failed
					{
						pPixelShader[*ppShader] = *ppShader; // modified pixel shader creation failed. use original shader instead
						ADD_LOG("CreateModifiedPixelShader() : pixel shader creation failed, original = %p", *ppShader);
					}
				}
				else
				{
					ADD_LOG("D3DXAssembleShader failed");
					if (hr == D3DERR_INVALIDCALL) {
						ADD_LOG("D3DERR_INVALIDCALL");
					}
					else if (hr == D3DXERR_INVALIDDATA) {
						ADD_LOG("D3DXERR_INVALIDDATA");
					}
					else if (hr == E_OUTOFMEMORY) {
						ADD_LOG("E_OUTOFMEMORY");
					}
					else {
						ADD_LOG("unknown error");
					}

					ADD_LOG("Error log says : \n");
					ADD_LOG((char*)ppErrorMsgs->GetBufferPointer());
				}

				_SAFE_RELEASE(pNewFunction);
			}// end if psVersion == 20
			else if (psVersion == 30)
			{

				ADD_LOG("1");

				int nchar = str1 - strPS;
				strncat(strOut, strPS, nchar);
				strOut = strOut + nchar;
				strIn = strPS + nchar;

				const char* strDcl =
					"    def c100, 1, 1, 0, 0\n"
					"    dcl_texcoord9 v9.xy\n";

				ADD_LOG("2");

				UINT strDclSize = (UINT)strlen(strDcl);
				strncat(strOut, strDcl, strDclSize);
				strOut = strOut + strDclSize;

				char* strComment = strrchr(strIn, '/');
				if (strComment != NULL)
				{
					strComment -= 2;
					nchar = strComment - strIn;
					strncat(strOut, strIn, nchar);
					strOut = strOut + nchar;
				}

				ADD_LOG("3");

				char strAsmCode[700];
				sprintf(&strAsmCode[0],
					"    if_gt v9.x, v9.y\n"
					"      mov oC0.xyzw, c100.yzzy\n"
					"    endif\n"
					"\0");

				ADD_LOG("4");

				int strAsmCodeSize = (UINT)strlen(&strAsmCode[0]);
				strncat(strOut, &strAsmCode[0], strAsmCodeSize);

#ifdef LOG_PIXELSHADERS
				ADD_LOG("\n");
				ADD_LOG("Trying to add output log now for the above input");
				ADD_LOG(&BufOut[0]);
				ADD_LOG("pass output!");
				ADD_LOG("\n\n\n");
#endif

				LPD3DXBUFFER pNewFunction;
				LPD3DXBUFFER ppErrorMsgs;

				hr = D3DXAssembleShader(&BufOut[0], strlen(&BufOut[0]), NULL, NULL, 0, &pNewFunction, &ppErrorMsgs);

				if (hr == D3D_OK)
				{
					ADD_LOG("CreateModifiedPixelShader() : D3DXAssembleShader succeeded");
					// Create modified pixel shader
					IDirect3DPixelShader9* pPixelShaderTemp;
					hr = oCreatePixelShader(pD3Ddev, (DWORD*)pNewFunction->GetBufferPointer(), &pPixelShaderTemp);
					if (hr == D3D_OK) { // CreatePixelShader succeeded
						ShaderCreated = true;
						pPixelShader[*ppShader] = pPixelShaderTemp;
						ADD_LOG("CreateModifiedPixelShader() : pixel shader created, original = %p, stereo shader = %p", *ppShader, pPixelShaderTemp);
						// TODO : do hashing of original shader here and store in pPixelShaderHash
					}
					else // CreatePixelShader failed
					{
						pPixelShader[*ppShader] = *ppShader; // modified pixel shader creation failed. use original shader instead
						ADD_LOG("CreateModifiedPixelShader() : pixel shader creation failed, original = %p", *ppShader);
					}
				}
				else
				{
					ADD_LOG("CreateModifiedPixelShader() : D3DXAssembleShader failed"); // most likely uses add instruction and this code pattern is not supported yet
					switch (hr)
					{
					case D3DERR_INVALIDCALL:
						ADD_LOG("CreateModifiedPixelShader() : D3DERR_INVALIDCALL");
						break;
					case D3DXERR_INVALIDDATA:
						ADD_LOG("CreateModifiedPixelShader() : D3DXERR_INVALIDDATA");
						break;
					case E_OUTOFMEMORY:
						ADD_LOG("CreateModifiedPixelShader() : E_OUTOFMEMORY");
						break;
					default:
						ADD_LOG("CreateModifiedPixelShader() : unknown error");
					}
					ADD_LOG("CreateModifiedPixelShader() : Error log says : \n");
					ADD_LOG((char*)ppErrorMsgs->GetBufferPointer());
				}
				_SAFE_RELEASE(pNewFunction);
			} // end if psVersion == 30
		}	
		pBufIn->Release();		
	}

	return hr;
}

void AddMenuItems(void)
{
	ADD_LOG("AddMenuItems()");

	static bool InitMenuItems = false;
	if (InitMenuItems)return;
	InitMenuItems = true;

	GID[0] = menu.AddMenuGroup("[Vertex shaders]");
}

void addVSMenuItem(IDirect3DVertexShader9* pShader)
{
	// Add to menu
	int itemsCount = menu.getItemsCount();
	ADD_LOG("AddMenuItems() : itemsCount = %d", itemsCount);

	std::string shader_id("Shader " + std::to_string(itemsCount + 1));
	menu.AddMenuItem(shader_id.c_str(), &sVS.enabled.at(pShader), 0, 1, GID[0]);
}
