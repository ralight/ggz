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

import java.applet.Applet;
import java.applet.AppletContext;
import java.applet.AppletStub;
import java.applet.AudioClip;
import java.awt.BorderLayout;
import java.awt.Image;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.Properties;
import java.util.logging.Logger;

import javax.swing.JFrame;
import javax.swing.JLabel;

/**
 * Simply main that creates a simulated environment to run the applet in. As
 * well as command line options, it supports all applet parameters via the
 * appletparams.properties file in the current directory.
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
                return null;
            }

            public String getParameter(String name) {
                String value = null;
                if ("xmlout".equals(name)) {
                    value = xmlout;
                } else if ("xmlin".equals(name)) {
                    value = xmlin;
                } else if ("uri".equals(name)) {
                    value = uri;
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
        frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                applet.stop();
                frame.dispose();
            }
        });
        frame.setSize(640, 480);
        frame.setVisible(true);
        applet.init();
        applet.start();
        applet.server_state_changed();
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
        try {
            FileInputStream file = new FileInputStream(
                    "appletparams.properties");
            appletParameters = new Properties();
            appletParameters.load(file);
        } catch (FileNotFoundException e) {
            // Ignore, applet will simply use defaults.
        } catch (IOException e) {
            log.warning(e.getMessage());
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
        System.err.println();
        System.err
                .println("-xmlout and -xmlin can be the same file and the special values 'stderr' and 'stdout' can be used.");
    }
}
