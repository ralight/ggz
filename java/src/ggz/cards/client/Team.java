package ggz.cards.client;

import java.util.ArrayList;

import ggz.cards.common.ScoreData;

public class Team {
    private ScoreData score;

    private ArrayList scores = new ArrayList();

    public void addScore(ScoreData s) {
        this.scores.add(s);
    }

    public void setScore(ScoreData s) {
        this.score = s;
    }

    public ScoreData getScore() {
        return this.score;
    }
}
