#ifndef _WINDOW_MANAGER_H_
#define _WINDOW_MANAGER_H_

class GraphicDevice;
class InputDevice;
class InputEventListener;
class EventPtrQueue;
class Timer;
class TimerEventListener;

class WindowManager : public InputEventListener, public TimerEventListener
{
public:
	WindowManager(void);
	~WindowManager(void);

	/** set focus window. focus window gets onKeyXX messages when key events come. */
	Window* setFocus(Window* focus);
	Window* getFocus(void) { return m_focus; }

	Window* getScreen(void) { return m_screen; }
	Size getScreenSize(void) { return m_maxScrnSize; }

	GraphicDevice* getGraphicDevice(Window* wnd);

	/** run window system - handle window events. this call is blocked until quit() is called */
	int run(void);
	/** quit window system */
	void quit(void);

	/** force window update. it draws a window immediately (Window::onDraw is generated within this call). */
	void update(void);
	void invalidateRect(Rect rc);

	void processEvent(Event* evt);
	/** post event to window system. */
	void postEvent(Event* evt);

	/**
	 * add timer which generates timer event to window (wnd).
	 * @return timer id
 	 */
	unsigned int addTimer(Window* wnd, TimerType type, unsigned int millisec);
	void deleteTimer(Window* wnd, TimerId id);
	void deleteAllTimer(Window* wnd);

	virtual void onKeyEvent(InputKeyEvent event, KeyCode code);
	virtual void onTimerEvent(TimerId id);

	static WindowManager* getManager(void);
private:
	void _idle(void);
	void _setupGraphicDevice(Window* wnd, Rect rcInvalidate);
	void _drawWindows(Window* wnd);
	void _drawEndWindows(Window* wnd);

	GraphicDevice* m_grpDev;
	InputDevice* m_inpDev;
	EventPtrQueue* m_evtPtrQue;
	Timer* m_timer;

	Window* m_screen;
	Window* m_focus;

	Rect m_invalidateRect;
	std::vector<std::pair<Window*,TimerId> > m_timerList;

	bool m_quit;

	Size m_maxScrnSize;			// max screen size

	static WindowManager* m_windowManager;
};

#endif /* _WINDOW_MANAGER_H_ */
