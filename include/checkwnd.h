#ifndef _CHECK_WND_H_
#define _CHECK_WND_H_

class CheckWnd : public Window
{
public:
	CheckWnd(void) : m_check(false) {}
	CheckWnd(Window* parent, Rect rc) : m_check(false), Window(parent, rc) {}

	virtual void onDraw(GraphicDevice* gd)
	{
		Color fgClr, bgClr;
		if (isFocused())
		{
			fgClr = COLOR_WHITE; bgClr = COLOR_BLUE;
		}
		else
		{
			fgClr = COLOR_BLACK; bgClr = COLOR_WHITE;
		}
		gd->drawRect(getClientRect(), bgClr);

		// draw check box
		if (m_check)
			gd->drawString(Point(getClientRect().getLeft(), getClientRect().getTop()), fgClr, "[O]");
		else
			gd->drawString(Point(getClientRect().getLeft(), getClientRect().getTop()), fgClr, "[ ]");
	
		gd->drawString(Point(50, 0), fgClr, m_text);
	}

	virtual bool onKeyUp(KeyCode key)
	{
		if (key == KEY_OK)
		{
			m_check = !m_check;
			invalidate();
		}
		return true;
	}

	bool getCheck(void) { return m_check; }
	void setCheck(bool check) { m_check = check; invalidate(); }

	std::string getText(void) { return m_text; }
	void setText(std::string text) { m_text = text; invalidate(); }

private:
	bool m_check;
	std::string m_text;
};

#endif /* _CHECK_WND_H_ */

