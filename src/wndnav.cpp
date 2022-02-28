#include "windows.h"

void WindowNavigator::add(Window* wnd)
{
	m_list.push_back(wnd);
}

bool WindowNavigator::set(Window* wnd)
{
	for (std::list<Window*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if (*it == wnd)
		{
			m_it = it;
			return true;
		}
	}
	return false;
}

Window* WindowNavigator::get(void)
{
	return *m_it;
}

Window* WindowNavigator::movePrev(void)
{
	if (m_it == m_list.begin())
		return NULL;
	m_it--;
	return *m_it;
}

Window* WindowNavigator::moveNext(void)
{
	std::list<Window*>::iterator it = m_it;
	it++;
	if (it == m_list.end())
		return NULL;
	m_it++;
	return *m_it;
}
