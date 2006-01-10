package ggz.ui;

import java.applet.Applet;
import java.applet.AppletContext;
import java.applet.AppletStub;
import java.applet.AudioClip;
import java.awt.BorderLayout;
import java.awt.Image;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.InputStream;
import java.net.URL;
import java.util.Enumeration;
import java.util.Iterator;

import javax.swing.JFrame;
import javax.swing.JLabel;

public class ClientApp {

    private ClientApp() {
        super();
    }

    /**
     * @param args
     */
    public static void main(String[] args) throws Exception {
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
                return null;
            }

            public AppletContext getAppletContext() {
                return new AppletContext() {

                    public Applet getApplet(String name) {
                        // TODO Auto-generated method stub
                        return null;
                    }

                    public Enumeration<Applet> getApplets() {
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

                    public Iterator<String> getStreamKeys() {
                        // TODO Auto-generated method stub
                        return null;
                    }

                    public void setStream(String key, InputStream stream) {
                        // TODO Auto-generated method stub

                    }

                    public void showDocument(URL url, String target) {
                        // TODO Auto-generated method stub

                    }

                    public void showDocument(URL url) {
                        // TODO Auto-generated method stub

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
}
