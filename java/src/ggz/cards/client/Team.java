package ggz.cards.client;

import ggz.cards.common.ScoreData;

import java.util.ArrayList;

public class Team {
    private ArrayList scores = new ArrayList();

    /**
     * Adds a new score for this team. If handNum is not the same same as the
     * next index in the score list then an IllegalArgumentException is thrown.
     * Scores are stored indexed by hand number.
     */
    public void addScore(ScoreData s, int handNum) {
        if (handNum > this.scores.size()) {
            throw new IllegalArgumentException(
                    "Adding score that isn't for the next hand number:"
                            + handNum);
        }

        if (handNum < this.scores.size())
            this.scores.set(handNum, s);
        else
            this.scores.add(s);
    }

    public ScoreData getScore() {
        return (ScoreData) this.scores.get(this.scores.size() - 1);
    }
}
