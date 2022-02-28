#ifndef _WNDNAV_H_
#define _WNDNAV_H_

class WindowNavigator
{
public:
	/** add window to navigator list. */
	void add(Window* wnd);

	/** set current window.
		if window is not in the navigator list, return false.
	*/
	bool set(Window* wnd);
	/** get current window. */
	Window* get(void);
	
	/** get previous window and set it as current window. */
	Window* movePrev(void);
	/** get next window and set it as current window. */
	Window* moveNext(void);
	
private:
	std::list<Window*>::iterator m_it;
	std::list<Window*> m_list;
};

#endif