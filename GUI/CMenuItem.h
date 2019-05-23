#pragma once

#include "tinyxml2.h"
using namespace tinyxml2;

#include <string>
#include <algorithm>
#include <sstream>

class CMenuItem
{
public:
	virtual void GetSize(int& w, int& h) = 0;
	virtual void GetPosition(int& x, int& y) = 0;
	virtual void SetPosition(int x, int y) = 0;
	virtual void SetSize(int w, int h) = 0;
	virtual void Draw() = 0;
	virtual bool InItem(int X, int Y) = 0;
	virtual void MousePos(int X, int Y) = 0;
	virtual void LMouseButtonDown() = 0;
	virtual void LMouseButtonUp() = 0;
	virtual void LMouseButtonClick() = 0;
	virtual void KeyboardState(BYTE* keyStates) { }
	virtual std::string GetItemName() = 0;
	virtual void GetAbsPosition(int& x, int& y) {}
	virtual void LoadSettings(XMLElement* ele) = 0;
	virtual std::string SaveSettings() = 0;
	virtual bool IsSelected() { return false; }

	virtual float GetflValue() = 0;
	virtual int GetiValue() = 0;
	virtual bool GetbValue() = 0;

	int StringToInt(std::string iValue)
	{
		std::stringstream sstr;
		sstr << iValue;
		int b;
		sstr >> b;

		return b;
	}

	float StringToFloat(std::string flValue)
	{
		std::stringstream sstr;
		sstr << flValue;
		float b;
		sstr >> b;

		return b;
	}

	bool StringToBool(std::string bValue)
	{
		std::stringstream sstr;
		sstr << bValue;
		bool b;
		sstr >> b;

		return b;
	}

	std::string FloatToString(float f)
	{
		std::stringstream sstr;
		sstr << f;
		return sstr.str();
	}

	std::string IntToString(int i)
	{
		std::stringstream sstr;
		sstr << i;
		return sstr.str();
	}

	std::string ULongLongToString(unsigned long long i)
	{
		std::stringstream sstr;
		sstr << i;
		return sstr.str();
	}

	std::string RemoveSpaces(std::string x)
	{
		x.erase(std::remove_if(x.begin(), x.end(), std::isspace), x.end());
		return x;
	}
};