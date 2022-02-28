#ifndef _COMBO_WND_H_
#define _COMBO_WND_H_

class ListWnd;

class ComboWnd : public Window
{
public:
	ComboWnd(void) : m_cur(0) {}
	ComboWnd(Window* parent, Rect rc) : m_cur(0), Window(parent, rc) {}

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

		gd->drawString(Point(getClientRect().getLeft(), getClientRect().getTop()), fgClr, "<");
		gd->drawString(Point(30, 0), fgClr, m_strList[m_cur]);
		gd->drawString(Point(getClientRect().getRight() - 30, getClientRect().getTop()), fgClr, ">");
	}
	
	virtual bool onKeyUp(KeyCode key)
	{
		switch (key)
		{
		case KEY_OK:
			break;
		case KEY_LEFT:
		case KEY_UP:
			m_cur--;
			if (m_cur < 0)
				m_cur = m_strList.size() - 1;
			invalidate();
			onSelect(m_cur);
			break;
		case KEY_RIGHT:
		case KEY_DOWN:
			m_cur++;
			if (m_cur >= m_strList.size())
				m_cur = 0;
			invalidate();
			onSelect(m_cur);
			break;
		}
		return true;
	}
	
	virtual void onSelect(int pos)
	{
		// send WE_SELECT to the parent.
		send(getParent(), WE_SELECT, pos, 0);		
	}	
	
	void setItems(std::vector<std::string> list)
	{
		m_strList = list;
		invalidate();
	}
	
private:
	int m_cur;
	std::vector<std::string> m_strList;
};

#endif /* _COMBO_WND_H_ */

