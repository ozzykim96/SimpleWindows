#ifndef _GRAPHIC_DEVICE_H_
#define _GRAPHIC_DEVICE_H_

class Point;
class Rect;
class Color;

enum FontType
{
	SMALL_FONT = 0,
	NORMAL_FONT,
	LARGE_FONT,
	USER_SIZE_FONT
};

class Font
{
public:
	virtual FontType getType(void) = 0;
	virtual int getStringWidth(std::string str) = 0;
	virtual int getHeight(void) = 0;
};

/**
 * get device allocated font. see FontType.
 * @note the returned font must not be released. it is released when device is shut.
 */
Font* getDeviceFont(FontType type);

enum TextHorzAlign
{
	TEXT_HALIGN_LEFT = 0,
	TEXT_HALIGN_CENTER,
	TEXT_HALIGN_RIGHT
};

enum TextVertAlign
{
	TEXT_VALIGN_TOP = 0,
	TEXT_VALIGN_MIDDLE,
	TEXT_VALIGN_BOTTOM
};

class GraphicDevice
{
public:
	virtual bool init(void) = 0;
	virtual void shutdown(void) = 0;

	/**
	 * change logical start point(viewport) in drawing functions.
	 * @note clip() function is not affected.
	 */
	virtual void setViewport(Point viewport) = 0;
	/** clip drawing region(rect). only this region has an impact on drawing. */
	virtual void clip(Rect rc) = 0;

	virtual void drawRect(Rect rc, Color clr) = 0;
	virtual void drawBorder(Rect rc, Color clr, int size) = 0;
	virtual void drawLine(Point start, Point end, Color clr) = 0;
	virtual void drawString(Point pt, Color clr, std::string str) = 0;
	virtual void drawAlignedText(Rect rc, Color clr, TextHorzAlign horzAlign, TextVertAlign vertAlign, std::string str) = 0;
	virtual void drawMultiAlignedText(Rect rc, TextHorzAlign horzAlign, TextVertAlign vertAlign, std::string str) = 0;

	virtual Font* getFont(void) = 0;
	virtual void setFont(Font* font) = 0;
	virtual void releaseFont(Font** pfont) = 0;
	virtual void disposeFont(Font** pfont) = 0;
	virtual Font* createFont(unsigned int height, std::string fontfile="") =0;

	/**
	 * update drawings. if rc is empty (or NULL_RECT), the whole screen will be updated.
	 */
	virtual void update(Rect rc) = 0;
};

GraphicDevice* createGraphicDevice(void);
void destroyGraphicDevice(GraphicDevice* device);

#endif /* _GRAPHIC_DEVICE_H_ */
