package ggz.cards.client;

import java.util.ArrayList;

import ggz.cards.common.ScoreData;

public class Team {
    private ScoreData score;

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

    public void setScore(ScoreData s) {
        this.score = s;
    }

    public ScoreData getScore() {
        return this.score;
    }
}
