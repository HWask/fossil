#pragma once

#include <functional>

template<typename ...T>
class Base
{
public:
	virtual void HandleNotification(T...) = 0;
};

template<typename ...T>
class Listener : public Base<T...>
{
private:
	typedef std::function<void(T...)> Callback;

	Callback _f;
public:
	Listener(Callback f) : _f(f) {}

	void HandleNotification(T... args)
	{
		_f(args...);
	}
};