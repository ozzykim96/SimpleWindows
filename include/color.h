#ifndef _COLOR_H_
#define _COLOR_H_

class Color
{
public:
	Color(void) {}
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xff) : m_r(r), m_g(g), m_b(b), m_a(a) {}
	
	int set(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { m_r = r; m_g = g; m_b = b; m_a = a; }
	
	int getRed(void) { return m_r; }
	int getGreen(void) { return m_g; }
	int getBlue(void) { return m_b; }
	int getAlpha(void) { return m_a; }
private:
	unsigned char m_r;
	unsigned char m_g;
	unsigned char m_b;
	unsigned char m_a;
};

#define COLOR_BLACK		Color(0, 0, 0)
#define COLOR_WHITE		Color(255, 255, 255)
#define COLOR_RED		Color(255,0,0)
#define COLOR_LIME		Color(0,255,0)
#define COLOR_BLUE		Color(0,0,255)
#define COLOR_YELLOW	Color(255,255,0)
#define COLOR_CYAN 		Color(0,255,255)
#define COLOR_MAGENTA 	Color(255,0,255)
#define COLOR_SILVER	Color(192,192,192)
#define COLOR_GRAY		Color(128,128,128)
#define COLOR_MAROON	Color(128,0,0)
#define COLOR_OLIVE		Color(128,128,0)
#define COLOR_GREEN		Color(0,128,0)
#define COLOR_PURPLE	Color(128,0,128)
#define COLOR_TEAL		Color(0,128,128)
#define COLOR_NAVY		Color(0,0,128)

#endif /* _COLOR_H_ */

