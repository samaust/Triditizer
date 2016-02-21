#ifndef _D3D9DEV_H
#define _D3D9DEV_H

#include <unordered_map>
#include <string>

#include "main.h"

#define OVERUNDER

class CD3DManager
{
public:
	CD3DManager(IDirect3DDevice9 *pD3Ddev)
	{
		m_pD3Ddev = pD3Ddev;
	}
	~CD3DManager() {}

	HRESULT Initialize();
	HRESULT PreReset();
	HRESULT PostReset();
	HRESULT Release();

private:
	IDirect3DDevice9	*m_pD3Ddev;	
};

class settingsVS
{
public:
	settingsVS() {}
	~settingsVS() {}

	std::unordered_map<IDirect3DVertexShader9*, IDirect3DVertexShader9*> stereoVS; // Old shader, Stereo shader
	std::unordered_map<IDirect3DVertexShader9*, float> enabled; // Old shader, enabled (0 = false, 1 = true)

	// TODO : Implement hash of shader and use it to identify shader
	//std::unordered_map<IDirect3DVertexShader9*, hash type> pVertexShaderHash; // Old shader, hash of shader code
};

typedef enum _MOVE_DIRECTION {
	LEFT = 0,
	RIGHT = 1,
	DO_NOT_MOVE = 2,
	TOGGLE_DO_NOT_MOVE = 3,
	TOGGLE_MOVE = 4
} MOVE_DIRECTION;

void readIni(void);
void move_view(LPDIRECT3DDEVICE9 pD3Ddev, MOVE_DIRECTION move_dir);
void decreaseSeparation();
void increaseSeparation();
void decreaseConvergence();
void increaseConvergence();
BOOL IsDepthFormatOk(IDirect3D9 *pD3D9,
	D3DFORMAT DepthFormat,
	D3DFORMAT AdapterFormat,
	D3DFORMAT BackBufferFormat);
void initSurfaces(LPDIRECT3DDEVICE9 pD3Ddev);
void AddMenuItems(void);
void addVSMenuItem(IDirect3DVertexShader9* pShader);

HRESULT APIENTRY CreateStereoVertexShader(LPDIRECT3DDEVICE9 pD3Ddev, CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader);
HRESULT APIENTRY CreateModifiedPixelShader(LPDIRECT3DDEVICE9 pD3Ddev, CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader);

// CreateStereoVertexShader helpers
HRESULT APIENTRY VS_appendDef_V11_LS0(char** ppOut);
HRESULT APIENTRY VS_appendDef_V11_LS1(char** ppOut);
HRESULT APIENTRY VS_appendDef_V20_LS0(char** ppOut);
HRESULT APIENTRY VS_appendDef_V20_LS1(char** ppOut);
HRESULT APIENTRY VS_appendDef_V30_LS0(char** ppOut);
HRESULT APIENTRY VS_appendDef_V30_LS1(char** ppOut);

HRESULT APIENTRY VS_replaceOR_V11(char** ppIn, int* pcountOR, char* pOR, char** ppOut);
HRESULT APIENTRY VS_replaceOR_V20(char** ppIn, int* pcountOR, char* pOR, char** ppOut);
HRESULT APIENTRY VS_replaceOR_V30(char** ppIn, int* pcountOR, char* pOR, char** ppOut);

HRESULT APIENTRY VS_addcode_common(char** ppIn, int cond, char* pFindOR, char** ppOut, char* strSource, char* strSource2, char* strSource3);
HRESULT APIENTRY VS_addcode_ORe1_VS11_LS0(char** ppIn, char* strOR, char** ppOut);
HRESULT APIENTRY VS_addcode_ORe1_VS11_LS1(char** ppIn, char* strOR, char** ppOut);
HRESULT APIENTRY VS_addcode_ORe1_VS20_LS0(char** ppIn, char* strOR, char** ppOut);
HRESULT APIENTRY VS_addcode_ORe1_VS20_LS1(char** ppIn, char* strOR, char** ppOut);
HRESULT APIENTRY VS_addcode_ORe1_VS30_LS0(char** ppIn, char* strOR, char** ppOut);
HRESULT APIENTRY VS_addcode_ORe1_VS30_LS1(char** ppIn, char* strOR, char** ppOut);

HRESULT APIENTRY VS_addcode_ORgt1_VS11_LS0(char** ppIn, char* strOR, char** ppOut);
HRESULT APIENTRY VS_addcode_ORgt1_VS11_LS1(char** ppIn, char* strOR, char** ppOut);
HRESULT APIENTRY VS_addcode_ORgt1_VS20_LS0(char** ppIn, char* strOR, char** ppOut);
HRESULT APIENTRY VS_addcode_ORgt1_VS20_LS1(char** ppIn, char* strOR, char** ppOut);
HRESULT APIENTRY VS_addcode_ORgt1_VS30_LS0(char** ppIn, char* strOR, char** ppOut);
HRESULT APIENTRY VS_addcode_ORgt1_VS30_LS1(char** ppIn, char* strOR, char** ppOut);

#ifdef OVERUNDER
HRESULT hkIDirect3DDevice9_takeScreenshot(LPDIRECT3DDEVICE9 pD3Ddev);
#endif

// constructor, to manually call once.
void hkIDirect3DDevice9_IDirect3DDevice9(LPDIRECT3DDEVICE9 pD3Ddev);

// IDirect3DDevice9 functions
// 0
typedef HRESULT(WINAPI * tQueryInterface)(LPDIRECT3DDEVICE9, REFIID, void**);
extern tQueryInterface oQueryInterface;
HRESULT APIENTRY hkIDirect3DDevice9_QueryInterface(LPDIRECT3DDEVICE9, REFIID riid, LPVOID *ppvObj);
// 1
typedef ULONG(WINAPI * tAddRef)(LPDIRECT3DDEVICE9);
extern tAddRef oAddRef;
ULONG APIENTRY hkIDirect3DDevice9_AddRef(LPDIRECT3DDEVICE9);
// 2
typedef ULONG(WINAPI * tRelease)(LPDIRECT3DDEVICE9);
extern tRelease oRelease;
ULONG APIENTRY hkIDirect3DDevice9_Release(LPDIRECT3DDEVICE9);
// 3
typedef HRESULT(WINAPI * tTestCooperativeLevel)(LPDIRECT3DDEVICE9);
extern tTestCooperativeLevel oTestCooperativeLevel;
HRESULT APIENTRY hkIDirect3DDevice9_TestCooperativeLevel(LPDIRECT3DDEVICE9);
// 4
typedef UINT(WINAPI * tGetAvailableTextureMem)(LPDIRECT3DDEVICE9);
extern tGetAvailableTextureMem oGetAvailableTextureMem;
UINT APIENTRY hkIDirect3DDevice9_GetAvailableTextureMem(LPDIRECT3DDEVICE9);
// 5
typedef HRESULT(WINAPI * tEvictManagedResources)(LPDIRECT3DDEVICE9);
extern tEvictManagedResources oEvictManagedResources;
HRESULT APIENTRY hkIDirect3DDevice9_EvictManagedResources(LPDIRECT3DDEVICE9);
// 6
typedef HRESULT(WINAPI * tGetDirect3D)(LPDIRECT3DDEVICE9, IDirect3D9**);
extern tGetDirect3D oGetDirect3D;
HRESULT APIENTRY hkIDirect3DDevice9_GetDirect3D(LPDIRECT3DDEVICE9, IDirect3D9 **ppD3D9);
// 7
typedef HRESULT(WINAPI * tGetDeviceCaps)(LPDIRECT3DDEVICE9, D3DCAPS9*);
extern tGetDeviceCaps oGetDeviceCaps;
HRESULT APIENTRY hkIDirect3DDevice9_GetDeviceCaps(LPDIRECT3DDEVICE9, D3DCAPS9 *pCaps);
// 8
typedef HRESULT(WINAPI * tGetDisplayMode)(LPDIRECT3DDEVICE9, UINT, D3DDISPLAYMODE*);
extern tGetDisplayMode oGetDisplayMode;
HRESULT APIENTRY hkIDirect3DDevice9_GetDisplayMode(LPDIRECT3DDEVICE9, UINT iSwapChain, D3DDISPLAYMODE* pMode);
// 9
typedef HRESULT(WINAPI * tGetCreationParameters)(LPDIRECT3DDEVICE9, D3DDEVICE_CREATION_PARAMETERS *);
extern tGetCreationParameters oGetCreationParameters;
HRESULT APIENTRY hkIDirect3DDevice9_GetCreationParameters(LPDIRECT3DDEVICE9, D3DDEVICE_CREATION_PARAMETERS *pParameters);
// 10
typedef HRESULT(WINAPI * tSetCursorProperties)(LPDIRECT3DDEVICE9, UINT, UINT, IDirect3DSurface9*);
extern tSetCursorProperties oSetCursorProperties;
HRESULT APIENTRY hkIDirect3DDevice9_SetCursorProperties(LPDIRECT3DDEVICE9, UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9 *pCursorBitmap);
// 11
typedef void(WINAPI * tSetCursorPosition)(LPDIRECT3DDEVICE9, int, int, DWORD);
extern tSetCursorPosition oSetCursorPosition;
void APIENTRY hkIDirect3DDevice9_SetCursorPosition(LPDIRECT3DDEVICE9, int X, int Y, DWORD Flags);
// 12
typedef BOOL(WINAPI * tShowCursor)(LPDIRECT3DDEVICE9, BOOL);
extern tShowCursor oShowCursor;
BOOL APIENTRY hkIDirect3DDevice9_ShowCursor(LPDIRECT3DDEVICE9, BOOL bShow);
// 13
typedef HRESULT(WINAPI * tCreateAdditionalSwapChain)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*, IDirect3DSwapChain9**);
extern tCreateAdditionalSwapChain oCreateAdditionalSwapChain;
HRESULT APIENTRY hkIDirect3DDevice9_CreateAdditionalSwapChain(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain);
// 14
typedef HRESULT(WINAPI * tGetSwapChain)(LPDIRECT3DDEVICE9, UINT, IDirect3DSwapChain9**);
extern tGetSwapChain oGetSwapChain;
HRESULT APIENTRY hkIDirect3DDevice9_GetSwapChain(LPDIRECT3DDEVICE9, UINT iSwapChain, IDirect3DSwapChain9** pSwapChain);
// 15
typedef UINT(WINAPI * tGetNumberOfSwapChains)(LPDIRECT3DDEVICE9);
extern tGetNumberOfSwapChains oGetNumberOfSwapChains;
unsigned int APIENTRY hkIDirect3DDevice9_GetNumberOfSwapChains(LPDIRECT3DDEVICE9);
// 16
typedef HRESULT(WINAPI * tReset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
extern tReset oReset;
HRESULT APIENTRY hkIDirect3DDevice9_Reset(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS *pPresentationParameters);
// 17
typedef HRESULT(WINAPI * tPresent)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
extern tPresent oPresent;
HRESULT APIENTRY hkIDirect3DDevice9_Present(LPDIRECT3DDEVICE9, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion);
// 18
typedef HRESULT(WINAPI * tGetBackBuffer)(LPDIRECT3DDEVICE9, UINT, UINT, D3DBACKBUFFER_TYPE, IDirect3DSurface9**);
extern tGetBackBuffer oGetBackBuffer;
HRESULT APIENTRY hkIDirect3DDevice9_GetBackBuffer(LPDIRECT3DDEVICE9, UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer);
// 19
typedef HRESULT(WINAPI * tGetRasterStatus)(LPDIRECT3DDEVICE9, UINT, D3DRASTER_STATUS*);
extern tGetRasterStatus oGetRasterStatus;
HRESULT APIENTRY hkIDirect3DDevice9_GetRasterStatus(LPDIRECT3DDEVICE9, UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus);
// 20
typedef HRESULT(WINAPI * tSetDialogBoxMode)(LPDIRECT3DDEVICE9, BOOL);
extern tSetDialogBoxMode oSetDialogBoxMode;
HRESULT APIENTRY hkIDirect3DDevice9_SetDialogBoxMode(LPDIRECT3DDEVICE9, BOOL bEnableDialogs);
// 21
typedef void(WINAPI * tSetGammaRamp)(LPDIRECT3DDEVICE9, UINT, DWORD, CONST D3DGAMMARAMP*);
extern tSetGammaRamp oSetGammaRamp;
void APIENTRY hkIDirect3DDevice9_SetGammaRamp(LPDIRECT3DDEVICE9, UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp);
// 22
typedef void(WINAPI * tGetGammaRamp)(LPDIRECT3DDEVICE9, UINT, D3DGAMMARAMP*);
extern tGetGammaRamp oGetGammaRamp;
void APIENTRY hkIDirect3DDevice9_GetGammaRamp(LPDIRECT3DDEVICE9, UINT iSwapChain, D3DGAMMARAMP* pRamp);
// 23
typedef HRESULT(WINAPI * tCreateTexture)(LPDIRECT3DDEVICE9, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9**, HANDLE*);
extern tCreateTexture oCreateTexture;
HRESULT APIENTRY hkIDirect3DDevice9_CreateTexture(LPDIRECT3DDEVICE9, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle);
// 24
typedef HRESULT(WINAPI * tCreateVolumeTexture)(LPDIRECT3DDEVICE9, UINT, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DVolumeTexture9**, HANDLE*);
extern tCreateVolumeTexture oCreateVolumeTexture;
HRESULT APIENTRY hkIDirect3DDevice9_CreateVolumeTexture(LPDIRECT3DDEVICE9, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle);
// 25
typedef HRESULT(WINAPI * tCreateCubeTexture)(LPDIRECT3DDEVICE9, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DCubeTexture9**, HANDLE*);
extern tCreateCubeTexture oCreateCubeTexture;
HRESULT APIENTRY hkIDirect3DDevice9_CreateCubeTexture(LPDIRECT3DDEVICE9, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle);
// 26
typedef HRESULT(WINAPI * tCreateVertexBuffer)(LPDIRECT3DDEVICE9, UINT, DWORD, DWORD, D3DPOOL, IDirect3DVertexBuffer9**, HANDLE*);
extern tCreateVertexBuffer oCreateVertexBuffer;
HRESULT APIENTRY hkIDirect3DDevice9_CreateVertexBuffer(LPDIRECT3DDEVICE9, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle);
// 27
typedef HRESULT(WINAPI * tCreateIndexBuffer)(LPDIRECT3DDEVICE9, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DIndexBuffer9**, HANDLE*);
extern tCreateIndexBuffer oCreateIndexBuffer;
HRESULT APIENTRY hkIDirect3DDevice9_CreateIndexBuffer(LPDIRECT3DDEVICE9, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle);
// 28
typedef HRESULT(WINAPI * tCreateRenderTarget)(LPDIRECT3DDEVICE9, UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, DWORD, BOOL, IDirect3DSurface9**, HANDLE*);
extern tCreateRenderTarget oCreateRenderTarget;
HRESULT APIENTRY hkIDirect3DDevice9_CreateRenderTarget(LPDIRECT3DDEVICE9, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
// 29
typedef HRESULT(WINAPI * tCreateDepthStencilSurface)(LPDIRECT3DDEVICE9, UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, DWORD, BOOL, IDirect3DSurface9**, HANDLE*);
extern tCreateDepthStencilSurface oCreateDepthStencilSurface;
HRESULT APIENTRY hkIDirect3DDevice9_CreateDepthStencilSurface(LPDIRECT3DDEVICE9, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
// 30
typedef HRESULT(WINAPI * tUpdateSurface)(LPDIRECT3DDEVICE9, IDirect3DSurface9*, CONST RECT*, IDirect3DSurface9*, CONST POINT*);
extern tUpdateSurface oUpdateSurface;
HRESULT APIENTRY hkIDirect3DDevice9_UpdateSurface(LPDIRECT3DDEVICE9, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint);
// 31
typedef HRESULT(WINAPI * tUpdateTexture)(LPDIRECT3DDEVICE9, IDirect3DBaseTexture9*, IDirect3DBaseTexture9*);
extern tUpdateTexture oUpdateTexture;
HRESULT APIENTRY hkIDirect3DDevice9_UpdateTexture(LPDIRECT3DDEVICE9, IDirect3DBaseTexture9 *pSourceTexture, IDirect3DBaseTexture9 *pDestinationTexture);
// 32
typedef HRESULT(WINAPI * tGetRenderTargetData)(LPDIRECT3DDEVICE9, IDirect3DSurface9*, IDirect3DSurface9*);
extern tGetRenderTargetData oGetRenderTargetData;
HRESULT APIENTRY hkIDirect3DDevice9_GetRenderTargetData(LPDIRECT3DDEVICE9, IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface);
// 33
typedef HRESULT(WINAPI * tGetFrontBufferData)(LPDIRECT3DDEVICE9, UINT, IDirect3DSurface9*);
extern tGetFrontBufferData oGetFrontBufferData;
HRESULT APIENTRY hkIDirect3DDevice9_GetFrontBufferData(LPDIRECT3DDEVICE9, UINT iSwapChain, IDirect3DSurface9* pDestSurface);
// 34
typedef HRESULT(WINAPI * tStretchRect)(LPDIRECT3DDEVICE9, IDirect3DSurface9*, CONST RECT*, IDirect3DSurface9*, CONST RECT*, D3DTEXTUREFILTERTYPE);
extern tStretchRect oStretchRect;
HRESULT APIENTRY hkIDirect3DDevice9_StretchRect(LPDIRECT3DDEVICE9, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter);
// 35
typedef HRESULT(WINAPI * tColorFill)(LPDIRECT3DDEVICE9, IDirect3DSurface9*, CONST RECT*, D3DCOLOR);
extern tColorFill oColorFill;
HRESULT APIENTRY hkIDirect3DDevice9_ColorFill(LPDIRECT3DDEVICE9, IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color);
// 36
typedef HRESULT(WINAPI * tCreateOffscreenPlainSurface)(LPDIRECT3DDEVICE9, UINT, UINT, D3DFORMAT, D3DPOOL, IDirect3DSurface9**, HANDLE*);
extern tCreateOffscreenPlainSurface oCreateOffscreenPlainSurface;
HRESULT APIENTRY hkIDirect3DDevice9_CreateOffscreenPlainSurface(LPDIRECT3DDEVICE9, UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
// 37
typedef HRESULT(WINAPI * tSetRenderTarget)(LPDIRECT3DDEVICE9, DWORD, IDirect3DSurface9*);
extern tSetRenderTarget oSetRenderTarget;
HRESULT APIENTRY hkIDirect3DDevice9_SetRenderTarget(LPDIRECT3DDEVICE9, DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget);
// 38
typedef HRESULT(WINAPI * tGetRenderTarget)(LPDIRECT3DDEVICE9, DWORD, IDirect3DSurface9**);
extern tGetRenderTarget oGetRenderTarget;
HRESULT APIENTRY hkIDirect3DDevice9_GetRenderTarget(LPDIRECT3DDEVICE9, DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget);
// 39
typedef HRESULT(WINAPI * tSetDepthStencilSurface)(LPDIRECT3DDEVICE9, IDirect3DSurface9*);
extern tSetDepthStencilSurface oSetDepthStencilSurface;
HRESULT APIENTRY hkIDirect3DDevice9_SetDepthStencilSurface(LPDIRECT3DDEVICE9, IDirect3DSurface9* pNewZStencil);
// 40
typedef HRESULT(WINAPI * tGetDepthStencilSurface)(LPDIRECT3DDEVICE9, IDirect3DSurface9**);
extern tGetDepthStencilSurface oGetDepthStencilSurface;
HRESULT APIENTRY hkIDirect3DDevice9_GetDepthStencilSurface(LPDIRECT3DDEVICE9, IDirect3DSurface9 **ppZStencilSurface);
// 41
typedef HRESULT(WINAPI * tBeginScene)(LPDIRECT3DDEVICE9);
extern tBeginScene oBeginScene;
HRESULT APIENTRY hkIDirect3DDevice9_BeginScene(LPDIRECT3DDEVICE9);
// 42
typedef HRESULT(WINAPI * tEndScene)(LPDIRECT3DDEVICE9);
extern tEndScene oEndScene;
HRESULT APIENTRY hkIDirect3DDevice9_EndScene(LPDIRECT3DDEVICE9);
// 43
typedef HRESULT(WINAPI * tClear)(LPDIRECT3DDEVICE9, DWORD, CONST D3DRECT*, DWORD, D3DCOLOR, float, DWORD);
extern tClear oClear;
HRESULT APIENTRY hkIDirect3DDevice9_Clear(LPDIRECT3DDEVICE9, DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
// 44
typedef HRESULT(WINAPI * tSetTransform)(LPDIRECT3DDEVICE9, D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*);
extern tSetTransform oSetTransform;
HRESULT APIENTRY hkIDirect3DDevice9_SetTransform(LPDIRECT3DDEVICE9, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix);
// 45
typedef HRESULT(WINAPI * tGetTransform)(LPDIRECT3DDEVICE9, D3DTRANSFORMSTATETYPE, D3DMATRIX*);
extern tGetTransform oGetTransform;
HRESULT APIENTRY hkIDirect3DDevice9_GetTransform(LPDIRECT3DDEVICE9, D3DTRANSFORMSTATETYPE State, D3DMATRIX *pMatrix);
// 46
typedef HRESULT(WINAPI * tMultiplyTransform)(LPDIRECT3DDEVICE9, D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*);
extern tMultiplyTransform oMultiplyTransform;
HRESULT APIENTRY hkIDirect3DDevice9_MultiplyTransform(LPDIRECT3DDEVICE9, D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix);
// 47
typedef HRESULT(WINAPI * tSetViewport)(LPDIRECT3DDEVICE9, CONST D3DVIEWPORT9*);
extern tSetViewport oSetViewport;
HRESULT APIENTRY hkIDirect3DDevice9_SetViewport(LPDIRECT3DDEVICE9, CONST D3DVIEWPORT9 *pViewport);
// 48
typedef HRESULT(WINAPI * tGetViewport)(LPDIRECT3DDEVICE9, D3DVIEWPORT9*);
extern tGetViewport oGetViewport;
HRESULT APIENTRY hkIDirect3DDevice9_GetViewport(LPDIRECT3DDEVICE9, D3DVIEWPORT9 *pViewport);
// 49
typedef HRESULT(WINAPI * tSetMaterial)(LPDIRECT3DDEVICE9, CONST D3DMATERIAL9*);
extern tSetMaterial oSetMaterial;
HRESULT APIENTRY hkIDirect3DDevice9_SetMaterial(LPDIRECT3DDEVICE9, CONST D3DMATERIAL9 *pMaterial);
// 50
typedef HRESULT(WINAPI * tGetMaterial)(LPDIRECT3DDEVICE9, D3DMATERIAL9*);
extern tGetMaterial oGetMaterial;
HRESULT APIENTRY hkIDirect3DDevice9_GetMaterial(LPDIRECT3DDEVICE9, D3DMATERIAL9 *pMaterial);
// 51
typedef HRESULT(WINAPI * tSetLight)(LPDIRECT3DDEVICE9, DWORD, CONST D3DLIGHT9*);
extern tSetLight oSetLight;
HRESULT APIENTRY hkIDirect3DDevice9_SetLight(LPDIRECT3DDEVICE9, DWORD Index, CONST D3DLIGHT9 *pLight);
// 52
typedef HRESULT(WINAPI * tGetLight)(LPDIRECT3DDEVICE9, DWORD, D3DLIGHT9*);
extern tGetLight oGetLight;
HRESULT APIENTRY hkIDirect3DDevice9_GetLight(LPDIRECT3DDEVICE9, DWORD Index, D3DLIGHT9 *pLight);
// 53
typedef HRESULT(WINAPI * tLightEnable)(LPDIRECT3DDEVICE9, DWORD, BOOL);
extern tLightEnable oLightEnable;
HRESULT APIENTRY hkIDirect3DDevice9_LightEnable(LPDIRECT3DDEVICE9, DWORD LightIndex, BOOL bEnable);
// 54
typedef HRESULT(WINAPI * tGetLightEnable)(LPDIRECT3DDEVICE9, DWORD, BOOL*);
extern tGetLightEnable oGetLightEnable;
HRESULT APIENTRY hkIDirect3DDevice9_GetLightEnable(LPDIRECT3DDEVICE9, DWORD Index, BOOL *pEnable);
// 55
typedef HRESULT(WINAPI * tSetClipPlane)(LPDIRECT3DDEVICE9, DWORD, CONST float*);
extern tSetClipPlane oSetClipPlane;
HRESULT APIENTRY hkIDirect3DDevice9_SetClipPlane(LPDIRECT3DDEVICE9, DWORD Index, CONST float *pPlane);
// 56
typedef HRESULT(WINAPI * tGetClipPlane)(LPDIRECT3DDEVICE9, DWORD, float*);
extern tGetClipPlane oGetClipPlane;
HRESULT APIENTRY hkIDirect3DDevice9_GetClipPlane(LPDIRECT3DDEVICE9, DWORD Index, float *pPlane);
// 57
typedef HRESULT(WINAPI * tSetRenderState)(LPDIRECT3DDEVICE9, D3DRENDERSTATETYPE, DWORD);
extern tSetRenderState oSetRenderState;
HRESULT APIENTRY hkIDirect3DDevice9_SetRenderState(LPDIRECT3DDEVICE9, D3DRENDERSTATETYPE State, DWORD Value);
// 58
typedef HRESULT(WINAPI * tGetRenderState)(LPDIRECT3DDEVICE9, D3DRENDERSTATETYPE, DWORD*);
extern tGetRenderState oGetRenderState;
HRESULT APIENTRY hkIDirect3DDevice9_GetRenderState(LPDIRECT3DDEVICE9, D3DRENDERSTATETYPE State, DWORD *pValue);
// 59
typedef HRESULT(WINAPI * tCreateStateBlock)(LPDIRECT3DDEVICE9, D3DSTATEBLOCKTYPE, IDirect3DStateBlock9**);
extern tCreateStateBlock oCreateStateBlock;
HRESULT APIENTRY hkIDirect3DDevice9_CreateStateBlock(LPDIRECT3DDEVICE9, D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB);
// 60
typedef HRESULT(WINAPI * tBeginStateBlock)(LPDIRECT3DDEVICE9);
extern tBeginStateBlock oBeginStateBlock;
HRESULT APIENTRY hkIDirect3DDevice9_BeginStateBlock(LPDIRECT3DDEVICE9);
// 61
typedef HRESULT(WINAPI * tEndStateBlock)(LPDIRECT3DDEVICE9, IDirect3DStateBlock9**);
extern tEndStateBlock oEndStateBlock;
HRESULT APIENTRY hkIDirect3DDevice9_EndStateBlock(LPDIRECT3DDEVICE9, IDirect3DStateBlock9** ppSB);
// 62
typedef HRESULT(WINAPI * tSetClipStatus)(LPDIRECT3DDEVICE9, CONST D3DCLIPSTATUS9*);
extern tSetClipStatus oSetClipStatus;
HRESULT APIENTRY hkIDirect3DDevice9_SetClipStatus(LPDIRECT3DDEVICE9, CONST D3DCLIPSTATUS9 *pClipStatus);
// 63
typedef HRESULT(WINAPI * tGetClipStatus)(LPDIRECT3DDEVICE9, D3DCLIPSTATUS9*);
extern tGetClipStatus oGetClipStatus;
HRESULT APIENTRY hkIDirect3DDevice9_GetClipStatus(LPDIRECT3DDEVICE9, D3DCLIPSTATUS9 *pClipStatus);
// 64
typedef HRESULT(WINAPI * tGetTexture)(LPDIRECT3DDEVICE9, DWORD, IDirect3DBaseTexture9**);
extern tGetTexture oGetTexture;
HRESULT APIENTRY hkIDirect3DDevice9_GetTexture(LPDIRECT3DDEVICE9, DWORD Stage, IDirect3DBaseTexture9 **ppTexture);
// 65
typedef HRESULT(WINAPI * tSetTexture)(LPDIRECT3DDEVICE9, DWORD, IDirect3DBaseTexture9*);
extern tSetTexture oSetTexture;
HRESULT APIENTRY hkIDirect3DDevice9_SetTexture(LPDIRECT3DDEVICE9, DWORD Stage, IDirect3DBaseTexture9 *pTexture);
// 66
typedef HRESULT(WINAPI * tGetTextureStageState)(LPDIRECT3DDEVICE9, DWORD, D3DTEXTURESTAGESTATETYPE, DWORD*);
extern tGetTextureStageState oGetTextureStageState;
HRESULT APIENTRY hkIDirect3DDevice9_GetTextureStageState(LPDIRECT3DDEVICE9, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue);
// 67
typedef HRESULT(WINAPI * tSetTextureStageState)(LPDIRECT3DDEVICE9, DWORD, D3DTEXTURESTAGESTATETYPE, DWORD);
extern tSetTextureStageState oSetTextureStageState;
HRESULT APIENTRY hkIDirect3DDevice9_SetTextureStageState(LPDIRECT3DDEVICE9, DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
// 68
typedef HRESULT(WINAPI * tGetSamplerState)(LPDIRECT3DDEVICE9, DWORD, D3DSAMPLERSTATETYPE, DWORD*);
extern tGetSamplerState oGetSamplerState;
HRESULT APIENTRY hkIDirect3DDevice9_GetSamplerState(LPDIRECT3DDEVICE9, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue);
// 69
typedef HRESULT(WINAPI * tSetSamplerState)(LPDIRECT3DDEVICE9, DWORD, D3DSAMPLERSTATETYPE, DWORD);
extern tSetSamplerState oSetSamplerState;
HRESULT APIENTRY hkIDirect3DDevice9_SetSamplerState(LPDIRECT3DDEVICE9, DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
// 70
typedef HRESULT(WINAPI * tValidateDevice)(LPDIRECT3DDEVICE9, DWORD*);
extern tValidateDevice oValidateDevice;
HRESULT APIENTRY hkIDirect3DDevice9_ValidateDevice(LPDIRECT3DDEVICE9, DWORD *pNumPasses);
// 71
typedef HRESULT(WINAPI * tSetPaletteEntries)(LPDIRECT3DDEVICE9, UINT, CONST PALETTEENTRY*);
extern tSetPaletteEntries oSetPaletteEntries;
HRESULT APIENTRY hkIDirect3DDevice9_SetPaletteEntries(LPDIRECT3DDEVICE9, UINT PaletteNumber, CONST PALETTEENTRY *pEntries);
// 72
typedef HRESULT(WINAPI * tGetPaletteEntries)(LPDIRECT3DDEVICE9, UINT, PALETTEENTRY*);
extern tGetPaletteEntries oGetPaletteEntries;
HRESULT APIENTRY hkIDirect3DDevice9_GetPaletteEntries(LPDIRECT3DDEVICE9, UINT PaletteNumber, PALETTEENTRY *pEntries);
// 73
typedef HRESULT(WINAPI * tSetCurrentTexturePalette)(LPDIRECT3DDEVICE9, UINT);
extern tSetCurrentTexturePalette oSetCurrentTexturePalette;
HRESULT APIENTRY hkIDirect3DDevice9_SetCurrentTexturePalette(LPDIRECT3DDEVICE9, UINT PaletteNumber);
// 74
typedef HRESULT(WINAPI * tGetCurrentTexturePalette)(LPDIRECT3DDEVICE9, UINT *);
extern tGetCurrentTexturePalette oGetCurrentTexturePalette;
HRESULT APIENTRY hkIDirect3DDevice9_GetCurrentTexturePalette(LPDIRECT3DDEVICE9, UINT *pPaletteNumber);
// 75
typedef HRESULT(WINAPI * tSetScissorRect)(LPDIRECT3DDEVICE9, CONST RECT*);
extern tSetScissorRect oSetScissorRect;
HRESULT APIENTRY hkIDirect3DDevice9_SetScissorRect(LPDIRECT3DDEVICE9, CONST RECT* pRect);
// 76
typedef HRESULT(WINAPI * tGetScissorRect)(LPDIRECT3DDEVICE9, RECT*);
extern tGetScissorRect oGetScissorRect;
HRESULT APIENTRY hkIDirect3DDevice9_GetScissorRect(LPDIRECT3DDEVICE9, RECT* pRect);
// 77
typedef HRESULT(WINAPI * tSetSoftwareVertexProcessing)(LPDIRECT3DDEVICE9, BOOL);
extern tSetSoftwareVertexProcessing oSetSoftwareVertexProcessing;
HRESULT APIENTRY hkIDirect3DDevice9_SetSoftwareVertexProcessing(LPDIRECT3DDEVICE9, BOOL bSoftware);
// 78
typedef BOOL(WINAPI * tGetSoftwareVertexProcessing)(LPDIRECT3DDEVICE9);
extern tGetSoftwareVertexProcessing oGetSoftwareVertexProcessing;
BOOL APIENTRY hkIDirect3DDevice9_GetSoftwareVertexProcessing(LPDIRECT3DDEVICE9);
// 79
typedef HRESULT(WINAPI * tSetNPatchMode)(LPDIRECT3DDEVICE9, float);
extern tSetNPatchMode oSetNPatchMode;
HRESULT APIENTRY hkIDirect3DDevice9_SetNPatchMode(LPDIRECT3DDEVICE9, float nSegments);
// 80
typedef float(WINAPI * tGetNPatchMode)(LPDIRECT3DDEVICE9);
extern tGetNPatchMode oGetNPatchMode;
float APIENTRY hkIDirect3DDevice9_GetNPatchMode(LPDIRECT3DDEVICE9);
// 81
typedef HRESULT(WINAPI * tDrawPrimitive)(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, UINT, UINT);
extern tDrawPrimitive oDrawPrimitive;
HRESULT APIENTRY hkIDirect3DDevice9_DrawPrimitive(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
// 82
typedef HRESULT(WINAPI * tDrawIndexedPrimitive)(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
extern tDrawIndexedPrimitive oDrawIndexedPrimitive;
HRESULT APIENTRY hkIDirect3DDevice9_DrawIndexedPrimitive(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
// 83
typedef HRESULT(WINAPI * tDrawPrimitiveUP)(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, UINT, CONST void*, UINT);
extern tDrawPrimitiveUP oDrawPrimitiveUP;
HRESULT APIENTRY hkIDirect3DDevice9_DrawPrimitiveUP(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride);
// 84
typedef HRESULT(WINAPI * tDrawIndexedPrimitiveUP)(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, UINT, UINT, UINT, CONST void*, D3DFORMAT, CONST void*, UINT);
extern tDrawIndexedPrimitiveUP oDrawIndexedPrimitiveUP;
HRESULT APIENTRY hkIDirect3DDevice9_DrawIndexedPrimitiveUP(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride);
// 85
typedef HRESULT(WINAPI * tProcessVertices)(LPDIRECT3DDEVICE9, UINT, UINT, UINT, IDirect3DVertexBuffer9*, IDirect3DVertexDeclaration9*, DWORD);
extern tProcessVertices oProcessVertices;
HRESULT APIENTRY hkIDirect3DDevice9_ProcessVertices(LPDIRECT3DDEVICE9, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags);
// 86
typedef HRESULT(WINAPI * tCreateVertexDeclaration)(LPDIRECT3DDEVICE9, CONST D3DVERTEXELEMENT9*, IDirect3DVertexDeclaration9**);
extern tCreateVertexDeclaration oCreateVertexDeclaration;
HRESULT APIENTRY hkIDirect3DDevice9_CreateVertexDeclaration(LPDIRECT3DDEVICE9, CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl);
// 87
typedef HRESULT(WINAPI * tSetVertexDeclaration)(LPDIRECT3DDEVICE9, IDirect3DVertexDeclaration9*);
extern tSetVertexDeclaration oSetVertexDeclaration;
HRESULT APIENTRY hkIDirect3DDevice9_SetVertexDeclaration(LPDIRECT3DDEVICE9, IDirect3DVertexDeclaration9* pDecl);
// 88
typedef HRESULT(WINAPI * tGetVertexDeclaration)(LPDIRECT3DDEVICE9, IDirect3DVertexDeclaration9**);
extern tGetVertexDeclaration oGetVertexDeclaration;
HRESULT APIENTRY hkIDirect3DDevice9_GetVertexDeclaration(LPDIRECT3DDEVICE9, IDirect3DVertexDeclaration9** ppDecl);
// 89
typedef HRESULT(WINAPI * tSetFVF)(LPDIRECT3DDEVICE9, DWORD);
extern tSetFVF oSetFVF;
HRESULT APIENTRY hkIDirect3DDevice9_SetFVF(LPDIRECT3DDEVICE9, DWORD FVF);
// 90
typedef HRESULT(WINAPI * tGetFVF)(LPDIRECT3DDEVICE9, DWORD*);
extern tGetFVF oGetFVF;
HRESULT APIENTRY hkIDirect3DDevice9_GetFVF(LPDIRECT3DDEVICE9, DWORD* pFVF);
// 91
typedef HRESULT(WINAPI * tCreateVertexShader)(LPDIRECT3DDEVICE9, CONST DWORD*, IDirect3DVertexShader9**);
extern tCreateVertexShader oCreateVertexShader;
HRESULT APIENTRY hkIDirect3DDevice9_CreateVertexShader(LPDIRECT3DDEVICE9, CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader);
// 92
typedef HRESULT(WINAPI * tSetVertexShader)(LPDIRECT3DDEVICE9, IDirect3DVertexShader9*);
extern tSetVertexShader oSetVertexShader;
HRESULT APIENTRY hkIDirect3DDevice9_SetVertexShader(LPDIRECT3DDEVICE9, IDirect3DVertexShader9* pShader);
// 93
typedef HRESULT(WINAPI * tGetVertexShader)(LPDIRECT3DDEVICE9, IDirect3DVertexShader9**);
extern tGetVertexShader oGetVertexShader;
HRESULT APIENTRY hkIDirect3DDevice9_GetVertexShader(LPDIRECT3DDEVICE9, IDirect3DVertexShader9** ppShader);
// 94
typedef HRESULT(WINAPI * tSetVertexShaderConstantF)(LPDIRECT3DDEVICE9, UINT, CONST float*, UINT);
extern tSetVertexShaderConstantF oSetVertexShaderConstantF;
HRESULT APIENTRY hkIDirect3DDevice9_SetVertexShaderConstantF(LPDIRECT3DDEVICE9, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
// 95
typedef HRESULT(WINAPI * tGetVertexShaderConstantF)(LPDIRECT3DDEVICE9, UINT, float*, UINT);
extern tGetVertexShaderConstantF oGetVertexShaderConstantF;
HRESULT APIENTRY hkIDirect3DDevice9_GetVertexShaderConstantF(LPDIRECT3DDEVICE9, UINT StartRegister, float* pConstantData, UINT Vector4fCount);
// 96
typedef HRESULT(WINAPI * tSetVertexShaderConstantI)(LPDIRECT3DDEVICE9, UINT, CONST int*, UINT);
extern tSetVertexShaderConstantI oSetVertexShaderConstantI;
HRESULT APIENTRY hkIDirect3DDevice9_SetVertexShaderConstantI(LPDIRECT3DDEVICE9, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount);
// 97
typedef HRESULT(WINAPI * tGetVertexShaderConstantI)(LPDIRECT3DDEVICE9, UINT, int*, UINT);
extern tGetVertexShaderConstantI oGetVertexShaderConstantI;
HRESULT APIENTRY hkIDirect3DDevice9_GetVertexShaderConstantI(LPDIRECT3DDEVICE9, UINT StartRegister, int* pConstantData, UINT Vector4iCount);
// 98
typedef HRESULT(WINAPI * tSetVertexShaderConstantB)(LPDIRECT3DDEVICE9, UINT, CONST BOOL*, UINT);
extern tSetVertexShaderConstantB oSetVertexShaderConstantB;
HRESULT APIENTRY hkIDirect3DDevice9_SetVertexShaderConstantB(LPDIRECT3DDEVICE9, UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount);
// 99
typedef HRESULT(WINAPI * tGetVertexShaderConstantB)(LPDIRECT3DDEVICE9, UINT, BOOL*, UINT);
extern tGetVertexShaderConstantB oGetVertexShaderConstantB;
HRESULT APIENTRY hkIDirect3DDevice9_GetVertexShaderConstantB(LPDIRECT3DDEVICE9, UINT StartRegister, BOOL* pConstantData, UINT BoolCount);
// 100
typedef HRESULT(WINAPI * tSetStreamSource)(LPDIRECT3DDEVICE9, UINT, IDirect3DVertexBuffer9*, UINT, UINT);
extern tSetStreamSource oSetStreamSource;
HRESULT APIENTRY hkIDirect3DDevice9_SetStreamSource(LPDIRECT3DDEVICE9, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride);
// 101
typedef HRESULT(WINAPI * tGetStreamSource)(LPDIRECT3DDEVICE9, UINT, IDirect3DVertexBuffer9**, UINT*, UINT*);
extern tGetStreamSource oGetStreamSource;
HRESULT APIENTRY hkIDirect3DDevice9_GetStreamSource(LPDIRECT3DDEVICE9, UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride);
// 102
typedef HRESULT(WINAPI * tSetStreamSourceFreq)(LPDIRECT3DDEVICE9, UINT, UINT);
extern tSetStreamSourceFreq oSetStreamSourceFreq;
HRESULT APIENTRY hkIDirect3DDevice9_SetStreamSourceFreq(LPDIRECT3DDEVICE9, UINT StreamNumber, UINT Divider);
// 103
typedef HRESULT(WINAPI * tGetStreamSourceFreq)(LPDIRECT3DDEVICE9, UINT, UINT*);
extern tGetStreamSourceFreq oGetStreamSourceFreq;
HRESULT APIENTRY hkIDirect3DDevice9_GetStreamSourceFreq(LPDIRECT3DDEVICE9, UINT StreamNumber, UINT* Divider);
// 104
typedef HRESULT(WINAPI * tSetIndices)(LPDIRECT3DDEVICE9, IDirect3DIndexBuffer9*);
extern tSetIndices oSetIndices;
HRESULT APIENTRY hkIDirect3DDevice9_SetIndices(LPDIRECT3DDEVICE9, IDirect3DIndexBuffer9* pIndexData);
// 105
typedef HRESULT(WINAPI * tGetIndices)(LPDIRECT3DDEVICE9, IDirect3DIndexBuffer9**);
extern tGetIndices oGetIndices;
HRESULT APIENTRY hkIDirect3DDevice9_GetIndices(LPDIRECT3DDEVICE9, IDirect3DIndexBuffer9** ppIndexData);
// 106
typedef HRESULT(WINAPI * tCreatePixelShader)(LPDIRECT3DDEVICE9, CONST DWORD*, IDirect3DPixelShader9**);
extern tCreatePixelShader oCreatePixelShader;
HRESULT APIENTRY hkIDirect3DDevice9_CreatePixelShader(LPDIRECT3DDEVICE9, CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader);
// 107
typedef HRESULT(WINAPI * tSetPixelShader)(LPDIRECT3DDEVICE9, IDirect3DPixelShader9*);
extern tSetPixelShader oSetPixelShader;
HRESULT APIENTRY hkIDirect3DDevice9_SetPixelShader(LPDIRECT3DDEVICE9, IDirect3DPixelShader9* pShader);
// 108
typedef HRESULT(WINAPI * tGetPixelShader)(LPDIRECT3DDEVICE9, IDirect3DPixelShader9**);
extern tGetPixelShader oGetPixelShader;
HRESULT APIENTRY hkIDirect3DDevice9_GetPixelShader(LPDIRECT3DDEVICE9, IDirect3DPixelShader9** ppShader);
// 109
typedef HRESULT(WINAPI * tSetPixelShaderConstantF)(LPDIRECT3DDEVICE9, UINT, CONST float*, UINT);
extern tSetPixelShaderConstantF oSetPixelShaderConstantF;
HRESULT APIENTRY hkIDirect3DDevice9_SetPixelShaderConstantF(LPDIRECT3DDEVICE9, UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
// 110
typedef HRESULT(WINAPI * tGetPixelShaderConstantF)(LPDIRECT3DDEVICE9, UINT, float*, UINT);
extern tGetPixelShaderConstantF oGetPixelShaderConstantF;
HRESULT APIENTRY hkIDirect3DDevice9_GetPixelShaderConstantF(LPDIRECT3DDEVICE9, UINT StartRegister, float* pConstantData, UINT Vector4fCount);
// 111
typedef HRESULT(WINAPI * tSetPixelShaderConstantI)(LPDIRECT3DDEVICE9, UINT, CONST int*, UINT);
extern tSetPixelShaderConstantI oSetPixelShaderConstantI;
HRESULT APIENTRY hkIDirect3DDevice9_SetPixelShaderConstantI(LPDIRECT3DDEVICE9, UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount);
// 112
typedef HRESULT(WINAPI * tGetPixelShaderConstantI)(LPDIRECT3DDEVICE9, UINT, int*, UINT);
extern tGetPixelShaderConstantI oGetPixelShaderConstantI;
HRESULT APIENTRY hkIDirect3DDevice9_GetPixelShaderConstantI(LPDIRECT3DDEVICE9, UINT StartRegister, int* pConstantData, UINT Vector4iCount);
// 113
typedef HRESULT(WINAPI * tSetPixelShaderConstantB)(LPDIRECT3DDEVICE9, UINT, CONST BOOL*, UINT);
extern tSetPixelShaderConstantB oSetPixelShaderConstantB;
HRESULT APIENTRY hkIDirect3DDevice9_SetPixelShaderConstantB(LPDIRECT3DDEVICE9, UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount);
// 114
typedef HRESULT(WINAPI * tGetPixelShaderConstantB)(LPDIRECT3DDEVICE9, UINT, BOOL*, UINT);
extern tGetPixelShaderConstantB oGetPixelShaderConstantB;
HRESULT APIENTRY hkIDirect3DDevice9_GetPixelShaderConstantB(LPDIRECT3DDEVICE9, UINT StartRegister, BOOL* pConstantData, UINT BoolCount);
// 115
typedef HRESULT(WINAPI * tDrawRectPatch)(LPDIRECT3DDEVICE9, UINT, CONST float*, CONST D3DRECTPATCH_INFO*);
extern tDrawRectPatch oDrawRectPatch;
HRESULT APIENTRY hkIDirect3DDevice9_DrawRectPatch(LPDIRECT3DDEVICE9, UINT Handle, CONST float *pNumSegs, CONST D3DRECTPATCH_INFO *pRectPatchInfo);
// 116
typedef HRESULT(WINAPI * tDrawTriPatch)(LPDIRECT3DDEVICE9, UINT, CONST float*, CONST D3DTRIPATCH_INFO*);
extern tDrawTriPatch oDrawTriPatch;
HRESULT APIENTRY hkIDirect3DDevice9_DrawTriPatch(LPDIRECT3DDEVICE9, UINT Handle, CONST float *pNumSegs, CONST D3DTRIPATCH_INFO *pTriPatchInfo);
// 117
typedef HRESULT(WINAPI * tDeletePatch)(LPDIRECT3DDEVICE9, UINT);
extern tDeletePatch oDeletePatch;
HRESULT APIENTRY hkIDirect3DDevice9_DeletePatch(LPDIRECT3DDEVICE9, UINT Handle);
// 118
typedef HRESULT(WINAPI * tCreateQuery)(LPDIRECT3DDEVICE9, D3DQUERYTYPE, IDirect3DQuery9**);
extern tCreateQuery oCreateQuery;
HRESULT APIENTRY hkIDirect3DDevice9_CreateQuery(LPDIRECT3DDEVICE9, D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery);

#endif
