#include "statistics.h"
#include "statistics_private.h"

using namespace KGGZMod;

Statistics::Statistics()
{
	d = new StatisticsPrivate();

	d->wins = -1;
	d->losses = -1;
	d->ties = -1;
	d->forfeits = -1;
	d->rating = -1;
	d->ranking = -1;
	d->highscore = -1;

	d->hasrecord = false;
	d->hasranking = false;
	d->hasrating = false;
	d->hashighscore = false;
}

Statistics::~Statistics()
{
	delete d;
}

int Statistics::wins() const
{
	return d->wins;
}

int Statistics::losses() const
{
	return d->losses;
}

int Statistics::ties() const
{
	return d->ties;
}

int Statistics::forfeits() const
{
	return d->forfeits;
}

int Statistics::rating() const
{
	return d->rating;
}

int Statistics::ranking() const
{
	return d->ranking;
}

int Statistics::highscore() const
{
	return d->highscore;
}

bool Statistics::hasRecord() const
{
	return d->hasrecord;
}

bool Statistics::hasRating() const
{
	return d->hasrating;
}

bool Statistics::hasRanking() const
{
	return d->hasranking;
}

bool Statistics::hasHighscore() const
{
	return d->hashighscore;
}

void Statistics::init(StatisticsPrivate *x)
{
	delete d;
	d = x;
}

