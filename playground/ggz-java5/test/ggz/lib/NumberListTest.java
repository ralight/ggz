package ggz.lib;

import ggz.common.NumberList;
import junit.framework.TestCase;

public class NumberListTest extends TestCase {

    public void testRead() {
        NumberList list; 
        
        list = NumberList.read("");
        assertEquals(0, list.get_min());
        assertEquals(0, list.get_max());
        assertFalse(list.isset(1));

        list = NumberList.read(null);
        assertEquals(0, list.get_min());
        assertEquals(0, list.get_max());
        assertFalse(list.isset(1));

        list = NumberList.read("2 3 4");
        assertEquals(2, list.get_min());
        assertEquals(4, list.get_max());
        assertFalse(list.isset(1));
        assertTrue(list.isset(2));
        assertTrue(list.isset(3));
        assertTrue(list.isset(4));
        
        list = NumberList.read("2..4");
        assertEquals(2, list.get_min());
        assertEquals(4, list.get_max());
        assertFalse(list.isset(1));
        assertTrue(list.isset(2));
        assertTrue(list.isset(3));
        assertTrue(list.isset(4));
        
        list = NumberList.read("1..1000");
        assertEquals(1, list.get_min());
        assertEquals(1000, list.get_max());
        assertTrue(list.isset(1));
        assertTrue(list.isset(2));
        assertTrue(list.isset(3));
        assertTrue(list.isset(4));
        assertTrue(list.isset(678));
        assertTrue(list.isset(1000));
        assertFalse(list.isset(1001));
        
        list = NumberList.read("6 2..4");
        assertEquals(2, list.get_min());
        assertEquals(6, list.get_max());
        assertFalse(list.isset(1));
        assertTrue(list.isset(2));
        assertTrue(list.isset(3));
        assertTrue(list.isset(4));
        assertFalse(list.isset(5));
        assertTrue(list.isset(6));
        assertFalse(list.isset(7));
        
//        list = NumberList.read("2, 3, 10 15-50");
//        assertEquals(50, list.get_max());
    }

}
