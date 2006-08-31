package ggz.cards.common;

public class ScoreData {
    private int score;

    private int[] extra;

    public ScoreData(int score, int[] extras) {
        this.score = score;
        this.extra = extras;
    }

    public int getScore() {
        return this.score;
    }

    public int getExtra(int index) {
        return this.extra[index];
    }

    public String toString() {
        return score + ":" + extra;
    }
}
