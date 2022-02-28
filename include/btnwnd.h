#ifndef _BTNWND_H_
#define _BTNWND_H_

using namespace std;

class ButtonWnd : public Window
{
private:
	bool m_bStyleEx;
	Rect m_toprc;
	Rect m_midrc;
	Rect m_bottomrc;

public:
	ButtonWnd(void) : m_click(false), m_font(NULL) {}
	ButtonWnd(std::string text) : m_click(false), m_font(NULL) { setText(text); }
	ButtonWnd(Window* parent, Rect rc) : m_click(false), m_bStyleEx(false), m_font(NULL), Window(parent, rc) {}

	virtual void InitButtonEx()
	{
		m_bStyleEx = true;
		unsigned int LeftMargine = 4;
	        unsigned int BottomMargine = 4;

		m_toprc.set(getClientRect().getLeft()+LeftMargine, getClientRect().getTop(), getClientRect().getRight()-(2*LeftMargine), getClientRect().getBottom()-BottomMargine);
		m_midrc.set(getClientRect().getLeft(), getClientRect().getTop()+(BottomMargine-1), getClientRect().getRight()-LeftMargine, getClientRect().getBottom()-BottomMargine);
		m_bottomrc.set(getClientRect().getLeft()+LeftMargine, getClientRect().getTop()+(2*BottomMargine), getClientRect().getRight(), getClientRect().getBottom());
	}

	virtual void onDraw(GraphicDevice* gd)
	{
		Color bgClr;
		Color bgClr_top;
		Color bgClr_mid;
		Color bgClr_bottom;
		Color txtClr;
		Font *Tempfont;
		unsigned int font_width;
		int nFontX, nFontY;

		InitButtonEx();

		if( m_font == NULL )
		{
			m_font = gd->createFont((getClientRect().getHeight()/3));
		}

		if (isFocused())
		{
			if (m_click)
			{
				bgClr = COLOR_GRAY;
				bgClr_top =  Color(96,94,129);
				bgClr_mid =  Color(21,20,67);
				bgClr_bottom =	Color(21,20,17);
			}
			else
			{
				bgClr = COLOR_BLUE;
				bgClr_top =  Color(96,94,129);
				bgClr_mid =  Color(21,20,67);
				bgClr_bottom =	Color(21,20,17);
			}

			txtClr = COLOR_WHITE;
		}
		else
		{
			bgClr = COLOR_GRAY;
			bgClr_top =  Color(96,94,89);
			bgClr_mid =  Color(55,53,46);
			bgClr_bottom =	Color(21,20,17);

			txtClr = COLOR_WHITE;
		}

		if( !m_bStyleEx )
		{
			gd->drawRect(getClientRect(), bgClr);

			if (m_str.size() > 0)
				gd->drawString(Point(0, 0), txtClr, m_str);

			gd->drawBorder(getClientRect(), COLOR_GRAY);
		}
		else
		{
	              	gd->drawRect(m_toprc, bgClr_top);
			gd->drawRect(m_bottomrc, bgClr_bottom);
			gd->drawRect(m_midrc, bgClr_mid);

			if (m_str.size() > 0)
			{
				Tempfont = gd->getFont();
				gd->setFont(m_font);

				font_width = m_font->getHeight()*0.4;

				cout << "font Height : " <<  m_font->getHeight() << endl;
				cout << "Rect Width : " << getClientRect().getWidth() << endl;
				cout << "Rect Height : " << getClientRect().getHeight() << endl;

				nFontX = ((int)(getClientRect().getWidth()- m_str.size()*font_width))*0.5;
				nFontY = (int)(getClientRect().getHeight()*1/3);

				if( nFontX < 0 )
					nFontX=0;

				if ( nFontY < 0 )
				{
					cout << "nFontY = " << nFontY << endl;
					//nFontY= getClientRect().getHeight();
					nFontY= (getClientRect().getHeight()-m_font->getHeight())/2;
				}

				cout << "nFontX, Y : " << nFontX << ", " << nFontY << " getFontHeight : " << m_font->getHeight() << std::endl;
				cout << "window W, H : " << getClientRect().getWidth() << ", " << getClientRect().getHeight()+50 << endl;
				gd->drawString(Point(nFontX, nFontY), txtClr, m_str);

				gd->setFont(Tempfont);
			}
		}
	}

	virtual void onEndDraw(GraphicDevice* gd)
	{
		if( m_font != NULL )
		{
			gd->disposeFont(&m_font);
		}
	}

	virtual bool onKeyUp(KeyCode key)
	{
		LOG_DEBUG("onKeyUp:" << key);
		if (key == KEY_OK)
		{
			m_click = true;
			invalidate();
			update();

			usleep(100000);
			m_click = false;
			invalidate();

			onClick();
		}
		return true;
	}

	virtual void onClick(void)
	{
		// send WM_CLICK to the parent.
		send(getParent(), WE_CLICK, 0, 0);
	}

	void setText(std::string str)
	{
		m_str = str;
		invalidate();
	}
	std::string getText(void) { return m_str; }

	bool getClicked(void) { return m_click; }

private:
	Font *m_font;
	std::string m_str;
	bool m_click;
};

#endif /* _BTNWND_H_ */

