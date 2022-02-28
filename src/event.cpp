#include "windows.h"

//
// key event
//
void KeyEvent::process(void)
{
	Window* focus = WindowManager::getManager()->getFocus();
	
	_process(focus);
}

void KeyEvent::_process(Window* wnd)
{
	bool handled = false;
	
	handled = wnd->onOverrideChildKey(m_keyEvent, m_keyCode);
	
	if (handled)
	{
		if (m_keyEvent == KEY_RELEASE)
			wnd->onKeyUp(m_keyCode);
		else
			wnd->onKeyDown(m_keyCode);
	}
	else
	{
		// if focus child is set in key events, it will be ignored this time.
		Window* focusChild = wnd->getFocusChild();
		if (focusChild)
			_process(focusChild);
	}
}

void KeyEvent::dump(void)
{
	LOG_DEBUG("[KeyEvent]keyEvent:" << m_keyEvent << ",keyCode:" << m_keyCode);
}

//
// window event
//
void WindowEvent::process(void)
{
	m_dest->onCommand(m_src, m_event, m_param1, m_param2);
}

void WindowEvent::dump(void)
{
	LOG_DEBUG("[WindowEvent]dest:" << m_dest << ",src:" << m_src << ",evt:" << m_event 
		<< ",param1:" << m_param1 << ",param2:" << m_param2);
}

//
// timer event
//
void TimerEvent::process(void)
{
	m_dest->onTimer(m_timer);
}
