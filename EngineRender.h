#pragma once
#include "SDK.h"

class EngineRender
{
private:
	bool bIsCreated = false;
	bool bIsFontCreated = false;
	ISurface* m_pSurface;
	unsigned long HFONT;
	DWORD Colors[COLOR_MAX];

	void conv(DWORD color, int& r, int& g, int& b, int& a)
	{
		b = color & 255;
		color >>= 8;
		g = color & 255;
		color >>= 8;
		r = color & 255;
		color >>= 8;
		a = color & 255;
	}
public:
	enum eColors
	{
		BLACK,
		WHITE,
		RED,
		GREEN,
		DARKBLUE,
		YELLOW,
		SKYBLUE,
		PINK,
		BROWN,
		ORANGE,
		GRAY,
		MENUTOPBORDERSTART,
		MENUTOPBORDEREND,
		MENUBUTTONSTART,
		MENUBUTTONEND,
		MENUBORDER,
		MENUDEFAULT,
		MENUBACKGROUND,
		MENUTEXT,
		MENUSLIDER,
		MENUCOMBOBOXSELECTED,
		MENUCOMBOBOXNOTSELECTED,
		MENUBLOP,
		MENUBLOPSELECTED,
		MENUTITLEBG,
		BRIGHTGREEN,
		TRANSPARENT_BLACK,
		TRANSPARENT_GRAY,
		AZURE,
		GOLDENROD,
		DARKGOLDENROD,
		GOLD,
		LAVENDER,
		TEAL,
		ScoreBoardHeader,
		ScoreBoardRowEven,
		ScoreBoardRowOdd,
		COLOR_MAX,
	};

	EngineRender()
	{
		Colors[BLACK] = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
		Colors[WHITE] = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
		Colors[RED] = D3DCOLOR_COLORVALUE(1.0f, 0.0f, 0.0f, 1.0f);
		Colors[GREEN] = D3DCOLOR_COLORVALUE(0.0f, 1.0f, 0.0f, 1.0f);
		Colors[DARKBLUE] = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 1.0f, 1.0f);
		Colors[YELLOW] = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 0.0f, 1.0f);
		Colors[SKYBLUE] = D3DCOLOR_COLORVALUE(0.0f, 0.6f, 1.0f, 1.0f);
		Colors[PINK] = D3DCOLOR_COLORVALUE(1.0f, 0.0f, 0.7f, 1.0f);
		Colors[BROWN] = D3DCOLOR_COLORVALUE(0.3f, 0.1f, 0.0f, 1.0f);
		Colors[ORANGE] = D3DCOLOR_COLORVALUE(1.0f, 0.6f, 0.0f, 1.0f);
		Colors[GRAY] = D3DCOLOR_COLORVALUE(0.3f, 0.3f, 0.3f, 1.0f);
		Colors[MENUTOPBORDERSTART] = D3DCOLOR_COLORVALUE(0.5f, 0.0f, 0.0f, 1.0f);
		Colors[MENUTOPBORDEREND] = D3DCOLOR_COLORVALUE(0.3f, 0.0f, 0.0f, 1.0f);
		Colors[MENUBUTTONSTART] = D3DCOLOR_COLORVALUE(0.5f, 0.0f, 0.0f, 1.0f);
		Colors[MENUBUTTONEND] = D3DCOLOR_COLORVALUE(0.3f, 0.0f, 0.0f, 1.0f);
		Colors[MENUBORDER] = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
		Colors[MENUDEFAULT] = D3DCOLOR_COLORVALUE(0.15f, 0.15f, 0.15f, 1.0f);
		Colors[MENUBACKGROUND] = D3DCOLOR_RGBA(20, 20, 20, 255);
		Colors[MENUTEXT] = D3DCOLOR_RGBA(217, 235, 255, 255);
		Colors[MENUSLIDER] = D3DCOLOR_RGBA(54, 66, 88, 255);
		Colors[MENUCOMBOBOXNOTSELECTED] = D3DCOLOR_RGBA(56, 72, 91, 255);
		Colors[MENUCOMBOBOXSELECTED] = D3DCOLOR_RGBA(203, 210, 226, 255);
		Colors[MENUBLOP] = D3DCOLOR_RGBA(96, 96, 96, 255);
		Colors[GOLDENROD] = D3DCOLOR_RGBA(218, 165, 32, 255);
		Colors[MENUBLOPSELECTED] = D3DCOLOR_RGBA(144, 155, 173, 255);
		Colors[MENUTITLEBG] = D3DCOLOR_RGBA(71, 83, 102, 255);
		Colors[BRIGHTGREEN] = D3DCOLOR_RGBA(16, 183, 18, 255);
		Colors[DARKGOLDENROD] = D3DCOLOR_RGBA(184, 134, 11, 255);
		Colors[GOLD] = D3DCOLOR_RGBA(255, 215, 0, 255);
		Colors[AZURE] = D3DCOLOR_RGBA(240, 255, 255, 255);
		Colors[TRANSPARENT_BLACK] = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 0.3f);
		Colors[TRANSPARENT_GRAY] = D3DCOLOR_COLORVALUE(0.3f, 0.3f, 0.3f, 0.3f);
		Colors[LAVENDER] = D3DCOLOR_RGBA(230, 230, 250, 255);
		Colors[TEAL] = 0xFF696969;
		Colors[ScoreBoardHeader] = D3DCOLOR_RGBA(119, 136, 153, 255);
		Colors[ScoreBoardRowEven] = D3DCOLOR_RGBA(32, 178, 170, 255);
		Colors[ScoreBoardRowOdd] = D3DCOLOR_RGBA(173, 216, 230, 255);
	}

	void Create(ISurface* pSurface)
	{
		m_pSurface = pSurface;
		bIsCreated = true;
	}

	bool IsCreated()
	{
		return bIsCreated;
	}

	void InitFont()
	{
		HFONT = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(HFONT, "Arial Black", 16, 250, 0, 0, 0x200);
		bIsFontCreated = true;
	}

	bool IsFontCreated()
	{
		return bIsFontCreated;
	}

	unsigned long GetFont()
	{
		return HFONT;
	}

	void DrawLine(int x0, int y0, int x1, int y1, DWORD color)
	{
		int r, g, b, a;
		conv(color, r, g, b, a);
		m_pSurface->DrawSetColor(r,g,b,a);
		m_pSurface->DrawLine(x0, y0, x1, y1);
	}

	void DrawOutlinedFontText(int x, int y, DWORD color, wchar_t* format, ...)
	{
		wchar_t buffer[255] = { 0 };
		va_list args;
		va_start(args, format);
		vswprintf(buffer, format, args);
		va_end(args);

		m_pSurface->DrawSetTextFont(HFONT);
		m_pSurface->DrawSetTextPos(x, y);

		int r, g, b, a;
		conv(color, r, g, b, a);
		m_pSurface->DrawSetTextColor(r,g,b,a);
		m_pSurface->DrawPrintText(buffer, wcslen(buffer));
	}

	void DrawOutlinedFontText(int x, int y, DWORD color, const wchar_t* format, ...)
	{
		wchar_t buffer[255] = { 0 };
		va_list args;
		va_start(args, format);
		vswprintf(buffer, format, args);
		va_end(args);

		m_pSurface->DrawSetTextFont(HFONT);
		m_pSurface->DrawSetTextPos(x, y);

		int r, g, b, a;
		conv(color, r, g, b, a);
		m_pSurface->DrawSetTextColor(r, g, b, a);
		m_pSurface->DrawPrintText(buffer, wcslen(buffer));
	}

	void DrawBox(int x, int y, int width, int height, int thickness, DWORD color)
	{
		int r, g, b, a;
		conv(color, r, g, b, a);
		m_pSurface->DrawSetColor(r, g, b, a);
		if (thickness > 0)
		{
			for (int i = 0; i < thickness; i++)
			{ 
				m_pSurface->DrawOutlinedRect(x-i, y-i, x + width+i, y + height+i);
			}
		}
	}

	void DrawFilledBox(int x, int y, int width, int height, int thickness, DWORD color)
	{
		int r, g, b, a;
		conv(color, r, g, b, a);
		m_pSurface->DrawSetColor(r, g, b, a);
		if (thickness > 0)
		{
			for (int i = 0; i < thickness; i++)
			{
				m_pSurface->DrawFilledRect(x - i, y - i, x + width + i, y + height + i);
			}
		}
	}

	void DrawHealthBar(int x, int y, int w, int h, int Health)
	{
		DrawFilledBox(x, y, w, h, 1, GetColor(eColors::BLACK)); // Background

		//Calc width of HP bar relative to HP and bar width
		if (Health > 100)
			Health = 100;

		int calc = (int)(((float)(w - 2) / 100.0f) * (float)Health);

		//Fading color
		int G = (int)((float)(255 * Health) / 100.0); //Map interval linearly from [0,100] to [0,255]
		int R = 255 - G;

		DrawFilledBox(x + 1, y + 1, calc, h - 2, 1, D3DCOLOR_RGBA(R, G, 0, 255)); // health bar
	}

	DWORD GetColor(eColors color)
	{
		return Colors[color];
	}
};