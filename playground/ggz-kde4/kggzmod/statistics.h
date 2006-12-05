#ifndef KGGZMOD_STATISTICS_H
#define KGGZMOD_STATISTICS_H

namespace KGGZMod
{

class StatisticsPrivate;

class Statistics
{
	friend class ModulePrivate;

	public:
		~Statistics();

		int wins() const;
		int losses() const;
		int ties() const;
		int forfeits() const;

		int rating() const;
		int ranking() const;
		int highscore() const;

		bool hasRecord() const;
		bool hasRating() const;
		bool hasRanking() const;
		bool hasHighscore() const;

	private:
		Statistics();
		StatisticsPrivate *d;
		void init(StatisticsPrivate *x);
};

}

#endif

