#pragma once
#include "CMenuTab.h"
#include "ConCheatClass.h"
#include <unordered_map>
#include <string>
#include <algorithm>
#include <regex>
#include <fstream>
#include <vector>

#define REDCOL _pRender->GetColor(Colors::RED)

class Console : public CMenuTab
{
private:
	Textarea* textarea;
	Textbox* textbox;
	CRender* _pRender;
public:
	enum Datatype
	{
		FLOAT,
		BOOL,
		INT,
		ARRAY
	};

	struct CMD
	{
		ConsoleCheatClass* pCheat;
		std::string tooltip;
		Datatype type;
		int index;

		float min;
		float max;
		std::vector<int> vecValues;
	};

	std::unordered_map<std::string, CMD> values;
	std::vector<std::string> weapons;
public:
	Console(int x, int y, int w, int h, CRender* pRender) : CMenuTab("Console", x, y, pRender),
		weapons({
		"None", "Deagle", "Elite", "Fiveseven", "Glock", "P228", "USP", "AK47",
		"AUG", "AWP", "Famas", "G3SG1", "Galil", "Galilar", "M249", "M3", "M4A1",
		"Mac10", "MP5Navy", "P90", "Scout", "SG550", "SG552", "TMP", "UMP45",
		"XM1014", "Bizon", "Mag7", "Negev", "Sawedoff", "Tec9", "Taser", "HKP2000",
		"MP7", "MP9", "Nova", "P250", "Scar17", "Scar20", "SG556", "SSG08", "Knifeegg",
		"Knife", "Flash", "He", "Smoke", "Molotov", "Decoy", "Incgrenade", "C4"
	})
	{
		textarea = new Textarea(10, 30, w-20, h-30-30-10, pRender, this);
		textbox = new Textbox(10, h-30-5, w-20, 30, pRender, this);

		this->AddMenuItemAtPos(textarea);
		this->AddMenuItemAtPos(textbox);

		_pRender = pRender;

		textbox->SetCallBack(std::bind(&Console::InputHandler, this, std::placeholders::_1));
	}

	void RegisterFloat(std::string name, std::string tooltip, float min, float max, ConsoleCheatClass* pCheat, int index)
	{
		CMD cmd{
			pCheat,
			tooltip,
			Datatype::FLOAT,
			index,
			min,
			max
		};
		values[name] = cmd;
	}

	void RegisterBool(std::string name, std::string tooltip, ConsoleCheatClass* pCheat, int index)
	{
		CMD cmd{
			pCheat,
			tooltip,
			Datatype::BOOL,
			index
		};
		values[name] = cmd;
	}

	void RegisterInt(std::string name, std::string tooltip, int min, int max, ConsoleCheatClass* pCheat, int index)
	{
		CMD cmd{
			pCheat,
			tooltip,
			Datatype::INT,
			index,
			min,
			max
		};
		values[name] = cmd;
	}

	void RegisterArray(std::string name, std::string tooltip, std::vector<int> allowedValues, 
		ConsoleCheatClass* pCheat, int index)
	{
		CMD cmd{
			pCheat,
			tooltip,
			Datatype::ARRAY,
			index,
			0,
			0,
			allowedValues
		};
		values[name] = cmd;
	}

	std::string trim(std::string s)
	{
		if (s.empty())
			return s;
		else
		{
			int i = 0;
			while (i < s.length() && i == ' ')
				i++;

			int j = s.length() - 1;
			while (j >= 0 && j == ' ')
				j--;

			return s.substr(i, j - i + 1);
		}
	}

	std::string vectos(std::vector<int> vec)
	{
		std::string s;

		s += "{ ";
		for (int i = 0; i < vec.size(); i++)
		{
			if (i != vec.size() - 1)
				s += std::to_string(vec[i]) + ", ";
			else
				s += std::to_string(vec[i]);
		}
		s += " }";

		return s;
	}

	void InputHandler(std::string input)
	{
		auto trimmed = trim(input);
		if (trimmed.empty()) //invalid command
		{
			textarea->AddTextLine("Invalid command. Type help to get a list of commands.", REDCOL);
			return;
		}

		textarea->AddTextLine(trimmed);

		if (!trimmed.compare("weapons"))
		{
			for (auto weapon : weapons)
				textarea->AddTextLine(weapon);

			return;
		}

		if (!trimmed.compare("settings"))
		{
			for (auto& val : values)
			{
				switch (val.second.type)
				{
				case Datatype::ARRAY:
				{
					textarea->AddTextLine(val.first+" " + vectos(val.second.pCheat->GetVECValue(
						val.second.index)));
					break;
				}
				case Datatype::INT:
				{
					textarea->AddTextLine(val.first + " " + std::to_string(val.second.pCheat->GetIValue(
						val.second.index)));
					break;
				}
				case Datatype::BOOL:
				{
					auto bVal = val.second.pCheat->GetBoolValue(
						val.second.index);
					textarea->AddTextLine(val.first + " " + (bVal == true ? "true" : "false"));
					break;
				}
				case Datatype::FLOAT:
				{
					textarea->AddTextLine(val.first + " " + std::to_string(val.second.pCheat->GetFLValue(
						val.second.index)));
					break;
				}
				default: //unknown type
					textarea->AddTextLine("Unkown Type", REDCOL);
					break;
				}
			}

			return;
		}

		if (!trimmed.compare("help"))
		{
			for (auto& val : values)
			{
				textarea->AddTextLine(val.first + " - " + val.second.tooltip);
				switch (val.second.type)
				{
				case Datatype::ARRAY:
				{
					textarea->AddTextLine("range " + vectos(val.second.vecValues));
					break;
				}
				case Datatype::INT:
				{
					textarea->AddTextLine("range min " + std::to_string((int)val.second.min) + 
						" max " + std::to_string((int)val.second.max));
					break;
				}
				case Datatype::BOOL:
				{
					textarea->AddTextLine("range false or true");
					break;
				}
				case Datatype::FLOAT:
				{
					textarea->AddTextLine("range min " + std::to_string(val.second.min) + 
						" max " + std::to_string(val.second.max));
					break;
				}
				default: //unknown type
					textarea->AddTextLine("Unkown Type", REDCOL);
					break;
				}
				textarea->AddTextLine(" ");
			}

			return;
		}

		if (!trimmed.compare("clear"))
		{
			textarea->clear();
			return;
		}

		//setCMD
		{
			std::regex setCmdInt("^([A-Za-z0-9.]+)\\s+(\\d+)$");
			std::regex setCmdBool("^([A-Za-z0-9.]+)\\s+(false|true)$");
			std::regex setCmdFloat("^([A-Za-z0-9.]+)\\s+(\\d+\\.\\d+)$");
			std::regex setCmdArray("^([A-Za-z0-9.]+)\\s+(\\{(\\s*\\d+\\s*,)*\\s*\\d+\\s*\\})$");
			std::regex ArrayIterator("\\s*\\d+\\s*");

			std::smatch groups3;
			std::regex_search(trimmed, groups3, setCmdInt);
			if (groups3.size() == 3 && groups3[1].str().length() > 0 &&
				groups3[2].str().length() > 0)
			{
				if (values.count(groups3[1].str()) != 0)
				{
					auto val = values[groups3[1].str()];
					if (val.type == Datatype::INT)
					{
						int k = std::stoi(groups3[2].str());
						if (val.min <= k && k <= val.max)
						{
							val.pCheat->SetIValue(k, val.index);
							return;
						}
						else
							textarea->AddTextLine("Value out of range " + groups3[2].str(), REDCOL);
					}
					else
						textarea->AddTextLine("Invalid Datatype", REDCOL);
				}
			}

			std::smatch groups4;
			std::regex_search(trimmed, groups4, setCmdBool);
			if (groups4.size() == 3 && groups4[1].str().length() > 0 &&
				groups4[2].str().length() > 0)
			{
				if (values.count(groups4[1].str()) != 0)
				{
					auto val = values[groups4[1].str()];
					if (val.type == Datatype::BOOL)
					{
						auto szBool = trim(groups4[2].str());
						val.pCheat->SetBoolValue(szBool == "true" ? true : false, val.index);
						return;
					}
					else
						textarea->AddTextLine("Invalid Datatype", REDCOL);
				}
			}

			std::smatch groups5;
			std::regex_search(trimmed, groups5, setCmdFloat);
			if (groups5.size() == 3 && groups5[1].str().length() > 0 &&
				groups5[2].str().length() > 0)
			{
				if (values.count(groups5[1].str()) != 0)
				{
					auto val = values[groups5[1].str()];
					if (val.type == Datatype::FLOAT)
					{
						float k = std::stof(groups5[2].str());
						if (val.min <= k && k <= val.max)
						{
							val.pCheat->SetFLValue(k, val.index);
							return;
						}
						else
							textarea->AddTextLine("Value ouf of range: " + groups5[2].str(), REDCOL);
					}
					else
						textarea->AddTextLine("Invalid Datatype", REDCOL);
				}
			}

			std::smatch groups6;
			std::regex_search(trimmed, groups6, setCmdArray);
			if (groups6.size() >= 3 && groups6[1].str().length() > 0 &&
				groups6[2].str().length() > 0)
			{
				if (values.count(groups6[1].str()) != 0)
				{
					auto val = values[groups6[1].str()];
					if (val.type == Datatype::ARRAY)
					{
						std::smatch matches;
						std::string vector = groups6[2].str();
						std::sregex_iterator begin(vector.begin(), vector.end(), ArrayIterator);
						std::sregex_iterator end;
						std::vector<int> toFill;
						for (std::sregex_iterator it = begin; it != end; ++it)
						{
							std::smatch match = *it;
							if (std::find(val.vecValues.begin(), val.vecValues.end(), std::stoi(match[0].str()))
								!= val.vecValues.end())
							{
								toFill.push_back(std::stoi(match[0].str()));
							}
							else
								textarea->AddTextLine("Value ouf of range " + match[0].str(), REDCOL);
						}

						val.pCheat->SetVECValue(toFill, val.index);
						return;
					}
					else
						textarea->AddTextLine("Invalid Datatype", REDCOL);
				}
			}
		}

		std::regex getCmd("^([A-Za-z0-9.]+)$");
		std::smatch groups7;
		std::regex_search(trimmed, groups7, getCmd);
		//getCmd
		if (groups7.size() == 2 && groups7[1].str().length() > 0)
		{
			auto test = groups7[1].str();
			if (values.count(groups7[1].str()) != 0)
			{
				auto val = values[groups7[1].str()];
				switch (val.type)
				{
				case Datatype::ARRAY:
				{
					auto vec = val.pCheat->GetVECValue(val.index);
					textarea->AddTextLine(groups7[1].str() + " " + vectos(vec));
					return;
				}
				case Datatype::INT:
				{
					auto ival = val.pCheat->GetIValue(val.index);
					textarea->AddTextLine(groups7[1].str() + " " + std::to_string(ival));
					return;
				}
				case Datatype::BOOL:
				{
					auto bval = val.pCheat->GetBoolValue(val.index);
					textarea->AddTextLine(groups7[1].str() + " " + (bval == true ? "true" : "false"));
					return;
				}
				case Datatype::FLOAT:
				{
					auto flVal = val.pCheat->GetFLValue(val.index);
					textarea->AddTextLine(groups7[1].str() + " " + std::to_string(flVal));
					return;
				}
				default: //unknown type
					textarea->AddTextLine("Unkown Type", REDCOL);
					return;
				}
			}
		}

		textarea->AddTextLine("Invalid command. Type help to get a list of commands.", REDCOL);
	}

	void LoadSettings(XMLElement* ele) override
	{
		int x = StringToInt(ele->FirstChildElement("x")->GetText());
		int y = StringToInt(ele->FirstChildElement("y")->GetText());
		int z = StringToInt(ele->FirstChildElement("z")->GetText());
		bool on = StringToBool(ele->FirstChildElement("on")->GetText());

		this->x = x;
		this->y = y;
		this->zBuffer[this->id] = z;
		this->bIsActive = on;

		XMLElement* p = ele->FirstChildElement("on");
		if (!p)
			return;

		XMLElement* q = p->NextSiblingElement();
		while (q)
		{
			if (values.count(std::string(q->Name())) != 0)
			{
				auto type = values[std::string(q->Name())].type;
				switch (type)
				{
				case Datatype::INT:
				{
					values[std::string(q->Name())].pCheat->SetIValue(std::stoi(q->GetText()), values[std::string(q->Name())].index);
					break;
				}
				case Datatype::BOOL:
				{
					values[std::string(q->Name())].pCheat->SetBoolValue(std::string(q->GetText()) == "true" ? true : false, values[std::string(q->Name())].index);
					break;
				}
				case Datatype::FLOAT:
				{
					values[std::string(q->Name())].pCheat->SetFLValue(std::stof(q->GetText()), values[std::string(q->Name())].index);
					break;
				}
				case Datatype::ARRAY:
				{
					/*
					std::regex iIntsPattern("\\s*\\d+\\s*");
					std::sregex_iterator begin(value.begin(), value.end(), iIntsPattern);
					std::sregex_iterator end;
					std::vector<int> toSet;
					for (auto it = begin; it != end; ++it)
					{
						std::smatch iInts = *it;
						toSet.push_back(std::stoi(iInts[0].str()));
					}
					values[name].pCheat->SetVECValue(toSet, values[name].index);
					*/
					break;
				}
				default:
					break;
				}
			}

			q = q->NextSiblingElement();
		}
	}

	std::string SaveSettings() override
	{
		tinyxml2::XMLDocument cfg;
		std::string AllMenuSettings;

		for (auto& val : values)
		{
			std::string setting;

			switch (val.second.type)
			{
			case Datatype::INT:
			{
				setting += "<" + val.first + ">" + std::to_string(val.second.pCheat->GetIValue(val.second.index)) + "</" + val.first + ">";
				break;
			}
			case Datatype::BOOL:
			{
				auto b = val.second.pCheat->GetBoolValue(val.second.index);
				setting += "<" + val.first + ">" + (val.second.pCheat->GetBoolValue(val.second.index) == true ? "true" : "false") + "</" + val.first + ">";
				break;
			}
			case Datatype::FLOAT:
			{
				setting += "<" + val.first + ">" + std::to_string(val.second.pCheat->GetFLValue(val.second.index)) + "</" + val.first + ">";
				break;
			}
			case Datatype::ARRAY:
			{
				auto vec = val.second.pCheat->GetVECValue(val.second.index);
				setting += "<" + val.first + ">" + vectos(vec) + "</" + val.first + ">";
				break;
			}
			default:
				break;
			}

			AllMenuSettings += setting;
		}

		AllMenuSettings = "<" + RemoveSpaces(this->GetItemName()) + ">" + AllMenuSettings + "</" + RemoveSpaces(this->GetItemName()) + ">";
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
};