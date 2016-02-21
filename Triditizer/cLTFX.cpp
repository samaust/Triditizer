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
#include <windows.h>
#include "cltfx.h"
#include <stdio.h>
#include <math.h>

#include "cLTFx.h"


cMenu::cMenu(int MAX_MENU, int MAX_GROUP, int MAX_DRAW)
{
	MAX_ITEMS = 0; // Current number of items
	MAX_GROUPS = 0;
	menuItem = new pItem[MAX_MENU];
	menuGroup = new pGroup[MAX_GROUP];
	this->bMenu = true;
	height = 30;
	itemOffset = 0;
	MaxItemsToDrawAtOnce = MAX_DRAW;
}

cMenu::~cMenu()
{
	MAX_ITEMS = 0;
	MAX_GROUPS = 0;
	delete        menuItem;
	delete        menuGroup;
}

void cMenu::SetActiveMode(bool m_bMode) { bMenu = m_bMode; }
void cMenu::SetColor(int type, const D3DCOLOR color) { MenuColors[type] = 0; MenuColors[type] = color; }
void cMenu::SetInitialCoords(int x, int y) { menuPos.x = (x > 0) ? x : 1; menuPos.y = (y > 0) ? y : 1; }
//void cMenu::SetSettingFile(char *SettingsFile) { strncpy(szSettingsFile, SettingsFile, 256); bCanUseSettings = true; }
void cMenu::InitializeMenu(void) { bInitialized = true; }

void cMenu::InitalizeFont(char *font, int size, IDirect3DDevice9* pD3Ddev)
{
	HRESULT hr = D3DXCreateFont(pD3Ddev,            // D3D device
		size,               // Height
		0,                     // Width
		FW_BOLD,               // Weight
		1,                     // MipLevels, 0 = autogen mipmaps
		FALSE,                 // Italic
		DEFAULT_CHARSET,       // CharSet
		OUT_DEFAULT_PRECIS,    // OutputPrecision
		DEFAULT_QUALITY,       // Quality
		DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
		font,              // pFaceName
		&menuFont);              // ppFont
	//menuFont->InitDeviceObjects(pD3Ddev);
	//menuFont->RestoreDeviceObjects();
	this->FontCreated = true;
	this->firstInit = true;
}

void cMenu::PreReset(void)
{
	if (menuFont)
	{
		menuFont->OnLostDevice();
		menuFont->OnResetDevice();
		//menuFont = NULL;
		//this->FontCreated = false;
	}
}

void cMenu::PostReset(char *font, int size, IDirect3DDevice9* pD3Ddev)
{
	if (menuFont == NULL)
	{
		HRESULT hr = D3DXCreateFont(pD3Ddev,            // D3D device
			size,               // Height
			0,                     // Width
			FW_BOLD,               // Weight
			1,                     // MipLevels, 0 = autogen mipmaps
			FALSE,                 // Italic
			DEFAULT_CHARSET,       // CharSet
			OUT_DEFAULT_PRECIS,    // OutputPrecision
			DEFAULT_QUALITY,       // Quality
			DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
			font,              // pFaceName
			&menuFont);              // ppFont
		//menuFont->InitDeviceObjects(pD3Ddev);
		//menuFont->RestoreDeviceObjects();
		this->FontCreated = true;
	}
}

int cMenu::AddMenuGroup(const char title[20])
{
	if (!bInitialized)return -1;

	int index = MAX_GROUPS;
	strncpy(menuGroup[index].title, title, 20);
	menuGroup[index].value = 0.00f;
	menuGroup[index].GroupID = index;
	menuGroup[index].min = 0;
	menuGroup[index].max = 1;

	if (!menuGroup[index].settings && bCanUseSettings)
	{
		char VarKey[20] = { 0 };
		strncpy(VarKey, title, 20);
		//tools.RemoveChar(VarKey, '[');
		//tools.RemoveChar(VarKey, ']');
		menuGroup[index].settings = true;
		//menuGroup[index].value = (float)tools.iGetValue(MenuGroupOptions, VarKey, 0, szSettingsFile);
		if (menuGroup[index].value > menuGroup[index].max)
			menuGroup[index].value = (float)menuGroup[index].max;
		if (menuGroup[index].value < menuGroup[index].min)
			menuGroup[index].value = (float)menuGroup[index].min;
	}
	MAX_GROUPS++;
	return index;
}

int cMenu::AddMenuItem(const char title[20], float *cvar, int min, int max, int GroupID, int vKey, char *Opt, ...)
{
	if (!bInitialized)return -1;
	if (MAX_GROUPS < GroupID)return -1;

	int index = MAX_ITEMS;
	strncpy(menuItem[index].title, title, 20);
	menuItem[index].value = cvar;
	menuItem[index].max = max;
	menuItem[index].min = min;
	menuItem[index].GroupID = GroupID;

	if (!menuItem[index].settings && bCanUseSettings)
	{
		char VarKey[20] = { 0 };
		strncpy(VarKey, title, 20);
		//tools.RemoveChar(VarKey, '[');
		//tools.RemoveChar(VarKey, ']');
		menuItem[index].settings = true;
		//menuItem[index].value[0] = (float)tools.iGetValue(MenuItemOptions, VarKey, 0, szSettingsFile);
		if (menuItem[index].value[0] > max)
			menuItem[index].value[0] = (float)max;
		if (menuItem[index].value[0] < min)
			menuItem[index].value[0] = (float)min;
	}
	if (vKey == 0 && bCanUseSettings)
	{
		char VarKey[20] = { 0 };
		strncpy(VarKey, title, 20);
		//tools.RemoveChar(VarKey, '[');
		//tools.RemoveChar(VarKey, ']');
		//menuItem[index].vKey = tools.iGetValue(MenuHotkeys, VarKey, 0, szSettingsFile);
	}
	else
		menuItem[index].vKey = vKey;

	if (Opt != NULL)
	{
		menuItem[index].useDesc = true;
		va_list vl;
		int i = 0;
		char* str = Opt;
		va_start(vl, Opt);
		do {
			strncpy(menuItem[index].desc[i], str, 50);
			str = va_arg(vl, char*);
			i++;
		} while (str != NULL);
		va_end(vl);
	}
	else
		menuItem[index].useDesc = false;

	MAX_ITEMS++;
	return index;
}

int cMenu::GetMaxItemsToDraw(void)
{
	if (!bInitialized)return -1;

	int ItemsToDraw = 0;
	for (int i = 0; i < MAX_GROUPS; i++)
	{
		ItemsToDraw++;
		if (menuGroup[i].value)
		{
			for (int f = 0; f < MAX_ITEMS; f++)
			{
				if (menuItem[f].GroupID == menuGroup[i].GroupID)
					ItemsToDraw++;
			}
		}
	}
	return ItemsToDraw - 1;
}

int cMenu::GetMaxItemsInGroup(int index)
{
	if (!bInitialized)return -1;

	int Items = 0;
	for (int i = 0; i <= (MAX_ITEMS - 1); i++)
	{
		if (menuItem[i].GroupID == index)
			Items++;
	}
	return Items;
}

int cMenu::GetRealIterator(int Item, int &Type)
{
	if (!bInitialized)return false;

	int Items = 0;

	for (int i = 0; i <= (MAX_GROUPS - 1); i++)
	{
		if (Items == Item)
		{
			Type = T_GROUP;
			return i;
		}
		Items++;

		if (menuGroup[i].value)
		{
			for (int f = 0; f <= (MAX_ITEMS - 1); f++)
			{
				if (menuItem[f].GroupID == menuGroup[i].GroupID)
				{
					if (Items == Item)
					{
						Type = T_ITEM;
						return f;
					}
					Items++;
				}
			}
		}
	}
	return -1;
}

void cMenu::DisableAllMenuItems()
{
	if (!bInitialized)return;

	curItem = 0;

	for (int i = 0; i <= (MAX_ITEMS - 1); i++)
		menuItem[i].value[0] = 0.00f;

	for (int i = 0; i <= (MAX_GROUPS - 1); i++)
		menuGroup[i].value = 0.00f;
}

bool cMenu::IsMouseInVector(int x, int y, int w, int h)
{
	return ((m_mouse.x >= x) && (m_mouse.x <= (x + w)) && (m_mouse.y >= y) && (m_mouse.y <= (y + h))) ? true : false;
}

void cMenu::DrawMenu(char* title, IDirect3DDevice9* pDevice)
{
	if (!bMenu || !bInitialized)return;
	int x = menuPos.x;
	int y = menuPos.y;
	int curDrawItem = 0 + itemOffset;

	int MaxItems = min(GetMaxItemsToDraw(), MaxItemsToDrawAtOnce - 1);
	
	//if ((MaxItems < 1) || (MAX_GROUPS < 1) || (MAX_ITEMS < 1)) // Fails with one group
	//	return;
	/*====================================================*/
	DrawBox((x - 10), y, 240, 49, MenuColors[C_BOX_BORDER], MenuColors[C_BOX], pDevice);
	DrawGameTextPixel((x + 3), (y + 4), MenuColors[C_BOX], title); y += 60;
	DrawBox((x - 10), (y - 10), 240, ((MaxItems * height) + 60), MenuColors[C_BOX_BORDER], MenuColors[C_BOX], pDevice);
	/*====================================================*/
	
	for (int i = 0; i <= (MAX_GROUPS - 1); i++)
	{
		D3DCOLOR GroupColor = MenuColors[C_DEFAULT];
		if (IsMouseInVector((menuPos.x + 5), ((menuPos.y + 36) + (curDrawItem * height)), 120, 10))
			GroupColor = MenuColors[C_MOUSEOVER];
		else if (curDrawItem == curItem)
			GroupColor = MenuColors[C_SELECTED];
		else
			GroupColor = MenuColors[C_GROUP];

		// Draw group title if it's in the visible range
		if ((curDrawItem >= 0) && (curDrawItem < MaxItemsToDrawAtOnce))
		{
			DrawGameTextPixel(x, y + (curDrawItem * height), GroupColor, menuGroup[i].title);
			DrawGameTextPixel(x + 210, y + (curDrawItem * height), GroupColor, (menuGroup[i].value) ? "+" : "-");
		}
		curDrawItem++;

		if (menuGroup[i].value)
		{
			for (int f = 0; f <= (MAX_ITEMS - 1); f++)
			{
				if (menuItem[f].GroupID == menuGroup[i].GroupID)
				{
					D3DCOLOR DrawColor = MenuColors[C_DEFAULT];
					//if (IsMouseInVector((menuPos.x + 5), ((menuPos.y + 36) + (curDrawItem * height)), 120, 10))
					//	DrawColor = MenuColors[C_MOUSEOVER];
					if (curDrawItem == (curItem + itemOffset))
						DrawColor = MenuColors[C_SELECTED];
					else if (menuItem[f].value[0])
						DrawColor = MenuColors[C_ACTIVATED];

					if ((curDrawItem >= 0) && (curDrawItem < MaxItemsToDrawAtOnce))
					{
						DrawGameTextPixel(x + 10, y + (curDrawItem * height), DrawColor, menuItem[f].title);
						if ((int)menuItem[f].max != 1 && (int)menuItem[f].value[0] != 0)
						{
							if (menuItem[f].useDesc)
								DrawGameTextPixel(x + 130, y + (curDrawItem * height), DrawColor, menuItem[f].desc[(int)menuItem[f].value[0] - 1]);
							else
								DrawGameTextPixel(x + 130, y + (curDrawItem * height), DrawColor, "%2.2f", menuItem[f].value[0]);
						}
						else {
							DrawGameTextPixel(x + 130, y + (curDrawItem * height), DrawColor, ((int)menuItem[f].value[0]) ? "On" : "Off");
						}
					}
					curDrawItem++;
				}
			}
		}
	}
}

void cMenu::HandleItemHotkeys(WPARAM wKey)
{
	if (!bInitialized)return;

	for (int i = 0; i <= (MAX_ITEMS - 1); i++)
	{
		if (menuItem[i].vKey == NULL)continue;
		//if (wKey == menuItem[i].vKey)
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			if (GetAsyncKeyState(menuItem[i].vKey))
			{
				float iNextValue = (menuItem[i].value[0] + 1);

				if (iNextValue <= menuItem[i].max)
				{
					menuItem[i].value[0]++;
				}

				if (iNextValue > menuItem[i].max)
				{
					menuItem[i].value[0] = (float)menuItem[i].min;
				}
			}
		}
	}
}

void cMenu::HandleKeyNavigation(WPARAM wKey)
{
	if (bInitialized == false)return;

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			bMenu = !bMenu;
		}

		POINT mousepos;
		GetCursorPos(&mousepos);
		m_mouse.x = mousepos.x;
		m_mouse.y = mousepos.y;

		if (bMenu == false)return;

		if (GetAsyncKeyState(VK_UP) & 1)
		{
			if (curItem > 0)
				curItem--;

			if ((curItem + itemOffset) == -1) // move menu
				itemOffset++;
		}

		if (GetAsyncKeyState(VK_DOWN) & 1)
		{
			if (curItem < GetMaxItemsToDraw())
				curItem++;

			if ((curItem + itemOffset) == (MaxItemsToDrawAtOnce)) // move menu
				itemOffset--;
		}

		if (GetAsyncKeyState(VK_RIGHT) & 1)
		{
			int iType = -1;
			int iterator = GetRealIterator(curItem, iType);
			if (iterator == -1)return;

			if (iType == T_GROUP)
			{
				if (menuGroup[iterator].value < menuGroup[iterator].max)
				{
					menuGroup[iterator].value++;
				}
			}
			else if (iType == T_ITEM) {
				if (menuItem[iterator].value[0] < menuItem[iterator].max)
				{
					menuItem[iterator].value[0]++;
				}
			}
		}

		if (GetAsyncKeyState(VK_LEFT) & 1)
		{
			int iType = -1;
			int iterator = GetRealIterator(curItem, iType);
			if (iterator == -1)return;

			if (iType == T_GROUP)
			{
				if (menuGroup[iterator].value > menuGroup[iterator].min)
				{
					menuGroup[iterator].value--;
				}
			}
			else if (iType == T_ITEM) {
				if (menuItem[iterator].value[0] > menuItem[iterator].min)
				{
					menuItem[iterator].value[0]--;
				}
			}
		}
	}
}

//void cMenu::HandleMouseNavigation(WPARAM wKey)
//{
//	if (bMenu == false || bInitialized == false || moving[0] || moving[1] || moving[2])return;
//
//	int curNavItem = 0;
//	for (int i = 0; i <= (MAX_GROUPS - 1); i++)
//	{
//		if (IsMouseInVector((menuPos.x + 5), ((menuPos.y + 36) + (curNavItem * height)), 120, 10) && GetAsyncKeyState(VK_LBUTTON) & 1)
//		{
//			//Groups
//			float fNextValue = (menuGroup[i].value + 1);
//
//			if (fNextValue <= menuGroup[i].max)
//			{
//				menuGroup[i].value++;
//				if (curItem > curNavItem)curItem += GetMaxItemsInGroup(menuGroup[i].GroupID);
//			}
//
//			if (fNextValue > menuGroup[i].max)
//			{
//				menuGroup[i].value = (float)menuGroup[i].min;
//				if (curItem > curNavItem)curItem -= GetMaxItemsInGroup(menuGroup[i].GroupID);
//			}
//
//			if (curItem < 0)curItem = 0;
//			if (curItem > GetMaxItemsToDraw())curItem = GetMaxItemsToDraw();
//		}
//		curNavItem++;
//
//		if (menuGroup[i].value)
//		{
//			for (int f = 0; f <= (MAX_ITEMS - 1); f++)
//			{
//				if (menuItem[f].GroupID == menuGroup[i].GroupID)
//				{
//					if (IsMouseInVector((menuPos.x + 5), ((menuPos.y + 36) + (curNavItem * height)), 120, 10) && GetAsyncKeyState(VK_LBUTTON) & 1)
//					{
//						//MenuItems
//						float fNextValue = (menuItem[f].value[0] + 1);
//
//						if (fNextValue <= menuItem[f].max)
//							menuItem[f].value[0]++;
//
//						if (fNextValue > menuItem[f].max)
//							menuItem[f].value[0] = (float)menuItem[f].min;
//					}
//					curNavItem++;
//				}
//			}
//		}
//	}
//}

void cMenu::DragMenuWindow(void)
{
	if (bMenu == false || bInitialized == false || menu.moving[1] || menu.moving[2])return;

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		if (IsMouseInVector(menuPos.x - 20, menuPos.y - 20, 170, 40))
		{
			menuPos.x = (m_mouse.x - 30);
			menuPos.y = (m_mouse.y - 10);
			moving[0] = true;
		}
	}
	else {
		moving[0] = false;
	}
}

void cMenu::HandleKeys(WPARAM wParams, LPARAM lParams, UINT msg)
{
	//store the left click state for single-click checking
	bool m_bLeftClickOldState = m_mouse.m_bIsLeftSingleClicked;

	//clear the old data...sucka!
	m_mouse.m_bIsRightClicked = false;
	m_mouse.m_bIsLeftSingleClicked = false;
	m_mouse.m_bIsMiddleClicked = false;
	m_mouse.m_bIsLeftHeld = false;

	if (msg == WM_KEYDOWN)
	{
		if (wParams == VK_INSERT || wParams == VK_F12)
		{
			SetActiveMode(!bMenu);
		}
		else {
			HandleKeyNavigation(wParams);
			HandleItemHotkeys(wParams);
		}
	}

	if (msg == WM_NCHITTEST)
	{
		m_mouse.x = ((int)(short)LOWORD(lParams));
		m_mouse.y = ((int)(short)HIWORD(lParams));
	}
	//when we don't check msg, this means its being held ( useful for dragging windows )
	m_mouse.m_bIsRightClicked = (wParams == MK_RBUTTON) ? true : false;
	m_mouse.m_bIsLeftHeld = (wParams == MK_LBUTTON) ? true : false;
	m_mouse.m_bIsMiddleClicked = (wParams == MK_MBUTTON) ? true : false;

	if (m_bLeftClickOldState == false)
	{
		m_mouse.m_bIsLeftSingleClicked = (msg == WM_LBUTTONDOWN && wParams == MK_LBUTTON);
	}
	DragMenuWindow();
	//HandleMouseNavigation(wParams);
}

//Draw Funcs
void cMenu::DrawText1(int x, int y, DWORD color, char *szFormat, ...)
{
	if (!szFormat)return;

	char logbuf[256] = { 0 };

	va_list va_alist;
	va_start(va_alist, szFormat);
	_vsnprintf(logbuf, sizeof(logbuf), szFormat, va_alist);
	va_end(va_alist);

	RECT font_rect;
	SetRect(&font_rect, x, y, 0+1, 0+1); // Don't understand how rect work with DrawText
	//menuFont->DrawText((FLOAT)x, (FLOAT)y, color, logbuf);
	HRESULT hr = menuFont->DrawText(NULL, logbuf, -1, &font_rect, DT_NOCLIP, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
}

void cMenu::DrawGameTextPixel(int x, int y, DWORD color, char *szFormat, ...)
{
	if (!szFormat)return;

	char logbuf[256] = { 0 };

	va_list va_alist;
	va_start(va_alist, szFormat);
	_vsnprintf(logbuf, sizeof(logbuf), szFormat, va_alist);
	va_end(va_alist);

	RECT font_rect1;
	SetRect(&font_rect1, x, y, x + 1, y + 1);
	HRESULT hr1 = menuFont->DrawText(NULL, logbuf, -1, &font_rect1, DT_NOCLIP, color);
}

//int cMenu::GetTextWidth(char* text)
//{
//	SIZE Size = { 0,0 };
//	if (menuFont->GetTextExtent(text, &Size) == S_OK)
//		return Size.cx;
//	return 0;
//}

void cMenu::FillRGB(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* pDevice)
{
	if (w < 0)w = 1;
	if (h < 0)h = 1;
	if (x < 0)x = 1;
	if (y < 0)y = 1;

	D3DRECT rec = { x, y, x + w, y + h };
	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, color, 0, 0);
}

void cMenu::FillARGB(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* pDevice)
{
	//Transparent Draw Func by Hans211, removed.
	//do it yourself you dont really need it...
	FillRGB(x, y, w, h, color, pDevice);
}

void cMenu::FillARGBGradient(int x, int y, int w, int h, D3DCOLOR color1, DWORD color2, IDirect3DDevice9* pDevice)
{
	//removed.
	//do it yourself you dont really need it...
}

void cMenu::DrawBorder(int x, int y, int w, int h, int px, D3DCOLOR BorderColor, IDirect3DDevice9* pDevice)
{
	FillARGB(x, (y + h - px), w, px, BorderColor, pDevice);
	FillARGB(x, y, px, h, BorderColor, pDevice);
	FillARGB(x, y, w, px, BorderColor, pDevice);
	FillARGB((x + w - px), y, px, h, BorderColor, pDevice);
}

void cMenu::DrawBox(int x, int y, int w, int h, D3DCOLOR BoxColor, D3DCOLOR BorderColor, IDirect3DDevice9* pDevice)
{
	FillARGB(x, y, w, h, BoxColor, pDevice);
	DrawBorder(x, y, w, h, 1, BorderColor, pDevice);
}

void cMenu::DrawHealthBox(int x, int y, DWORD m_dColorOut, DWORD m_dColorIn, int m_iHealth, int m_iMaxHealth, IDirect3DDevice9* pDevice)
{
	float mx = (float)m_iMaxHealth / 4;
	float w = (float)m_iHealth / 4;
	x -= ((int)mx / 2);

	//background
	FillARGB(x, y, (int)mx, 4, m_dColorOut, pDevice);
	//inside
	FillARGB(x, y, (int)w, 4, m_dColorIn, pDevice);
	//outline
	DrawBorder(x - 1, y - 1, (int)mx + 2, 6, 1, D3DCOLOR_ARGB(255, 30, 30, 30), pDevice);
}

void cMenu::DrawMouse(IDirect3DDevice9* pDevice, bool DrawIt)
{
	if (bMenu == false && !DrawIt)return;

	DWORD black = D3DCOLOR_ARGB(255, 0, 0, 0);
	DWORD color = D3DCOLOR_ARGB(255, 0, 0, 255);
	int x = m_mouse.x, y = m_mouse.y;

	//removed, thats a stylie thing that you can do yourself :)
}

void cMenu::DrawRadar(int x, int y, int size, IDirect3DDevice9* pDevice)
{
	//not needed on the menu but well i used it...
	//int x = 300, y = 300, size = 150;
	DrawBox(x, y, size, size, D3DCOLOR_ARGB(150, 51, 51, 51), D3DCOLOR_ARGB(250, 25, 220, 251), pDevice);
	DrawBox((x + 5), (y + 5), (size - 10), (size - 10), D3DCOLOR_ARGB(255, 31, 31, 31), D3DCOLOR_ARGB(255, 184, 184, 184), pDevice);
	FillARGB((x + (size / 2)), (y + 6), 1, (size - 12), D3DCOLOR_ARGB(255, 0, 255, 0), pDevice);
	FillARGB((x + 6), (y + (size / 2)), (size - 12), 1, D3DCOLOR_ARGB(255, 0, 255, 0), pDevice);
}

void cMenu::DrawDot(int x, int y, int r, int g, int b, IDirect3DDevice9* pDevice)
{
	DWORD color = D3DCOLOR_ARGB(255, r, g, b);
	FillRGB(x - 1, y - 1, 2, 2, color, pDevice);
}

void cMenu::DrawLine(int x1, int y1, int x2, int y2, D3DCOLOR col, IDirect3DDevice9* pDevice)
{
	const DWORD D3D_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	LineVertex points[2] =
	{
		{ (float)x1, (float)y1, 0.0f, 0.0f, col, 1.0f, 1.0f },
		{ (float)x2, (float)y2, 0.0f, 0.0f, col, 1.0f, 1.0f },
	};
	//allow for translucency
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pDevice->SetRenderState(D3DRS_FOGENABLE, false);

	pDevice->SetFVF(D3D_FVF);
	pDevice->SetTexture(0, NULL);
	pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, points, sizeof(LineVertex));
}

LineVertex cMenu::CreateD3DTLVERTEX(float X, float Y, float Z, float RHW, D3DCOLOR color, float U, float V) { LineVertex v = { X,Y,Z,RHW,color,U,V }; return v; }

void cMenu::DrawCircle(int x, int y, float radius, D3DCOLOR color, IDirect3DDevice9* pDevice)
{
	//cool for Sphere ESP :D
	const DWORD D3D_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	const int NUMPOINTS = 24;
	const float PI = (float)3.141592654f;
	LineVertex Circle[NUMPOINTS + 1];
	float X, Y, Theta, WedgeAngle;
	WedgeAngle = (float)((2 * PI) / NUMPOINTS);

	for (int i = 0; i <= NUMPOINTS; i++)
	{
		Theta = i * WedgeAngle;
		X = (float)(x + radius * cos(Theta));
		Y = (float)(y - radius * sin(Theta));
		Circle[i] = CreateD3DTLVERTEX(X, Y, 0.0f, 1.0f, color, 0.0f, 0.0f);
	}
	pDevice->SetFVF(D3D_FVF);
	pDevice->SetTexture(0, NULL);
	pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, NUMPOINTS, &Circle[0], sizeof(Circle[0]));
}

int cMenu::getItemsCount()
{
	return MAX_ITEMS;
}