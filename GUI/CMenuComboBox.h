#pragma once

#include "..\CRender.h"
#include "CMenuItem.h"
#include <string>

class CMenuComboBox : public CMenuItem
{
private:
	int x, y, MouseX, MouseY;
	int w, h;
	bool selected;
	std::string name;
	CRender* m_pRender;
	CMenuItem* parent;
public:
	CMenuComboBox(std::string name, CMenuItem* parent, int x, int y, CRender* pRenderer)
	{
		selected = false;
		m_pRender = pRenderer;
		this->name = name;
		this->parent = parent;
		this->x = x;
		this->y = y;
		this->MouseX = 0;
		this->MouseY = 0;

		m_pRender->GetStringSize(this->w, this->h, "%s", name.c_str());
		SetSize(this->w + this->h + 4, this->h);
	}

	void GetSize(int& w, int& h)
	{
		w = this->w;
		h = this->h;
	}

	void SetbValue(bool val) { selected = val; }

	void Draw()
	{
		int posx = 0;
		int posy = 0;

		if (parent)
			parent->GetPosition(posx, posy);

		if (selected)
		{
			m_pRender->DrawGradientRect(posx + x, posy + y, h, h, m_pRender->GetColor(Colors::GRAY), m_pRender->GetColor(Colors::BLACK));

			m_pRender->DrawFilledCircle(posx + x + 7, posy + y + 7, 2.7, 0.01f, m_pRender->GetColor(Colors::MENUCOMBOBOXSELECTED));
			m_pRender->DrawFontText(posx + x + h + 4, posy + y, m_pRender->GetColor(Colors::BLACK), (char*)name.c_str());
		}
		else
		{
			m_pRender->DrawGradientRect(posx + x, posy + y, h, h, m_pRender->GetColor(Colors::GRAY), m_pRender->GetColor(Colors::BLACK));

			m_pRender->DrawFontText(posx + x + h + 4, posy + y, m_pRender->GetColor(Colors::BLACK), (char*)name.c_str());
		}
	}

	void GetPosition(int& x, int& y)
	{
		x = this->x;
		y = this->y;
	}

	void SetPosition(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	void SetSize(int w, int h)
	{
		this->w = w;
		this->h = h;
	}

	bool InItem(int X, int Y)
	{
		if (X >= x && X <= x + h && Y >= y && Y <= y + h)
			return true;
		else
			return false;
	}

	void LMouseButtonDown() {}
	void LMouseButtonUp() {}

	void LMouseButtonClick()
	{
		if (InItem(this->MouseX, this->MouseY) && parent->IsSelected())
		{
			selected = !selected;
		}
	}

	void MousePos(int X, int Y)
	{
		this->MouseX = X;
		this->MouseY = Y;
	}

	bool GetbValue()
	{
		return selected;
	}

	std::string GetItemName()
	{
		return name;
	}

	void LoadSettings(XMLElement* ele)
	{
		bool selected = StringToBool(ele->GetText());
		this->selected = selected;
	}

	std::string SaveSettings()
	{
		return "<" + RemoveSpaces(GetItemName()) + ">" + std::to_string(selected) + "</" + RemoveSpaces(GetItemName()) + ">";
	}

	float GetflValue()
	{
		return 0;
	}

	int GetiValue()
	{
		return 0;
	}
};