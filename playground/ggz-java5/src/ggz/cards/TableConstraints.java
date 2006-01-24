package ggz.cards;

public class TableConstraints {
    /**
     * The player index for which this component belongs.
     */
    protected int playerIndex;

    /**
     * The purpose of this component. E.g. player lable, card in hand, card in
     * trick.
     */
    protected int type;

    protected int cardIndex;

    public static final int PLAYER_LABEL = 0;

    public static final int CARD_IN_HAND = 1;

    public static final int CARD_IN_TRICK = 2;

    public TableConstraints(int type, int playerIndex) {
        this.type = type;
        this.playerIndex = playerIndex;
    }

    public TableConstraints(int type, int playerIndex, int cardIndex) {
        this(type, playerIndex);
        this.cardIndex = cardIndex;
    }
}
