package ggz.client.mod;

public class Launcher {

    /**
     * @param args 0 - name of the game class to launch.
     */
    public static void main(String[] args) {
        if (args.length < 1) {
            System.err
                    .println("Usage: java -jar ggz-java-client.jar classname");
            System.exit(-1);
        }
        String className = args[0];

        try {
            Class c = Class.forName(className);
            ModEventHandler game = (ModEventHandler) c.newInstance();
            ModGame mod = new ModGame();
            game.init(mod);
            mod.connect();
        } catch (Throwable e) {
            System.err.print("Failed to launch game.");
            e.printStackTrace();
            System.exit(-1);
        }
    }

}
