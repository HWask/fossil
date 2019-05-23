#pragma once

#include "..\CRender.h"
#include "CMenuItem.h"
#include <string>

class CMenuDropDownList : public CMenuItem
{
private:
	int x, y, MouseX, MouseY;
	int w, h, W, H;
	int selected;
	bool isOpen;
	std::string name;
	CRender* m_pRender;
	CMenuItem* parent;
	std::vector<std::string> DropDownItems;
public:
	CMenuDropDownList(std::string name, CMenuItem* parent, int x, int y, CRender* pRenderer)
	{
		m_pRender = pRenderer;
		this->name = name;
		this->parent = parent;
		this->x = x;
		this->y = y;
		this->MouseX = 0;
		this->MouseY = 0;
		isOpen = false;

		m_pRender->GetStringSize(this->W, this->H, "%s", name.c_str());
		SetSize(66+80, 21);
	}

	void GetSize(int& w, int& h)
	{
		w = this->w;
		h = this->h;
	}

	void Draw()
	{
		int posx = 0;
		int posy = 0;

		if (parent)
			parent->GetPosition(posx, posy);

		if (!parent->IsSelected())
			isOpen = false;

		m_pRender->DrawRect(posx+x + 66, posy+y, 80, 21, m_pRender->GetColor(Colors::WHITE));
		m_pRender->DrawOutlinedRect(posx + x + 66, posy + y, 80, 21, m_pRender->GetColor(Colors::BLACK));
		m_pRender->DrawFontText(posx + x + 66 + 70, posy + y + 21/2 - H/2, m_pRender->GetColor(Colors::BLACK), "v");
		m_pRender->DrawFontText(posx + x + 66 + 5, posy + y + 21/2 - H/2, m_pRender->GetColor(Colors::BLACK), (char*)DropDownItems[selected].c_str());
		m_pRender->DrawFontText(posx + x, posy + y + 21 / 2 - H / 2, m_pRender->GetColor(Colors::BLACK), (char*)this->name.c_str());

		if (isOpen)
		{
			for (int i = 0; i < DropDownItems.size(); i++)
			{
				m_pRender->DrawRect(posx + x + 66, posy + y + (i + 1) * 21, 80, 21, m_pRender->GetColor(Colors::WHITE));
				m_pRender->DrawOutlinedRect(posx + x + 66, posy + y + (i + 1) * 21, 80, 21, m_pRender->GetColor(Colors::BLACK));
				m_pRender->DrawFontText(posx + x + 66 + 5, posy + y + (i + 1) * 21 + 21 / 2 - H / 2, m_pRender->GetColor(Colors::BLACK), (char*)DropDownItems[i].c_str());

				if (MouseY >= y+(i+1)*21 && MouseY < y+(i+1) * 21 + 21 && MouseX >= x + 66 && MouseX <= x + 66 + 80)
				{
					m_pRender->DrawRect(posx + x + 66, posy + y + (i + 1) * 21, 80, 21, m_pRender->GetColor(Colors::SKYBLUE));
					m_pRender->DrawOutlinedRect(posx + x + 66, posy + y + (i + 1) * 21, 80, 21, m_pRender->GetColor(Colors::BLACK));
					m_pRender->DrawFontText(posx + x + 66 + 5, posy + y + (i + 1) * 21 + 21 / 2 - H / 2, m_pRender->GetColor(Colors::BLACK), (char*)DropDownItems[i].c_str());
				}
			}
		}
	}

	void AddListItem(std::string x)
	{
		DropDownItems.push_back(x);
		selected = 0;
		SetSize(w, h + 21);
	}

	std::string GetSelectedItem()
	{
		return DropDownItems[selected];
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
		if (X >= x+66 && X <= x + 66+80 && Y >= y && Y < y+21)
			return true;
		else
			return false;
	}

	bool IsOnDropItem(int k, int X, int Y)
	{
		if (X >= x + 66 && X <= x + 66 + 80 && Y >= y+k*21 && Y < y + k*21+21)
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
			isOpen = !isOpen;
		}

		for (int i = 0; i < DropDownItems.size(); i++)
		{
			if (IsOnDropItem(i + 1, MouseX, MouseY))
			{
				selected = i;
				isOpen = false;
				return;
			}
		}

		if (!InItem(MouseX, MouseY))
			isOpen = false;
	}

	void MousePos(int X, int Y)
	{
		this->MouseX = X;
		this->MouseY = Y;
	}

	std::string GetItemName()
	{
		return name;
	}

	void LoadSettings(XMLElement* ele)
	{
		int selected = StringToInt(ele->GetText());
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
		return selected;
	}

	bool GetbValue()
	{
		return false;
	}
};