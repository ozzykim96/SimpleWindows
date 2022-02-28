#include "windows.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <queue>

#include <directfb.h>
#include <directfb_keynames.h>

#include <direct/util.h>

/* macro for a safe call to DirectFB functions */
#define DFBCHECK(x...) \
     {                                                               \
           err = x;                                                  \
           if (err != DFB_OK) {                                      \
              LOG_ERROR( __FILE__ << ":" << __LINE__ ); \
              DirectFBErrorFatal( #x, err );                         \
           }                                                         \
     }

#define SMALL_FONT_HEIGHT		24
#define NORMAL_FONT_HEIGHT		32
#define LARGE_FONT_HEIGHT		48

class DfbFont : public Font
{
public:
	DfbFont(IDirectFBFont* dfbFont, FontType type, int height) : m_dfbFont(dfbFont), m_type(type), m_height(height) {}

	virtual FontType getType(void) { return m_type; }
	virtual int getHeight(void)
	{
		m_dfbFont->GetHeight(m_dfbFont, &m_height);
		return m_height;
	}

	virtual int getStringWidth(std::string str)
	{
		int width;
		m_dfbFont->GetStringWidth(m_dfbFont, str.c_str(), -1, &width);
		return width;
	}

	IDirectFBFont* getDFBFont(void) { return m_dfbFont; }

private:
	FontType m_type;
	int m_height;

	IDirectFBFont* m_dfbFont;
};

static std::vector<DfbFont*> s_deviceFonts;

Font* getDeviceFont(FontType type)
{
	for (std::vector<DfbFont*>::iterator it = s_deviceFonts.begin(); it != s_deviceFonts.end(); ++it)
	{
		if ((*it)->getType() == type)
			return (*it);
	}
	return NULL;
}

struct KeyMap
{
	KeyCode code;
	unsigned int dfbkey;
};

static const KeyMap s_keyMap[] =
{
    { KEY_POWER, 0xFF001000 }, { KEY_UP, 0xEE111000}, { KEY_DOWN, 0xEA151000 }, { KEY_LEFT, 0xED121000 }, { KEY_RIGHT, 0xEB141000 },
    { KEY_OK, 0xEC131000 }, { KEY_VOLUP, 0xE01F1000}, { KEY_VOLDOWN, 0xBF401000 }, { KEY_CHUP, 0xEF101000 }, { KEY_CHDOWN, 0xF00F1000 },
    { KEY_0, 0xF30C1000 }, { KEY_1, 0xFC031000}, { KEY_2, 0xFB041000 }, { KEY_3, 0xFA051000 }, { KEY_4, 0xF9061000 },
    { KEY_5, 0xF8071000 }, { KEY_6, 0xF7081000}, { KEY_7, 0xF6091000 }, { KEY_8, 0xF50A1000 }, { KEY_9, 0xF40B1000 },
    { KEY_RED, 0xE31C1000 }, { KEY_GREEN, 0xE21D1000}, { KEY_YELLOW, 0xE51A1000 }, { KEY_BLUE, 0xE11E1000 }, { KEY_SUBTITLE, 0xB9461000 },
    { KEY_TELETEXT, 0x916E1000 }, { KEY_ZOOM, 0xFE011000}, { KEY_CLOSE, 0xE9161000 }, { KEY_HELP, 0xE8171000 }, { KEY_MUTE, 0xE7181000 },
    { KEY_MYVIEW, 0xE6191000 }, { KEY_GUIDE, 0xE41B1000}, { KEY_BACK, 0xBE411000 }, { KEY_INFO, 0xBC431000 }, { KEY_AD, 0xBB441000 },
    { KEY_SEARCH, 0xB8471000 }, { KEY_PLAY, 0x9F601000}, { KEY_RECORD, 0x9E611000 }, { KEY_PAUSE, 0x9D621000 }, { KEY_STOP, 0x9C631000 },
    { KEY_FF, 0x9B641000 }, { KEY_REW, 0x9A651000}, { KEY_PREV, 0x99661000 }, { KEY_NEXT, 0x98671000 }, { KEY_MENU, 0xF10E1000 }
};

#define KEYEVTTHRD_WAITEVT_TIMEOUT		500			// 500 ms

class KeyEventProcessor
{
public:
	KeyEventProcessor(IDirectFBEventBuffer *events, InputEventListener* evtListener) : 
		m_events(events), m_evtListener(evtListener), m_quit(false) { }
	
	void quit(void)
	{
		m_quit = true;
	}

	void operator()()
	{
		DFBInputEvent evt;
		KeyCode code;
	
		while (!m_quit)
		{			
			while (m_events->GetEvent(m_events, DFB_EVENT(&evt)) == DFB_OK)
			{
				switch (evt.type)
				{
				case DIET_KEYPRESS:
					LOG_DEBUG("key symbol:" << evt.key_symbol << ",code:" << evt.key_code);

					code = _translateDFBKeyToKeyCode(evt.key_code);
					m_evtListener->onKeyEvent(KEY_PRESSED, code);
					break;
				case DIET_KEYRELEASE:
					LOG_DEBUG("key symbol:" << evt.key_symbol << ",code:" << evt.key_code);

					code = _translateDFBKeyToKeyCode(evt.key_code);
					m_evtListener->onKeyEvent(KEY_RELEASE, code);
					break;
				default:
					LOG_DEBUG("unknown key type:" << evt.type);
					break;
				}
			}
			m_events->WaitForEventWithTimeout(m_events, 0, KEYEVTTHRD_WAITEVT_TIMEOUT);
		}
	}
private:
	KeyCode _translateDFBKeyToKeyCode(unsigned int dfbKey)
	{
		for (int i = 0; i < sizeof(s_keyMap) / sizeof(KeyMap); i++)
		{
			if (s_keyMap[i].dfbkey == dfbKey)
				return s_keyMap[i].code;
		}
		return KEY_UNKNOWN;
	}
	
	IDirectFBEventBuffer *m_events;
	InputEventListener* m_evtListener;
	bool m_quit;
};

class DfbDevice : public GraphicDevice, public InputDevice
{
public:
	DfbDevice(void);

	// Graphic & Input Device
	virtual bool init(void);
	virtual void shutdown(void);

	// for Graphic Device
	virtual void setViewport(Point viewport);
	virtual void clip(Rect rc);
	
	// draw
	virtual void drawRect(Rect rc, Color clr);
	virtual void drawBorder(Rect rc, Color clr);
	virtual void drawLine(Point start, Point end, Color clr);
	virtual void drawString(Point pt, Color clr, std::string str);
	virtual void drawAlignedText(Rect rc, Color clr, TextAlignType alignType, std::string str);

	// font
	virtual Font* getFont(void) { return (Font*)m_font; }
	virtual void setFont(Font* font);
	virtual Font* createFont(unsigned int height, std::string fontfile="");
	virtual void releaseFont(Font**  pfont);
	virtual void disposeFont(Font**  pfont);

	// update
	virtual void update(Rect rc);

	// for Input Device
	virtual bool listen(void);
	virtual bool stop(void);

private:
	/* the super interface */
	IDirectFB            *m_dfb;

	/* the primary surface (surface of primary layer) */
	IDirectFBSurface     *m_primary;
	IDirectFBSurface		*m_screen;
	int m_screenWidth;
	int m_screenHeight;
	
	IDirectFBEventBuffer *m_events;	
	
	bool m_init;
	Point m_viewport;
	DfbFont* m_font;
	
	KeyEventProcessor* m_keyEvtProc;
	boost::thread m_inpThrd;
};

static DfbDevice* s_dfbDevice = NULL;


DfbDevice::DfbDevice(void)
{
	m_init = false;
}

bool DfbDevice::init(void)
{
	if (m_init)
		return true;

	DFBResult              err;
	DFBSurfaceDescription  sdsc;
	DFBFontDescription     fdsc;
	const char            *fontfile = "/usr/local/share/fonts/Default.ttf";
	int                    n;
	IDirectFBFont        *dfbfont;

	DFBCHECK(DirectFBInit( NULL, NULL ));

	DirectFBSetOption ("bg-none", NULL);

	/* create the super interface */
	DFBCHECK(DirectFBCreate( &m_dfb ));

	/* create an event buffer for all devices */
	DFBCHECK(m_dfb->CreateInputEventBuffer( m_dfb, DICAPS_ALL,
									   DFB_FALSE, &m_events ));

	/* set our cooperative level to DFSCL_FULLSCREEN
	for exclusive access to the primary layer */
	m_dfb->SetCooperativeLevel( m_dfb, DFSCL_FULLSCREEN );

	/* get the primary surface, i.e. the surface of the
	primary layer we have exclusive access to */
	sdsc.flags = DSDESC_CAPS;
	sdsc.caps  = (DFBSurfaceCapabilities)(DSCAPS_PRIMARY);
	
	DFBCHECK(m_dfb->CreateSurface( m_dfb, &sdsc, &m_primary ));

	m_primary->GetSize( m_primary, &m_screenWidth, &m_screenHeight );

	/* create memory screen surface */
	sdsc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT);
	sdsc.width = m_screenWidth;
	sdsc.height = m_screenHeight;
	sdsc.caps = (DFBSurfaceCapabilities)DSCAPS_NONE;

	DFBCHECK(m_dfb->CreateSurface( m_dfb, &sdsc, &m_screen ));

	// create font objects
	fdsc.flags = DFDESC_HEIGHT;

	fdsc.height = SMALL_FONT_HEIGHT;
	DFBCHECK(m_dfb->CreateFont( m_dfb, fontfile, &fdsc, &dfbfont ));
	s_deviceFonts.push_back(new DfbFont(dfbfont, SMALL_FONT, fdsc.height));

	fdsc.height = NORMAL_FONT_HEIGHT;
	DFBCHECK(m_dfb->CreateFont( m_dfb, fontfile, &fdsc, &dfbfont ));
	m_font = new DfbFont(dfbfont, NORMAL_FONT, fdsc.height);
	s_deviceFonts.push_back(m_font);

	fdsc.height = LARGE_FONT_HEIGHT;
	DFBCHECK(m_dfb->CreateFont( m_dfb, fontfile, &fdsc, &dfbfont ));
	s_deviceFonts.push_back(new DfbFont(dfbfont, LARGE_FONT, fdsc.height));

   	fdsc.height = 15;
	DFBCHECK(m_dfb->CreateFont( m_dfb, fontfile, &fdsc, &dfbfont ));
	s_deviceFonts.push_back(new DfbFont(dfbfont, USER_SIZE_FONT, fdsc.height));

	// set default font
	m_screen->SetFont(m_screen, m_font->getDFBFont());

	// clear screen
	m_screen->Clear(m_screen, 0, 0, 0, 0);

	DFBRectangle rect;
	rect.x = rect.y = 0;
	rect.w = m_screenWidth;
	rect.h = m_screenHeight;

	m_primary->Blit( m_primary, m_screen, &rect, 0, 0);
	m_primary->Flip( m_primary, NULL, (DFBSurfaceFlipFlags)0 );

	m_init = true;

	return true;
}

void DfbDevice::setViewport(Point viewport)
{
	m_viewport = viewport;
}

void DfbDevice::clip(Rect rc)
{
	DFBRegion reg;

	reg.x1 = rc.getLeft();
	reg.y1 = rc.getTop();
	reg.x2 = rc.getRight() - 1;
	reg.y2 = rc.getBottom() - 1;

	m_screen->SetClip(m_screen, &reg);
}

void DfbDevice::drawRect(Rect rc, Color clr)
{
	rc.offsetRect(m_viewport.getX(), m_viewport.getY());

	m_screen->SetColor(m_screen, clr.getRed(), clr.getGreen(), clr.getBlue(), clr.getAlpha());
	m_screen->FillRectangle(m_screen, rc.getLeft(), rc.getTop(), rc.getWidth(), rc.getHeight());
}

void DfbDevice::drawBorder(Rect rc, Color clr, int size)
{
	rc.offsetRect(m_viewport.getX(), m_viewport.getY());

	m_screen->SetColor(m_screen, clr.getRed(), clr.getGreen(), clr.getBlue(), clr.getAlpha());
	m_screen->FillRectangle(m_screen, rc.getLeft(), rc.getTop(), rc.getWidth(), size);
	m_screen->FillRectangle(m_screen, rc.getLeft(), rc.getTop(), size, rc.getHeight());
	m_screen->FillRectangle(m_screen, rc.getLeft(), rc.getBottom() - size, rc.getWidth(), size);
	m_screen->FillRectangle(m_screen, rc.getRight() - size, rc.getTop(), size, rc.getHeight());
}

void DfbDevice::drawString(Point pt, Color clr, std::string str)
{
	pt.offsetPoint(m_viewport.getX(), m_viewport.getY());

	m_screen->SetColor(m_screen, clr.getRed(), clr.getGreen(), clr.getBlue(), clr.getAlpha());
	m_screen->DrawString(m_screen, str.c_str(), -1, pt.getX(), pt.getY(), DSTF_TOPLEFT);
}

void DfbDevice::drawAlignedText(Rect rc, Color clr, TextHorzAlign horzAlign, TextVertAlign vertAlign, std::string str)
{
	ASSERT(m_font);
	
	// get width and height
	int width = m_font->getStringWidth(str);
	int height = m_font->getHeight();
	
	// get starting point according to align type
	int x = 0;
	switch (horzAlign)
	{
	case TEXT_HALIGN_LEFT: x = 0; break;
	case TEXT_HALIGN_CENTER: x = (rc.getWidth() - width) / 2; break;
	case TEXT_HALIGN_RIGHT: x = rc.getWidth() - width; break;
	}
	int y = 0;
	switch (vertAlign)
	{
	case TEXT_VALIGN_TOP: y = 0; break;
	case TEXT_VALIGN_MIDDLE: y = (rc.getHeight() - height) / 2; break;
	case TEXT_VALIGN_BOTTOM: y = rc.getHeight() - height; break;
	}
	
	// draw string
	drawString(Point(x, y), clr, str);
}

void DfbDevice::drawMultiAlignedText(Rect rc, TextHorzAlign horzAlign, TextVertAlign vertAlign, std::string str)
{
	// split string into lines. '\n' is parsed as a new line character
	// also, get the max width and height of strings.
	std::string lineString;
	std::vector<std::string> lineVector;
	int lineWidth = 0, maxWidth = 0, charWidth = 0;
	bool newLine = false;

	int i = 0;
	while (i < str.size())
	{
		if (str[i] == "\n")
		{
			// skip new line character
			i++;
			newLine = true;
		}
		
		charWidth = m_font->getStringWidth(str[i]);
		if (lineWidth + charWidth > rc.getWidth())
			newLine = true;
		
		if (newLine)
		{
			newLine = false;
			lineWidth = 0;
			lineVector.push_back(lineString);
		}
		
		lineWidth += charWidth;
		if (maxWidth < lineWidth)
			maxWidth = lineWidth;
		
		lineString += str[i];
		i++
	}
	// push back the remaining string
	lineVector.push_back(lineString);
		
	// get start position according to align type
	int startX = 0;
	switch (horzAlign)
	{
	case TEXT_HALIGN_LEFT: startX = 0; break;
	case TEXT_HALIGN_CENTER: startX = (rc.getWidth() - maxWidth) / 2; break;
	case TEXT_HALIGN_RIGHT: startX = rc.getWidth() - maxWidth; break;
	}
	
	int startY = 0;
	int maxHeight = (int)lineVector.size() * m_font->getHeight();
	switch (vertAlign)
	{
	case TEXT_VALIGN_TOP: startY = 0; break;
	case TEXT_VALIGN_MIDDLE: startY = (rc.getHeight() - maxHeight) / 2; break;
	case TEXT_VALIGN_BOTTOM: startY = rc.getHeight() - maxHeight; break;
	}
	
	// draw string line by line
	for (i = 0; i < lineVector.size(); i++)
	{
		drawString(Point(startX, startY), clr, lineVector[i]);
		startY += m_font->getHeight();
	}
}

void DfbDevice::drawLine(Point start, Point end, Color clr)
{
	start.offsetPoint(m_viewport.getX(), m_viewport.getY());
	end.offsetPoint(m_viewport.getX(), m_viewport.getY());

	m_screen->SetColor(m_screen, clr.getRed(), clr.getGreen(), clr.getBlue(), clr.getAlpha());
	m_screen->DrawLine(m_screen, start.getX(), start.getY(), end.getX(), end.getY());
}

void DfbDevice::setFont(Font* font)
{
	ASSERT(font);

	m_font = (DfbFont*)font;
	m_screen->SetFont(m_screen, m_font->getDFBFont());
}

Font* DfbDevice::createFont(unsigned int height, std::string fontfile)
{
	DFBFontDescription     fdsc;
	IDirectFBFont *idfbfont;
	DfbFont *dfbfont;
	DFBResult              err;

	fdsc.flags = DFDESC_HEIGHT;
	fdsc.height = height;

	if( fontfile.size() == 0 )
		fontfile = "/usr/local/share/fonts/Default.ttf";

	DFBCHECK(m_dfb->CreateFont( m_dfb, fontfile.c_str(), &fdsc, &idfbfont ));
	dfbfont = new DfbFont(idfbfont, USER_SIZE_FONT, fdsc.height);

	return (Font *)dfbfont;
}

void DfbDevice::releaseFont(Font** pfont)
{
	IDirectFBFont  *idfbfont;
	DfbFont* dfbfont = (DfbFont *)*pfont;

	idfbfont = dfbfont->getDFBFont();
	idfbfont->Release(idfbfont);

	delete *pfont;
	*pfont = NULL;
}

void DfbDevice::disposeFont(Font** pfont)
{
	IDirectFBFont  *idfbfont;
	DfbFont* dfbfont = (DfbFont *)*pfont;

	idfbfont = dfbfont->getDFBFont();
	idfbfont->Dispose(idfbfont);

	delete *pfont;
	*pfont = NULL;
}

void DfbDevice::update(Rect rc)
{
	DFBRectangle rect;

	// if rect is empty, draw the whole screen
	if (rc.isEmpty())
	{
		rect.x = rect.y = 0;
		rect.w = m_screenWidth;
		rect.h = m_screenHeight;
	}
	else
	{
		rect.x = rc.getLeft();
		rect.y = rc.getTop();
		rect.w = rc.getWidth();
		rect.h = rc.getHeight();
	}
	
	m_primary->Blit( m_primary, m_screen, &rect, rect.x, rect.y);
	m_primary->Flip( m_primary, NULL, (DFBSurfaceFlipFlags)0 );
}

bool DfbDevice::listen(void)
{
	m_keyEvtProc = new KeyEventProcessor(m_events, getInputEventListener());
	m_inpThrd = boost::thread(*m_keyEvtProc);

	return true;
}

bool DfbDevice::stop(void)
{
	ASSERT(m_keyEvtProc);
	
	m_keyEvtProc->quit();
	m_inpThrd.join();
	
	delete m_keyEvtProc;

	return true;
}

void DfbDevice::shutdown(void)
{
	if (!m_init)
	{
		LOG_ERROR("not initialized.");
		return;
	}
	
	// destroy font
	IDirectFBFont* dfbfont;

	for (std::vector<DfbFont*>::iterator it = s_deviceFonts.begin(); it != s_deviceFonts.end(); ++it)
	{
		dfbfont = (*it)->getDFBFont();
		dfbfont->Release(dfbfont);
		delete (*it);
	}
	s_deviceFonts.clear();

	// release dfb resources.
	m_primary->Release( m_primary );
	m_events->Release( m_events );
	m_dfb->Release( m_dfb );

	m_init = false;
}

GraphicDevice* createGraphicDevice(void)
{
	if (!s_dfbDevice)
	{
		s_dfbDevice = new DfbDevice;
	}

	return s_dfbDevice;
}

InputDevice* createInputDevice(void)
{
	if (!s_dfbDevice)
	{
		s_dfbDevice = new DfbDevice;
	}

	return s_dfbDevice;
}

void destroyGraphicDevice(GraphicDevice* device)
{
	if (s_dfbDevice)
	{
		ASSERT(s_dfbDevice == device);

		delete s_dfbDevice;
		s_dfbDevice = NULL;
	}
}

void destroyInputDevice(InputDevice* device)
{
	if (s_dfbDevice)
	{
		ASSERT(s_dfbDevice == device);

		delete s_dfbDevice;
		s_dfbDevice = NULL;
	}
}
