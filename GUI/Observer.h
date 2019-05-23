#pragma once
#include "CMenuItem.h"
#include <list>

class Observer
{
public:
	std::list<CMenuItem*> items;

	virtual void Update() = 0;
	virtual void addObserver(CMenuItem* item)
	{
		items.push_back(item);
	}
};