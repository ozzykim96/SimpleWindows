#ifndef _FOCUSNAV_H_
#define _FOCUSNAV_H_

enum FocusWindowNavigatorStyle
{
	FWNS_UPDOWN 	= 0x0001,
	FWNS_LEFTRIGHT	= 0x0002
};

class FocusWindowNavigator
{
public:
	FocusWindowNavigator(Window* parent, int style);
	void addChild(Window* wnd);

	bool setFocusChild(Window* wnd);
	Window* getFocusChild(void);
	
	bool onKeyUp(KeyCode code);
	
private:
	Window* _prev(void);
	Window* _next(void);
	
	Window* m_parent;
	std::list<Window*>::iterator m_it;
	int m_style;
	std::list<Window*> m_list;
};

#endif /* _FOCUSNAV_H_ */
