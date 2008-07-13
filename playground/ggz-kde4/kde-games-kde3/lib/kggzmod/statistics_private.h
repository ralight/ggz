#ifndef KGGZMOD_STATISTICS_PRIVATE_H
#define KGGZMOD_STATISTICS_PRIVATE_H

namespace KGGZMod {

class StatisticsPrivate
{
	public:
		StatisticsPrivate(){}
		int wins;
		int losses;
		int ties;
		int forfeits;
		int rating;
		int ranking;
		int highscore;
		bool hasrecord;
		bool hasrating;
		bool hasranking;
		bool hashighscore;
};

}

#endif

