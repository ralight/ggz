#ifndef GGZAP_GAME_H
#define GGZAP_GAME_H

class GGZapGame
{
	public:
		GGZapGame();
		~GGZapGame();

		void addGame(const char *name, const char *frontend);

		int count();
		char *name(int id);
		char *frontend(int id);

		void clear();

		void autoscan();

	private:
		int m_count;
		char ***gamelist;
};

#endif

