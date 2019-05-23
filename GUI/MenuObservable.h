#pragma once

#include <set>
#include <algorithm>
#include "MenuListener.h"

template<typename ...T>
class Observable
{
protected:
	void Notify(T... args)
	{
		std::for_each(_listeners.begin(), _listeners.end(), [&](Base<T...>* l) {l->HandleNotification(args...); });
	}
public:
	Observable& operator+=(Base<T...>* l)
	{
		_listeners.insert(l);
		return *this;
	}

	void RegisterListener(Base<T...>* l)
	{
		_listeners.insert(l);
	}

	void UnregisterListener(Base<T...>* l)
	{
		std::set<Base<T...>*>::const_iterator iter = _listeners.find(l);
		if (iter != _listeners.end())
			_listeners.erase(iter);
	}
private:
	std::set<Base<T...>*> _listeners;
};