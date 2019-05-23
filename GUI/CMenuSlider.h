#pragma once

#include "..\CRender.h"
#include "CMenuItem.h"
#include <string>
#include <sstream>

enum Datatype_t
{
	FL_FLOAT,
	I_INT
};

class CMenuSlider : public CMenuItem
{
private:
	int x, y;
	int w, h, W, H, MouseX, MouseY;
	float flValue, flMin, flMax, flSliderLen;
	float BlobPos;
	bool bDrag, PlusClicked, MinusClicked;
	Datatype_t Datatype;

	std::string name;
	CRender* m_pRender;
	CMenuItem* parent;
public:
	CMenuSlider(std::string name, CMenuItem* parent, int x, int y, float flMin, float flMax, float flSliderLen, Datatype_t Datatype, CRender* pRenderer)
	{
		m_pRender = pRenderer;
		this->name = name;
		this->parent = parent;
		this->x = x;
		this->y = y;
		this->Datatype = Datatype;
		this->flMax = flMax;
		this->flMin = flMin;
		this->flValue = flMin;
		this->flSliderLen = flSliderLen;
		this->BlobPos = 0.0f;
		this->MouseX = 0;
		this->MouseY = 0;
		this->bDrag = false;
		this->PlusClicked = false;
		this->MinusClicked = false;

		m_pRender->GetStringSize(this->W, this->H, "%s", name.c_str());
		SetSize(66 + 12 + 10 + flSliderLen + 10 + 12 + 10 + 30, this->H + 3);
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

		//Name
		m_pRender->DrawFontText(posx + x, posy + y + h / 2.f - 2 - H/2 + 1, m_pRender->GetColor(Colors::BLACK), (char*)name.c_str());

		//+
		if (PlusClicked){
			m_pRender->DrawGradientRect(posx + x + 66 + 12 + 10 + flSliderLen + 10, posy + y + h / 2.f - 2 - 5, 13, 13, m_pRender->GetColor(Colors::GRAY), m_pRender->GetColor(Colors::MENUBLOPSELECTED));
			PlusClicked = false;
		}
		else
			m_pRender->DrawGradientRect(posx + x + 66 + 12 + 10 + flSliderLen + 10, posy + y + h / 2.f - 2 - 5, 13, 13, m_pRender->GetColor(Colors::GRAY), m_pRender->GetColor(Colors::BLACK));

		m_pRender->DrawFontText(posx + x + 66 + 12 + 10 + flSliderLen + 10 + 6 - 2, posy + y + h / 2.f - 2 - H / 2 + 1, m_pRender->GetColor(Colors::WHITE), "+");

		//-
		if (MinusClicked){
			m_pRender->DrawGradientRect(posx + x + 66, posy + y + h / 2.f - 2 - 5, 13, 13, m_pRender->GetColor(Colors::GRAY), m_pRender->GetColor(Colors::MENUBLOPSELECTED));
			MinusClicked = false;
		}
		else
			m_pRender->DrawGradientRect(posx + x + 66, posy + y + h / 2.f - 2 - 5, 13, 13, m_pRender->GetColor(Colors::GRAY), m_pRender->GetColor(Colors::BLACK));

		m_pRender->DrawFontText(posx + x + 66 + 6 - 2 + 1, posy + y + h / 2.f - 2 - H / 2, m_pRender->GetColor(Colors::WHITE), "-");

		//Slider
		m_pRender->DrawRect(posx + x + 66 + 13 + 10, posy + y + h / 2.f - 2, flSliderLen, 4, m_pRender->GetColor(Colors::BLACK));
		//Blob
		if (bDrag){
			m_pRender->DrawRect(posx + x + 66 + 13 + 10 + BlobPos - 3, posy + y + h / 2.f - 2 - 5, 6, 13, m_pRender->GetColor(Colors::MENUBLOPSELECTED));
			bDrag = false;
		}
		else{
			m_pRender->DrawRect(posx + x + 66 + 13 + 10 + BlobPos - 3, posy + y + h / 2.f - 2 - 5, 6, 13, m_pRender->GetColor(Colors::MENUBLOP));
		}

		//Value
		if (Datatype == FL_FLOAT)
			m_pRender->DrawFontText(posx + x + 66 + 13 + 10 + flSliderLen + 10 + 12 + 10, posy + y + h / 2.f - 2 - H / 2 + 1, m_pRender->GetColor(Colors::BLACK), (char*)FloatToString(flValue).c_str());
		else
			m_pRender->DrawFontText(posx + x + 66 + 13 + 10 + flSliderLen + 10 + 12 + 10, posy + y + h / 2.f - 2 - H / 2 + 1, m_pRender->GetColor(Colors::BLACK), (char*)IntToString(flValue).c_str());
		
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

	//In Slider?
	bool InItem(int X, int Y)
	{
		//slider
		if (X >= x + 66 + 13 + 10 - 3 && X <= x + 66 + 13 + 10 + flSliderLen + 3 && Y >= y && Y <= y + h)
			return true;
		else
			return false;
	}

	bool InPlus(int X, int Y)
	{
		//+
		if (X >= x + 66 + 13 + 10 + flSliderLen + 10 && X <= x + 66 + 13 + 10 + flSliderLen + 10 + 13 && Y >= y + h / 2.f - 2 - 5 && Y <= y + h / 2.f - 2 - 5 + 13)
			return true;
		else
			return false;
	}

	bool InMinus(int X, int Y)
	{
		//-
		if (X >= x + 66 && X <= x + 66 + 13 && Y >= y + h / 2.f - 2 - 5 && Y <= y + h / 2.f - 2 - 5 + 13)
			return true;
		else
			return false;
	}

	//Position relative to parent
	void SetSliderPosition(int X, int Y)
	{
		BlobPos = X - (66 + 13 + 10 + 9);

		MapSliderToValue();

		if (BlobPos <= 0)
		{
			flValue = flMin;
			BlobPos = 0.0f;
		}
		if (BlobPos >= flSliderLen)
		{
			flValue = flMax;
			BlobPos = flSliderLen;
		}
	}

	void MapSliderToValue()
	{
		flValue = ((flMax - flMin) / flSliderLen) * BlobPos + flMin;
	}

	void MapValueToSlider()
	{
		BlobPos = ((flValue - flMin)*flSliderLen) / (flMax - flMin);

		if (BlobPos <= 0)
		{
			flValue = flMin;
			BlobPos = 0.0f;
		}
		if (BlobPos >= flSliderLen)
		{
			flValue = flMax;
			BlobPos = flSliderLen;
		}
	}

	void DragSlider(int X, int Y)
	{
		SetSliderPosition(X, Y);
	}

	void LMouseButtonDown()
	{
		if (InItem(this->MouseX, this->MouseY) && parent->IsSelected())
		{
			bDrag = true;
			DragSlider(this->MouseX, this->MouseY);
		}

		if (InPlus(this->MouseX, this->MouseY) && parent->IsSelected())
		{
			PlusClicked = true;
		}

		if (InMinus(this->MouseX, this->MouseY) && parent->IsSelected())
		{
			MinusClicked = true;
		}
	}

	void LMouseButtonClick()
	{
		if (InPlus(this->MouseX, this->MouseY) && parent->IsSelected())
		{
			if (Datatype == Datatype_t::FL_FLOAT)
				flValue += 0.1;
			else
				flValue++;

			MapValueToSlider();
		}

		if (InMinus(this->MouseX, this->MouseY) && parent->IsSelected())
		{
			if (Datatype == Datatype_t::FL_FLOAT)
				flValue -= 0.1;
			else
				flValue--;

			MapValueToSlider();
		}
	}

	void LMouseButtonUp()
	{
		bDrag = false;
		MinusClicked = false;
		PlusClicked = false;
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
		float val = StringToFloat(ele->GetText());
		this->flValue = val;
		MapValueToSlider();
	}

	std::string SaveSettings()
	{
		return "<" + RemoveSpaces(GetItemName()) + ">" + std::to_string(flValue) + "</" + RemoveSpaces(GetItemName()) + ">";
	}

	float GetflValue()
	{
		return flValue;
	}

	int GetiValue()
	{
		return (int)flValue;
	}

	bool GetbValue()
	{
		return false;
	}
};