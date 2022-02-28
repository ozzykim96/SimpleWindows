#include "windows.h"

#define MAX_SCREEN_WINDOW_WIDTH		1280
#define MAX_SCREEN_WINDOW_HEIGHT	720

WindowManager* WindowManager::m_windowManager = NULL;

// window and id pair comparison
class WndIdPairCmp
{
public:
	WndIdPairCmp(Window* wnd, TimerId id) : m_wnd(wnd), m_id(id) {}
	bool operator() (std::pair<Window*,TimerId> p) const
	{
		if (m_id != 0)
		{
			if (p.first == m_wnd && p.second == m_id)
				return true;
			return false;
		}
		else
		{
			if (p.first == m_wnd)
				return true;
			return false;
		}
	}
private:
	Window* m_wnd;
	TimerId m_id;
};

class ScreenWindow : public Window
{
public:
	ScreenWindow(int width, int height)
	{
		createScreen(Rect(0, 0, width, height));
	}

	virtual void onDraw(GraphicDevice* gd)
	{
		// erase screen
		gd->drawRect(Rect(0, 0, getClientRect().getWidth(), getClientRect().getHeight()), Color(0, 0, 0, 0));
	}
};

WindowManager::WindowManager(void) : m_maxScrnSize(MAX_SCREEN_WINDOW_WIDTH, MAX_SCREEN_WINDOW_HEIGHT)
{
	ASSERT(!m_windowManager);
	m_screen = NULL;
	m_windowManager = this;

	m_focus = NULL;
	m_quit = false;

	// create devices
	m_grpDev = createGraphicDevice();
	m_inpDev = createInputDevice();
	m_timer = createTimer();
	m_evtPtrQue = createEventPtrQueue();

	// initialize graphic device
	if (m_grpDev)
	{
		if (m_grpDev->init() == false)
		{
			LOG_ERROR("graphic device initialization failed.");
		}
	}
	
	// initialize input device
	if (m_inpDev)
	{
		if (m_inpDev->init() == false)
		{
			LOG_ERROR("input device initialization failed.");
		}
		// listen input
		m_inpDev->setInputEventListener(this);
		m_inpDev->listen();
	}

	// start timer
	if (m_timer)
	{
		m_timer->setTimerEventListener(this);
		m_timer->start();
	}
	
	// create screen window
	m_screen = new ScreenWindow(m_maxScrnSize.getWidth(), m_maxScrnSize.getHeight());
	m_screen->show();
}

WindowManager::~WindowManager(void)
{
	delete m_screen;

	if (m_timer)
	{
		// delete all timers
		for (int i = 0; i < m_timerList.size(); i++)
		{
			m_timer->remove(m_timerList[i].second);
		}
	}
	m_timerList.clear();

	// shutdown all devices
	if (m_grpDev) m_grpDev->shutdown();
	if (m_inpDev) m_inpDev->shutdown();
	if (m_timer) m_timer->stop();

	// destroy all devices
	if (m_grpDev) destroyGraphicDevice(m_grpDev);
	if (m_inpDev) destroyInputDevice(m_inpDev);
	if (m_evtPtrQue) destroyEventPtrQueue(m_evtPtrQue);
	if (m_timer) destroyTimer(m_timer);
}

// window message loop
int WindowManager::run(void)
{
	Event* evt;

	while(!m_quit)
	{
		evt = m_evtPtrQue->peek();
		if (evt)
		{
			processEvent(evt);
			continue;
		}

		_idle();
		evt = m_evtPtrQue->get();
		if (evt)
		{
			processEvent(evt);
		}
	}
	return 0;
}

void WindowManager::quit(void)
{
	m_quit = true;
}

void WindowManager::processEvent(Event* evt)
{
	LOG_DEBUG("process event:");
	evt->dump();
	evt->process();
	delete evt;
}

void WindowManager::postEvent(Event* evt)
{
	m_evtPtrQue->post(evt);
}

TimerId WindowManager::addTimer(Window* wnd, TimerType type, unsigned int millisec)
{
	TimerId id;
	
	if (!m_timer)
	{
		LOG_ERROR("timer is null");
		return 0;
	}
	
	if (type == TT_ONCE)
		id = m_timer->setOnce(millisec);
	else // TT_REPEAT
		id = m_timer->setRepeat(millisec);

	m_timerList.push_back(std::make_pair(wnd, id));
	return id;
}

void WindowManager::deleteTimer(Window* wnd, TimerId id)
{
	if (!m_timer)
	{
		LOG_ERROR("timer is null");
		return;
	}

	for (int i = 0; i < m_timerList.size(); i++)
	{
		if (m_timerList[i].first == wnd && m_timerList[i].second == id)
		{
			m_timer->remove(m_timerList[i].second);
			break;
		}
	}

	m_timerList.erase(std::remove_if(m_timerList.begin(), m_timerList.end(),
					WndIdPairCmp(wnd, id)), m_timerList.end());
}

void WindowManager::deleteAllTimer(Window* wnd)
{
	if (!m_timer)
	{
		LOG_ERROR("timer is null");
		return;
	}

	for (int i = 0; i < m_timerList.size(); i++)
	{
		if (m_timerList[i].first == wnd)
			m_timer->remove(m_timerList[i].second);
	}

	m_timerList.erase(std::remove_if(m_timerList.begin(), m_timerList.end(),
					WndIdPairCmp(wnd, 0)), m_timerList.end());
}

void WindowManager::_idle(void)
{
	LOG_DEBUG("idle.");

	update();
}

Window* WindowManager::setFocus(Window* focus)
{
	Window* prev = m_focus;

	m_focus = focus;	
	if (prev)
		prev->onKillFocus();
	m_focus->onSetFocus();
	return prev;
}

GraphicDevice* WindowManager::getGraphicDevice(Window* wnd)
{
	_setupGraphicDevice(wnd, getScreen()->getScreenRect());
	return m_grpDev;
}

WindowManager* WindowManager::getManager(void)
{
	ASSERT(m_windowManager);
	return m_windowManager;
}

void WindowManager::update(void)
{
	if (m_invalidateRect.isEmpty())
		return;

	// draw all invalidated windows
	_drawWindows(getScreen());
	// update drawings
	m_grpDev->update(m_invalidateRect);
	// release resource
	_drawEndWindows(getScreen());

	// clear invalidated region
	m_invalidateRect.makeEmpty();
}

void WindowManager::invalidateRect(Rect rc)
{
	m_invalidateRect.unionRect(rc);
}

void WindowManager::onKeyEvent(InputKeyEvent event, KeyCode code)
{
	KeyEvent* keyEvt = new KeyEvent(event, code);
	m_evtPtrQue->post(keyEvt);
}

void WindowManager::onTimerEvent(unsigned int id)
{
	for (int i = 0; i < m_timerList.size(); i++)
	{
		if (m_timerList[i].second == id)
		{
			TimerEvent* timerEvt = new TimerEvent(m_timerList[i].first, m_timerList[i].second);
			m_evtPtrQue->post(timerEvt);
			break;
		}
	}
}

// set up graphic device with current window and invalid rect
void WindowManager::_setupGraphicDevice(Window* wnd, Rect rcInvalidate)
{
	// set up graphic device
	Rect rc = rcInvalidate;
	rc.intersectRect(wnd->getScreenRect());

	// set window font
	if (wnd->getFont() != NULL)
		m_grpDev->setFont(wnd->getFont());

	// clip draw region of window
	m_grpDev->clip(rc);
	// set viewport of window
	m_grpDev->setViewport(Point(wnd->getScreenRect().getLeft(), wnd->getScreenRect().getTop()));
}

// draw windows if they have invalidated regions
void WindowManager::_drawWindows(Window* wnd)
{
	LOG_DEBUG("[WndMgr] wnd:" << wnd << "] draw.");
	if (!wnd->isVisible())
		return;
	if (!wnd->isInvalidated())
		return;

	// if there is no intersection between window rect and invalidate rect, just return
	Rect rc = m_invalidateRect;
	rc.intersectRect(wnd->getScreenRect());
	if (rc.isEmpty())
		return;

	// set up graphic device
	_setupGraphicDevice(wnd, m_invalidateRect);

	// draw window
	wnd->onDraw(m_grpDev);

	// child first, next sibling
	Window* child = wnd->getFirstChild();
	while (child)
	{
		_drawWindows(child);
		child = child->getNextSibling();
	}
}

void WindowManager::_drawEndWindows(Window* wnd)
{
	LOG_DEBUG("[WndMgr] wnd:" << wnd << "] draw.");
	if (!wnd->isVisible())
		return;
	if (!wnd->isInvalidated())
		return;

	// if there is no intersection between window rect and invalidate rect, just return
	Rect rc = m_invalidateRect;
	rc.intersectRect(wnd->getScreenRect());
	if (rc.isEmpty())
		return;

	// draw window
	wnd->onEndDraw(m_grpDev);

	// child first, next sibling
	Window* child = wnd->getFirstChild();
	while (child)
	{
		_drawEndWindows(child);
		child = child->getNextSibling();
	}
}

