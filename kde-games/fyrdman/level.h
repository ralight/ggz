#ifndef FYRDMAN_LEVEL_H
#define FYRDMAN_LEVEL_H

class Level
{
	public:
		Level();
		~Level();

		void loadFromNetwork(int fd);
		bool loadFromFile(const char *filename);

		const char *title();
		const char *version();
		const char *author();
		const char *graphics();
		int width();
		int height();
		int players();
		int cell(int i, int j);
		char cellboard(int i, int j);
		int cellown(int i, int j);

		void setCell(int i, int j, int value);

		const char *location();

	private:
		char *m_title, *m_version, *m_author, *m_graphics;
		int m_width, m_height, m_players;
		char **m_cell;
		char **m_cellboard;
		char **m_cellown;
		char *m_location;
};

#endif

