#pragma once
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include "../Detours-1.5/include/detours.h"

#include "main.h"
#include "d3d9dev.h"
#include "logging.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")


// IDirect3DDevice9
tQueryInterface oQueryInterface;						// 0
tAddRef oAddRef;										// 1
tRelease oRelease;										// 2
tTestCooperativeLevel oTestCooperativeLevel;			// 3
tGetAvailableTextureMem oGetAvailableTextureMem;		// 4
tEvictManagedResources oEvictManagedResources;			// 5
tGetDirect3D oGetDirect3D;								// 6
tGetDeviceCaps oGetDeviceCaps;							// 7
tGetDisplayMode oGetDisplayMode;						// 8
tGetCreationParameters oGetCreationParameters;			// 9
tSetCursorProperties oSetCursorProperties;				// 10
tSetCursorPosition oSetCursorPosition;					// 11
tShowCursor oShowCursor;								// 12
tCreateAdditionalSwapChain oCreateAdditionalSwapChain;	// 13
tGetSwapChain oGetSwapChain;							// 14
tGetNumberOfSwapChains oGetNumberOfSwapChains;			// 15
tReset oReset;											// 16
tPresent oPresent;										// 17
tGetBackBuffer oGetBackBuffer;							// 18
tGetRasterStatus oGetRasterStatus;						// 19
tSetDialogBoxMode oSetDialogBoxMode;					// 20
tSetGammaRamp oSetGammaRamp;							// 21
tGetGammaRamp oGetGammaRamp;							// 22
tCreateTexture oCreateTexture;							// 23
tCreateVolumeTexture oCreateVolumeTexture;				// 24
tCreateCubeTexture oCreateCubeTexture;					// 25
tCreateVertexBuffer oCreateVertexBuffer;				// 26
tCreateIndexBuffer oCreateIndexBuffer;					// 27
tCreateRenderTarget oCreateRenderTarget;				// 28
tCreateDepthStencilSurface oCreateDepthStencilSurface;	// 29
tUpdateSurface oUpdateSurface;							// 30
tUpdateTexture oUpdateTexture;							// 31
tGetRenderTargetData oGetRenderTargetData;				// 32
tGetFrontBufferData oGetFrontBufferData;				// 33
tStretchRect oStretchRect;								// 34
tColorFill oColorFill;									// 35
tCreateOffscreenPlainSurface oCreateOffscreenPlainSurface;	// 36
tSetRenderTarget oSetRenderTarget;						// 37
tGetRenderTarget oGetRenderTarget;						// 38
tSetDepthStencilSurface oSetDepthStencilSurface;		// 39
tGetDepthStencilSurface oGetDepthStencilSurface;		// 40
tBeginScene oBeginScene;								// 41
tEndScene oEndScene;									// 42
tClear oClear;											// 43
tSetTransform oSetTransform;							// 44
tGetTransform oGetTransform;							// 45
tMultiplyTransform oMultiplyTransform;					// 46
tSetViewport oSetViewport;								// 47
tGetViewport oGetViewport;								// 48
tSetMaterial oSetMaterial;								// 49
tGetMaterial oGetMaterial;								// 50
tSetLight oSetLight;									// 51
tGetLight oGetLight;									// 52
tLightEnable oLightEnable;								// 53
tGetLightEnable oGetLightEnable;						// 54
tSetClipPlane oSetClipPlane;							// 55
tGetClipPlane oGetClipPlane;							// 56
tSetRenderState oSetRenderState;						// 57
tGetRenderState oGetRenderState;						// 58
tCreateStateBlock oCreateStateBlock;					// 59
tBeginStateBlock oBeginStateBlock;						// 60
tEndStateBlock oEndStateBlock;							// 61
tSetClipStatus oSetClipStatus;							// 62
tGetClipStatus oGetClipStatus;							// 63
tGetTexture oGetTexture;								// 64
tSetTexture oSetTexture;								// 65
tGetTextureStageState oGetTextureStageState;			// 66
tSetTextureStageState oSetTextureStageState;			// 67
tGetSamplerState oGetSamplerState;						// 68
tSetSamplerState oSetSamplerState;						// 69
tValidateDevice oValidateDevice;						// 70
tSetPaletteEntries oSetPaletteEntries;					// 71
tGetPaletteEntries oGetPaletteEntries;					// 72
tSetCurrentTexturePalette oSetCurrentTexturePalette;	// 73
tGetCurrentTexturePalette oGetCurrentTexturePalette;	// 74
tSetScissorRect oSetScissorRect;						// 75
tGetScissorRect oGetScissorRect;						// 76
tSetSoftwareVertexProcessing oSetSoftwareVertexProcessing;	// 77
tGetSoftwareVertexProcessing oGetSoftwareVertexProcessing;	// 78
tSetNPatchMode oSetNPatchMode;							// 79
tGetNPatchMode oGetNPatchMode;							// 80
tDrawPrimitive oDrawPrimitive;							// 81
tDrawIndexedPrimitive oDrawIndexedPrimitive;			// 82
tDrawPrimitiveUP oDrawPrimitiveUP;						// 83
tDrawIndexedPrimitiveUP oDrawIndexedPrimitiveUP;		// 84
tProcessVertices oProcessVertices;						// 85
tCreateVertexDeclaration oCreateVertexDeclaration;		// 86
tSetVertexDeclaration oSetVertexDeclaration;			// 87
tGetVertexDeclaration oGetVertexDeclaration;			// 88
tSetFVF oSetFVF;										// 89
tGetFVF oGetFVF;										// 90
tCreateVertexShader oCreateVertexShader;				// 91
tSetVertexShader oSetVertexShader;						// 92
tGetVertexShader oGetVertexShader;						// 93
tSetVertexShaderConstantF oSetVertexShaderConstantF;	// 94
tGetVertexShaderConstantF oGetVertexShaderConstantF;	// 95
tSetVertexShaderConstantI oSetVertexShaderConstantI;	// 96
tGetVertexShaderConstantI oGetVertexShaderConstantI;	// 97
tSetVertexShaderConstantB oSetVertexShaderConstantB;	// 98
tGetVertexShaderConstantB oGetVertexShaderConstantB;	// 99
tSetStreamSource oSetStreamSource;						// 100
tGetStreamSource oGetStreamSource;						// 101
tSetStreamSourceFreq oSetStreamSourceFreq;				// 102
tGetStreamSourceFreq oGetStreamSourceFreq;				// 103
tSetIndices oSetIndices;								// 104
tGetIndices oGetIndices;								// 105
tCreatePixelShader oCreatePixelShader;					// 106
tSetPixelShader oSetPixelShader;						// 107
tGetPixelShader oGetPixelShader;						// 108
tSetPixelShaderConstantF oSetPixelShaderConstantF;		// 109
tGetPixelShaderConstantF oGetPixelShaderConstantF;		// 110
tSetPixelShaderConstantI oSetPixelShaderConstantI;		// 111
tGetPixelShaderConstantI oGetPixelShaderConstantI;		// 112
tSetPixelShaderConstantB oSetPixelShaderConstantB;		// 113
tGetPixelShaderConstantB oGetPixelShaderConstantB;		// 114
tDrawRectPatch oDrawRectPatch;							// 115
tDrawTriPatch oDrawTriPatch;							// 116
tDeletePatch oDeletePatch;								// 117
tCreateQuery oCreateQuery;								// 118

LRESULT CALLBACK MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }

void DX_Init(DWORD* table)
{
	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
	RegisterClassExA(&wc);
	HWND hWnd = CreateWindowA("DX", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, GetDesktopWindow(), NULL, wc.hInstance, NULL);
	LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	LPDIRECT3DDEVICE9 pd3dDevice;
	pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);

	DWORD* pVTable = (DWORD*)pd3dDevice;
	pVTable = (DWORD*)pVTable[0];

	table[0] = pVTable[42];

	DestroyWindow(hWnd);
}

typedef HRESULT(WINAPI * tCreateDevice)(LPDIRECT3D9, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, IDirect3DDevice9**);
tCreateDevice oCreateDevice;
HRESULT APIENTRY myCreateDevice(LPDIRECT3D9 pd3dint, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	ADD_LOG("myCreateDevice()")
	HRESULT hRet = oCreateDevice(pd3dint, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	// do not need this hook anymore so remove detour 
	// endless loop caused by steam. GameOverlayRenderer.dll also detours the function even with the overlay deactivated
	// temporarily renaming the file (i.e. GameOverlayRenderer.dll.bak) allows this dll to work
	DetourRemove((PBYTE)oCreateDevice, (PBYTE)myCreateDevice);
	
	// Hook all the functions we need
	DWORD* pdwNewDevice = (DWORD*)*ppReturnedDeviceInterface;
	pdwNewDevice = (DWORD*)pdwNewDevice[0];

	// Setting to the original function so that these can be used in the code even if the functions are not hooked
	oQueryInterface = (tQueryInterface)(PBYTE)pdwNewDevice[0];
	oAddRef = (tAddRef)(PBYTE)pdwNewDevice[1];
	oRelease = (tRelease)(PBYTE)pdwNewDevice[2];
	oTestCooperativeLevel = (tTestCooperativeLevel)(PBYTE)pdwNewDevice[3];
	oGetAvailableTextureMem = (tGetAvailableTextureMem)(PBYTE)pdwNewDevice[4];
	oEvictManagedResources = (tEvictManagedResources)(PBYTE)pdwNewDevice[5];
	oGetDirect3D = (tGetDirect3D)(PBYTE)pdwNewDevice[6];
	oGetDeviceCaps = (tGetDeviceCaps)(PBYTE)pdwNewDevice[7];
	oGetDisplayMode = (tGetDisplayMode)(PBYTE)pdwNewDevice[8];
	oGetCreationParameters = (tGetCreationParameters)(PBYTE)pdwNewDevice[9];
	oSetCursorProperties = (tSetCursorProperties)(PBYTE)pdwNewDevice[10];
	oSetCursorPosition = (tSetCursorPosition)(PBYTE)pdwNewDevice[11];
	oShowCursor = (tShowCursor)(PBYTE)pdwNewDevice[12];
	oCreateAdditionalSwapChain = (tCreateAdditionalSwapChain)(PBYTE)pdwNewDevice[13];
	oGetSwapChain = (tGetSwapChain)(PBYTE)pdwNewDevice[14];
	oGetNumberOfSwapChains = (tGetNumberOfSwapChains)(PBYTE)pdwNewDevice[15];
	oReset = (tReset)(PBYTE)pdwNewDevice[16];
	oPresent = (tPresent)(PBYTE)pdwNewDevice[17];
	oGetBackBuffer = (tGetBackBuffer)(PBYTE)pdwNewDevice[18];
	oGetRasterStatus = (tGetRasterStatus)(PBYTE)pdwNewDevice[19];
	oSetDialogBoxMode = (tSetDialogBoxMode)(PBYTE)pdwNewDevice[20];
	oSetGammaRamp = (tSetGammaRamp)(PBYTE)pdwNewDevice[21];
	oGetGammaRamp = (tGetGammaRamp)(PBYTE)pdwNewDevice[22];
	oCreateTexture = (tCreateTexture)(PBYTE)pdwNewDevice[23];
	oCreateVolumeTexture = (tCreateVolumeTexture)(PBYTE)pdwNewDevice[24];
	oCreateCubeTexture = (tCreateCubeTexture)(PBYTE)pdwNewDevice[25];
	oCreateVertexBuffer = (tCreateVertexBuffer)(PBYTE)pdwNewDevice[26];
	oCreateIndexBuffer = (tCreateIndexBuffer)(PBYTE)pdwNewDevice[27];
	oCreateRenderTarget = (tCreateRenderTarget)(PBYTE)pdwNewDevice[28];
	oCreateDepthStencilSurface = (tCreateDepthStencilSurface)(PBYTE)pdwNewDevice[29];
	oUpdateSurface = (tUpdateSurface)(PBYTE)pdwNewDevice[30];
	oUpdateTexture = (tUpdateTexture)(PBYTE)pdwNewDevice[31];
	oGetRenderTargetData = (tGetRenderTargetData)(PBYTE)pdwNewDevice[32];
	oGetFrontBufferData = (tGetFrontBufferData)(PBYTE)pdwNewDevice[33];
	oStretchRect = (tStretchRect)(PBYTE)pdwNewDevice[34];
	oColorFill = (tColorFill)(PBYTE)pdwNewDevice[35];
	oCreateOffscreenPlainSurface = (tCreateOffscreenPlainSurface)(PBYTE)pdwNewDevice[36];
	oSetRenderTarget = (tSetRenderTarget)(PBYTE)pdwNewDevice[37];
	oGetRenderTarget = (tGetRenderTarget)(PBYTE)pdwNewDevice[38];
	oSetDepthStencilSurface = (tSetDepthStencilSurface)(PBYTE)pdwNewDevice[39];
	oGetDepthStencilSurface = (tGetDepthStencilSurface)(PBYTE)pdwNewDevice[40];
	oBeginScene = (tBeginScene)(PBYTE)pdwNewDevice[41];
	oEndScene = (tEndScene)(PBYTE)pdwNewDevice[42];
	oClear = (tClear)(PBYTE)pdwNewDevice[43];
	oSetTransform = (tSetTransform)(PBYTE)pdwNewDevice[44];
	oGetTransform = (tGetTransform)(PBYTE)pdwNewDevice[45];
	oMultiplyTransform = (tMultiplyTransform)(PBYTE)pdwNewDevice[46];
	oSetViewport = (tSetViewport)(PBYTE)pdwNewDevice[47];
	oGetViewport = (tGetViewport)(PBYTE)pdwNewDevice[48];
	oSetMaterial = (tSetMaterial)(PBYTE)pdwNewDevice[49];
	oGetMaterial = (tGetMaterial)(PBYTE)pdwNewDevice[50];
	oSetLight = (tSetLight)(PBYTE)pdwNewDevice[51];
	oGetLight = (tGetLight)(PBYTE)pdwNewDevice[52];
	oLightEnable = (tLightEnable)(PBYTE)pdwNewDevice[53];
	oGetLightEnable = (tGetLightEnable)(PBYTE)pdwNewDevice[54];
	oSetClipPlane = (tSetClipPlane)(PBYTE)pdwNewDevice[55];
	oGetClipPlane = (tGetClipPlane)(PBYTE)pdwNewDevice[56];
	oSetRenderState = (tSetRenderState)(PBYTE)pdwNewDevice[57];
	oGetRenderState = (tGetRenderState)(PBYTE)pdwNewDevice[58];
	oCreateStateBlock = (tCreateStateBlock)(PBYTE)pdwNewDevice[59];
	oBeginStateBlock = (tBeginStateBlock)(PBYTE)pdwNewDevice[60];
	oEndStateBlock = (tEndStateBlock)(PBYTE)pdwNewDevice[61];
	oSetClipStatus = (tSetClipStatus)(PBYTE)pdwNewDevice[62];
	oGetClipStatus = (tGetClipStatus)(PBYTE)pdwNewDevice[63];
	oGetTexture = (tGetTexture)(PBYTE)pdwNewDevice[64];
	oSetTexture = (tSetTexture)(PBYTE)pdwNewDevice[65];
	oGetTextureStageState = (tGetTextureStageState)(PBYTE)pdwNewDevice[66];
	oSetTextureStageState = (tSetTextureStageState)(PBYTE)pdwNewDevice[67];
	oGetSamplerState = (tGetSamplerState)(PBYTE)pdwNewDevice[68];
	oSetSamplerState = (tSetSamplerState)(PBYTE)pdwNewDevice[69];
	oValidateDevice = (tValidateDevice)(PBYTE)pdwNewDevice[70];
	oSetPaletteEntries = (tSetPaletteEntries)(PBYTE)pdwNewDevice[71];
	oGetPaletteEntries = (tGetPaletteEntries)(PBYTE)pdwNewDevice[72];
	oSetCurrentTexturePalette = (tSetCurrentTexturePalette)(PBYTE)pdwNewDevice[73];
	oGetCurrentTexturePalette = (tGetCurrentTexturePalette)(PBYTE)pdwNewDevice[74];
	oSetScissorRect = (tSetScissorRect)(PBYTE)pdwNewDevice[75];
	oGetScissorRect = (tGetScissorRect)(PBYTE)pdwNewDevice[76];
	oSetSoftwareVertexProcessing = (tSetSoftwareVertexProcessing)(PBYTE)pdwNewDevice[77];
	oGetSoftwareVertexProcessing = (tGetSoftwareVertexProcessing)(PBYTE)pdwNewDevice[78];
	oSetNPatchMode = (tSetNPatchMode)(PBYTE)pdwNewDevice[79];
	oGetNPatchMode = (tGetNPatchMode)(PBYTE)pdwNewDevice[80];
	oDrawPrimitive = (tDrawPrimitive)(PBYTE)pdwNewDevice[81];
	oDrawIndexedPrimitive = (tDrawIndexedPrimitive)(PBYTE)pdwNewDevice[82];
	oDrawPrimitiveUP = (tDrawPrimitiveUP)(PBYTE)pdwNewDevice[83];
	oDrawIndexedPrimitiveUP = (tDrawIndexedPrimitiveUP)(PBYTE)pdwNewDevice[84];
	oProcessVertices = (tProcessVertices)(PBYTE)pdwNewDevice[85];
	oCreateVertexDeclaration = (tCreateVertexDeclaration)(PBYTE)pdwNewDevice[86];
	oSetVertexDeclaration = (tSetVertexDeclaration)(PBYTE)pdwNewDevice[87];
	oGetVertexDeclaration = (tGetVertexDeclaration)(PBYTE)pdwNewDevice[88];
	oSetFVF = (tSetFVF)(PBYTE)pdwNewDevice[89];
	oGetFVF = (tGetFVF)(PBYTE)pdwNewDevice[90];
	oCreateVertexShader = (tCreateVertexShader)(PBYTE)pdwNewDevice[91];
	oSetVertexShader = (tSetVertexShader)(PBYTE)pdwNewDevice[92];
	oGetVertexShader = (tGetVertexShader)(PBYTE)pdwNewDevice[93];
	oSetVertexShaderConstantF = (tSetVertexShaderConstantF)(PBYTE)pdwNewDevice[94];
	oGetVertexShaderConstantF = (tGetVertexShaderConstantF)(PBYTE)pdwNewDevice[95];
	oSetVertexShaderConstantI = (tSetVertexShaderConstantI)(PBYTE)pdwNewDevice[96];
	oGetVertexShaderConstantI = (tGetVertexShaderConstantI)(PBYTE)pdwNewDevice[97];
	oSetVertexShaderConstantB = (tSetVertexShaderConstantB)(PBYTE)pdwNewDevice[98];
	oGetVertexShaderConstantB = (tGetVertexShaderConstantB)(PBYTE)pdwNewDevice[99];
	oSetStreamSource = (tSetStreamSource)(PBYTE)pdwNewDevice[100];
	oGetStreamSource = (tGetStreamSource)(PBYTE)pdwNewDevice[101];
	oSetStreamSourceFreq = (tSetStreamSourceFreq)(PBYTE)pdwNewDevice[102];
	oGetStreamSourceFreq = (tGetStreamSourceFreq)(PBYTE)pdwNewDevice[103];
	oSetIndices = (tSetIndices)(PBYTE)pdwNewDevice[104];
	oGetIndices = (tGetIndices)(PBYTE)pdwNewDevice[105];
	oCreatePixelShader = (tCreatePixelShader)(PBYTE)pdwNewDevice[106];
	oSetPixelShader = (tSetPixelShader)(PBYTE)pdwNewDevice[107];
	oGetPixelShader = (tGetPixelShader)(PBYTE)pdwNewDevice[108];
	oSetPixelShaderConstantF = (tSetPixelShaderConstantF)(PBYTE)pdwNewDevice[109];
	oGetPixelShaderConstantF = (tGetPixelShaderConstantF)(PBYTE)pdwNewDevice[110];
	oSetPixelShaderConstantI = (tSetPixelShaderConstantI)(PBYTE)pdwNewDevice[111];
	oGetPixelShaderConstantI = (tGetPixelShaderConstantI)(PBYTE)pdwNewDevice[112];
	oSetPixelShaderConstantB = (tSetPixelShaderConstantB)(PBYTE)pdwNewDevice[113];
	oGetPixelShaderConstantB = (tGetPixelShaderConstantB)(PBYTE)pdwNewDevice[114];
	oDrawRectPatch = (tDrawRectPatch)(PBYTE)pdwNewDevice[115];
	oDrawTriPatch = (tDrawTriPatch)(PBYTE)pdwNewDevice[116];
	oDeletePatch = (tDeletePatch)(PBYTE)pdwNewDevice[117];
	oCreateQuery = (tCreateQuery)(PBYTE)pdwNewDevice[118];

	// Now install hooks. Functions we don't need are commented.
	//oQueryInterface = (tQueryInterface)DetourFunction((PBYTE)pdwNewDevice[0], (PBYTE)hkIDirect3DDevice9_QueryInterface);
	oAddRef = (tAddRef)DetourFunction((PBYTE)pdwNewDevice[1], (PBYTE)hkIDirect3DDevice9_AddRef);
	oRelease = (tRelease)DetourFunction((PBYTE)pdwNewDevice[2], (PBYTE)hkIDirect3DDevice9_Release);
	//oTestCooperativeLevel = (tTestCooperativeLevel)DetourFunction((PBYTE)pdwNewDevice[3], (PBYTE)hkIDirect3DDevice9_TestCooperativeLevel);
	//oGetAvailableTextureMem = (tGetAvailableTextureMem)DetourFunction((PBYTE)pdwNewDevice[4], (PBYTE)hkIDirect3DDevice9_GetAvailableTextureMem);
	//oEvictManagedResources = (tEvictManagedResources)DetourFunction((PBYTE)pdwNewDevice[5], (PBYTE)hkIDirect3DDevice9_EvictManagedResources);
	//oGetDirect3D = (tGetDirect3D)DetourFunction((PBYTE)pdwNewDevice[6], (PBYTE)hkIDirect3DDevice9_GetDirect3D);
	//oGetDeviceCaps = (tGetDeviceCaps)DetourFunction((PBYTE)pdwNewDevice[7], (PBYTE)hkIDirect3DDevice9_GetDeviceCaps);
	//oGetDisplayMode = (tGetDisplayMode)DetourFunction((PBYTE)pdwNewDevice[8], (PBYTE)hkIDirect3DDevice9_GetDisplayMode);
	//oGetCreationParameters = (tGetCreationParameters)DetourFunction((PBYTE)pdwNewDevice[9], (PBYTE)hkIDirect3DDevice9_GetCreationParameters);
	//oSetCursorProperties = (tSetCursorProperties)DetourFunction((PBYTE)pdwNewDevice[10], (PBYTE)hkIDirect3DDevice9_SetCursorProperties);
	//oSetCursorPosition = (tSetCursorPosition)DetourFunction((PBYTE)pdwNewDevice[11], (PBYTE)hkIDirect3DDevice9_SetCursorPosition);
	//oShowCursor = (tShowCursor)DetourFunction((PBYTE)pdwNewDevice[12], (PBYTE)hkIDirect3DDevice9_ShowCursor);
	//oCreateAdditionalSwapChain = (tCreateAdditionalSwapChain)DetourFunction((PBYTE)pdwNewDevice[13], (PBYTE)hkIDirect3DDevice9_CreateAdditionalSwapChain);
	//oGetSwapChain = (tGetSwapChain)DetourFunction((PBYTE)pdwNewDevice[14], (PBYTE)hkIDirect3DDevice9_GetSwapChain);
	//oGetNumberOfSwapChains = (tGetNumberOfSwapChains)DetourFunction((PBYTE)pdwNewDevice[15], (PBYTE)hkIDirect3DDevice9_GetNumberOfSwapChains);
	oReset = (tReset)DetourFunction((PBYTE)pdwNewDevice[16], (PBYTE)hkIDirect3DDevice9_Reset);
	//oPresent = (tPresent)DetourFunction((PBYTE)pdwNewDevice[17], (PBYTE)hkIDirect3DDevice9_Present);
	//oGetBackBuffer = (tGetBackBuffer)DetourFunction((PBYTE)pdwNewDevice[18], (PBYTE)hkIDirect3DDevice9_GetBackBuffer);
	//oGetRasterStatus = (tGetRasterStatus)DetourFunction((PBYTE)pdwNewDevice[19], (PBYTE)hkIDirect3DDevice9_GetRasterStatus);
	//oSetDialogBoxMode = (tSetDialogBoxMode)DetourFunction((PBYTE)pdwNewDevice[20], (PBYTE)hkIDirect3DDevice9_SetDialogBoxMode);
	//oSetGammaRamp = (tSetGammaRamp)DetourFunction((PBYTE)pdwNewDevice[21], (PBYTE)hkIDirect3DDevice9_SetGammaRamp);
	//oGetGammaRamp = (tGetGammaRamp)DetourFunction((PBYTE)pdwNewDevice[22], (PBYTE)hkIDirect3DDevice9_GetGammaRamp);
	//oCreateTexture = (tCreateTexture)DetourFunction((PBYTE)pdwNewDevice[23], (PBYTE)hkIDirect3DDevice9_CreateTexture);
	//oCreateVolumeTexture = (tCreateVolumeTexture)DetourFunction((PBYTE)pdwNewDevice[24], (PBYTE)hkIDirect3DDevice9_CreateVolumeTexture);
	//oCreateCubeTexture = (tCreateCubeTexture)DetourFunction((PBYTE)pdwNewDevice[25], (PBYTE)hkIDirect3DDevice9_CreateCubeTexture);
	//oCreateVertexBuffer = (tCreateVertexBuffer)DetourFunction((PBYTE)pdwNewDevice[26], (PBYTE)hkIDirect3DDevice9_CreateVertexBuffer);
	//oCreateIndexBuffer = (tCreateIndexBuffer)DetourFunction((PBYTE)pdwNewDevice[27], (PBYTE)hkIDirect3DDevice9_CreateIndexBuffer);
	//oCreateRenderTarget = (tCreateRenderTarget)DetourFunction((PBYTE)pdwNewDevice[28], (PBYTE)hkIDirect3DDevice9_CreateRenderTarget);
	//oCreateDepthStencilSurface = (tCreateDepthStencilSurface)DetourFunction((PBYTE)pdwNewDevice[29], (PBYTE)hkIDirect3DDevice9_CreateDepthStencilSurface);
	//oUpdateSurface = (tUpdateSurface)DetourFunction((PBYTE)pdwNewDevice[30], (PBYTE)hkIDirect3DDevice9_UpdateSurface);
	//oUpdateTexture = (tUpdateTexture)DetourFunction((PBYTE)pdwNewDevice[31], (PBYTE)hkIDirect3DDevice9_UpdateTexture);
	//oGetRenderTargetData = (tGetRenderTargetData)DetourFunction((PBYTE)pdwNewDevice[32], (PBYTE)hkIDirect3DDevice9_GetRenderTargetData);
	//oGetFrontBufferData = (tGetFrontBufferData)DetourFunction((PBYTE)pdwNewDevice[33], (PBYTE)hkIDirect3DDevice9_GetFrontBufferData);
	//oStretchRect = (tStretchRect)DetourFunction((PBYTE)pdwNewDevice[34], (PBYTE)hkIDirect3DDevice9_StretchRect);
	//oColorFill = (tColorFill)DetourFunction((PBYTE)pdwNewDevice[35], (PBYTE)hkIDirect3DDevice9_ColorFill);
	//oCreateOffscreenPlainSurface = (tCreateOffscreenPlainSurface)DetourFunction((PBYTE)pdwNewDevice[36], (PBYTE)hkIDirect3DDevice9_CreateOffscreenPlainSurface);
	oSetRenderTarget = (tSetRenderTarget)DetourFunction((PBYTE)pdwNewDevice[37], (PBYTE)hkIDirect3DDevice9_SetRenderTarget);
	//oGetRenderTarget = (tGetRenderTarget)DetourFunction((PBYTE)pdwNewDevice[38], (PBYTE)hkIDirect3DDevice9_GetRenderTarget);
	oSetDepthStencilSurface = (tSetDepthStencilSurface)DetourFunction((PBYTE)pdwNewDevice[39], (PBYTE)hkIDirect3DDevice9_SetDepthStencilSurface);
	oGetDepthStencilSurface = (tGetDepthStencilSurface)DetourFunction((PBYTE)pdwNewDevice[40], (PBYTE)hkIDirect3DDevice9_GetDepthStencilSurface);
	oBeginScene = (tBeginScene)DetourFunction((PBYTE)pdwNewDevice[41], (PBYTE)hkIDirect3DDevice9_BeginScene);
	oEndScene = (tEndScene)DetourFunction((PBYTE)pdwNewDevice[42], (PBYTE)hkIDirect3DDevice9_EndScene);
	oClear = (tClear)DetourFunction((PBYTE)pdwNewDevice[43], (PBYTE)hkIDirect3DDevice9_Clear);
	//oSetTransform = (tSetTransform)DetourFunction((PBYTE)pdwNewDevice[44], (PBYTE)hkIDirect3DDevice9_SetTransform);
	//oGetTransform = (tGetTransform)DetourFunction((PBYTE)pdwNewDevice[45], (PBYTE)hkIDirect3DDevice9_GetTransform);
	//oMultiplyTransform = (tMultiplyTransform)DetourFunction((PBYTE)pdwNewDevice[46], (PBYTE)hkIDirect3DDevice9_MultiplyTransform);
	//oSetViewport = (tSetViewport)DetourFunction((PBYTE)pdwNewDevice[47], (PBYTE)hkIDirect3DDevice9_SetViewport);
	//oGetViewport = (tGetViewport)DetourFunction((PBYTE)pdwNewDevice[48], (PBYTE)hkIDirect3DDevice9_GetViewport);
	//oSetMaterial = (tSetMaterial)DetourFunction((PBYTE)pdwNewDevice[49], (PBYTE)hkIDirect3DDevice9_SetMaterial);
	//oGetMaterial = (tGetMaterial)DetourFunction((PBYTE)pdwNewDevice[50], (PBYTE)hkIDirect3DDevice9_GetMaterial);
	//oSetLight = (tSetLight)DetourFunction((PBYTE)pdwNewDevice[51], (PBYTE)hkIDirect3DDevice9_SetLight);
	//oGetLight = (tGetLight)DetourFunction((PBYTE)pdwNewDevice[52], (PBYTE)hkIDirect3DDevice9_GetLight);
	//oLightEnable = (tLightEnable)DetourFunction((PBYTE)pdwNewDevice[53], (PBYTE)hkIDirect3DDevice9_LightEnable);
	//oGetLightEnable = (tGetLightEnable)DetourFunction((PBYTE)pdwNewDevice[54], (PBYTE)hkIDirect3DDevice9_GetLightEnable);
	//oSetClipPlane = (tSetClipPlane)DetourFunction((PBYTE)pdwNewDevice[55], (PBYTE)hkIDirect3DDevice9_SetClipPlane);
	//oGetClipPlane = (tGetClipPlane)DetourFunction((PBYTE)pdwNewDevice[56], (PBYTE)hkIDirect3DDevice9_GetClipPlane);
	//oSetRenderState = (tSetRenderState)DetourFunction((PBYTE)pdwNewDevice[57], (PBYTE)hkIDirect3DDevice9_SetRenderState);
	//oGetRenderState = (tGetRenderState)DetourFunction((PBYTE)pdwNewDevice[58], (PBYTE)hkIDirect3DDevice9_GetRenderState);
	//oCreateStateBlock = (tCreateStateBlock)DetourFunction((PBYTE)pdwNewDevice[59], (PBYTE)hkIDirect3DDevice9_CreateStateBlock);
	//oBeginStateBlock = (tBeginStateBlock)DetourFunction((PBYTE)pdwNewDevice[60], (PBYTE)hkIDirect3DDevice9_BeginStateBlock);
	//oEndStateBlock = (tEndStateBlock)DetourFunction((PBYTE)pdwNewDevice[61], (PBYTE)hkIDirect3DDevice9_EndStateBlock);
	//oSetClipStatus = (tSetClipStatus)DetourFunction((PBYTE)pdwNewDevice[62], (PBYTE)hkIDirect3DDevice9_SetClipStatus);
	//oGetClipStatus = (tGetClipStatus)DetourFunction((PBYTE)pdwNewDevice[63], (PBYTE)hkIDirect3DDevice9_GetClipStatus);
	//oGetTexture = (tGetTexture)DetourFunction((PBYTE)pdwNewDevice[64], (PBYTE)hkIDirect3DDevice9_GetTexture);
	//oSetTexture = (tSetTexture)DetourFunction((PBYTE)pdwNewDevice[65], (PBYTE)hkIDirect3DDevice9_SetTexture);
	//oGetTextureStageState = (tGetTextureStageState)DetourFunction((PBYTE)pdwNewDevice[66], (PBYTE)hkIDirect3DDevice9_GetTextureStageState);
	//oSetTextureStageState = (tSetTextureStageState)DetourFunction((PBYTE)pdwNewDevice[67], (PBYTE)hkIDirect3DDevice9_SetTextureStageState);
	//oGetSamplerState = (tGetSamplerState)DetourFunction((PBYTE)pdwNewDevice[68], (PBYTE)hkIDirect3DDevice9_GetSamplerState);
	//oSetSamplerState = (tSetSamplerState)DetourFunction((PBYTE)pdwNewDevice[69], (PBYTE)hkIDirect3DDevice9_SetSamplerState);
	//oValidateDevice = (tValidateDevice)DetourFunction((PBYTE)pdwNewDevice[70], (PBYTE)hkIDirect3DDevice9_ValidateDevice);
	//oSetPaletteEntries = (tSetPaletteEntries)DetourFunction((PBYTE)pdwNewDevice[71], (PBYTE)hkIDirect3DDevice9_SetPaletteEntries);
	//oGetPaletteEntries = (tGetPaletteEntries)DetourFunction((PBYTE)pdwNewDevice[72], (PBYTE)hkIDirect3DDevice9_GetPaletteEntries);
	//oSetCurrentTexturePalette = (tSetCurrentTexturePalette)DetourFunction((PBYTE)pdwNewDevice[73], (PBYTE)hkIDirect3DDevice9_SetCurrentTexturePalette);
	//oGetCurrentTexturePalette = (tGetCurrentTexturePalette)DetourFunction((PBYTE)pdwNewDevice[74], (PBYTE)hkIDirect3DDevice9_GetCurrentTexturePalette);
	//oSetScissorRect = (tSetScissorRect)DetourFunction((PBYTE)pdwNewDevice[75], (PBYTE)hkIDirect3DDevice9_SetScissorRect);
	//oGetScissorRect = (tGetScissorRect)DetourFunction((PBYTE)pdwNewDevice[76], (PBYTE)hkIDirect3DDevice9_GetScissorRect);
	//oSetSoftwareVertexProcessing = (tSetSoftwareVertexProcessing)DetourFunction((PBYTE)pdwNewDevice[77], (PBYTE)hkIDirect3DDevice9_SetSoftwareVertexProcessing);
	//oGetSoftwareVertexProcessing = (tGetSoftwareVertexProcessing)DetourFunction((PBYTE)pdwNewDevice[78], (PBYTE)hkIDirect3DDevice9_GetSoftwareVertexProcessing);
	//oSetNPatchMode = (tSetNPatchMode)DetourFunction((PBYTE)pdwNewDevice[79], (PBYTE)hkIDirect3DDevice9_SetNPatchMode);
	//oGetNPatchMode = (tGetNPatchMode)DetourFunction((PBYTE)pdwNewDevice[80], (PBYTE)hkIDirect3DDevice9_GetNPatchMode);
	oDrawPrimitive = (tDrawPrimitive)DetourFunction((PBYTE)pdwNewDevice[81], (PBYTE)hkIDirect3DDevice9_DrawPrimitive);
	//oDrawIndexedPrimitive = (tDrawIndexedPrimitive)DetourFunction((PBYTE)pdwNewDevice[82], (PBYTE)hkIDirect3DDevice9_DrawIndexedPrimitive);
	oDrawPrimitiveUP = (tDrawPrimitiveUP)DetourFunction((PBYTE)pdwNewDevice[83], (PBYTE)hkIDirect3DDevice9_DrawPrimitiveUP);
	oDrawIndexedPrimitiveUP = (tDrawIndexedPrimitiveUP)DetourFunction((PBYTE)pdwNewDevice[84], (PBYTE)hkIDirect3DDevice9_DrawIndexedPrimitiveUP);
	//oProcessVertices = (tProcessVertices)DetourFunction((PBYTE)pdwNewDevice[85], (PBYTE)hkIDirect3DDevice9_ProcessVertices);
	//oCreateVertexDeclaration = (tCreateVertexDeclaration)DetourFunction((PBYTE)pdwNewDevice[86], (PBYTE)hkIDirect3DDevice9_CreateVertexDeclaration);
	//oSetVertexDeclaration = (tSetVertexDeclaration)DetourFunction((PBYTE)pdwNewDevice[87], (PBYTE)hkIDirect3DDevice9_SetVertexDeclaration);
	//oGetVertexDeclaration = (tGetVertexDeclaration)DetourFunction((PBYTE)pdwNewDevice[88], (PBYTE)hkIDirect3DDevice9_GetVertexDeclaration);
	//oSetFVF = (tSetFVF)DetourFunction((PBYTE)pdwNewDevice[89], (PBYTE)hkIDirect3DDevice9_SetFVF);
	//oGetFVF = (tGetFVF)DetourFunction((PBYTE)pdwNewDevice[90], (PBYTE)hkIDirect3DDevice9_GetFVF);
	oCreateVertexShader = (tCreateVertexShader)DetourFunction((PBYTE)pdwNewDevice[91], (PBYTE)hkIDirect3DDevice9_CreateVertexShader);
	oSetVertexShader = (tSetVertexShader)DetourFunction((PBYTE)pdwNewDevice[92], (PBYTE)hkIDirect3DDevice9_SetVertexShader);
	//oGetVertexShader = (tGetVertexShader)DetourFunction((PBYTE)pdwNewDevice[93], (PBYTE)hkIDirect3DDevice9_GetVertexShader);
	//oSetVertexShaderConstantF = (tSetVertexShaderConstantF)DetourFunction((PBYTE)pdwNewDevice[94], (PBYTE)hkIDirect3DDevice9_SetVertexShaderConstantF);
	//oGetVertexShaderConstantF = (tGetVertexShaderConstantF)DetourFunction((PBYTE)pdwNewDevice[95], (PBYTE)hkIDirect3DDevice9_GetVertexShaderConstantF);
	//oSetVertexShaderConstantI = (tSetVertexShaderConstantI)DetourFunction((PBYTE)pdwNewDevice[96], (PBYTE)hkIDirect3DDevice9_SetVertexShaderConstantI);
	//oGetVertexShaderConstantI = (tGetVertexShaderConstantI)DetourFunction((PBYTE)pdwNewDevice[97], (PBYTE)hkIDirect3DDevice9_GetVertexShaderConstantI);
	//oSetVertexShaderConstantB = (tSetVertexShaderConstantB)DetourFunction((PBYTE)pdwNewDevice[98], (PBYTE)hkIDirect3DDevice9_SetVertexShaderConstantB);
	//oGetVertexShaderConstantB = (tGetVertexShaderConstantB)DetourFunction((PBYTE)pdwNewDevice[99], (PBYTE)hkIDirect3DDevice9_GetVertexShaderConstantB);
	//oSetStreamSource = (tSetStreamSource)DetourFunction((PBYTE)pdwNewDevice[100], (PBYTE)hkIDirect3DDevice9_SetStreamSource);
	//oGetStreamSource = (tGetStreamSource)DetourFunction((PBYTE)pdwNewDevice[101], (PBYTE)hkIDirect3DDevice9_GetStreamSource);
	//oSetStreamSourceFreq = (tSetStreamSourceFreq)DetourFunction((PBYTE)pdwNewDevice[102], (PBYTE)hkIDirect3DDevice9_SetStreamSourceFreq);
	//oGetStreamSourceFreq = (tGetStreamSourceFreq)DetourFunction((PBYTE)pdwNewDevice[103], (PBYTE)hkIDirect3DDevice9_GetStreamSourceFreq);
	//oSetIndices = (tSetIndices)DetourFunction((PBYTE)pdwNewDevice[104], (PBYTE)hkIDirect3DDevice9_SetIndices);
	//oGetIndices = (tGetIndices)DetourFunction((PBYTE)pdwNewDevice[105], (PBYTE)hkIDirect3DDevice9_GetIndices);
	oCreatePixelShader = (tCreatePixelShader)DetourFunction((PBYTE)pdwNewDevice[106], (PBYTE)hkIDirect3DDevice9_CreatePixelShader);
	oSetPixelShader = (tSetPixelShader)DetourFunction((PBYTE)pdwNewDevice[107], (PBYTE)hkIDirect3DDevice9_SetPixelShader);
	//oGetPixelShader = (tGetPixelShader)DetourFunction((PBYTE)pdwNewDevice[108], (PBYTE)hkIDirect3DDevice9_GetPixelShader);
	//oSetPixelShaderConstantF = (tSetPixelShaderConstantF)DetourFunction((PBYTE)pdwNewDevice[109], (PBYTE)hkIDirect3DDevice9_SetPixelShaderConstantF);
	//oGetPixelShaderConstantF = (tGetPixelShaderConstantF)DetourFunction((PBYTE)pdwNewDevice[110], (PBYTE)hkIDirect3DDevice9_GetPixelShaderConstantF);
	//oSetPixelShaderConstantI = (tSetPixelShaderConstantI)DetourFunction((PBYTE)pdwNewDevice[111], (PBYTE)hkIDirect3DDevice9_SetPixelShaderConstantI);
	//oGetPixelShaderConstantI = (tGetPixelShaderConstantI)DetourFunction((PBYTE)pdwNewDevice[112], (PBYTE)hkIDirect3DDevice9_GetPixelShaderConstantI);
	//oSetPixelShaderConstantB = (tSetPixelShaderConstantB)DetourFunction((PBYTE)pdwNewDevice[113], (PBYTE)hkIDirect3DDevice9_SetPixelShaderConstantB);
	//oGetPixelShaderConstantB = (tGetPixelShaderConstantB)DetourFunction((PBYTE)pdwNewDevice[114], (PBYTE)hkIDirect3DDevice9_GetPixelShaderConstantB);
	//oDrawRectPatch = (tDrawRectPatch)DetourFunction((PBYTE)pdwNewDevice[115], (PBYTE)hkIDirect3DDevice9_DrawRectPatch);
	//oDrawTriPatch = (tDrawTriPatch)DetourFunction((PBYTE)pdwNewDevice[116], (PBYTE)hkIDirect3DDevice9_DrawTriPatch);
	//oDeletePatch = (tDeletePatch)DetourFunction((PBYTE)pdwNewDevice[117], (PBYTE)hkIDirect3DDevice9_DeletePatch);
	//oCreateQuery = (tCreateQuery)DetourFunction((PBYTE)pdwNewDevice[118], (PBYTE)hkIDirect3DDevice9_CreateQuery);

	// Init function
	hkIDirect3DDevice9_IDirect3DDevice9(*ppReturnedDeviceInterface);

	return hRet;
}

typedef IDirect3D9 *(APIENTRY *tDirect3DCreate9)(UINT);
tDirect3DCreate9 oDirect3DCreate9;
IDirect3D9 *APIENTRY myDirect3DCreate9(UINT SDKVersion)
{
	ADD_LOG("myDirect3DCreate9()")
	IDirect3D9 *d3dint = oDirect3DCreate9(SDKVersion);

	// do not need this hook anymore so remove detour 
	DetourRemove((PBYTE)oDirect3DCreate9, (PBYTE)myDirect3DCreate9);

	// Hook CreateDevice
	DWORD* pdwd3dint = (DWORD*)d3dint;
	pdwd3dint = (DWORD*)pdwd3dint[0];

	
	oCreateDevice = (tCreateDevice)DetourFunction((PBYTE)pdwd3dint[16], (PBYTE)myCreateDevice);
	ADD_LOG("Detoured CreateDevice")

	return d3dint;
}

DWORD WINAPI HookD3D9(LPVOID)
{

	// Hook Create Direct3DCreate9, which will hook CreateDevice to get the device
	// Then hook other functions in CreateDevice
	// Need to hook early to catch CreateVertexShader calls
	HMODULE hMod = LoadLibrary("d3d9.dll");
	oDirect3DCreate9 = (tDirect3DCreate9)DetourFunction((PBYTE)GetProcAddress(hMod, "Direct3DCreate9"), 
														(PBYTE)myDirect3DCreate9);
	ADD_LOG("Detoured Direct3DCreate9")

	return 0;
}

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hDll);
		open_log();
		ADD_LOG("DLL STARTED")
		HookD3D9(NULL);
		//CreateThread(NULL, NULL, HookD3D9, NULL, NULL, NULL);
		break;
	case DLL_PROCESS_DETACH:
		close_log();
		break;
	}

	return TRUE;
}