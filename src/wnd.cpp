#include "windows.h"

Window::Window(void)
{
	_init();
}

Window::Window(Window* parent, Rect rc, WindowStyle style)
{
	_init();
	create(parent, rc, style);
}

// destroy window.
// break off relationship of parent and siblings and destroy this.
// NOTE: this does not destroy other children.
Window::~Window(void)
{
	if (m_parent)
	{
		m_parent->removeChild(this);
	}
	// remove timers if any.
	WindowManager::getManager()->deleteAllTimer(this);
		
	LOG_DEBUG("[wnd:" << this 
		<< ",(" << m_scrnRect.getLeft() << "," << m_scrnRect.getTop() 
		<< "," << m_scrnRect.getRight() << "," << m_scrnRect.getBottom() << ")] destroyed.");
}

void Window::create(Window* parent, Rect rc, WindowStyle style)
{
	m_font = getDeviceFont(NORMAL_FONT);

	m_scrnRect = m_clientRect = rc;
	m_clientRect.offsetRect(-m_scrnRect.getLeft(), -m_scrnRect.getTop());
	
	if (parent == NULL)
		parent = WindowManager::getManager()->getScreen();
	
	parent->addChild(this);
	
	m_style = style;
	if (style & WS_VISIBLE)
		show();
	
	LOG_DEBUG("[wnd:" << this 
		<< ",(" << m_scrnRect.getLeft() << "," << m_scrnRect.getTop() 
		<< "," << m_scrnRect.getRight() << "," << m_scrnRect.getBottom() << ")] created.");	
}

void Window::createScreen(Rect rc)
{
	ASSERT(!WindowManager::getManager()->getScreen());

	m_font = getDeviceFont(NORMAL_FONT);

	m_scrnRect = m_clientRect = rc;
	m_clientRect.offsetRect(-m_scrnRect.getLeft(), -m_scrnRect.getTop());
	
	// no parent
	
	LOG_DEBUG("[wnd:" << this 
		<< ",(" << m_scrnRect.getLeft() << "," << m_scrnRect.getTop() 
		<< "," << m_scrnRect.getRight() << "," << m_scrnRect.getBottom() << ")] created.");	
}

void Window::move(int x, int y, int width, int height)
{
	ASSERT(m_parent);
	
	Rect oldRc, newRc, parentRc;
	
	oldRc = m_scrnRect;
	parentRc = m_parent->m_scrnRect;
	
	// set new offset
	_offsetWindow(parentRc.getLeft() - m_scrnRect.getLeft() + x,
		parentRc.getTop() - m_scrnRect.getTop() + y);
	
	// set new width and height
	newRc.set(m_scrnRect.getLeft(), m_scrnRect.getTop(), 
		m_scrnRect.getLeft() + width, m_scrnRect.getTop() + height);
	
	m_clientRect = m_scrnRect = newRc;
	m_clientRect.offsetRect(-m_scrnRect.getLeft(), -m_scrnRect.getTop());
	
	if (m_show)
	{
		_repaint(oldRc);
		_repaint(m_scrnRect);
	}
}

void Window::addChild(Window* child)
{
	ASSERT(child != NULL);
	
	child->m_parent = this;
	
	if (m_child == NULL)
	{
		// no child. this is the first child
		m_child = child;
	}
	else
	{
		// if parent has children, this is the last child
		Window* sibling = m_child;
		while (sibling->m_sibling)
			sibling = sibling->m_sibling;
		sibling->m_sibling = child;		
	}

	// adjust window rect to client rect of this window
	child->m_scrnRect.offsetRect(m_scrnRect.getLeft(), m_scrnRect.getTop());
	child->m_clientRect = child->m_scrnRect;
	child->m_clientRect.offsetRect(-child->m_scrnRect.getLeft(), -child->m_scrnRect.getTop());

	// by default, window event listener is parent
	child->m_eventListener = this;
	
	// repaint child
	_repaint(child->m_scrnRect);
}

void Window::removeChild(Window* child)
{
	ASSERT(child != NULL);
	ASSERT(child->m_parent != NULL);

	if (m_focus == child)
		m_focus = NULL;

	// break off relationship of parent and siblings
	child->m_parent = NULL;

	Window* sibling;	
	sibling = m_child;
	if (sibling == child)
	{
		m_child = child->m_sibling;
	}
	else
	{
		// find previous sibling (getPrevSibling())
		while (sibling->m_sibling != child)
			sibling = sibling->m_sibling;
		sibling->m_sibling = child->m_sibling;		
	}
	
	// repaint child
	_repaint(child->m_scrnRect);
}

Rect Window::clientToScreen(Rect rc)
{
	rc.offsetRect(getScreenRect().getLeft(), getScreenRect().getTop());
	return rc;
}

Rect Window::screenToClient(Rect rc)
{
	rc.offsetRect(-getScreenRect().getLeft(), -getScreenRect().getTop());
	return rc;
}

Font* Window::setFont(Font* font)
{
	Font* prev = m_font;
	m_font = font;
	return prev;
}

GraphicDevice* Window::getGraphicDevice(void)
{
	return WindowManager::getManager()->getGraphicDevice(this);
}

unsigned int Window::setTimer(TimerType type, unsigned int millisec)
{
	return WindowManager::getManager()->addTimer(this, type, millisec);
}

void Window::killTimer(unsigned int timer)
{
	WindowManager::getManager()->deleteTimer(this, timer);
}

void Window::invalidateRect(Rect rect)
{
	Rect rc = rect;
	
	// client to screen
	rc.offsetRect(m_scrnRect.getLeft(), m_scrnRect.getTop());	
	_repaint(rc);
}

void Window::update(void) 
{ 
	WindowManager::getManager()->update(); 
}

void Window::show(void)
{
	m_show = true;
	_repaint(m_scrnRect);
	onShow();
}

void Window::hide(void)
{
	m_show = false;
	_repaint(m_scrnRect);
	onHide();
}

bool Window::isFocused(void)
{
	return (this == m_parent->m_focus);
}

Window* Window::setFocusChild(Window* focus)
{
	ASSERT(focus);
	ASSERT(this == focus->m_parent);
	
	Window* prevFocus = m_focus;

	m_focus = focus;
	if (prevFocus)
		prevFocus->onKillFocus();
	m_focus->onSetFocus();
	return prevFocus;		
}

Window* Window::setEventListener(Window* listener)
{
	Window* prev = m_eventListener;
	m_eventListener = listener;
	return prev;
}


void Window::post(Window* dest, unsigned int event, unsigned int param1, unsigned int param2)
{
	ASSERT(dest);

	WindowEvent* wndEvt = new WindowEvent(dest, this, event, param1, param2);
	WindowManager::getManager()->postEvent(wndEvt);
}

unsigned int Window::send(Window* dest, unsigned int event, unsigned int param1, unsigned int param2)
{
	ASSERT(dest);
	
	return dest->onCommand(this, event, param1, param2);
}

void Window::dump(void)
{
	_dumpWithDepth(0);
}

void Window::dumpTree(void)
{
	_dumpRecursive(0);
}

void Window::_init(void)
{
	m_parent = m_sibling = m_child = m_focus = m_eventListener = NULL;
	m_show = m_invalidate = false;
	m_style = WS_NONE;
	m_font = NULL;
}

void Window::_repaint(Rect rc)
{
	// set invalidate rect
	WindowManager::getManager()->invalidateRect(rc);

	// set invalidate flags
	WindowManager::getManager()->getScreen()->_invalidateWindows(rc);
}

void Window::_invalidateWindows(Rect rc)
{	
	// if there is no intersection between window rect and invalidate rect, just return
	Rect rect = rc;
	rect.intersectRect(getScreenRect());	
	if (rect.isEmpty())
		return;
	
	m_invalidate = true;

	// invalidate it's children
	Window* child = m_child;
	while(child)
	{
		child->_invalidateWindows(rc);
		child = child->m_sibling;
	}
}

void Window::_offsetWindow(int x, int y)
{
	m_scrnRect.offsetRect(x, y);
	
	Window* child = m_child;
	while (child)
	{
		child->_offsetWindow(x, y);
		child = child->m_sibling;
	}
}

void Window::_dumpWithDepth(int depth)
{
	std::string tab;
	
	for(int i = 0; i < depth; i++)
		tab += "  ";
	
	LOG_DEBUG(tab.c_str() << "[wnd:" << this 
		<< ",(" << m_scrnRect.getLeft() << "," << m_scrnRect.getTop() 
		<< "," << m_scrnRect.getRight() << "," << m_scrnRect.getBottom() << "),("
		<< m_clientRect.getLeft() << "," << m_clientRect.getTop() 
		<< "," << m_clientRect.getRight() << "," << m_clientRect.getBottom()
		<< ")] child:" << m_child << ",sibling:" << m_sibling
		<< ",visible:" << m_show);	
}

void Window::_dumpRecursive(int depth)
{
	_dumpWithDepth(depth);
	
	depth++;
	Window* child = m_child;
	while(child)
	{
		child->_dumpRecursive(depth);
		child = child->m_sibling;
	}
}