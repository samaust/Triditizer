#pragma once

/*LTFX Menu Advanced
s0beit & GHOSTER - created the first version of LTFX Menu
NeoIII             - ReCoded full menu:
- added automated Item Settings loading and saving option.
- added possibility to add random count of 'describe' chars for addmenuitem func.
- added Menu Group system.
- added menuitem Hotkeys. ( i am not anymore sure if they was there on original version )
- added Drag&Drop funcionality.
- added navigation support for mouse & keys +  handling keys with 'ProcKeyHook' ( SetWindowLong( ... ) on the Game Window )
- tweaked and changed here and there... you can look yourself...
Hans211             - Transparent Draw Functions ( i left them out here, sry )
Last Update         - 01.01.2010 by NeoIII
Credits : http://www.unknowncheats.me/forum/c-and-c-/62587-tweaked-ltfx-menu-dx-8-9-a.html
Modified by samaust
*/

#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <stdio.h>

#ifndef _PI
#define _PI ((FLOAT)3.141592654f)
#endif

#ifndef DEGTORAD
#define DEGTORAD(degree) ((PI / 180.0f) * (degree))
#endif

#ifndef DEGTORAD
#define RADTODEG(radian) ((180.0f / PI) * (radian))
#endif

#define MenuGroupOptions "MENUGROUP_OPTIONS"///*MENUGROUP_OPTIONS*/XorStr<0x56,18,0xC317327E>("\x1B\x12\x16\x0C\x1D\x09\x13\x08\x0E\x00\x2F\x31\x36\x2A\x2B\x2B\x35"+0xC317327E).s
#define MenuItemOptions "MENUITEM_OPTIONS"///*MENUITEM_OPTIONS*/XorStr<0xC5,17,0x20CC126A>("\x88\x83\x89\x9D\x80\x9E\x8E\x81\x92\x81\x9F\x84\x98\x9D\x9D\x87"+0x20CC126A).s
#define MenuHotkeys "MENU_HOTKEYS"///*MENU_HOTKEYS*/XorStr<0x00,13,0x48B5D0AA>("\x4D\x44\x4C\x56\x5B\x4D\x49\x53\x43\x4C\x53\x58"+0x48B5D0AA).s

#ifndef CLTFX_H
#define CLTFX_H

typedef enum menu_colors_s
{
	C_DEFAULT,
	C_MOUSEOVER,
	C_SELECTED,
	C_VALUES,
	C_ACTIVATED,
	C_GROUP,
	C_BOX_BORDER,
	C_BOX
} menu_colors_t;

typedef enum menu_itemtypes_s
{
	T_ITEM,
	T_GROUP
} menu_itemtypes_t;

struct pGroup
{
	char    title[20];
	float    value;
	bool    settings;
	int        max, min, GroupID;
};

struct pItem
{
	char    title[20];
	float*    value;
	int        max, min, vKey, GroupID;
	char    desc[50][50];
	bool    useDesc, settings;
};

struct QuadVertex
{
	float x, y, z, rhw;
	DWORD dwColor;
};

struct LineVertex
{
	float x, y, z, rhw;
	D3DCOLOR color;
	float tu, tv;
};

struct Mouse
{
	bool m_bIsLeftSingleClicked, m_bIsRightClicked, m_bIsMiddleClicked, m_bIsLeftHeld, Drag, m_bIsMenuOpen;
	int x, y;
};

class cMenu
{
public:
	cMenu(int MAX_MENU, int MAX_GROUP, int MAX_DRAW);
	~cMenu();

	//==========SetUp Functions ( Call InitializeMenu or nothing will work! )
	void        SetActiveMode(bool m_bMode);
	void        SetColor(int type, const D3DCOLOR color);
	void        SetInitialCoords(int x, int y);
	//void        SetSettingFile(char *SettingsFile);
	void        InitializeMenu(void);

	//==========D3D Font Stuff
	void        InitalizeFont(char *font, int size, IDirect3DDevice9* pD3Ddev);
	void        PostReset(char *font, int size, IDirect3DDevice9* pD3Ddev);
	void        PreReset(void);

	//==========Menu & Group -Items ( MenuItems cannot be added without a valid ItemGroup! )
	int            AddMenuGroup(const char title[20]);
	int            AddMenuItem(const char title[20], float *cvar, int min, int max, int GroupID, int vKey = NULL, char *Opt = NULL, ...);
	int			   getItemsCount();

	//==========Help Functions
	void        DisableAllMenuItems();
	bool        IsMouseInVector(int x, int y, int w, int h);

	//==========Main MenuTree Draw Function
	void        DrawMenu(char* title, IDirect3DDevice9* pDevice);

	//==========Navigation & Control
	void        HandleItemHotkeys(WPARAM wKey);
	void        HandleKeyNavigation(WPARAM wKey);
	//void        HandleMouseNavigation(WPARAM wKey);
	void        DragMenuWindow(void);
	void        HandleKeys(WPARAM wParams, LPARAM lParams, UINT msg);

	//==========Drawing Stuff
	//int            GetTextWidth(char* text);
	void        DrawText1(int x, int y, DWORD color, char *szFormat, ...);
	void        DrawGameTextPixel(int x, int y, DWORD color, char *szFormat, ...);
	void        FillRGB(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* pDevice);
	void        FillARGB(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* pDevice);//removed, redirected to FillRGB()
	void        FillARGBGradient(int x, int y, int w, int h, D3DCOLOR color1, DWORD color2, IDirect3DDevice9* pDevice);//removed.
	void        DrawBorder(int x, int y, int w, int h, int px, D3DCOLOR BorderColor, IDirect3DDevice9* pDevice);
	void        DrawBox(int x, int y, int w, int h, D3DCOLOR BoxColor, D3DCOLOR BorderColor, IDirect3DDevice9* pDevice);
	void        DrawHealthBox(int x, int y, DWORD m_dColorOut, DWORD m_dColorIn, int m_iHealth, int m_iMaxHealth, IDirect3DDevice9* pDevice);
	void        DrawMouse(IDirect3DDevice9* pDevice, bool DrawIt = false);//removed.
	void        DrawRadar(int x, int y, int size, IDirect3DDevice9* pDevice);
	void        DrawDot(int x, int y, int r, int g, int b, IDirect3DDevice9* pDevice);
	void        DrawLine(int x1, int y1, int x2, int y2, D3DCOLOR col, IDirect3DDevice9* pDevice);
	void        DrawCircle(int x, int y, float radius, D3DCOLOR color, IDirect3DDevice9* pDevice);;

	bool        bMenu, FontCreated, firstInit, moving[3];
	Mouse        m_mouse;
private:
	//==========Private Help Functions
	int            GetMaxItemsToDraw(void);
	int            GetMaxItemsInGroup(int index);
	int            GetRealIterator(int Item, int &Type);
	LineVertex    CreateD3DTLVERTEX(float X, float Y, float Z, float RHW, D3DCOLOR color, float U, float V);

	ID3DXFont*    menuFont;
	pItem*        menuItem;
	pGroup*        menuGroup;
	POINT        menuPos;
	int            MAX_ITEMS, MAX_GROUPS, curItem;
	bool        bInitialized, bCanUseSettings;
	D3DCOLOR    MenuColors[10];
	int height;
	int itemOffset;
	int MaxItemsToDrawAtOnce;
	//char        szSettingsFile[256];
};

#endif

extern cMenu menu;