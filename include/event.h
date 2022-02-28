#ifndef _EVENT_H_
#define _EVENT_H_

#include "wndevt.h"

class Window;

enum EventClass
{
	WindowEventClass 	= 0x1000,
	KeyEventClass 		= 0x1001,
	TimerEventClass		= 0x1002
};

class Event
{
public:
	Event(EventClass evtClass) : m_class(evtClass) {}

	EventClass getClass(void) { return m_class; }
	
	virtual void process(void) = 0;
	
	// DEBUG
	virtual void dump(void) {};
private:
	EventClass m_class;
};

class KeyEvent : public Event
{
public:
	KeyEvent(InputKeyEvent keyEvent, KeyCode keyCode) : m_keyEvent(keyEvent), m_keyCode(keyCode), Event(KeyEventClass) {}

	virtual void process(void);
	
	virtual void dump(void);
private:
	void _process(Window* wnd);

	InputKeyEvent m_keyEvent;
	KeyCode m_keyCode;
};

class WindowEvent : public Event
{
public:
	WindowEvent(Window* dest, Window* src, unsigned int event, unsigned int param1, unsigned int param2) :
		m_dest(dest), m_src(src), m_event(event), m_param1(param1), m_param2(param2), Event(WindowEventClass) {}

	virtual void process(void);
				
	virtual void dump(void);	
private:
	Window* m_src;
	Window* m_dest;
	unsigned int m_event;
	unsigned int m_param1;
	unsigned int m_param2;
};

class TimerEvent : public Event
{
public:
	TimerEvent(Window* dest, unsigned int timer) : m_dest(dest), m_timer(timer), Event(TimerEventClass) {}
	
	virtual void process(void);

private:
	Window* m_dest;
	unsigned int m_timer;
};

/*
class BindEvent : public Event
{
};
*/
#endif /* _EVENT_H_ */
