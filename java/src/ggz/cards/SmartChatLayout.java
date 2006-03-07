package ggz.cards;

import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.LayoutManager;

public class SmartChatLayout implements LayoutManager {
	private Component table;

	private Component chat;

	public static final String CHAT = "chat";

	public static final String TABLE = "table";

	public void addLayoutComponent(String name, Component comp) {
		if (TABLE.equals(name)) {
			table = comp;
		} else if (CHAT.equals(name)) {
			chat = comp;
		} else {
			throw new IllegalArgumentException(
					"Unsupported layout constraint name: " + name);
		}

	}

	/**
	 * Lays out the table so that it's always square, chat gets the rest.
	 */
	public void layoutContainer(Container parent) {
		if (parent.getWidth() > parent.getHeight()) {
			// The container is horizontal so put chat to the side.
			table.setLocation(0, 0);
			table.setSize(parent.getHeight(), parent.getHeight());
			chat.setLocation(parent.getHeight(), 0);
			chat.setSize( parent.getWidth()
					- parent.getHeight(), parent.getHeight());
		} else {
			// Put chat to the bottom.
			table.setLocation(0, 0);
			table.setSize(parent.getWidth(), parent.getWidth());
			chat.setLocation(0, parent.getWidth());
			chat.setSize(parent
					.getWidth(), parent.getHeight() - parent.getWidth());
		}
	}

	public Dimension minimumLayoutSize(Container parent) {
		return new Dimension(640, 480);
	}

	public Dimension preferredLayoutSize(Container parent) {
		return new Dimension(800, 600);
	}

	public void removeLayoutComponent(Component comp) {
		if (table == comp) {
			table = null;
		} else if (chat == comp) {
			chat = null;
		}
	}

}
