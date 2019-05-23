#pragma once
#include "..\CRender.h"
#include "CMenuItem.h"
#include "CBoneSelectionBox.h"
#include <map>
#include <string>

class CBoneSelector : public CMenuItem
{
private:
	int x, y;
	int w, h, MouseX, MouseY;
	CRender* m_pRender;

	std::map<int, CBoneSelectionBox*> Hitboxes;
	CMenuItem* Parent;
	std::string name;
	bool MultipleSelection, hasSelection;
public:
	CBoneSelector(std::string name, CMenuItem* parent, int x, int y, CRender* pRenderer)
	{
		Parent = parent;
		m_pRender = pRenderer;
		this->x = x;
		this->y = y;
		this->MouseX = 0;
		this->MouseY = 0;
		this->name = name;
		MultipleSelection = false;
		hasSelection = false;

		SetSize(200, 230);
	}

	void AllowMultipleSelections()
	{
		MultipleSelection = true;
	}

	void GetSize(int& w, int& h)
	{
		w = this->w;
		h = this->h;
	}

	void Draw()
	{
		int posx, posy;
		Parent->GetPosition(posx, posy);

		//title
		m_pRender->DrawFontText(posx + x, posy + y, m_pRender->GetColor(Colors::BLACK), (char*)name.c_str());
		//head
		m_pRender->DrawFilledCircle(posx + x + 100, posy + y + 25, 25, 0.07, m_pRender->GetColor(Colors::BLACK));
		//neck
		m_pRender->DrawRect(posx + x + 100 - 10, posy + y + 25 + 10, 20, 30, m_pRender->GetColor(Colors::BLACK));
		//torso
		m_pRender->DrawRect(posx + x + 100 - 40, posy + y + 25 + 10 + 30, 80, 100, m_pRender->GetColor(Colors::BLACK));
		//left arm
		m_pRender->DrawRect(posx + x + 100 - 40 - 60, posy + y + 25 + 10 + 30, 60, 20, m_pRender->GetColor(Colors::BLACK));
		//right arm
		m_pRender->DrawRect(posx + x + 100 - 40 + 80, posy + y + 25 + 10 + 30, 60, 20, m_pRender->GetColor(Colors::BLACK));
		//left leg
		m_pRender->DrawRect(posx + x + 100 - 40, posy + y + 25 + 10 + 30 + 100, 20, 40, m_pRender->GetColor(Colors::BLACK));
		//right leg
		m_pRender->DrawRect(posx + x + 100 - 40 + 80 - 20, posy + y + 25 + 10 + 30 + 100, 20, 40, m_pRender->GetColor(Colors::BLACK));
		//left foot
		m_pRender->DrawRect(posx + x + 100 - 40 - 20, posy + y + 25 + 10 + 30 + 100 + 40, 40, 20, m_pRender->GetColor(Colors::BLACK));
		//right foot
		m_pRender->DrawRect(posx + x + 100 - 40 + 80 - 20, posy + y + 25 + 10 + 30 + 100 + 40, 40, 20, m_pRender->GetColor(Colors::BLACK));

		for (auto it = Hitboxes.begin(); it != Hitboxes.end(); it++)
		{
			it->second->Draw();
		}
	}

	void AddHitboxSelection(int iHitbox, int x, int y)
	{
		int posx, posy;
		Parent->GetPosition(posx, posy);

		if (!hasSelection && !MultipleSelection)
		{
			CBoneSelectionBox* SelectionBox = new CBoneSelectionBox(this, x, y, m_pRender);
			SelectionBox->Select();
			Hitboxes[iHitbox] = SelectionBox;
			hasSelection = true;
		}
		else
		{
			CBoneSelectionBox* SelectionBox = new CBoneSelectionBox(this, x, y, m_pRender);
			Hitboxes[iHitbox] = SelectionBox;
		}
	}

	void GetPosition(int& x, int& y)
	{
		int X, Y;
		Parent->GetPosition(X, Y);
		x = this->x + X;
		y = this->y + Y;
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

	bool InItem(int X, int Y)
	{
		if (X >= x && X <= x + w && Y >= y && Y <= y + h)
			return true;
		else
			return false;
	}

	void LMouseButtonDown()
	{
	}

	void LMouseButtonUp()
	{
	}

	void LMouseButtonClick()
	{
		if (Parent->IsSelected() && InItem(MouseX, MouseY))
		{
			for (auto it = Hitboxes.begin(); it != Hitboxes.end(); it++)
			{
				int childX, childY;
				it->second->GetMousePos(childX, childY);

				if (MultipleSelection)
				{
					it->second->LMouseButtonClick();
				}
				else
				{
					if (it->second->InItem(childX, childY))
					{
						for (auto itz = Hitboxes.begin(); itz != Hitboxes.end(); itz++)
						{
							itz->second->Deselect();
						}

						it->second->LMouseButtonClick();
					}
				}
				
			}
		}
	}

	std::vector<int> GetSelection()
	{
		std::vector<int> hitboxes;

		for (auto it = Hitboxes.begin(); it != Hitboxes.end(); it++)
		{
			if (it->second->IsSelected())
				hitboxes.push_back(it->first);
		}

		return hitboxes;
	}

	bool IsSelected() 
	{
		return true;
	}

	void MousePos(int X, int Y)
	{
		this->MouseX = X;
		this->MouseY = Y;

		//Propagate Mouse Position relative to this window to all children
		for (auto it = Hitboxes.begin(); it != Hitboxes.end(); it++)
		{
			it->second->MousePos(X - x, Y - y);
		}
	}


	void AddHelmet(int iHitbox)
	{
		//head
		AddHitboxSelection(iHitbox, 93, 14);
	}

	void AddHead(int iHitbox)
	{
		//head
		AddHitboxSelection(iHitbox, 93, 34);
	}

	void AddNeck(int iHitbox)
	{
		//neck
		AddHitboxSelection(iHitbox, 93, 64);
	}

	void AddTorso1(int iHitbox)
	{
		//torsoe
		AddHitboxSelection(iHitbox, 93, 80);
	}

	void AddTorso2(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 93, 100);
	}

	void AddTorso3(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 93, 120);
	}

	void AddPelvis(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 93, 140);
	}

	void AddLeftArm3(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 5, 68);
	}

	void AddLeftArm2(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 25, 68);
	}

	void AddLeftArm1(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 45, 68);
	}

	void AddRightArm1(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 140, 68);
	}

	void AddRightArm2(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 160, 68);
	}

	void AddRightArm3(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 180, 68);
	}

	void AddLeftLeg1(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 63, 170);
	}

	void AddLeftLeg2(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 63, 190);
	}

	void AddRightLeg1(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 123, 170);
	}

	void AddRightLeg2(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 123, 190);
	}

	void AddLeftFoot1(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 63, 208);
	}

	void AddLeftFoot2(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 43, 208);
	}

	void AddRightFoot1(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 123, 208);
	}

	void AddRightFoot2(int iHitbox)
	{
		AddHitboxSelection(iHitbox, 143, 208);
	}

	std::string GetItemName()
	{
		return name;
	}

	void LoadSettings(XMLElement* ele)
	{
		int i = 0;
		auto it = Hitboxes.begin();
		XMLElement* p = ele->FirstChildElement(RemoveSpaces(it->second->GetItemName()).c_str());

		while (p)
		{
			it->second->LoadSettings(p);
			i++;

			if (i >= Hitboxes.size())
				break;

			std::advance(it, 1);
			p = p->NextSiblingElement(RemoveSpaces(it->second->GetItemName()).c_str());
		}
	}

	std::string SaveSettings()
	{
		tinyxml2::XMLDocument cfg;
		std::string AllMenuSettings;

		for (auto it = Hitboxes.begin(); it != Hitboxes.end(); it++)
		{
			std::string TabSettings = it->second->SaveSettings();
			AllMenuSettings += TabSettings;
		}

		AllMenuSettings = "<" + RemoveSpaces(name) + ">" + AllMenuSettings + "</" + RemoveSpaces(name) + ">";
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