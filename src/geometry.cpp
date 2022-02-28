#include "windows.h"

#include <algorithm>

void Rect::set(int left, int top, int right, int bottom)
{
	m_left = left;
	m_top = top;
	m_right = right;
	m_bottom = bottom;
}

bool Rect::isEmpty()
{
	return (m_right <= m_left || m_bottom <= m_top);
}

void Rect::offsetRect(int x, int y)
{
	m_left += x;
	m_top += y;
	m_right += x;
	m_bottom += y;
}

void Rect::unionRect(Rect rc)
{
	if (rc.isEmpty())
		return;

	if (isEmpty())
	{
		m_left = rc.getLeft();
		m_top = rc.getTop();
		m_right = rc.getRight();
		m_bottom = rc.getBottom();
	}
	else
	{
		m_left = std::min(m_left, rc.getLeft());
		m_top = std::min(m_top, rc.getTop());
		m_right = std::max(m_right, rc.getRight());
		m_bottom = std::max(m_bottom, rc.getBottom());
	}
}

void Rect::intersectRect(Rect rc)
{
	if (rc.isEmpty() || isEmpty())
	{
		makeEmpty();
	}
	else
	{
		m_left = std::max(m_left, rc.getLeft());
		m_top = std::max(m_top, rc.getTop());
		m_right = std::min(m_right, rc.getRight());
		m_bottom = std::min(m_bottom, rc.getBottom());		
	}
}

void Rect::deflateRect(int x, int y)
{
	if (isEmpty())
		return;
	
	m_left += x;
	m_top += y;
	m_right -= x;
	m_bottom -= y;
}

void Rect::inflateRect(int x, int y)
{
	m_left -= x;
	m_top -= y;
	m_right += x;
	m_bottom += y;
}

void Rect::makeEmpty(void)
{
	m_left = m_top = m_right = m_bottom = 0;
}

void Rect::dump(void)
{
	LOG_DEBUG("[rect:(" << m_left << "," << m_top << "," << m_right << "," << m_bottom << ")]");
}
