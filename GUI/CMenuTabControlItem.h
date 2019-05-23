#pragma once

#include "..\CRender.h"
#include "CMenuComboBox.h"
#include "CMenuSlider.h"
#include "CMenuButton.h"
#include "CMenuItem.h"
#include "CMenuSelectBox.h"
#include "CMenuTextbox.h"
#include <string>
#include <vector>

class CMenuTabControlItem : public CMenuItem
{
private:
	int x, y;
	int w, h, W, H, MAX_Y, MouseX, MouseY;
	std::string name;
	CRender* m_pRender;
	CMenuItem* Parent;
	std::vector<CMenuItem*> MenuItems;
public:
	CMenuTabControlItem(std::string name, int x, int y, CMenuItem* parent, CRender* pRenderer)
	{
		Parent = parent;
		m_pRender = pRenderer;
		this->name = name;
		this->x = x;
		this->y = y;
		this->MouseX = 0;
		this->MouseY = 0;

		m_pRender->GetStringSize(W, H, "%s", (char*)this->name.c_str());
		SetSize(30, 30); // !
		MAX_Y = 7;
	}

	std::vector<CMenuItem*> GetMenuItems()
	{
		return MenuItems;
	}

	void GetSize(int& w, int& h)
	{
		w = this->w;
		h = this->h;
	}

	void AddMenuItem(CMenuItem* item)
	{
		item->SetPosition(10, MAX_Y);
		MenuItems.push_back(item);

		int W, H;
		item->GetSize(W, H);
		MAX_Y += H + 5;

		if (W + 25 > w)
			SetSize(W + 25, MAX_Y);
		else
			SetSize(w, MAX_Y);
	}

	void AddButton(std::string name)
	{
		AddMenuItem(new CMenuButton(name, this, 0, 0, m_pRender));
	}

	void AddComboxBox(std::string name)
	{
		AddMenuItem(new CMenuComboBox(name, this, 0, 0, m_pRender));
	}

	void AddSlider(std::string name, float flMin, float flMax, float flSliderLen, Datatype_t FloatOrInt)
	{
		AddMenuItem(new CMenuSlider(name, this, 0, 0, flMin, flMax, flSliderLen, FloatOrInt, m_pRender));
	}

	void Draw()
	{
		for (std::vector<CMenuItem*>::iterator it = MenuItems.begin(); it != MenuItems.end(); ++it)
		{
			(*it)->Draw();
		}
	}

	void GetPosition(int& x, int& y)
	{
		int posx, posy;
		Parent->GetAbsPosition(posx, posy);

		x = this->x+posx;
		y = this->y+posy;
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
		if (X >= x && X <= x + w && Y >= y && Y <= y + h)
			return true;
		else
			return false;
	}

	void LMouseButtonDown()
	{
		for (std::vector<CMenuItem*>::iterator it = MenuItems.begin(); it != MenuItems.end(); ++it)
		{
			(*it)->LMouseButtonDown();
		}
	}

	void LMouseButtonUp()
	{
		for (std::vector<CMenuItem*>::iterator it = MenuItems.begin(); it != MenuItems.end(); ++it)
		{
			(*it)->LMouseButtonUp();
		}
	}

	void LMouseButtonClick()
	{
		for (std::vector<CMenuItem*>::iterator it = MenuItems.begin(); it != MenuItems.end(); ++it)
		{
			(*it)->LMouseButtonClick();
		}
	}


	bool IsSelected()
	{
		return true;
	}

	void KeyboardState(BYTE* keyStates)
	{
	}

	void MousePos(int X, int Y)
	{
		this->MouseX = X;
		this->MouseY = Y;

		//Propagate Mouse Position relative to this window to all children
		for (std::vector<CMenuItem*>::iterator it = MenuItems.begin(); it != MenuItems.end(); ++it)
		{
			(*it)->MousePos(X - x, Y - y);
		}
	}

	std::string GetItemName()
	{
		return name;
	}

	void LoadSettings(XMLElement* ele)
	{
		int i = 0;
		XMLElement* p = ele->FirstChildElement(RemoveSpaces(MenuItems[i]->GetItemName()).c_str());

		while (p)
		{
			MenuItems[i]->LoadSettings(p);
			i++;

			if (i >= MenuItems.size())
				break;

			p = p->NextSiblingElement(RemoveSpaces(MenuItems[i]->GetItemName()).c_str());
		}
	}

	std::string SaveSettings()
	{
		tinyxml2::XMLDocument cfg;
		std::string AllMenuSettings;

		for (CMenuItem* item : MenuItems)
		{
			std::string TabSettings = item->SaveSettings();
			AllMenuSettings += TabSettings;
		}

		AllMenuSettings = "<" + RemoveSpaces(GetItemName()) + ">" + AllMenuSettings + "</" + RemoveSpaces(GetItemName()) + ">";
		cfg.Parse(AllMenuSettings.c_str());

		XMLPrinter printer;
		cfg.Print(&printer);

		return std::string(printer.CStr());
	}

	float GetflValue()
	{
		return 0;
	}

	int GetiValue()
	{
		return 0;
	}

	bool GetbValue()
	{
		return false;
	}
};