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
package ggz.ui;

import ggz.client.core.ChatEventData;
import ggz.client.core.ErrorEventData;
import ggz.client.core.Game;
import ggz.client.core.GameEventListener;
import ggz.client.core.Module;
import ggz.client.core.Room;
import ggz.client.core.RoomChangeEventData;
import ggz.client.core.RoomListener;
import ggz.client.core.Server;
import ggz.client.core.StateID;
import ggz.client.core.Table;
import ggz.client.core.TableLeaveEventData;
import ggz.common.LeaveType;
import ggz.common.SeatType;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.io.IOException;
import java.net.URL;
import java.text.MessageFormat;
import java.util.ResourceBundle;

import javax.swing.AbstractAction;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.SwingUtilities;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.AbstractTableModel;

public class RoomPanel extends JPanel implements RoomListener,
		ListSelectionListener {
	private static final ResourceBundle messages = ResourceBundle
			.getBundle("ggz.ui.messages");

	protected Server server;

	protected Room room;

	private JScrollPane tableScrollPane;

	protected JTable tableTable;

	protected JPanel tablePanel;

	private JPanel tableButtonPanel;

	private JButton newTableButton;

	private JButton joinTableButton;

	private JButton spectateButton;

	private JPanel headerPanel;

	private JPanel headerButtonPanel;

	private JLabel titleLabel;

	private JButton logoutButton;

	private JButton lobbyButton;

	private RoomChatPanel chatPanel;

	private TablesTableModel tables;

	public RoomPanel(Server server) {
		super(new BorderLayout(4, 4));
		this.server = server;

		// Set up title with room name and back to lobby button.
		headerPanel = new JPanel(new BorderLayout());
		headerButtonPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
		titleLabel = new JLabel();
		lobbyButton = new JButton(new BackToLobbyAction());
		logoutButton = new JButton(new LogoutAction());

		tablePanel = new JPanel(new BorderLayout());
		tableButtonPanel = new JPanel();
		newTableButton = new JButton(new NewTableAction());
		joinTableButton = new JButton(new JoinTableAction());
		joinTableButton.setEnabled(false);
		spectateButton = new JButton(new SpectateAction());
		spectateButton.setEnabled(false);
		tables = new TablesTableModel();
		tableTable = new JTable(tables);
		tableTable.getTableHeader().setBackground(new Color(0xce, 0xfa, 0xdf));
		tableTable.setRowHeight(tableTable.getRowHeight() * 4);
		tableTable.getSelectionModel().setSelectionMode(
				ListSelectionModel.SINGLE_SELECTION);
		tableTable.getSelectionModel().addListSelectionListener(this);
		tableScrollPane = new JScrollPane();
		tableScrollPane.getViewport().add(tableTable);
		tableButtonPanel.add(newTableButton);
		tableButtonPanel.add(joinTableButton);
		tableButtonPanel.add(spectateButton);

		tablePanel.add(tableScrollPane, BorderLayout.CENTER);
		tablePanel.add(tableButtonPanel, BorderLayout.SOUTH);
		add(tablePanel, BorderLayout.CENTER);
		chatPanel = new RoomChatPanel(true);
		// Set a preferred size to stop it from growing out of control,
		// we want it to stay one size.
		chatPanel.setPreferredSize(new Dimension(500, 200));
		add(chatPanel, BorderLayout.SOUTH);

		headerPanel.add(titleLabel, BorderLayout.WEST);
		headerPanel.add(headerButtonPanel, BorderLayout.EAST);
		headerButtonPanel.add(lobbyButton);
		headerButtonPanel.add(logoutButton);
		add(headerPanel, BorderLayout.NORTH);

		setOpaque(false);
		lobbyButton.setOpaque(false);
		logoutButton.setOpaque(false);
		joinTableButton.setOpaque(false);
		newTableButton.setOpaque(false);
		spectateButton.setOpaque(false);
		headerPanel.setOpaque(false);
		headerButtonPanel.setOpaque(false);
		tableButtonPanel.setOpaque(false);
		tablePanel.setOpaque(false);
		tableScrollPane.setOpaque(false);
		tableScrollPane.getViewport().setOpaque(false);
	}

	public void setRoom(Room room) throws IOException {
		this.room = room;
		this.room.add_event_hook(this);
		this.room.list_tables();
		chatPanel.setRoom(room);
		titleLabel.setText("<HTML><B>" + room.get_name()
				+ "</B><BR><EM><SPAN style='font-weight:normal'>"
				+ room.get_desc() + "</SPAN></EM></HTML>");

		Module module = Module.get_nth_by_type(room.get_gametype(), 0);
		if (module != null && module.get_icon_path() != null) {
			URL imageURL = getClass().getResource(module.get_icon_path());
			titleLabel.setIcon(new ImageIcon(imageURL));
		}

		lobbyButton.setEnabled(true);
		newTableButton.setEnabled(true);
		joinTableButton.setEnabled(false);
		spectateButton.setEnabled(false);
		spectateButton.setVisible(room.get_gametype().get_spectators_allowed());
	}

	public void chat_event(ChatEventData data) {
		// Handled by chat panel so do nothing.
	}

	public void player_count(int room_id) {
		// Ignore
	}

	public void player_lag(String player) {
		// Handled by chat panel so do nothing.
	}

	public void player_list(int room_id) {
		// Handled by chat panel so do nothing.
	}

	public void player_stats(String player) {
		// Ignore
	}

	public void room_enter(RoomChangeEventData data) {
		// Ignore
	}

	public void room_leave(RoomChangeEventData data) {
		// Ignore
	}

	public void table_join_fail(String error) {
		tables.fireTableDataChanged();
		JOptionPane.showMessageDialog(this, error);
	}

	public void table_joined(int table_index) {
		tables.fireTableDataChanged();
	}

	public void table_launch_fail(ErrorEventData data) {
		JOptionPane.showMessageDialog(this, data.message);
	}

	public void table_launched() {
		tables.fireTableDataChanged();
		lobbyButton.setEnabled(false);
		newTableButton.setEnabled(false);
		joinTableButton.setEnabled(false);
		spectateButton.setEnabled(false);
	}

	public void table_leave_fail(String error) {
		JOptionPane.showMessageDialog(this, error);
	}

	public void table_left(TableLeaveEventData data) {
		if (data.get_reason() == LeaveType.GGZ_LEAVE_BOOT) {
			JOptionPane.showMessageDialog(this, MessageFormat.format(messages
					.getString("RoomPanel.Message.LeaveBoot"),
					new Object[] { data.get_booter() }));
		} else if (data.get_reason() == LeaveType.GGZ_LEAVE_GAMEERROR) {
			JOptionPane.showMessageDialog(this, messages
					.getString("RoomPanel.Message.LeaveGameError"));
		} else if (data.get_reason() == LeaveType.GGZ_LEAVE_GAMEOVER) {
			JOptionPane.showMessageDialog(this, messages
					.getString("RoomPanel.Message.GameOver"));
		}
		tables.fireTableDataChanged();
		lobbyButton.setEnabled(true);
		newTableButton.setEnabled(true);
	}

	public void table_list() {
		tables.fireTableDataChanged();
	}

	public void table_update() {
		tables.fireTableDataChanged();
	}

	private class BackToLobbyAction extends AbstractAction {

		public Object getValue(String key) {
			if (NAME.equals(key)) {
				return messages.getString("RoomPanel.Button.BackToGames");
			}
			return super.getValue(key);
		}

		public void actionPerformed(ActionEvent e) {
			try {
				server.join_room(0);
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}

	}

	/**
	 * Called when selection in table changes.
	 * 
	 * @param e
	 */
	public void valueChanged(ListSelectionEvent e) {
		boolean canJoinTable;
		boolean canSpectate;

		if (tableTable.getSelectedRowCount() > 0
				&& server.get_state() == StateID.GGZ_STATE_IN_ROOM) {
			Table selectedTable = room.get_nth_table(tableTable
					.getSelectedRow());

			/* Make sure table has open seats */
			canJoinTable = selectedTable.get_seat_count(SeatType.GGZ_SEAT_OPEN)
					+ selectedTable.get_seat_count(SeatType.GGZ_SEAT_RESERVED) > 0;

			// Temporary limitation because we don't support more than four
			// players in card games.
			canJoinTable = canJoinTable && (selectedTable.get_num_seats() <= 4);
			canSpectate = room.get_gametype().get_spectators_allowed();
		} else {
			canJoinTable = false;
			canSpectate = false;
		}

		joinTableButton.setEnabled(canJoinTable);
		spectateButton.setEnabled(canSpectate);
	}

	private abstract class PlayGameAction extends AbstractAction implements
			GameEventListener {

		public void actionPerformed(ActionEvent e) {
			if (Module.get_num_by_type(room.get_gametype()) == 0) {
				JOptionPane
						.showMessageDialog(
								(Component) e.getSource(),
								messages
										.getString("RoomPanel.Message.GameNotSupported"));
			} else {
				Module module = Module.get_nth_by_type(room.get_gametype(), 0);
				Game game = new Game(server, module);
				game.add_event_hook(this);
				game.launch();
			}
		}

		public void game_launch_fail(Exception e) {
			e.printStackTrace();
			JOptionPane.showMessageDialog(RoomPanel.this, e.toString());
		}

		public void game_launched() {
			// Ignore
		}

		public void game_negotiate_fail() {
			JOptionPane
					.showMessageDialog(RoomPanel.this, "game_negotiate_fail");
		}

		public void game_negotiated() {
			server.create_channel();
		}
	}

	private class NewTableAction extends PlayGameAction {
		private Table table;

		public Object getValue(String key) {
			if (NAME.equals(key)) {
				return messages.getString("RoomPanel.Button.NewGame");
			}
			return super.getValue(key);
		}

		public void actionPerformed(ActionEvent e) {
			if (Module.get_num_by_type(room.get_gametype()) == 0) {
				JOptionPane
						.showMessageDialog(
								(Component) e.getSource(),
								messages
										.getString("RoomPanel.Message.GameNotSupported"));
			} else {
				table = SeatAllocationDialog.getTableSeatAllocation(
						(Component) e.getSource(), room.get_gametype());
				if (table != null) {
					// The panel wasn't repainting between the dialog being
					// closed and the game window appearing. Forcing a repaint
					// and then launching the game afterwards seems to have
					// cured it.
					RoomPanel.this.repaint();
					SwingUtilities.invokeLater(new Runnable() {
						public void run() {
							Module module = Module.get_nth_by_type(room
									.get_gametype(), 0);
							Game game = new Game(server, module);
							game.add_event_hook(NewTableAction.this);
							game.launch();
						}
					});
				}
			}
		}

		public void game_playing() {
			try {
				room.launch_table(table);
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}
	}

	private class JoinTableAction extends PlayGameAction {

		public Object getValue(String key) {
			if (NAME.equals(key)) {
				return messages.getString("RoomPanel.Button.JoinGame");
			}
			return super.getValue(key);
		}

		public void game_playing() {
			try {
				room.join_table(tableTable.getSelectedRow(), false);
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}

	}

	private class SpectateAction extends PlayGameAction {

		public Object getValue(String key) {
			if (NAME.equals(key)) {
				return messages.getString("RoomPanel.Button.Spectate");
			}
			return super.getValue(key);
		}

		public void game_playing() {
			try {
				room.join_table(tableTable.getSelectedRow(), true);
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}

	}

	private class LogoutAction extends AbstractAction {

		public Object getValue(String key) {
			if (NAME.equals(key)) {
				return messages.getString("RoomPanel.Button.Logout");
			}
			return super.getValue(key);
		}

		public void actionPerformed(ActionEvent e) {
			try {
				server.logout();
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}
	}

	private class TablesTableModel extends AbstractTableModel {

		public String getColumnName(int columnIndex) {
			switch (columnIndex) {
			case 0:
				return messages
						.getString("RoomPanel.ColumnHeader.TableDescription");
			case 1:
				return messages
						.getString("RoomPanel.ColumnHeader.TablePlayers");
			case 2:
				return messages
						.getString("RoomPanel.ColumnHeader.TableSpectators");
			default:
				return null;
			}
		}

		public int getColumnCount() {
			return 3;
		}

		public int getRowCount() {
			return room == null ? 0 : room.get_num_tables();
		}

		public Object getValueAt(int rowIndex, int columnIndex) {
			try {
				Table table = room.get_nth_table(rowIndex);
				StringBuffer buffer;

				if (table == null)
					return null;

				switch (columnIndex) {
				case 0:
					buffer = new StringBuffer("<HTML><B>");
					buffer.append(MessageFormat.format(messages
							.getString("RoomPanel.TableLabel"),
							new Object[] { new Integer(table.get_id()) }));
					if (table.get_desc() != null) {
						buffer.append("</B><BR><I>");
						buffer.append(table.get_desc());
					}
					return buffer.toString();
				case 1:
					buffer = new StringBuffer("<HTML><OL>");
					for (int player_num = 0; player_num < table.get_num_seats(); player_num++) {
						buffer.append("<LI>");
						SeatType type = table.get_nth_player_type(player_num);
						if (type == SeatType.GGZ_SEAT_ABANDONED) {
							buffer.append(messages
									.getString("RoomPanel.SeatAbandoned"));
						} else if (type == SeatType.GGZ_SEAT_BOT) {
							buffer.append(messages
									.getString("RoomPanel.SeatBot"));
						} else if (type == SeatType.GGZ_SEAT_NONE
								|| type == SeatType.GGZ_SEAT_PLAYER) {
							buffer
									.append(table
											.get_nth_player_name(player_num));
						} else if (type == SeatType.GGZ_SEAT_OPEN) {
							buffer.append(messages
									.getString("RoomPanel.SeatOpen"));
						} else if (type == SeatType.GGZ_SEAT_RESERVED) {
							buffer.append(messages
									.getString("RoomPanel.SeatReserved"));
						}
					}
					buffer.append("</OL></HTML>");
					return buffer.toString();
				case 2:
					buffer = new StringBuffer("<HTML><OL>");
					for (int spectator_num = 0; spectator_num < table
							.get_num_spectator_seats(); spectator_num++) {
						buffer.append("<LI>");
						SeatType type = table
								.get_nth_spectator_seat(spectator_num).type;
						if (type == SeatType.GGZ_SEAT_ABANDONED) {
							buffer.append(messages
									.getString("RoomPanel.AbandonedSeat"));
							break;
						} else if (type == SeatType.GGZ_SEAT_BOT) {
							buffer.append(messages
									.getString("RoomPanel.ComputerPlayer"));
							break;
						} else if (type == SeatType.GGZ_SEAT_NONE
								|| type == SeatType.GGZ_SEAT_PLAYER) {
							buffer.append(table
									.get_nth_spectator_name(spectator_num));
							break;
						} else if (type == SeatType.GGZ_SEAT_OPEN) {
							buffer.append(messages
									.getString("RoomPanel.EmptySeat"));
							break;
						} else if (type == SeatType.GGZ_SEAT_RESERVED) {
							buffer.append(messages
									.getString("RoomPanel.ReservedSeat"));
							break;
						}
					}
					buffer.append("</OL></HTML>");
					return buffer.toString();
				default:
					return null;
				}
			} catch (IndexOutOfBoundsException ex) {
				// This can sometimes happen due to a race condition but
				// it's OK.
				return null;
			}
		}
	}
}
