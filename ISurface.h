#pragma once
#include "Utils.h"

class ISurface
{
public:
	enum FontDrawType_t
	{
		FONT_DRAW_DEFAULT = 0,
		FONT_DRAW_NONADDITIVE,
		FONT_DRAW_ADDITIVE,
		FONT_DRAW_TYPE_COUNT = 2,
	};

	enum EFontFlags
	{
		FONTFLAG_NONE,
		FONTFLAG_ITALIC = 0x001,
		FONTFLAG_UNDERLINE = 0x002,
		FONTFLAG_STRIKEOUT = 0x004,
		FONTFLAG_SYMBOL = 0x008,
		FONTFLAG_ANTIALIAS = 0x010,
		FONTFLAG_GAUSSIANBLUR = 0x020,
		FONTFLAG_ROTARY = 0x040,
		FONTFLAG_DROPSHADOW = 0x080,
		FONTFLAG_ADDITIVE = 0x100,
		FONTFLAG_OUTLINE = 0x200,
		FONTFLAG_CUSTOM = 0x400,
		FONTFLAG_BITMAP = 0x800,
	};

	void DrawSetColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* oDrawSetColor)(PVOID, int,int,int,int);
		return Utils::vfunc< oDrawSetColor >(this, 15)(this, r,g,b,a);
	}
	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* oDrawFilledRect)(PVOID, int, int, int, int);
		return Utils::vfunc< oDrawFilledRect >(this, 16)(this, x0, y0, x1, y1);
	}
	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* oDrawOutlinedRect)(PVOID, int, int, int, int);
		return Utils::vfunc< oDrawOutlinedRect >(this, 18)(this, x0, y0, x1, y1);
	}
	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* oDrawLine)(PVOID, int, int, int, int);
		return Utils::vfunc< oDrawLine >(this, 19)(this, x0, y0, x1, y1);
	}
	void DrawSetTextFont(unsigned long hFont)
	{
		typedef void(__thiscall* oDrawSetTextFont)(PVOID, unsigned long);
		return Utils::vfunc< oDrawSetTextFont >(this, 23)(this, hFont);
	}
	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* oDrawSetTextColor)(PVOID, int,int,int,int);
		return Utils::vfunc< oDrawSetTextColor >(this, 25)(this, r,g,b,a);
	}
	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall* oDrawSetTextPos)(PVOID, int, int);
		return Utils::vfunc< oDrawSetTextPos >(this, 26)(this, x, y);
	}
	void DrawPrintText(wchar_t *text, int textLen, FontDrawType_t drawType = FONT_DRAW_DEFAULT)
	{
		typedef void(__thiscall* oDrawPrintText)(PVOID, wchar_t*, int, FontDrawType_t);
		return Utils::vfunc< oDrawPrintText >(this, 28)(this, text, textLen, drawType);
	}
	unsigned long FontCreate()
	{
		typedef unsigned long(__thiscall* oCreateFont)(PVOID);
		return Utils::vfunc< oCreateFont >(this, 71)(this);
	}
	bool SetFontGlyphSet(unsigned long font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0)
	{
		typedef bool(__thiscall* oCreateFont)(PVOID, unsigned long, const char *, int, int, int, int, int, int, int);
		return Utils::vfunc< oCreateFont >(this, 72)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax);
	}
	void GetTextSize(unsigned long font, const wchar_t *text, int &wide, int &tall)
	{
		typedef void(__thiscall* oGetTextSize)(PVOID, unsigned long, const wchar_t*, int&, int&);
		Utils::vfunc< oGetTextSize >(this, 79)(this, font, text, wide, tall);
	}
};