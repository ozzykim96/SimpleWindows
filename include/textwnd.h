#ifndef _TEXTWND_H_
#define _TEXTWND_H_

class TextWnd : public Window
{
public:
	TextWnd(void) {}
	TextWnd(Window* parent, Rect rc) : Window(parent, rc) {}
	
	virtual void onDraw(GraphicDevice* gd) 
	{
		Window::onDraw(gd);
		
		if (m_str.size() > 0)
			gd->drawString(Point(0, 0), COLOR_BLACK, m_str); 
	}
	
	void setText(std::string str)
	{
		m_str = str;
		invalidate();
	}
	std::string getText(void) { return m_str; }
	
private:
	std::string m_str;
};

#endif /* _TEXTWND_H_ */

