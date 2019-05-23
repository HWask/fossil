#pragma once
#include "CMenuItem.h"
#include "..\CRender.h"
#include <functional>
#include <algorithm>

class Textbox : public CMenuItem
{
private:
	int _x;
	int _y;
	int _w;
	int _h;

	int MouseX;
	int MouseY;

	CRender* _pRender;
	CMenuItem* _parent;

	bool keys[256];
	bool IsFocused = false;
	std::string textBuf;
	std::function<void(std::string)> callback = nullptr;

	int LeftMargin = 10;

	std::vector<std::string> buffer;
	int buffer_index = 0;
	int caret_index = 0;
public:
	Textbox(int x, int y, int w, int h, CRender* pRender, CMenuItem* parent) : _x(x), _y(y), _w(w), _h(h),
		_pRender(pRender), _parent(parent) 
	{
		memset(keys, 0, 256);
	}

	void GetSize(int& w, int& h) override
	{
		w = _w;
		h = _h;
	}

	void GetPosition(int& x, int& y) override
	{
		x = _x;
		y = _y;
	}

	void SetPosition(int x, int y) override
	{
		this->_x = x;
		this->_y = y;
	}

	void SetSize(int w, int h) override
	{
		this->_w = w;
		this->_h = h;
	}

	bool InItem(int X, int Y) override
	{
		if (X >= _x && X <= _x + _w && Y >= _y && Y <= _y + _h)
			return true;
		else
			return false;
	}

	void SetCallBack(std::function<void(std::string)> f)
	{
		callback = f;
	}

	void Draw() override
	{
		int __x, __y;
		_parent->GetPosition(__x, __y);

		_pRender->DrawRect(__x + _x, __y + _y, _w, _h, _pRender->GetColor(Colors::TEAL));

		if (IsFocused)
			_pRender->DrawOutlinedRect(__x + _x, __y + _y, _w, _h, _pRender->GetColor(Colors::MENUBLOPSELECTED));

		int textW, textH;
		_pRender->GetStringSize(textW, textH, "M");
		_pRender->DrawFontText(__x + _x + LeftMargin, __y + _y + _h / 2 - textH / 2,
			_pRender->GetColor(Colors::WHITE), (char*)textBuf.c_str());

		int width;
		int caretPos1;
		int caretPos2;

		if (caret_index < textBuf.size())
		{
			caretPos1 = WidthSum(caret_index);
			caretPos2 = WidthSum(caret_index + 1);
			width = caretPos2 - caretPos1;
		}
		else
		{
			caretPos1 = WidthSum(caret_index);
			width = 7;
		}

		if (IsFocused)
			_pRender->DrawRect(__x + _x + LeftMargin + caretPos1, __y + _y + _h / 2 + textH / 2,
			width, 2, _pRender->GetColor(Colors::BLACK));
	}

	bool isValidChar(char c)
	{
		if (c >= 32 && c <= 126)
			return true;
		else
			return false;
	}

	int WidthSum(int i)
	{
		if (i == 0)
			return 0;
		else
		{
			int textW, textH;

			char c[300] = { 0 };
			memcpy(c, textBuf.c_str(), i);
			_pRender->GetStringSize(textW, textH, c);

			auto spaceCount = trailingSpaces(c);
			textW += spaceCount * 3;

			return textW;
		}
	}

	int trailingSpaces(char* str)
	{
		int k = 0;
		for (int i = strlen(str) - 1; i >= 0; i--)
		{
			if (str[i] == ' ')
				k++;
			else
				break;
		}

		return k;
	}

	void KeyboardState(BYTE* keyStates) override
	{
		bool left = false;
		bool right = false;
		bool del = false;
		bool down = false;

		if (GetAsyncKeyState(VK_LEFT) & 1)
			left = true;

		if (GetAsyncKeyState(VK_RIGHT) & 1)
			right = true;

		if (GetAsyncKeyState(VK_DELETE) & 1)
			del = true;

		if (GetAsyncKeyState(VK_DOWN) & 1)
			down = true;

		if (!_parent->IsSelected())
			IsFocused = false;
		else
		{
			if (IsFocused)
			{
				if (left)
				{
					caret_index--;
					if (caret_index < 0)
						caret_index = 0;
				}

				if (right)
				{
					caret_index++;
					if (caret_index > textBuf.size())
						caret_index = textBuf.size();
				}

				if (del)
				{
					caret_index = 0;
					textBuf = "";
					return;
				}

				if (down)
				{
					if (buffer.size() <= buffer_index)
						buffer_index = 0;

					if (buffer.size() > 0)
					{
						textBuf = buffer[buffer_index];
						caret_index = textBuf.size();
					}

					buffer_index++;

					return;
				}

				int j = 0;
				for (int i = 0; i < 256; i++)
				{
					if (keyStates[i] & 0x80 && !keys[i]) //key is down
					{
						keys[i] = true;
						char res[2] = { 0 };

						if (ToAscii(i, MapVirtualKey(i, 0), (const BYTE*)keyStates, (LPWORD)res, 0) == 1)
						{
							if (i == VK_RETURN)
							{
								if (callback != nullptr)
									callback(textBuf);

								buffer.insert(buffer.begin(), textBuf);
								buffer_index = 0;
								if (buffer.size() > 5)
									buffer.pop_back();

								textBuf = "";
								caret_index = 0;
							}
							else if (i == VK_BACK)
							{
								if (textBuf.length() >= 1)
								{
									if (caret_index > 0)
									{
										textBuf.erase(caret_index - 1, 1);
										caret_index--;
									}
								}
							}
							else
							{
								if (isValidChar(res[0]))
								{
									textBuf.insert(caret_index, 1, res[0]);

									int textW, textH;
									_pRender->GetStringSize(textW, textH, (char*)textBuf.c_str());
									if (textW + LeftMargin >= _w - LeftMargin)
										textBuf = textBuf.erase(caret_index, 1);
									else
										caret_index++;
								}
							}
						}
					}
					else if (!(keyStates[i] & 0x80) && keys[i])
						keys[i] = false;
				}
			}
		}

	}

	void LMouseButtonDown() override {}
	void LMouseButtonUp() override {}

	void LMouseButtonClick() override
	{
		if (InItem(this->MouseX, this->MouseY) && _parent->IsSelected())
			IsFocused = true;
		else
			IsFocused = false;
	}

	void MousePos(int x, int y) override
	{
		MouseX = x;
		MouseY = y;
	}

	std::string GetItemName() override
	{
		return "Textbox";
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