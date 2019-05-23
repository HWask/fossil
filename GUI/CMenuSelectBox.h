#pragma once

#include "..\CRender.h"
#include "CMenuItem.h"
#include <string>
#include <vector>
#include "MenuObservable.h"

class CMenuSelectBox : public CMenuItem, public Observable<std::string>
{
private:
	int x, y, MouseX, MouseY;
	int w, h, Sections, ShowSection, selected, ItemsToShow;
	float ScrollPos;
	bool bDrag;
	CRender* m_pRender;
	CMenuItem* parent;

	std::vector<std::string> text;
public:
	CMenuSelectBox(CMenuItem* parent, int x, int y, int w, int h, CRender* pRenderer)
	{
		m_pRender = pRenderer;
		this->parent = parent;
		this->x = x;
		this->y = y;
		this->MouseX = 0;
		this->MouseY = 0;
		this->w = w;
		this->h = h;
		this->text.clear();
		ScrollPos = 0;
		ShowSection = 0;
		selected = -1;

		SetSize(w, h);
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

		//console window
		m_pRender->DrawRect(posx + x, posy + y, w - 20, h, m_pRender->GetColor(Colors::WHITE));
		m_pRender->DrawOutlinedRect(posx + x, posy + y, w - 20, h, m_pRender->GetColor(Colors::BLACK));

		//scrollbar
		m_pRender->DrawRect(posx + x + w - 20, posy + y, 20, h, m_pRender->GetColor(Colors::GRAY));
		m_pRender->DrawOutlinedRect(posx + x + w - 20, posy + y, 20, h, m_pRender->GetColor(Colors::BLACK));

		//scroll blop
		if (bDrag)
		{
			m_pRender->DrawRect(posx + x + w - 20, posy + y + ScrollPos, 19, 20, m_pRender->GetColor(Colors::MENUBLOPSELECTED));
			bDrag = false;
		}
		else
			m_pRender->DrawRect(posx + x + w - 20, posy + y + ScrollPos, 19, 20, m_pRender->GetColor(Colors::BLACK));

		//draw text
		int TextHeight = 5 + text.size() * 15;
		ItemsToShow = text.size();
		Sections = 0;
		if (TextHeight > h - 5) //needs scroll
		{
			ItemsToShow = (h - 10) / 15.f;

			//# items that are not displayed
			Sections = text.size() - ItemsToShow;
			if (Sections < 0)
				Sections = 0;
		}

		int szw, szh;
		for (int i = ShowSection; i < ShowSection + ItemsToShow; i++)
		{
			m_pRender->GetStringSize(szw, szh, (char*)text[i].c_str());

			if (szw + 4 > w - 16)
			{
				int charW = (float)szw / text[i].size();
				int charCount = (w - 12) / (float)charW;

				if (selected >= 0 && i == selected)
				{
					m_pRender->DrawRect(posx + x, posy + y + 5 + (i - ShowSection) * 15, w-21, 14, m_pRender->GetColor(Colors::SKYBLUE));
					m_pRender->DrawFontText(posx + x + 4, posy + y + 5 + (i - ShowSection) * 15, m_pRender->GetColor(Colors::BLACK), (char*)text[i].substr(0, charCount - 2).c_str());
				}
				else
					m_pRender->DrawFontText(posx + x + 4, posy + y + 5 + (i - ShowSection) * 15, m_pRender->GetColor(Colors::BLACK), (char*)text[i].substr(0, charCount - 2).c_str());
			}
			else
			{
				if (selected >= 0 && i == selected)
				{
					m_pRender->DrawRect(posx + x, posy + y + 5 + (i - ShowSection) * 15, w - 21, 14, m_pRender->GetColor(Colors::SKYBLUE));
					m_pRender->DrawFontText(posx + x + 4, posy + y + 5 + (i - ShowSection) * 15, m_pRender->GetColor(Colors::BLACK), (char*)text[i].c_str());
				}
				else
					m_pRender->DrawFontText(posx + x + 4, posy + y + 5 + (i - ShowSection) * 15, m_pRender->GetColor(Colors::BLACK), (char*)text[i].c_str());
			}
		}
	}

	void addItem(std::string argtext)
	{
		text.push_back(argtext);

		if (selected < 0)
		{
			selected = 0;
			Notify(argtext);
		}
	}

	int GetSelectedIndex()
	{
		return selected;
	}

	void removeItem(int i)
	{
		if (text.size() == 0)
			return;

		if (i < text.size() && i >= 0)
		{
			if (text.size() == 1 && i == 0)
			{
				text.erase(text.begin());
				selected = -1;
			}

			if (text.size() > 1)
			{
				if (i <= selected && i == 0)
				{
					text.erase(text.begin());
					return;
				}

				if (i <= selected)
				{
					selected--;
					text.erase(text.begin() + i);
					ShowSection = 0;
					ScrollPos = 0;
				}
				else
				{
					text.erase(text.begin());
					ShowSection = 0;
					ScrollPos = 0;
				}
			}
		}
	}

	void ClearItems()
	{
		selected = -1;
		ScrollPos = 0;
		ShowSection = 0;
		text.clear();
	}

	std::string GetSelection()
	{
		if (selected >= 0)
			return text[selected];
		else
			return std::string("");
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

	void LMouseButtonDown()
	{
		if (InItem(this->MouseX, this->MouseY) && parent->IsSelected())
		{
			bDrag = true;

			if (Sections != 0)
			{
				float delta = h / (float)(Sections + 1);

				int p = this->MouseY - y;
				if (p<0)
					p = 0;

				for (int i = 0; i <= Sections; i++)
				{
					if (p >= i*delta && p < i*delta + delta)
					{
						if (p > h - 20)
							p = h - 20;

						ScrollPos = p;
						ShowSection = i;
					}
				}
			}
		}
	}

	//on scrollbar?
	bool InItem(int X, int Y)
	{
		if (X >= x + w - 20 && X <= x + w && Y >= y - 2 && Y <= y + h)
			return true;
		else
			return false;
	}

	bool InTextBox(int X, int Y)
	{
		if (X >= x && X < x + w - 20 && Y >= y && Y <= y + h)
			return true;
		else
			return false;
	}

	void LMouseButtonUp()
	{
		bDrag = false;
	}

	void LMouseButtonClick()
	{
		if (InTextBox(MouseX, MouseY) && parent->IsSelected())
		{
			if (ItemsToShow > 0)
			{
				int p = MouseY - y;

				for (int i = 0; i < ItemsToShow; i++)
				{
					if (5 + i * 15 <= p && 5 + (i + 1) * 15 > p)
					{
						selected = i+ShowSection;
						Notify(text[selected]);
					}
				}
			}
		}
	}

	void MousePos(int X, int Y)
	{
		this->MouseX = X;
		this->MouseY = Y;
	}

	std::string GetItemName()
	{
		return "SelectBox";
	}

	void LoadSettings(XMLElement* ele)
	{
		float scroll = StringToFloat(ele->FirstChildElement("Scroll")->GetText());
		int section = StringToInt(ele->FirstChildElement("Section")->GetText());
		int selected = StringToInt(ele->FirstChildElement("Selected")->GetText());

		this->ScrollPos = scroll;
		this->ShowSection = section;
		this->selected = selected;
	}

	std::string SaveSettings()
	{
		tinyxml2::XMLDocument cfg;

		std::string AllMenuSettings = "<" + RemoveSpaces(GetItemName()) + ">" + "</" + RemoveSpaces(GetItemName()) + ">";
		cfg.Parse(AllMenuSettings.c_str());

		//add menu specific attributes here
		XMLElement* scrollpos = cfg.NewElement("Scroll");
		XMLElement* showsection = cfg.NewElement("Section");
		XMLElement* selected = cfg.NewElement("Selected");

		scrollpos->SetText(std::to_string(ScrollPos).c_str());
		showsection->SetText(std::to_string(ShowSection).c_str());
		selected->SetText(std::to_string(this->selected).c_str());

		XMLElement* root = cfg.RootElement();
		root->InsertFirstChild(selected);
		root->InsertFirstChild(showsection);
		root->InsertFirstChild(scrollpos);

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
		return selected;
	}

	bool GetbValue()
	{
		return false;
	}
};