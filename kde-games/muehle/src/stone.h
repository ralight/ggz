#ifndef STONE_H
#define STONE_H

class Stone
{
	public:
		Stone();
		~Stone();
		enum Owners
		{
			black,
			white,
			blackactive,
			whiteactive,
			blackidle,
			whiteidle,
			blackmuehle,
			whitemuehle
		};
		void move(int x, int y);
		void assign(int owner);
		int x();
		int y();
		int owner();
	private:
		int m_x, m_y;
		int m_owner;
};

#endif

