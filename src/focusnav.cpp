#include "windows.h"

FocusWindowNavigator::FocusWindowNavigator(Window* parent, int style)
	: m_parent(parent), m_style(style)
{
}

void FocusWindowNavigator::addChild(Window* wnd)
{
	m_list.push_back(wnd);
}

bool FocusWindowNavigator::setFocusChild(Window* wnd)
{
	for (std::list<Window*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if (*it == wnd)
		{
			m_it = it;
			
			m_parent->setFocusChild(*m_it);
			return true;
		}
	}
	return false;
}

Window* FocusWindowNavigator::getFocusChild(void)
{
	return *m_it;
}

bool FocusWindowNavigator::onKeyUp(KeyCode key)
{
	bool valid = false;
	if (m_style & FWNS_UPDOWN)
	{
		if (key == KEY_UP || key == KEY_DOWN)
			valid = true;
	}
	if (m_style & FWNS_LEFTRIGHT)
	{
		if (key == KEY_LEFT || key == KEY_RIGHT)
			valid = true;
	}
	
	if (!valid) return false;
	
	Window* wnd;
	
	switch (key)
	{
	case KEY_UP:
	case KEY_LEFT:
		wnd = _prev();
		break;
	case KEY_DOWN:
	case KEY_RIGHT:
		wnd = _next();
		break;
	}
	
	if (wnd)
	{
		m_parent->setFocusChild(wnd);
		return true;
	}
	return false;
}

Window* FocusWindowNavigator::_prev(void)
{
	if (m_it == m_list.begin())
		return NULL;
	m_it--;
	return *m_it;
}

Window* FocusWindowNavigator::_next(void)
{
	std::list<Window*>::iterator it = m_it;
	it++;
	if (it == m_list.end())
		return NULL;
	m_it++;
	return *m_it;
}



