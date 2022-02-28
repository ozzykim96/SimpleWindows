#ifndef _WINDOW_H_
#define _WINDOW_H_

enum WindowStyle
{
	WS_NONE		= 0x0000,
	WS_VISIBLE 	= 0x0001,
};

enum TimerType
{
	TT_ONCE		= 0,
	TT_REPEAT
};

class Window
{
public:
	Window(void);
	Window(Window* parent, Rect rc, WindowStyle style = WS_NONE);
	~Window(void);

	/** 
	 * draw window. called when updating occurs and window has invalidated region(invalidated rect). 
	 */
	virtual void onDraw(GraphicDevice* gd) { gd->drawRect(getClientRect(), COLOR_WHITE); }
	virtual void onEndDraw(GraphicDevice* gd) { }
	
	/**
	 * decide whether it handles (focus) child keys. if it returns true, onKeyUp/Down events will be generated and its children won't get this messages.
	 */
	virtual bool onOverrideChildKey(InputKeyEvent event, KeyCode key) { return false; }
	/**
	 * called when this is a focus child by setFocusChild() and there are key events. it receives key events. 
	 * also by returning false, key events can be propagated through focus children.
	 * @return true - if don't want focus child to get onKeyXX events, false - pass key event to focus child
	 */	
	virtual void onKeyUp(KeyCode key) { }
	virtual void onKeyDown(KeyCode key) { }
	/** called when this acquires a focus by setFocusChild(). */
	virtual void onSetFocus(void) { invalidate(); }
	/** called when losing a focus by setFocusChild(). */
	virtual void onKillFocus(void) { invalidate(); }
	/** called when showing (show() is called) */
	virtual void onShow(void) {}
	/** called when hiding (hide() is called) */
	virtual void onHide(void) {}

	/** called when getting window command event from other window */
	virtual unsigned int onCommand(Window* src, unsigned int event, unsigned int param1, unsigned int param2) { return 0; }
	/** called when timer is triggered by setTimer() */
	virtual void onTimer(TimerId timer) { }

	/**
	 * create window.
	 * @param parent window. if NULL, created as a child of screen window.
	 * @param window rect. created with relative offsets of parent.
	 * @param window style. if WS_VISIBLE, created with visible state(show()).
	 * @note if window is constructed with parameters, it doesn't need calling.
	 */
	void create(Window* parent, Rect rc, WindowStyle style = WS_NONE);

	/**
	 * create screen window.
	 * @note only called once in window manager.
	 */
	void createScreen(Rect rc);
	
	void move(int x, int y, int width, int height);
	
	void addChild(Window* child);
	void removeChild(Window* child);

	Window* getParent(void) { return m_parent; }
	Window* getNextSibling(void) { return m_sibling; }
	Window* getFirstChild(void) { return m_child; }

	Rect getClientRect(void) { return m_clientRect; }
	Rect getScreenRect(void) { return m_scrnRect; }
	Rect clientToScreen(Rect rc);
	Rect screenToClient(Rect rc);
	
	WindowStyle getStyle(void) { return m_style; }

	Font* setFont(Font* font);
	Font* getFont(void) { return m_font; }

	GraphicDevice* getGraphicDevice(void);

	unsigned int setTimer(TimerType type, unsigned int millisec);
	void killTimer(unsigned int timer);

	/** invalidate region(rect). this region will be drawn in onDraw() */
	void invalidate(void) { invalidateRect(m_clientRect); }
	void invalidateRect(Rect rect);

	/** force update. it draws a window immediately (onDraw is generated within this call). */
	void update(void);

	void show(void);
	void hide(void);

	/** set a focus child. focus window gets onKeyXX messages when key events come. */
	Window* setFocusChild(Window* focus);
	Window* getFocusChild(void) { return m_focus; }
	/** set event listener which gets window event message (WE_XXX) from onCommand() */
	Window* setEventListener(Window* listener);
	Window* getEventListener(void) { return m_eventListener; }

	bool isVisible(void) { return m_show; }
	bool isInvalidated(void) { return m_invalidate; }
	bool isFocused(void);

	/** post window event to destination window through message queue. */
	void post(Window* dest, unsigned int event, unsigned int param1, unsigned int param2);
	/** send window event to destination window. this call is made immediately. */
	unsigned int send(Window* dest, unsigned int event, unsigned int param1, unsigned int param2);

	virtual void dump(void);
	virtual void dumpTree(void);
private:
	void _init(void);
	void _repaint(Rect rc);
	void _invalidateWindows(Rect rc);
	void _offsetWindow(int x, int y);

	void _dumpRecursive(int depth);
	void _dumpWithDepth(int depth);

	WindowStyle m_style;
	Font* m_font;

	Window* m_parent;
	Window* m_sibling;
	Window* m_child;

	Window* m_focus;
	Window* m_eventListener;
	
	Rect m_scrnRect;
	Rect m_clientRect;

	bool m_show;
	bool m_invalidate;
};

#endif /* _WINDOW_H_ */
