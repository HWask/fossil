#pragma once

#include "..\CRender.h"
#include "CMenuTab.h"
#include "CPeriphery.h"
#include <list>
#include <fstream>
#include <algorithm>

#define ButtonMargin 15

struct sButton
{
	float x, y, w, h;
	std::string name;
	bool onclick;
	bool isToggled;
};

class CMenu
{
private:
	float ButtonMax_X;

	CPeriphery* m_pInput;
	CRender* m_pRenderer;
	bool bIsCreated;
	bool IsActive;
	std::string name;
	int MouseX, MouseY;

	std::vector<CMenuTab*> Tabs;
	std::vector<sButton*> Buttons;
public:
	void Initialize(std::string name)
	{
		bIsCreated = false;
		m_pRenderer = NULL;
		m_pInput = NULL;
		IsActive = false;
		this->name = name;
		ButtonMax_X = 10;
	}

	std::vector<CMenuTab*> GetTabs()
	{
		return Tabs;
	}
	
	void Create(CRender* pRenderer)
	{
		this->m_pRenderer = pRenderer;
		bIsCreated = true;
		m_pInput = new CPeriphery();
		m_pInput->Initialise();
		m_pInput->Create();
	}

	bool IsCreated()
	{
		return this->bIsCreated;
	}

	void AddTab(CMenuTab* tab)
	{
		Tabs.push_back(tab);
		sButton* button = new sButton();
		button->x = ButtonMax_X;
		button->w = 90;
		button->h = 25;
		button->onclick = false;
		button->isToggled = false;
		button->name = tab->GetItemName();
		RECT rect;
		GetClientRect(m_pInput->game, &rect);
		button->y = rect.bottom - 50/2 - 10;

		Buttons.push_back(button);

		ButtonMax_X += button->w + ButtonMargin;
	}

	void DrawOverlay()
	{
		if (!IsActive)
			return;

		RECT rect;
		GetClientRect(m_pInput->game, &rect);
		m_pRenderer->DrawRect(0, rect.bottom - 47, rect.right, 47, m_pRenderer->GetColor(Colors::BLACK));

		for (std::vector<sButton*>::iterator it = Buttons.begin(); it != Buttons.end(); ++it)
		{
			if ((*it)->onclick){
				m_pRenderer->DrawRect((*it)->x, (*it)->y, (*it)->w, (*it)->h, m_pRenderer->GetColor(Colors::MENUBLOP));
				(*it)->onclick = false;
			}
			else
			{
				if ((*it)->isToggled)
					m_pRenderer->DrawRect((*it)->x, (*it)->y, (*it)->w, (*it)->h, m_pRenderer->GetColor(Colors::MENUBLOP));
				else
					m_pRenderer->DrawRect((*it)->x, (*it)->y, (*it)->w, (*it)->h, m_pRenderer->GetColor(Colors::GRAY));
			}

			int textW, textH;
			m_pRenderer->GetStringSize(textW, textH, (char*)(*it)->name.c_str());
			m_pRenderer->DrawOutlinedFontText((*it)->x + (*it)->w / 2 - textW/2, (*it)->y + 4, m_pRenderer->GetColor(Colors::MENUTEXT), "%s", (*it)->name.c_str());
		}
	}

	bool InButton(int index, int X, int Y)
	{
		if (X >= Buttons[index]->x && X <= Buttons[index]->x + Buttons[index]->w && Y >= Buttons[index]->y && Y <= Buttons[index]->y + Buttons[index]->h)
			return true;
		else
			return false;
	}

	bool static pairCompare(const std::pair<unsigned long long, unsigned int>& firstElem, const std::pair<unsigned long long, unsigned int>& secondElem)
	{
		return firstElem.first < secondElem.first;
	}

	void DrawMenu()
	{
		//Sort, then draw with increasing zValue
		std::vector<std::pair<unsigned long long, unsigned int>> zBuffer;
		for (int i = 0; i < Tabs.size(); i++)
		{
			zBuffer.push_back(std::pair<unsigned long long, unsigned int>(CMenuTab::zBuffer[i], i));
		}
		std::sort(zBuffer.begin(), zBuffer.end(), pairCompare);

		for (std::vector<std::pair<unsigned long long, unsigned int>>::iterator it = zBuffer.begin(); it != zBuffer.end(); ++it)
		{
			if (Tabs[it->second]->IsActive() && Tabs[it->second]->ShouldAlwaysDraw())
				Tabs[it->second]->Draw();

			if (Tabs[it->second]->IsActive() && !Tabs[it->second]->ShouldAlwaysDraw() && IsActive)
				Tabs[it->second]->Draw();
		}
	}

	void WaterMark()
	{
		m_pRenderer->DrawOutlinedFontText(10, 10, m_pRenderer->GetColor(Colors::WHITE), "%s", name.c_str());
	}

	void PollInput()
	{
		m_pInput->PollInputState(IsActive);
		bool* pInputStates = m_pInput->GetInputStates();
		BYTE* keyStates = m_pInput->GetKeyStates();

		if (pInputStates[KEY_INSERT_PRESSED])
		{
			IsActive = !IsActive;

			if (IsActive) //menu open
			{
				//ShowCursor(false);
			}
			else //menu closed
			{
				//ShowCursor(true);

				//deselect all tabs when closing menu
				for (std::vector<CMenuTab*>::iterator it = Tabs.begin(); it != Tabs.end(); ++it)
				{
					(*it)->selected = false;
				}
			}

			//fix so the view doesnt change if you close the menu
			RECT rect;
			GetClientRect(m_pInput->game, &rect);

			POINT p;
			p.x = rect.right / 2.f;
			p.y = rect.bottom / 2.f;
			ClientToScreen(m_pInput->game, &p);
			SetCursorPos(p.x, p.y);
		}

		if (pInputStates[LBUTTON_DOWN])
		{
			//button is onclick?
			int i = 0;
			for (std::vector<sButton*>::iterator it = Buttons.begin(); it != Buttons.end(); ++it, i++)
			{
				if (InButton(i, MouseX, MouseY))
					Buttons[i]->onclick = true;
			}

			for (std::vector<CMenuTab*>::iterator it = Tabs.begin(); it != Tabs.end(); ++it, i++)
			{
				if((*it)->IsActive())
					(*it)->LMouseButtonDown();
			}
		}
		else
		{
			//button aint onclick no more
			int i = 0;
			for (std::vector<sButton*>::iterator it = Buttons.begin(); it != Buttons.end(); ++it, i++)
			{
				Buttons[i]->onclick = false;
			}

			for (std::vector<CMenuTab*>::iterator it = Tabs.begin(); it != Tabs.end(); ++it, i++)
			{
				if ((*it)->IsActive())
					(*it)->LMouseButtonUp();
			}
		}

		if (pInputStates[LBUTTON_CLICKED])
		{
			//button clicked?
			int i = 0;
			for (std::vector<sButton*>::iterator it = Buttons.begin(); it != Buttons.end(); ++it, i++)
			{
				if (InButton(i, MouseX, MouseY))
				{
					Buttons[i]->isToggled = !Buttons[i]->isToggled;
					Tabs[i]->bIsActive = !Tabs[i]->bIsActive;

					if (Tabs[i]->bIsActive) //tab poppin up
					{
						Tabs[i]->zBuffer[i] = 2;
						auto overlapping = Tabs[i]->GetOverlappingTabsOfTab();
						Tabs[i]->PutTabInBack(overlapping);
					}
					else //tab out in background
					{
						Tabs[i]->zBuffer[i] = 1;
					}
				}
			}

			//propagate click to tabs
			for (std::vector<CMenuTab*>::iterator it = Tabs.begin(); it != Tabs.end(); ++it, i++)
			{
				if ((*it)->IsActive())
					(*it)->LMouseButtonClick();
			}
		}

		//propagate keyStates
		for (std::vector<CMenuTab*>::iterator it = Tabs.begin(); it != Tabs.end(); ++it)
		{
			(*it)->KeyboardState(keyStates);
		}
	}

	void UpdateMousePos()
	{
		m_pInput->UpdateMousePos();
		m_pInput->GetMousePos(MouseX, MouseY);

		for (std::vector<CMenuTab*>::iterator it = Tabs.begin(); it != Tabs.end(); ++it)
		{
			(*it)->MousePos(MouseX, MouseY);
		}
	}

	void UpdateGUI()
	{
		for (int i = 0; i < Tabs.size();i++)
		{
			if (Tabs[i]->IsActive())
				Buttons[i]->isToggled = true;
			else
				Buttons[i]->isToggled = false;
		}
	}

	void RenderGUI()
	{
		PollInput();
		UpdateMousePos();
		DrawOverlay();
		DrawMenu();
		DrawMouse();
		UpdateGUI();
		WaterMark();
	}

	bool MenuOpen()
	{
		return IsActive;
	}

	void DrawMouse()
	{
		if (!IsActive)
			return;

		int x, y;
		m_pInput->GetMousePos(x, y);
		m_pRenderer->DrawCursor(x, y);
	}
};