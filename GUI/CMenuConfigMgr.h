#pragma once
#include "CMenu.h"
#include "MenuListener.h"

class CMenuConfigMgr
{
private:
	CMenu* m_pMenu;
	CRender* m_pRenderer;

	CMenuTextbox* textbox;
	CMenuSelectBox* ConfigSelector;
	std::string m_savePath;
public:
	CMenuConfigMgr(CMenu* pMenu, CRender* pRenderer, std::string savePath)
	{
		m_pMenu = pMenu;
		m_pRenderer = pRenderer;
		m_savePath = savePath;
	}

	void CreateConfigTab(int x, int y)
	{
		CMenuTab* p = new CMenuTab("Config", x, y, m_pRenderer);

		textbox = new CMenuTextbox("Name", p, 10, 113, 100, m_pRenderer);
		*textbox += new Listener<int>([&](int vKey){
			if (vKey == VK_DELETE)
				textbox->ClearText();

			if (vKey == VK_RETURN)
				SaveHandler();
		});

		ConfigSelector = new CMenuSelectBox(p, 0, 0, 136, 79, m_pRenderer);
		*ConfigSelector += new Listener<std::string>([&](std::string name){
			textbox->SetText(name);
		});

		CMenuButton* Load = new CMenuButton("Load", p, 160, 25, m_pRenderer);
		*Load += new Listener<>([&](){
			this->LoadHandler();
		});

		CMenuButton* Refresh = new CMenuButton("Refresh", p, 160, 50, m_pRenderer);
		*Refresh += new Listener<>([&](){
			this->RefreshHandler();
		});

		CMenuButton* Save = new CMenuButton("Save", p, 160, 110, m_pRenderer);
		*Save += new Listener<>([&](){
			this->SaveHandler();
		});

		p->AddMenuItem(ConfigSelector);
		p->AddMenuItemAtPos(Load);
		p->AddMenuItemAtPos(Save);
		p->AddMenuItemAtPos(Refresh);
		p->AddMenuItemAtPos(textbox);

		m_pMenu->AddTab(p);

		RefreshHandler();
	}

	void SaveHandler()
	{
		std::string config = textbox->GetText();
		if (config.empty())
			return;

		SaveSettings(config);
		RefreshHandler();
	}

	void LoadHandler()
	{
		std::string config = ConfigSelector->GetSelection();
		if (config.empty())
			return;

		LoadSettings(config);
	}

	void RefreshHandler()
	{
		ConfigSelector->ClearItems();
		FindConfigs();
	}

private:
	//helper functions
	void LoadSettings(std::string name)
	{
		tinyxml2::XMLDocument cfg;
		cfg.LoadFile((m_savePath + "\\" + name + ".cfg").c_str());

		XMLElement* root = cfg.RootElement();
		int i = 0;
		std::vector<CMenuTab*> Tabs = m_pMenu->GetTabs();

		XMLElement* p = root->FirstChildElement(RemoveSpaces(Tabs[i]->GetItemName()).c_str());

		while (p)
		{
			Tabs[i]->LoadSettings(p);
			i++;

			if (i >= Tabs.size())
				break;

			p = p->NextSiblingElement(RemoveSpaces(Tabs[i]->GetItemName()).c_str());
		}
	}

	void SaveSettings(std::string name)
	{
		tinyxml2::XMLDocument cfg;
		std::vector<CMenuTab*> Tabs = m_pMenu->GetTabs();
		std::string AllTabsSettings;

		for (CMenuTab* tab : Tabs)
		{
			std::string TabSettings = tab->SaveSettings();
			AllTabsSettings += TabSettings;
		}

		AllTabsSettings = "<" + name + ">" + AllTabsSettings + "</" + name + ">";
		cfg.Parse(AllTabsSettings.c_str());

		std::string SaveName = RemoveSpaces(name);

		if (cfg.SaveFile((m_savePath+"\\"+SaveName + ".cfg").c_str()))
			textbox->SetText("Error: Bad filename!");
		else
			textbox->SetText("Config saved!");
	}

	std::string RemoveSpaces(std::string x)
	{
		x.erase(std::remove_if(x.begin(), x.end(), std::isspace), x.end());
		return x;
	}

	void FindConfigs()
	{
		WIN32_FIND_DATA file;
		HANDLE hFirstFile;

		hFirstFile = FindFirstFile((m_savePath+"\\*").c_str(), &file);
		if (hFirstFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(file.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY))
				{
					if (strstr(file.cFileName, ".cfg"))
					{
						std::string config = std::string(file.cFileName);
						config = config.substr(0, config.length() - 4);
						ConfigSelector->addItem(config);
					}
				}
			} while (FindNextFile(hFirstFile, &file));

			FindClose(hFirstFile);
		}
	}
};