#pragma once

#include "..\CRender.h"

#include "ConTextarea.h"
#include "ConTextbox.h"

#include "CMenuComboBox.h"
#include "CMenuSlider.h"
#include "CMenuButton.h"
#include "CMenuItem.h"
#include "CMenuSelectBox.h"
#include "CMenuTextbox.h"
#include "CMenuTabControl.h"
#include "CMenuDropDownList.h"
#include "CBoneSelector.h"
#include <string>
#include <vector>

class CMenuTab : public CMenuItem
{
private:
	int w, h, W, H, MAX_Y, MouseX, MouseY, deltaX, deltaY;
	bool bDrag, bpermaDraw;
	std::string name;
	CRender* m_pRender;
	std::vector<CMenuItem*> MenuItems;

	static std::vector<CMenuTab*> Tabs;
	static int Shared_ID;
public:
	int x, y;
	static std::vector<unsigned long long> zBuffer;
	int id;
	bool bIsActive, selected;

	CMenuTab(std::string name, int x, int y, CRender* pRenderer)
	{
		id = Shared_ID;
		Shared_ID++;
		zBuffer.push_back(1);

		selected = false;
		bIsActive = false;
		bpermaDraw = false;
		m_pRender = pRenderer;
		this->name = name;
		this->x = x;
		this->y = y;
		this->MouseX = 0;
		this->MouseY = 0;
		this->bDrag = false;

		m_pRender->GetStringSize(W, H, "%s", (char*)this->name.c_str());
		SetSize(W+25, H+15); // !
		MAX_Y = H + 12;
	}

	static void SetTabItems(std::vector<CMenuTab*> tab)
	{
		Tabs = tab;
	}

	std::vector<CMenuItem*> GetMenuItems()
	{
		return MenuItems;
	}

	bool IsSelected()
	{
		return selected;
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
		if (W+25 > w)
			SetSize(W+25, h+H+5);
		else
			SetSize(w, h + H + 5);

		MAX_Y += H+5;
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

	void AddMenuItemAtPos(CMenuItem* item)
	{
		MenuItems.push_back(item);

		int W, H, x, y;
		item->GetSize(W, H);
		item->GetPosition(x, y);
		if (x + W + 25 > w)
			SetSize(x + W + 25, h);

		if (y + H + 5 > h)
		{
			SetSize(w, y+H+5);
			MAX_Y = y + H + 5;
		}
	}

	void Draw()
	{
		m_pRender->DrawRect(x, y, w, h, m_pRender->GetColor(Colors::AZURE));
		m_pRender->DrawGradientRect(x, y, w, H + 5, m_pRender->GetColor(Colors::GRAY), m_pRender->GetColor(Colors::BLACK));

		m_pRender->DrawFontText(x + w / 2.f - W / 2.f, y+3, m_pRender->GetColor(Colors::WHITE), (char*)name.c_str());

		if (selected)
			m_pRender->DrawOutlinedRect(x, y, w, h, m_pRender->GetColor(Colors::DARKBLUE));
		else
			m_pRender->DrawOutlinedRect(x, y, w, h, m_pRender->GetColor(Colors::BLACK));
			
		for (std::vector<CMenuItem*>::iterator it = MenuItems.begin(); it != MenuItems.end(); ++it)
		{
			(*it)->Draw();
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
		if (X >= x && X <= x + w && Y >= y && Y <= y + h)
			return true;
		else
			return false;
	}

	bool InTitle(int X, int Y)
	{
		if (X >= x && X <= x+w && Y >= y && Y <= y+H)
		{
			return true;
		}

		return false;
	}

	bool IsPointOnTab(CMenuTab* tab, int X, int Y)
	{
		int w, h, x, y;
		tab->GetPosition(x, y);
		tab->GetSize(w, h);

		if (X >= x && X <= x + w && Y >= y && Y <= y + h) //on tab?
			return true;
		else
			return false;
	}

	int GetTabWithBiggestDepthAt(int X, int Y)
	{
		int i = 0;
		int j = -1;
		int w, h, x, y;
		unsigned long long z = 0;

		for (std::vector<CMenuTab*>::iterator it = Tabs.begin(); it != Tabs.end(); ++it, i++)
		{
			(*it)->GetSize(w, h);
			(*it)->GetPosition(x, y);

			if (X >= x && X <= x + w && Y >= y && Y <= y + h) //on tab?
			{
				if (zBuffer[i] > z)
				{
					z = zBuffer[i];
					j = i;
				}
			}
		}

		return j;
	}

	std::vector<CMenuTab*> GetOverlappingTabsOfTab()
	{
		int i = 0;
		int w, h, x, y;
		std::vector<CMenuTab*> tabs;

		for (std::vector<CMenuTab*>::iterator it = Tabs.begin(); it != Tabs.end(); ++it, i++)
		{
			(*it)->GetPosition(x, y);
			(*it)->GetSize(w, h);

			if (x <= this->x + this->w && this->y <= y + h && y <= this->y + this->h && x + w >= this->x)
				tabs.push_back(*it);
		}

		return tabs;
	}

	void PutTabOnTop(std::vector<CMenuTab*> overlappingTabs)
	{
		if (overlappingTabs.empty())
			return;

		unsigned long long max = 0;

		for (int i = 0; i<overlappingTabs.size(); i++)
		{
			if (overlappingTabs[i]->id != this->id && zBuffer[overlappingTabs[i]->id] > max)
				max = zBuffer[overlappingTabs[i]->id];
		}

		if (max >= zBuffer[this->id])
			zBuffer[this->id] = max + 1;
	}

	void PutTabInBack(std::vector<CMenuTab*> overlappingTabs)
	{
		if (overlappingTabs.empty())
			return;

		unsigned long long min = MAXULONGLONG;

		for (int i = 0; i<overlappingTabs.size(); i++)
		{
			if (overlappingTabs[i]->id != this->id && overlappingTabs[i]->bIsActive && zBuffer[overlappingTabs[i]->id] < min)
				min = zBuffer[overlappingTabs[i]->id];
		}

		if (min == zBuffer[this->id])
		{
			for (int i = 0; i<overlappingTabs.size(); i++)
			{
				if (overlappingTabs[i]->id != this->id)
					zBuffer[overlappingTabs[i]->id]++;
			}
		}
	}

	void DragTab()
	{
		SetPosition(this->MouseX-deltaX, this->MouseY-deltaY);
	}

	void LMouseButtonDown()
	{
		if (bDrag)
		{
			DragTab();
			std::vector<CMenuTab*> overlapping = GetOverlappingTabsOfTab();
			PutTabOnTop(overlapping);
		}

		int k = GetTabWithBiggestDepthAt(MouseX, MouseY);
		if (this->id != -1 && this->id != k)
			return;

		if (InTitle(this->MouseX, this->MouseY) && selected)
		{
			if (!bDrag)
			{
				deltaX = MouseX - x;
				deltaY = MouseY - y;
				bDrag = true;
			}
		}

		for (std::vector<CMenuItem*>::iterator it = MenuItems.begin(); it != MenuItems.end(); ++it)
		{
			(*it)->LMouseButtonDown();
		}
	}

	void LMouseButtonUp()
	{
		if (bDrag) //we were draggin the tab
		{
			bDrag = false;
		}

		for (std::vector<CMenuItem*>::iterator it = MenuItems.begin(); it != MenuItems.end(); ++it)
		{
			(*it)->LMouseButtonUp();
		}
	}

	void LMouseButtonClick()
	{
		int k = GetTabWithBiggestDepthAt(MouseX, MouseY);

		if (!InItem(this->MouseX, this->MouseY) || (this->id != -1 && this->id != k))
		{
			selected = false;
		}

		if (this->id != -1 && this->id != k)
			return;

		if (InItem(this->MouseX, this->MouseY))
		{
			selected = true;
			std::vector<CMenuTab*> overlapping = GetOverlappingTabsOfTab();
			PutTabOnTop(overlapping);
		}

		for (std::vector<CMenuItem*>::iterator it = MenuItems.begin(); it != MenuItems.end(); ++it)
		{
			(*it)->LMouseButtonClick();
		}
	}

	void KeyboardState(BYTE* keyStates)
	{
		for (std::vector<CMenuItem*>::iterator it = MenuItems.begin(); it != MenuItems.end(); ++it)
		{
			(*it)->KeyboardState(keyStates);
		}
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

	bool IsActive()
	{
		return bIsActive;
	}

	void AlwaysDraw()
	{
		bpermaDraw = true;
		bIsActive = true;
		zBuffer[this->id] = 2;
	}

	bool ShouldAlwaysDraw()
	{
		return bpermaDraw;
	}

	void Activate()
	{
		bIsActive = true;
	}

	void Deactivate()
	{
		bIsActive = false;
	}

	void LoadSettings(XMLElement* ele)
	{
		int x = StringToInt(ele->FirstChildElement("x")->GetText());
		int y = StringToInt(ele->FirstChildElement("y")->GetText());
		int z = StringToInt(ele->FirstChildElement("z")->GetText());
		bool on = StringToBool(ele->FirstChildElement("on")->GetText());

		this->x = x;
		this->y = y;
		this->zBuffer[this->id] = z;
		this->bIsActive = on;

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

		AllMenuSettings = "<" + RemoveSpaces(name) + ">" + AllMenuSettings + "</" + RemoveSpaces(name) + ">";
		cfg.Parse(AllMenuSettings.c_str());

		//add menu specific attributes here
		XMLElement* z = cfg.NewElement("z");
		XMLElement* x = cfg.NewElement("x");
		XMLElement* y = cfg.NewElement("y");
		XMLElement* toggled = cfg.NewElement("on");

		z->SetText(ULongLongToString(zBuffer[id]).c_str());
		x->SetText(this->x);
		y->SetText(this->y);
		toggled->SetText(this->bIsActive);
		
		XMLElement* root = cfg.RootElement();
		root->InsertFirstChild(toggled);
		root->InsertFirstChild(y);
		root->InsertFirstChild(x);
		root->InsertFirstChild(z);

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