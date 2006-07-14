package ggz.common;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.NoSuchElementException;
import java.util.StringTokenizer;

public class CSVParser {

    private CSVParser() {
        // Private constructor to prevent access.
    }

    public static String[] parseLine(BufferedReader reader) throws IOException {
        return parseLine(reader.readLine());
    }

    public static String[] parseLine(String line) {
        if (line == null || "".equals(line)) {
            return new String[0];
        }

        ArrayList fields = new ArrayList();
        boolean isQuoteOpen = false;
        StringTokenizer tokenizer = new StringTokenizer(line, "\",", true);
        String tok = tokenizer.nextToken();
        StringBuffer field = new StringBuffer();

        try {
            do {
                if ("\"".equals(tok)) {
                    if (isQuoteOpen) {
                        // Could either be the closing quote or an escaped
                        // quote.
                        tok = tokenizer.nextToken();
                        if ("\"".equals(tok)) {
                            // an escaped quote
                            field.append(tok);
                        } else if (",".equals(tok)) {
                            // end of field.
                            isQuoteOpen = false;
                        } else {
                            // This should probably be an error since we
                            // have a single quote in the file but let it
                            // through for now.
                        }
                    } else {
                        isQuoteOpen = true;
                    }
                }
                if (",".equals(tok)) {
                    if (isQuoteOpen) {
                        field.append(tok);
                    } else {
                        // End of field.
                        fields.add(field.toString());
                        field.delete(0, field.length());
                    }
                } else {
                    if (!"\"".equals(tok)) {
                        field.append(tok);
                    }
                }
                tok = tokenizer.nextToken();
            } while (true);
        } catch (NoSuchElementException e) {
            // No more tokens
            fields.add(field.toString());
        }

        return (String[]) fields.toArray(new String[fields.size()]);
    }

    /**
     * Escapes a string according to CSV conventions:
     * <ul>
     * <li>fields that contain commas, double-quotes, or line-breaks must be
     * quoted,</li>
     * <li>a quote within a field must be escaped with an additional quote
     * immediately preceding the literal quote,</li>
     * <li>space before and after delimiter commas may be trimmed, and</li>
     * <li>a line break within an element must be preserved.</li>
     * </ul>
     * 
     * @param str
     * @return
     */
    public static String escape(String str) {
        if (str == null)
            return "";
        
        if (str.indexOf('"') >= 0 || str.indexOf(",") >= 0
                || str.indexOf("\n") >= 0) {
            return "\"" + StringUtil.replace(str, "\"", "\"\"") + "\"";
        }
        
        return str;
    }
}
