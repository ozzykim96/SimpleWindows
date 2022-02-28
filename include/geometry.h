#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

class Rect
{
public:
	Rect(void) : m_left(0), m_top(0), m_right(0), m_bottom(0) {}
	Rect(int left, int top, int right, int bottom) : m_left(left), m_top(top), m_right(right), m_bottom(bottom) {}

	void set(int left, int top, int right, int bottom);

	int getLeft(void) { return m_left; }
	int getRight(void) { return m_right; }
	int getTop(void) { return m_top; }
	int getBottom(void) { return m_bottom; }
	int getWidth(void) { return m_right - m_left; }
	int getHeight(void) { return m_bottom - m_top; }	
	
	bool isEmpty(void);
	void offsetRect(int x, int y);
	void unionRect(Rect rc);	
	void intersectRect(Rect rc);
	void deflateRect(int x, int y);
	void inflateRect(int x, int y);
	void makeEmpty(void);
	
	void dump(void);
	
private:
	int m_left;
	int m_right;
	int m_top;
	int m_bottom;
};

#define NULL_RECT	Rect(0, 0, 0, 0)

class Point
{
public:
	Point(void) : m_x(0), m_y(0) {}
	Point(int x, int y) : m_x(x), m_y(y) {}
	
	void set(int x, int y) { m_x = x; m_y = y; }
	
	int getX(void) { return m_x; }
	int getY(void) { return m_y; }
	
	void offsetPoint(int x, int y) { m_x += x; m_y += y; }
private:
	int m_x;
	int m_y;
};

class Size
{
public:
	Size(void) : m_width(0), m_height(0) {}
	Size(int width, int height) : m_width(width), m_height(height) {}
	
	void set(int width, int height) { m_width = width; m_height = height; }
	
	int getWidth(void) { return m_width; }
	int getHeight(void) { return m_height; }
private:
	int m_width;
	int m_height;
};

#endif /* _GEOMETRY_H_ */
