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
		int width();
		int height();
		int players();
		char cell(int i, int j);
		char cellboard(int i, int j);
		char cellown(int i, int j);

		void setCell(int i, int j, int value);

	private:
		char *m_title, *m_version, *m_author;
		int m_width, m_height, m_players;
		char **m_cell;
		char **m_cellboard;
		char **m_cellown;
};

#endif

