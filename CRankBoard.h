#pragma once
#include <tuple>
#include <algorithm>

#include "Factory.h"
#include "CRender.h"

class CRankBoard {
private:
	bool bCreated = false;
	Factory* m_pFactory;
	CRender* m_pRender;

	DWORD gameResourcesAddr;
	const DWORD winsOffset = 0x1B40;
	const DWORD rankOffset = 0x1A3C;

	const float rowWidth = 500;
	const float rowHeight = 23;
	const float headerHeight = 17;
	const float paddingLeft = 15;

	std::string Ranks[19];
	std::vector<std::tuple<int, int, int, std::string>> scoreBoard;
private:
	std::string trim(std::string s)
	{
		s.erase(s.find_last_not_of(" ") + 1);
		return s;
	}
public:
	CRankBoard()
	{
		Ranks[0] = std::string("Unranked");
		Ranks[1] = std::string("Silver I");
		Ranks[2] = std::string("Silver II");
		Ranks[3] = std::string("Silver III");
		Ranks[4] = std::string("Silver IV");
		Ranks[5] = std::string("Silver Elite");
		Ranks[6] = std::string("Silver Elite Master");
		Ranks[7] = std::string("Gold Nova I");
		Ranks[8] = std::string("Gold Nova II");
		Ranks[9] = std::string("Gold Nova III");
		Ranks[10] = std::string("Gold Nova Master");
		Ranks[11] = std::string("Master Guardian I");
		Ranks[12] = std::string("Master Guardian II");
		Ranks[13] = std::string("Master Guardian Elite");
		Ranks[14] = std::string("Distinguished Master Guardian");
		Ranks[15] = std::string("Legendary Eagle");
		Ranks[16] = std::string("Legendary Eagle Master");
		Ranks[17] = std::string("Supreme Master First Class");
		Ranks[18] = std::string("The Global Elite");
	}

	void Create(Factory* pFactory, CRender* pRender) {
		m_pFactory = pFactory;
		m_pRender = pRender;
		bCreated = true;
	}

	bool IsCreated() {
		return bCreated;
	}

	void getGameResourcesAddr() {
		auto client = (DWORD)GetModuleHandle("client.dll");
		auto addr = Utils::FindPattern(client, 0xFFFFFFFF, (BYTE*)"\x56\x57\x83\xF8\x01\x0F\x85\xF5\x01\x00\x00\x8B\x3D", "xxxxxxx????xx");
		gameResourcesAddr = *(DWORD*)(addr + 13);
	}

	bool GrabData()
	{
		if (!m_pFactory->clientModule || !m_pFactory->m_pEngine->IsInGame() || !m_pFactory->m_pEngine->IsConnected())
			return false;

		scoreBoard.clear();

		DWORD temp = *(DWORD*)gameResourcesAddr;
		int* wins = (int*)(temp + winsOffset);
		int* ranks = (int*)(temp + rankOffset);
		std::vector<std::tuple<int, int, int, std::string>> scoreBoardUnsorted;

		for (int i = 0; i < m_pFactory->m_pEngine->GetMaxClients(); i++)
		{
			auto ent = m_pFactory->m_pEntList->GetClientEntity(i);
			if (!ent)
				continue;

			player_info_t info;
			m_pFactory->m_pEngine->GetPlayerInfo(i, &info);

			if (trim(std::string(info.name)).find("GOTV") != std::string::npos || trim(std::string(info.name)).length() == 0)
				continue;

			scoreBoardUnsorted.push_back( std::make_tuple(ent->GetTeamNum(), wins[i], ranks[i], std::string(info.name)) );
		}

		int iLocal = m_pFactory->m_pEngine->GetLocalPlayer();
		auto ME = m_pFactory->m_pEntList->GetClientEntity(iLocal);
		if (!ME)
			return false;

		int myTeam = ME->GetTeamNum();

		//sort
		for (int i = 0; i < scoreBoardUnsorted.size(); i++)
		{
			if (std::get<0>(scoreBoardUnsorted[i]) == myTeam)
				scoreBoard.insert(scoreBoard.begin(), scoreBoardUnsorted[i]);
			else
				scoreBoard.push_back(scoreBoardUnsorted[i]);
		}

		return true;
	}

	void Render(int x, int y) {
		//Header
		m_pRender->DrawRect(x, y, rowWidth, headerHeight, m_pRender->GetColor(Colors::ScoreBoardHeader));
		m_pRender->DrawOutlinedRect(x, y, rowWidth, headerHeight, m_pRender->GetColor(Colors::WHITE));

		int w, h;
		m_pRender->GetStringSize(w, h, "Player");
		m_pRender->DrawOutlinedFontText(x + rowWidth / 6 - w / 2.f, y + headerHeight / 2 - h / 2.f, m_pRender->GetColor(Colors::WHITE), "Player");

		m_pRender->GetStringSize(w, h, "Wins");
		m_pRender->DrawOutlinedFontText(x + rowWidth / 2 - w / 2.f, y + headerHeight / 2 - h / 2.f, m_pRender->GetColor(Colors::WHITE), "Wins");

		m_pRender->GetStringSize(w, h, "Rank");
		m_pRender->DrawOutlinedFontText(x + 5 * rowWidth / 6 - w / 2.f, y + headerHeight / 2 - h / 2.f, m_pRender->GetColor(Colors::WHITE), "Rank");

		for (int i = 0; i < scoreBoard.size(); i++) {
			if (i % 2 == 0) {
				m_pRender->DrawRect(x, y + headerHeight + i * rowHeight, rowWidth, rowHeight, m_pRender->GetColor(Colors::ScoreBoardRowEven));
				m_pRender->DrawOutlinedRect(x, y + headerHeight + i * rowHeight, rowWidth, rowHeight, m_pRender->GetColor(Colors::WHITE));
			}
			else
			{
				m_pRender->DrawRect(x, y + headerHeight + i * rowHeight, rowWidth, rowHeight, m_pRender->GetColor(Colors::ScoreBoardRowOdd));
				m_pRender->DrawOutlinedRect(x, y + headerHeight + i * rowHeight, rowWidth, rowHeight, m_pRender->GetColor(Colors::WHITE));
			}

			DWORD color;
			if (std::get<0>(scoreBoard[i]) == 2)
				color = m_pRender->GetColor(Colors::RED);
			else
				color = m_pRender->GetColor(Colors::DARKBLUE);

			m_pRender->DrawOutlinedFontTextW(x + paddingLeft, y + headerHeight + i * rowHeight + rowHeight / 2 - h / 2.f,
				color, (wchar_t*)Utils::asciiToUnicode(std::get<3>(scoreBoard[i])).c_str());
			m_pRender->DrawOutlinedFontText(x + rowWidth / 3 + paddingLeft, y + headerHeight + i * rowHeight + rowHeight / 2 - h / 2.f,
				color, (char*)std::to_string(std::get<1>(scoreBoard[i])).c_str());
			m_pRender->DrawOutlinedFontText(x + 2 * rowWidth / 3 + paddingLeft, y + headerHeight + i * rowHeight + rowHeight / 2 - h / 2.f,
				color, (char*)Ranks[std::get<2>(scoreBoard[i])].c_str());
		}

		//dividing line
		m_pRender->DrawLine(x + rowWidth / 3, y, x + rowWidth / 3, y + headerHeight + scoreBoard.size() * rowHeight, 1, m_pRender->GetColor(Colors::WHITE));
		m_pRender->DrawLine(x + 2 * rowWidth / 3, y, x + 2 * rowWidth / 3, y + headerHeight + scoreBoard.size() * rowHeight, 1, m_pRender->GetColor(Colors::WHITE));
	}

	void GetDimensions(int& w, int& h) 
	{
		w = rowWidth;
		h = headerHeight + scoreBoard.size() * rowHeight;
	}
};
