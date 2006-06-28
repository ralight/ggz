package ggz.common;

import ggz.client.core.Player;

import java.text.Collator;
import java.util.Arrays;

import junit.framework.TestCase;

public class SortedListTest extends TestCase {

    /*
     * Test method for 'ggz.common.SortedList.add(Object)'
     */
    public void testAddObject() {
        SortedList list = new SortedList(Collator.getInstance());
        assertTrue(list.add("xyz"));
        assertTrue(list.add("aac"));
        assertTrue(list.add("abc"));
        assertTrue(list.add("abd"));
        assertEquals(4, list.size());
        assertEquals(0, list.indexOf("aac"));
        assertEquals(1, list.indexOf("abc"));
        assertEquals(2, list.indexOf("abd"));
        assertEquals(3, list.indexOf("xyz"));
        assertTrue(list.remove("aac"));
        assertEquals(3, list.size());
        assertTrue(list.remove("abc"));
        assertEquals(2, list.size());
        assertTrue(list.remove("xyz"));
        assertEquals(1, list.size());
        assertTrue(list.remove("abd"));
        assertEquals(0, list.size());
    }

    /*
     * Test method for 'ggz.common.SortedList.addAll(Collection)'
     */
    public void testAddAllCollection() {
        SortedList list = new SortedList(Collator.getInstance());
        assertTrue(list.addAll(Arrays.asList(new String[] { "xyz", "aac",
                "abc", "abd" })));
        assertEquals(4, list.size());
        assertEquals(0, list.indexOf("aac"));
        assertEquals(1, list.indexOf("abc"));
        assertEquals(2, list.indexOf("abd"));
        assertEquals(3, list.indexOf("xyz"));
        assertTrue(list.remove("aac"));
        assertEquals(3, list.size());
        assertTrue(list.remove("abc"));
        assertEquals(2, list.size());
        assertTrue(list.remove("xyz"));
        assertEquals(1, list.size());
        assertTrue(list.remove("abd"));
        assertEquals(0, list.size());

        list = new SortedList(Player.SORT_BY_NAME);
        assertTrue(list.addAll(Arrays.asList(new Player[] { new Player("xyz"),
                new Player("aac"), new Player("abc"), new Player("abd") })));
        assertEquals(4, list.size());
        assertEquals(0, list.indexOf(new Player("aac")));
        assertEquals(1, list.indexOf(new Player("abc")));
        assertEquals(2, list.indexOf(new Player("abd")));
        assertEquals(3, list.indexOf(new Player("xyz")));
        assertTrue(list.remove(new Player("aac")));
        assertEquals(3, list.size());
        assertTrue(list.remove(new Player("abc")));
        assertEquals(2, list.size());
        assertTrue(list.remove(new Player("xyz")));
        assertEquals(1, list.size());
        assertTrue(list.remove(new Player("abd")));
        assertEquals(0, list.size());
    }

}
