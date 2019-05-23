#pragma once
#include "CMenuItem.h"
#include "..\CRender.h"

class Textarea : public CMenuItem
{
private:
	int _x;
	int _y;
	int _w;
	int _h;

	int MouseX;
	int MouseY;

	CRender* _pRender;
	CMenuItem* _parent;

	int TextLeftRightMargins = 10;
	int TextTopMargin = 10;
	int TextBottomMargin = 5;
	int ScrollbarWidth = 20;
	int TextAreaWidth = _w - 2 * TextLeftRightMargins - ScrollbarWidth;
	int TextAreadWidthNoMargin = _w - ScrollbarWidth;
	int ScrollBarPos = 0;
	int ScrollRectHeight = 15;
	std::vector<std::pair<std::string, DWORD>> textLines;
public:
	Textarea(int x, int y, int w, int h, CRender* pRender, CMenuItem* parent) : _x(x), _y(y), _w(w), _h(h),
		_pRender(pRender), _parent(parent) {}

	void GetSize(int& w, int& h) override
	{
		w = _w;
		h = _h;
	}

	void GetPosition(int& x, int& y) override
	{
		x = _x;
		y = _y;
	}

	void SetSize(int w, int h) override
	{
		_w = w;
		_h = h;
	}

	void SetPosition(int x, int y) override
	{
		_x = x;
		_y = y;
	}

	void Draw() override
	{
		//Textarea Background
		int __x, __y;
		_parent->GetPosition(__x, __y);
		_pRender->DrawRect(__x + _x, __y + _y, TextAreadWidthNoMargin, _h, _pRender->GetColor(Colors::TEAL));

		int textW, textH;
		_pRender->GetStringSize(textW, textH, "M");

		int lines = floor((_h - TextTopMargin) / (float)(textH + TextBottomMargin));

		int k;
		if (textLines.size() > lines)
			k = ((textLines.size() - lines) / (float)(_h - ScrollRectHeight))*ScrollBarPos;
		else
			k = 0;

		int height = TextTopMargin;
		for (int i = k; i < textLines.size() && i < k+lines; i++, height+=(textH+TextBottomMargin))
			_pRender->DrawFontText(__x + _x + TextLeftRightMargins, 
				__y + _y + height, textLines[i].second, (char*)textLines[i].first.c_str());

		//Scrollbar
		_pRender->DrawRect(__x + _x + TextAreadWidthNoMargin, __y + _y, ScrollbarWidth, _h, _pRender->GetColor(Colors::BROWN));
		_pRender->DrawRect(__x + _x + TextAreadWidthNoMargin, __y + _y + ScrollBarPos, ScrollbarWidth, ScrollRectHeight, 
			_pRender->GetColor(Colors::MENUBLOPSELECTED));
	}

	std::string RemoveWhiteSpaces(std::string s)
	{
		int i = 0;
		while (i < s.length() && s[i] == ' ')
			i++;

		return s.substr(i, std::string::npos);
	}

	void clear()
	{
		textLines.clear();
		ScrollBarPos = 0;
	}

	void AddTextLine(std::string text)
	{
		AddTextLine(text, _pRender->GetColor(Colors::WHITE));
	}

	void AddTextLine(std::string text, DWORD color)
	{
		int textW, textH;
		_pRender->GetStringSize(textW, textH, "W");
		textW = 3;

		int nChars = floor(TextAreaWidth / textW);

		int LineAmount = ceil(text.length() / (float)nChars);
		for (int i = 0; i < LineAmount; i++)
		{
			if ((i + 1)*nChars < text.length())
				textLines.push_back(std::make_pair(RemoveWhiteSpaces(text.substr(i*nChars, nChars)), color));
			else
				textLines.push_back(std::make_pair(RemoveWhiteSpaces(text.substr(i*nChars, std::string::npos)), color));
		}

		int lines = floor((_h - TextTopMargin) / (float)(textH + TextBottomMargin));
		if (textLines.size() > lines)
			ScrollBarPos = _h - ScrollRectHeight;
	}

	bool InItem(int X, int Y)
	{
		if (MouseX >= _x + TextAreadWidthNoMargin && MouseX <= _x + TextAreadWidthNoMargin + ScrollbarWidth
			&& MouseY >= _y && MouseY <= _y+_h)
			return true;
		else
			return false;
	}

	void LMouseButtonDown() override
	{
		if (InItem(MouseX, MouseY))
		{
			ScrollBarPos = MouseY - _y;
			if (ScrollBarPos < 0)
				ScrollBarPos = 0;

			if (ScrollBarPos > _h - ScrollRectHeight)
				ScrollBarPos = _h - ScrollRectHeight;
		}
	}

	void LMouseButtonUp() override
	{

	}

	void LMouseButtonClick() override
	{

	}

	void MousePos(int x, int y)
	{
		MouseX = x;
		MouseY = y;
	}

	std::string GetItemName() override
	{
		return "Textarea";
	}

	void LoadSettings(XMLElement* x)
	{
	}

	std::string SaveSettings()
	{
		return "<" + RemoveSpaces(GetItemName()) + ">" + "*" + "</" + RemoveSpaces(GetItemName()) + ">";
	}

	float GetflValue() override { return 0; }
	int GetiValue() override { return 0; }
	bool GetbValue() override { return false; }
};