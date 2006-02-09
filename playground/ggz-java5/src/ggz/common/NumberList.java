/*
 /*
 * Copyright (C) 2006  Helg Bredow
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
package ggz.common;

import java.text.CharacterIterator;
import java.text.StringCharacterIterator;
import java.util.logging.Logger;

/**
 * This class provides a method for storing and retrieving a simple list of
 * positive integers. The list must follow a very restrictive form: each value
 * within [1..32] may be included explicitly in the list. Higher values may only
 * be included as the part of a single given range [x..y].
 * 
 * This provides GGZ-specific functionality that is common to some or all of the
 * ggz-server, game-server, ggz-client, and game-client.
 */
public class NumberList {
    private static final Logger log = Logger.getLogger(NumberList.class
            .getName());

    private int values;

    private int min;

    private int max;

    /** Creates an empty number list. */
    public NumberList() {
        values = 0;
        min = -1;
        max = -1;
    }

    /**
     * Reads a number list from a text string.
     * 
     * The string is comprised of a list of values (in the range 1..32)
     * separated by spaces, followed by an optional range (separated by "..").
     * Examples: "2 3 4", "2..4", "1..1000", "2, 3, 10 15-50"
     */
    public static NumberList read(String text) {
        NumberList list = new NumberList();

        if (text == null) {
            return list;
        }

        StringCharacterIterator iter_this = new StringCharacterIterator(text);
        StringCharacterIterator iter_next = new StringCharacterIterator(text);
        char next, ch = iter_this.current();
        int error = 0;

        do {
            int str;

            // Skip space characters.
            while (Character.isWhitespace(ch)) {
                ch = iter_this.next();
            }
            // If we reached the end of the string then break.
            if (ch == CharacterIterator.DONE) {
                break;
            }

            iter_next.setIndex(iter_this.getIndex());
            next = iter_next.next();
            while (next != CharacterIterator.DONE
                    && !Character.isWhitespace(next)) {
                next = iter_next.next();
            }

            if (next == CharacterIterator.DONE) {
                str = text.indexOf("..", iter_this.getIndex());
            } else {
                str = text
                        .substring(iter_this.getIndex(), iter_next.getIndex())
                        .indexOf("..");
            }

            if (str > -1) {
                int val1, val2;

                val1 = Integer.parseInt(text.substring(iter_this.getIndex(),
                        (next == CharacterIterator.DONE) ? str : iter_next
                                .getIndex()));
                val2 = Integer.parseInt(text.substring(str + 2,
                        (next == CharacterIterator.DONE) ? text.length()
                                : iter_next.getIndex()));

                if (val1 < 0 || val2 < 0 || val1 >= val2) {
                    error = 1;
                } else if (list.max >= 0) {
                    error = 1;
                } else {
                    list.min = val1;
                    list.max = val2;
                }
            } else {
                int val = Integer.parseInt(text.substring(iter_this.getIndex(),
                        iter_next.getIndex()));// atoi(this);

                if (val <= 0 || val > 32)
                    error = 1;
                else if ((list.values & (1 << (val - 1))) != 0)
                    error = 1;
                else
                    list.values |= (1 << (val - 1));
            }

            iter_this.setIndex(iter_next.getIndex());
            ch = iter_this.next();
        } while (next != CharacterIterator.DONE);

        if (error != 0) {
            log.severe("Error reading number list \"" + text + "\".");
        }

        return list;
    }

    /** Writes a number list to a text string. */
    public String write() {
        // /* The theoretical maximum string length is less than this. */
        // char str[256] = "";
        // int i;
        //
        // for (i = 1; i < 32; i++) {
        // if (list->values & (1 << (i - 1))) {
        // char this[10];
        // snprintf(this, sizeof(this), "%d ", i);
        // strcat(str, this);
        // }
        // }
        //
        // if (list->min > 0) {
        // char this[32];
        // if (list->max < 0) {
        // ggz_error_msg("Invalid range %d/%d in number list.",
        // list->min, list->max);
        // list->max = list->min;
        // }
        //
        // snprintf(this, sizeof(this), "%d..%d", list->min, list->max);
        // strcat(str, this);
        // } else {
        // /* Remove trailing space. */
        // str[strlen(str) - 1] = '\0';
        // }
        //
        // return ggz_strdup(str);
        throw new UnsupportedOperationException("Not ported from C code yet");
    }

    /** Checks to see if the given value is set in the number list. */
    public boolean isset(int value) {
        if (value <= 0) {
            return false;
        }

        if (this.min > 0 && value >= this.min && value <= this.max) {
            return true;
        }

        if (value > 32) {
            return false;
        }

        return !!((this.values & (1 << (value - 1))) != 0);
    }

    /** @return the smallest value in the set. */
    public int get_min() {
        int result = this.min;

        if (result <= 0) {
            result = 33;
        }

        /* FIXME: come up with a cool bit maniuplation to do this */
        for (int i = 1; i < result; i++) {
            if ((this.values & (1 << (i - 1))) != 0) {
                return i;
            }
        }

        return result == 33 ? 0 : result;
    }

    /** @return the largest value in the set. */
    public int get_max() {
        int result = this.max;

        if (result <= 0) {
            result = 0;
        }

        for (int i = 32; i > result; i--) {
            if ((this.values & (1 << (i - 1))) != 0) {
                return i;
            }
        }

        return result;
    }
}
