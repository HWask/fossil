#pragma once

#include "CMenuItem.h"
#include "CMenuTabControlItem.h"
#include "..\CRender.h"
#include <string>
#include <vector>

class CMenuTabControl : public CMenuItem
{
private:
	int x, y;
	int w, h, MouseX, MouseY;
	CRender* m_pRender;

	std::vector<CMenuTabControlItem*> TabItems;
	CMenuItem* Parent;
	int bIsActive;
public:
	CMenuTabControl(CMenuItem* parent, int x, int y, CRender* pRenderer)
	{
		Parent = parent;
		m_pRender = pRenderer;
		this->x = x;
		this->y = y;
		this->MouseX = 0;
		this->MouseY = 0;
		bIsActive = -1;

		SetSize(50, 50);
	}

	std::vector<CMenuTabControlItem*> GetTabControlItems()
	{
		return TabItems;
	}

	void GetSize(int& w, int& h)
	{
		w = this->w;
		h = this->h;
	}

	void Draw()
	{
		int posx, posy;
		int W, H;
		Parent->GetPosition(posx, posy);

		for (int i = 0; i < TabItems.size(); i++)
		{
			m_pRender->GetStringSize(W, H, (char*)TabItems[i]->GetItemName().c_str());

			if (i == bIsActive)
			{
				m_pRender->DrawGradientRect(posx + x + i * 75, posy + y, 70, 40, m_pRender->GetColor(Colors::GRAY), m_pRender->GetColor(Colors::BLACK));
				m_pRender->DrawFontText(posx + x + 35 - W / 2 + i * 75, posy + y + 20 - H / 2, m_pRender->GetColor(Colors::WHITE), (char*)TabItems[i]->GetItemName().c_str());
			}
			else
			{
				m_pRender->DrawRect(posx + x + i * 75, posy + y, 70, 40, m_pRender->GetColor(Colors::BLACK));
				m_pRender->DrawFontText(posx + x + 35 - W / 2 + i * 75, posy + y + 20 - H / 2, m_pRender->GetColor(Colors::MENUBLOPSELECTED), (char*)TabItems[i]->GetItemName().c_str());
			}
		}

		m_pRender->DrawOutlinedRect(posx + x, posy + y + 45, w, h-45, m_pRender->GetColor(Colors::BLACK));

		TabItems[bIsActive]->Draw();
	}

	void AddTab(CMenuTabControlItem* tab)
	{
		TabItems.push_back(tab);
		bIsActive = 0;

		int lw, lh;
		tab->GetSize(lw, lh);
		tab->SetPosition(0, 45);

		if (TabItems.size() * 75 - 5 > w )
			SetSize(TabItems.size() * 75 - 5, h);

		if (lw > w)
			SetSize(lw, h);
	
		if (lh + 45 > h)
			SetSize(w, lh + 45);
	}

	void GetPosition(int& x, int& y)
	{
		x = this->x;
		y = this->y;
	}

	void GetAbsPosition(int& x, int& y)
	{
		int X, Y;
		Parent->GetPosition(X, Y);
		x = this->x + X;
		y = this->y + Y;
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

	bool InItem(int X, int Y) //on tab control
	{
		if (X >= x && X <= x + w && Y >= y && Y <= y + h)
			return true;
		else
			return false;
	}

	int OnTabButton(int X, int Y)
	{
		for (int i = 0; i < TabItems.size(); i++)
		{
			if (X >= x + 75 * i && X <= x + 70 + 75*i && Y >= y && Y <= y + 40)
			{
				return i;
			}
		}

		return -1;
	}

	void LMouseButtonDown()
	{
		if (Parent->IsSelected() && InItem(MouseX, MouseY))
		{
			TabItems[bIsActive]->LMouseButtonDown();
		}
	}

	void LMouseButtonUp()
	{
		if (Parent->IsSelected() && InItem(MouseX, MouseY))
		{

			for (std::vector<CMenuTabControlItem*>::iterator it = TabItems.begin(); it != TabItems.end(); ++it)
			{
				(*it)->LMouseButtonUp();
			}
		}
	}

	void LMouseButtonClick()
	{
		if (Parent->IsSelected() && InItem(MouseX, MouseY))
		{
			int k = OnTabButton(MouseX, MouseY);
			if (k > -1)
			{
				bIsActive = k;
			}

			TabItems[bIsActive]->LMouseButtonClick();
		}
	}

	void MousePos(int X, int Y)
	{
		this->MouseX = X;
		this->MouseY = Y;

		//Propagate Mouse Position relative to this window to all children
		for (std::vector<CMenuTabControlItem*>::iterator it = TabItems.begin(); it != TabItems.end(); ++it)
		{
			(*it)->MousePos(X - x, Y - y);
		}
	}

	std::string GetItemName()
	{
		return "Tab Control";
	}

	void LoadSettings(XMLElement* ele)
	{
		int selectedTab = StringToInt(ele->FirstChildElement("selected")->GetText());
		this->bIsActive = selectedTab;

		int i = 0;
		XMLElement* p = ele->FirstChildElement(RemoveSpaces(TabItems[i]->GetItemName()).c_str());

		while (p)
		{
			TabItems[i]->LoadSettings(p);
			i++;

			if (i >= TabItems.size())
				break;

			p = p->NextSiblingElement(RemoveSpaces(TabItems[i]->GetItemName()).c_str());
		}
	}

	std::string SaveSettings()
	{
		tinyxml2::XMLDocument cfg;
		std::string AllMenuSettings;

		for (CMenuItem* item : TabItems)
		{
			std::string TabSettings = item->SaveSettings();
			AllMenuSettings += TabSettings;
		}

		AllMenuSettings = "<" + RemoveSpaces(GetItemName()) + ">" + AllMenuSettings + "</" + RemoveSpaces(GetItemName()) + ">";
		cfg.Parse(AllMenuSettings.c_str());

		//add menu specific attributes here
		XMLElement* selectedTab = cfg.NewElement("selected");
		selectedTab->SetText(this->bIsActive);

		XMLElement* root = cfg.RootElement();
		root->InsertFirstChild(selectedTab);

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
		return bIsActive;
	}

	bool GetbValue()
	{
		return false;
	}
};