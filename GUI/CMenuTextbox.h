#pragma once

#include "..\CRender.h"
#include "CMenuItem.h"
#include <string>
#include "MenuObservable.h"

class CMenuTextbox : public CMenuItem, public Observable<int>
{
private:
	int x, y, MouseX, MouseY;
	int w, h, W, H, length;
	bool selected;
	bool keys[256];
	std::string name, text;
	CRender* m_pRender;
	CMenuItem* parent;
public:
	CMenuTextbox(std::string name, CMenuItem* parent, int x, int y, int length, CRender* pRenderer)
	{
		selected = false;
		m_pRender = pRenderer;
		this->name = name;
		this->parent = parent;
		this->x = x;
		this->y = y;
		this->MouseX = 0;
		this->MouseY = 0;
		this->length = length;
		memset(keys, 0, 256);

		m_pRender->GetStringSize(this->W, this->H, "%s", name.c_str());
		SetSize(this->W + 10 + length, H + 4);
	}

	void GetSize(int& w, int& h)
	{
		w = this->w;
		h = this->h;
	}

	std::string GetText()
	{
		return text;
	}

	void SetText(std::string text)
	{
		this->text = text;
	}

	void Draw()
	{
		int posx = 0;
		int posy = 0;

		if (parent)
			parent->GetPosition(posx, posy);

		if (selected)
		{
			m_pRender->DrawRect(posx + x + W + 10, posy + y, length, h, m_pRender->GetColor(Colors::WHITE));
			m_pRender->DrawOutlinedRect(posx + x + W + 10, posy + y, length, h, m_pRender->GetColor(Colors::DARKBLUE));
			m_pRender->DrawFontText(posx + x, posy + y + 2, m_pRender->GetColor(Colors::BLACK), (char*)name.c_str());
		}
		else
		{
			m_pRender->DrawRect(posx + x + W + 10, posy + y, length, h, m_pRender->GetColor(Colors::WHITE));
			m_pRender->DrawOutlinedRect(posx + x + W + 10, posy + y, length, h, m_pRender->GetColor(Colors::BLACK));
			m_pRender->DrawFontText(posx + x, posy + y + 2, m_pRender->GetColor(Colors::BLACK), (char*)name.c_str());
		}

		if (text.size() > 0)
		{
			m_pRender->DrawFontText(posx + x + W + 10 + 4, posy + y + 2, m_pRender->GetColor(Colors::BLACK), (char*)text.c_str());
		}
	}

	bool static invalidChar(char c)
	{
		return !(c >= 0 && c <256);
	}

	void stripUnicode(std::string& str)
	{
		str.erase(std::remove_if(str.begin(), str.end(), invalidChar), str.end());
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
		if (X >= x + W + 10 && X <= x + W + 10 + length && Y >= y && Y <= y + h)
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
			selected = true;
			text.clear();
		}
		else
			selected = false;
	}

	void KeyboardState(BYTE* keyStates) 
	{
		if (!parent->IsSelected())
			selected = false;
		else
		{
			if (selected)
			{
				int j = 0;
				for (int i = 0; i < 256; i++)
				{
					if (keyStates[i] & 0x80 && !keys[i]) //key is down
					{
						keys[i] = true;
						char res[2] = { 0 };

						Notify(i);

						if (ToAscii(i, MapVirtualKey(i, 0), (const BYTE*)keyStates, (LPWORD)res, 0) == 1)
						{
							if (i == VK_BACK)
							{
								text = text.substr(0, text.size() - 1);
								continue;
							}

							int szw, szh;
							m_pRender->GetStringSize(szw, szh, (char*)text.c_str());
							if (szw < length - 12)
							{
								text += res;
								stripUnicode(text);
							}
						}
					}
					else if (!(keyStates[i] & 0x80) && keys[i])
						keys[i] = false;
				}
			}
		}
		
	}

	void ClearText()
	{
		this->text = std::string("");
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

	void LoadSettings(XMLElement* x)
	{
	}

	std::string SaveSettings()
	{
		return "<" + RemoveSpaces(GetItemName()) + ">" + "*" + "</" + RemoveSpaces(GetItemName()) + ">";
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