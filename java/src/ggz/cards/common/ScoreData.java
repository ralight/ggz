package ggz.cards.common;

public class ScoreData {
    private int score;

    private int extra;

    public ScoreData(int score, int extra) {
        this.score = score;
        this.extra = extra;
    }

    public int getScore() {
        return this.score;
    }

    public int getExtra() {
        return this.extra;
    }
}
