#ifndef _PRGWND_H_
#define _PRGWND_H_

class ProgressWnd : public Window
{
public:
	ProgressWnd(void) : m_min(0), m_max(100), m_value(0) {}
	ProgressWnd(Window* parent, Rect rc) : Window(parent, rc), m_min(0), m_max(100), m_value(0) {}

	virtual void onDraw(GraphicDevice* gd)
	{
		ASSERT(m_max - m_min);
		
		gd->drawRect(getClientRect(), COLOR_GRAY);

		int barWidth = (m_value * getClientRect().getWidth()) / (m_max - m_min);
		gd->drawRect(Rect(0, 0, barWidth, getClientRect().getHeight()), COLOR_BLUE);
	}
	
	void setMinMax(int min, int max)
	{
		ASSERT(max - min);
		
		m_min = min;
		m_max = max;
		if (m_value > m_max)
			m_value = m_max;
		if (m_value < m_min)
			m_value = m_min;
		
		invalidate();
	}
	int getMax(void) { return m_max; }
	int getMin(void) { return m_min; }
	
	void setValue(int value) 
	{ 
		if (value > m_max)
			value = m_max;
		if (value < m_min)
			value = m_min;
		
		if (m_value == value)
			return;

		m_value = value; 
		
		invalidate();
	}
	int getValue(void) { return m_value; }
	
private:
	int m_min;
	int m_max;
	int m_value;
};

#endif /* _PRGWND_H_ */

