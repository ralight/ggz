#ifndef QPOINT_H
#define QPOINT_H

class QPoint
{
	public:
		QPoint(){m_x = 0; m_y = 0;}
		QPoint(int x, int y){m_x = x; m_y = y;}
		~QPoint(){}
		int x(){return m_x;}
		int y(){return m_y;}
		int isNull(){return !(m_x && m_y);}
		friend bool operator== (const QPoint p1, const QPoint p2){
			return ((p1.m_x == p2.m_x) && (p1.m_y == p2.m_y));}

	private:
		int m_x;
		int m_y;
};

#endif

