/*
 * Copyright (C) 2006 Helg Bredow
 * 
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 * 
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

package ggz.ui;

import edu.stanford.ejalbert.BrowserLauncher;
import ggz.client.core.StateID;

import java.applet.Applet;
import java.applet.AppletContext;
import java.applet.AppletStub;
import java.applet.AudioClip;
import java.awt.BorderLayout;
import java.awt.Image;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.Properties;
import java.util.logging.Logger;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.WindowConstants;

/**
 * Simple main that creates a simulated environment to run the applet in. As
 * well as command line options, it supports all applet parameters via the
 * ggz-java.properties file in the current directory or the user's home
 * directory.
 * 
 * @author Helg.Bredow
 * 
 */
public class ClientApp {
    protected static final Logger log = Logger.getLogger(ClientApp.class
            .getName());

    protected static String xmlout;

    protected static String xmlin;

    protected static String uri;

    protected static String tls;

    protected static Properties appletParameters;

    private ClientApp() {
        super();
    }

    /**
     * @param args
     */
    public static void main(String[] args) throws Exception {
        // UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        if (!parseArgs(args)) {
            printUsage();
            return;
        }

        loadProperties();

        // Applet needs to be created first if we want antialiasing
        // since it sets it up in the static initialiser.
        final ClientApplet applet = new ClientApplet();
        final JFrame frame = new JFrame("GGZ Gaming Zone");
        final JLabel statusLabel = new JLabel();

        applet.setStub(new AppletStub() {
            public boolean isActive() {
                return true;
            }

            public URL getDocumentBase() {
                return null;
            }

            public URL getCodeBase() {
                try {
                    return new File(System.getProperty("user.dir")).toURL();
                } catch (MalformedURLException e) {
                    return null;
                }
            }

            public String getParameter(String name) {
                String value = null;
                if ("xmlout".equals(name)) {
                    value = xmlout;
                } else if ("xmlin".equals(name)) {
                    value = xmlin;
                } else if ("uri".equals(name)) {
                    value = uri;
                } else if ("tls".equals(name)) {
                    value = tls;
                }

                if (value == null && appletParameters != null) {
                    value = appletParameters.getProperty(name);
                }
                return value;
            }

            public AppletContext getAppletContext() {
                return new AppletContext() {

                    public Applet getApplet(String name) {
                        // TODO Auto-generated method stub
                        return null;
                    }

                    public Enumeration getApplets() {
                        // TODO Auto-generated method stub
                        return null;
                    }

                    public AudioClip getAudioClip(URL url) {
                        // TODO Auto-generated method stub
                        return null;
                    }

                    public Image getImage(URL url) {
                        // TODO Auto-generated method stub
                        return null;
                    }

                    public InputStream getStream(String key) {
                        // TODO Auto-generated method stub
                        return null;
                    }

                    public Iterator getStreamKeys() {
                        // TODO Auto-generated method stub
                        return null;
                    }

                    public void setStream(String key, InputStream stream) {
                        // TODO Auto-generated method stub
                    }

                    public void showDocument(URL url, String target) {
                        showDocument(url);
                    }

                    public void showDocument(URL url) {
                        try {
                            BrowserLauncher.openURL(url.toString());
                        } catch (IOException exception) {
                            exception.printStackTrace();
                        }
                    }

                    public void showStatus(String status) {
                        statusLabel.setText(status);
                    }

                };
            }

            public void appletResize(int width, int height) {
                frame.setSize(width, height);
            }
        });
        frame.getContentPane().add(applet, BorderLayout.CENTER);
        frame.getContentPane().add(statusLabel, BorderLayout.SOUTH);
        frame.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
        frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                if (applet.server != null) {
                    if (applet.server.get_state() == StateID.GGZ_STATE_JOINING_TABLE
                            || applet.server.get_state() == StateID.GGZ_STATE_LAUNCHING_TABLE
                            || applet.server.get_state() == StateID.GGZ_STATE_LEAVING_TABLE
                            || applet.server.get_state() == StateID.GGZ_STATE_AT_TABLE) {
                        JOptionPane
                                .showMessageDialog(frame,
                                        "You are currently playing a game. Please quit the game before exiting GGZ.");
                        return;
                    }
                }
                applet.stop();
                frame.dispose();
            }
        });
        frame.setSize(800, 600);
        frame.setIconImage(new ImageIcon(applet.getClass().getResource("/ggz/ui/images/ggz_logo_32x32.png")).getImage());
        applet.init();
        applet.start();
        applet.server_state_changed();

        // This must be the last line
        // <http://java.sun.com/products/jfc/tsc/articles/threads/threads1.html>.
        frame.setVisible(true);

    }

    private static boolean parseArgs(String[] argv) {
        try {
            for (int argPos = 0; argPos < argv.length; argPos++) {
                if ("-xmlout".equals(argv[argPos])) {
                    argPos++;
                    xmlout = argv[argPos];
                } else if ("-xmlin".equals(argv[argPos])) {
                    argPos++;
                    xmlin = argv[argPos];
                } else if ("-uri".equals(argv[argPos])) {
                    argPos++;
                    uri = argv[argPos];
                } else if ("-tls".equals(argv[argPos])) {
                    tls = "true";
                } else {
                    return false;
                }
            }
            return true;
        } catch (IndexOutOfBoundsException ex) {
            return false;
        }
    }

    protected static void loadProperties() {
        FileInputStream file = null;
        try {
            // Look in the current directory first.
            file = new FileInputStream("ggz-java.properties");
        } catch (FileNotFoundException e) {
            try {
                // Try the user's home dir next.
                file = new FileInputStream(new File(System
                        .getProperty("user.home"), "ggz-java.properties"));
            } catch (FileNotFoundException e2) {
                // Ignore, applet will simply use defaults.
            }
        }
        if (file != null) {
            try {
                appletParameters = new Properties();
                appletParameters.load(file);
            } catch (IOException e) {
                log.warning(e.getMessage());
            }
        }
    }

    private static void printUsage() {
        System.err.println("Usage: java ggz.ui.ClientApp [options]");
        System.err.println("Options:");
        System.err.println("  -uri <uri>      GGZ server to connect to.");
        System.err
                .println("  -xmlout <file>  Write XML sent to the server to this file.");
        System.err
                .println("  -xmlin <file>   Write XML received from the server to this file.");
        System.err.println("  -tls            Attempt to use transport layer security.");
        System.err.println();
        System.err
                .println("-xmlout and -xmlin can be the same file and the special values 'stderr' and 'stdout' can be used.");
    }
}
