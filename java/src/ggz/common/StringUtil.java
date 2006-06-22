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

import java.util.StringTokenizer;

public class StringUtil {

    /**
     * Replaces all occurrences of <CODE>ch</CODE> with the replacement
     * String.
     * 
     * @param text
     * @param character
     * @param replacement
     * @return
     */
    public static String replace(String text, String character,
            String replacement) {
        StringTokenizer parser = new StringTokenizer(text, character, true);
        StringBuffer result = new StringBuffer();
        while (parser.hasMoreTokens()) {
            String fragment = parser.nextToken();
            if (character.equals(fragment)) {
                result.append(replacement);
            } else {
                result.append(fragment);
            }
        }
        return result.toString();
    }
}
