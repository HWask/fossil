#ifndef _CMENUBUTTON_H_
#define _CMENUBUTTON_H_

#include "..\CRender.h"
#include "CMenuItem.h"
#include <string>
#include "MenuObservable.h"

class CMenuButton : public CMenuItem, public Observable<>
{
private:
	int x, y, MouseX, MouseY;
	int w, h, W, H;
	bool OnClick;
	std::string name;
	CRender* m_pRender;
	CMenuItem* parent;
public:
	CMenuButton(std::string name, CMenuItem* parent, int x, int y, CRender* pRenderer)
	{
		m_pRender = pRenderer;
		this->name = name;
		this->parent = parent;
		this->x = x;
		this->y = y;
		this->MouseX = 0;
		this->MouseY = 0;
		this->OnClick = false;

		m_pRender->GetStringSize(this->W, this->H, "%s", name.c_str());
		SetSize(W + 30, this->H + 10);
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

		if (OnClick)
		{
			m_pRender->DrawRect(posx + x, posy + y, w, h, m_pRender->GetColor(Colors::MENUBLOPSELECTED));
		}
		else
			m_pRender->DrawGradientRect(posx + x, posy + y, w, h, m_pRender->GetColor(Colors::GRAY), m_pRender->GetColor(Colors::BLACK));

		m_pRender->DrawFontText(posx + x + w / 2 - W / 2, posy + y + h / 2 - H / 2, m_pRender->GetColor(Colors::WHITE), (char*)name.c_str());
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

	void LMouseButtonDown()
	{
		if (InItem(this->MouseX, this->MouseY) && parent->IsSelected())
			OnClick = true;
		else
			OnClick = false;
	}

	void LMouseButtonUp()
	{
		OnClick = false;
	}

	void LMouseButtonClick()
	{
		if (InItem(this->MouseX, this->MouseY) && parent->IsSelected())
		{
			//Call handler here
			Notify();
		}
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

#endif