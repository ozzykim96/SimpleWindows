#ifndef _LISTWND_H_
#define _LISTWND_H_

class ListWnd : public Window
{
public:
	virtual void onDraw(GraphicDevice* gd)
	{
		Window::onDraw(gd);
		
		Rect rc;
		for (int i = m_disp; i < m_disp + m_dispItems; i++)
		{
			if (i >= m_list.size())
				break;
			
			rc.set(0, m_itemHeight * (i - m_disp), 
						getClientRect().getRight(), m_itemHeight * (i - m_disp + 1));
			
			onDrawListItem(gd, rc, i);
		}
	}

	virtual void onDrawListItem(GraphicDevice* gd, Rect rc, int index)
	{
		if (index == m_cur)
			gd->drawRect(rc, COLOR_BLUE);
		else if (index == m_sel)
			gd->drawRect(rc, COLOR_GRAY);
		
		if (index == m_sel)
			gd->drawString(Point(rc.getLeft(), rc.getTop()), COLOR_WHITE, m_list[index]);
		else
			gd->drawString(Point(rc.getLeft(), rc.getTop()), COLOR_BLACK, m_list[index]);		
	}
	
	virtual bool onKeyUp(KeyCode code)
	{
		switch (code)
		{
		case KEY_OK:
			m_sel = m_cur;
			onSelect(m_sel);
			invalidate();
			break;
		case KEY_UP:
			m_cur--;
			if (m_cur < 0)
				m_cur = m_list.size() - 1;
			_updatePos();
			break;
		case KEY_DOWN:
			m_cur++;
			if (m_cur >= m_list.size())
				m_cur = 0;
			_updatePos();
			break;
		case KEY_CHUP:
			if ((m_cur - m_dispItems) < 0)
				m_cur = m_list.size() - 1;
			else
				m_cur -= m_dispItems;
			_updatePos();
			break;
		case KEY_CHDOWN:
			if ((m_cur + m_dispItems) > m_list.size())
				m_cur = 0;
			else
				m_cur += m_dispItems;
			_updatePos();
			break;
		}
		return true;
	}
	
	virtual void onSelect(int pos)
	{
		// send WE_SELECT to the parent.
		send(getParent(), WE_SELECT, pos, 0);		
	}

	void setItems(std::vector <std::string> list)
	{
		m_list = list;
		
		m_itemHeight = getFont()->getHeight();
		ASSERT(m_itemHeight > 0);
		
		m_disp = m_cur = 0;
		m_sel = -1;
		
		// re-size listbox
		Rect rc = getClientRect();
		m_dispItems = rc.getHeight() / m_itemHeight;
		
		invalidate();
	}
	
	int getCurPos(void) { return m_cur; }
	int getSelect(void) { return m_sel; }

	int setCurPos(int pos)
	{
		if (pos < 0 || pos >= m_list.size())
			return m_cur;
		int prev = m_cur;
		
		m_cur = pos;
		_updatePos();
		
		return prev;		
	}
	
	int setSelect(int pos)
	{
		if (pos < 0 || pos >= m_list.size())
			return m_sel;
		int prev = m_sel;
		
		m_sel = pos;
		
		return prev;
	}
	
	void setItem(int pos, std::string str)
	{
		if (pos < 0 || pos >= m_list.size())
			return;
		
		m_list[pos] = str;
		
		invalidate();
	}
	
	std::string getItem(int pos) 
	{
		if (pos < 0 || pos >= m_list.size())
			return std::string();

		return m_list[pos];
	}
	
private:
	void _updatePos(void)
	{
		if (m_cur < m_disp ||
			m_cur >= m_disp + m_dispItems)
		{
			if (m_cur < m_disp)
				m_disp = m_cur;
			else
				m_disp = m_cur - m_dispItems + 1;
		}
		
		if (m_disp + m_dispItems >= m_list.size())
		{
			m_disp = m_list.size() - m_dispItems;
			if (m_disp < 0)
				m_disp = 0;
		}
		invalidate();
	}

	int m_cur;					/**< current index of list */
	int m_sel;					/**< select index of list. if not selected, -1 */
	int m_disp;					/**< first index of displayed list */
	int m_dispItems;			/**< the number of displayed items */
	int m_itemHeight;			/**< item height */
	std::vector <std::string> m_list;		/**< item list */
};

#endif /* _LISTWND_H_ */
