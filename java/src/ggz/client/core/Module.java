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
package ggz.client.core;

import java.util.ArrayList;

public class Module {

    /* Name of module */
    private String name;

    /* Game module version */
    private String version;

    /* Protocol engine implemented */
    private String prot_engine;

    /* Protocol version implemented */
    private String prot_version;

    /* Supported games */
    // private String[] games;
    /* Module author */
    private String author;

    /* Native frontend */
    private String frontend;

    /* Homepage for this module */
    private String url;

    /* Commandline for executing module */
    private String class_name;
    
    /* Present working directory for external modules */
    private String pwd;

    /* Path to icon for this game module */
    private String icon;

    /* Path to help file */
    private String help;

    /* Preferred runtime environment */
    private ModuleEnvironment environment;

    /* List of modules */
    private static ArrayList module_list = new ArrayList(1);

    // private static int mod_handle = -1;

    private boolean is_embedded;

    static {
        // We only support GGZCards modules at the moment.
        add(new Module("GGZCards", "0.0.7", "GGZCards", "15", "Author", "Java",
                "url", "ggz.cards.CardGamePanel", null,"/ggz/ui/images/euchre.gif",
                "help_path", true));
        add(new Module("GGZCards-Spades", "0.0.7", "GGZCards", "15", "Author",
                "Java", "url", "ggz.cards.CardGamePanel",null,
                "/ggz/ui/images/spades.gif", "help_path", true));

        // For oojah's server
        add(new Module("GGZCards-Spades-Rated", "0.0.7", "GGZCards", "15",
                "Author", "Java", "url", "ggz.cards.CardGamePanel",null,
                "/ggz/ui/images/spades.gif", "help_path", true));

        add(new Module("GGZCards-LaPocha", "0.0.7", "GGZCards", "15", "Author",
                "Java", "url", "ggz.cards.CardGamePanel",null,
                "/ggz/ui/images/pocha.gif", "help_path", true));
        // 42 is played with Dominoes and is not supported.
        // _add(new Module("GGZCards-FortyTwo", "0.0.7", "GGZCards", "15",
        // "Author",
        // "Java", "url", "ggz.cards.CardGamePanel", "/ggz/ui/images/42.gif",
        // "help_path"));
        add(new Module("GGZCards-Bridge", "0.0.7", "GGZCards", "15", "Author",
                "Java", "url", "ggz.cards.CardGamePanel",null,
                "/ggz/ui/images/bridge.gif", "help_path", true));
        add(new Module("GGZCards-Sueca", "0.0.7", "GGZCards", "15", "Author",
                "Java", "url", "ggz.cards.CardGamePanel",null,
                "/ggz/ui/images/sueca.gif", "help_path", true));
        // Suaro uses an unknown card set...
        add(new Module("GGZCards-Suaro", "0.0.7", "GGZCards", "15", "Author",
                "Java", "url", "ggz.cards.CardGamePanel",null,
                "/ggz/ui/images/suaro.gif", "help_path", true));
        add(new Module("GGZCards-Whist", "0.0.7", "GGZCards", "15", "Author",
                "Java", "url", "ggz.cards.CardGamePanel",null,
                "/ggz/ui/images/whist.gif", "help_path", true));
        add(new Module("GGZCards-Hearts", "0.0.7", "GGZCards", "15", "Author",
                "Java", "url", "ggz.cards.CardGamePanel",null,
                "/ggz/ui/images/hearts.gif", "help_path", true));
        add(new Module("GGZCards-Euchre", "0.0.7", "GGZCards", "15", "Author",
                "Java", "url", "ggz.cards.CardGamePanel", null,
                "/ggz/ui/images/euchre.gif", "help_path", true));

        // Test for non-embedded game client.
//        add(new Module(
//                "GGZCards-Spades",
//                "0.0.7",
//                "GGZCards",
//                "15",
//                "Author",
//                "Java",
//                "url",
//                "C:\\Program Files\\GGZ Gaming Zone\\lib\\ggz\\ggz.ggzcards.exe",
//                "C:\\Program Files\\GGZ Gaming Zone\\bin",
//                "/ggz/ui/images/spades.gif", "help_path", false));
    }

    /* Publicly exported functions */

    /* This returns the number of registered modules */
    public static int get_num_modules() {
        return module_list == null ? 0 : module_list.size();
    }

    public static int get_num_by_type(GameType type) {
        return get_num_by_type(type.get_name(), type.get_prot_engine(), type
                .get_prot_version());
    }

    /* Returns how many modules support this game and protocol */
    public static int get_num_by_type(String game, String engine, String version) {
        /* A NULL version means any version */
        if (engine == null)
            return -1;

        int numcount = module_list.size();
        for (int i = 0; i < module_list.size(); i++) {
            Module module = (Module) module_list.get(i);
            if (!module.prot_engine.equals(engine)
                    || !module.prot_version.equals(version)
                    || !module.get_name().equals(game)) {
                // game not included in game list

                numcount--;
            }
        }
        // int count, status, i, numcount;
        // String[] ids;
        // Module module;
        //
        // /* Get total count for this engine (regardless of version) */
        // status =
        // ggz_conf_read_list(mod_handle, "Games", engine, &count, &ids);
        //
        // if (status < 0)
        // return 0;
        //
        // numcount = count;
        // for (i = 0; i < count; i++) {
        // _read(&module, ids[i]);
        // /* Subtract out modules that aren't the same protocol */
        // if (ggz_strcmp(engine, module.prot_engine) != 0
        // || (version
        // && ggz_strcmp(version, module.prot_version) != 0)
        // /* || game not included in game list */
        // )
        // numcount--;
        // }

        return numcount;
    }

    public static Module get_nth_by_type(GameType type, int index) {
        return get_nth_by_type(type.get_name(), type.get_prot_engine(), type
                .get_prot_version(), index);

    }

    /* Returns n-th module that supports this game and protocol */
    public static Module get_nth_by_type(String game, String engine,
            String version, int num) {
        if (game == null || engine == null || version == null)
            return null;

        Module search = new Module(game, engine, version, "Java");
        int index = module_list.indexOf(search);
        return index > -1 ? (Module) module_list.get(index) : null;
        // int i, total, status, count;
        // char **ids;
        // GGZModule *module, *found = NULL;
        // GGZListEntry *entry;
        //
        // status =
        // ggz_conf_read_list(mod_handle, "Games", engine, &total, &ids);
        //
        // ggz_debug(GGZCORE_DBG_MODULE, "Found %d modules matching %s",
        // total, engine);
        //
        // if (status < 0)
        // return NULL;
        //
        // if (num >= total) {
        // _ggz_free_chars(ids);
        // return NULL;
        // }
        //
        // count = 0;
        // for (i = 0; i < total; i++) {
        // module = _new();
        // _read(module, ids[i]);
        // if (ggz_strcmp(version, this.prot_version) == 0) {
        // /* FIXME: also check to see if 'game' is in supported list */
        // if (count++ == num) {
        // /* Now find same module in list */
        // entry =
        // ggz_list_search(module_list, module);
        // found = ggz_list_get_data(entry);
        // _free(module);
        // break;
        // }
        // }
        // _free(module);
        // }
        //
        // /* Free the rest of the ggz_conf memory */
        // _ggz_free_chars(ids);
        //
        //    
        // /* Return found module (if any) */
        // return found;
    }

    /*
     * These functions lookup a particular property of a module. I've added icon
     * to the list we discussed at the meeting. This is an optional xpm file
     * that the module can provide to use for representing the game graphically.
     */
    public String get_name() {
        return this.name;
    }

    public String get_version() {
        return this.version;
    }

    public String get_prot_engine() {
        return this.prot_engine;
    }

    public String get_prot_version() {
        return this.prot_version;
    }

    public String get_author() {
        return this.author;
    }

    public String get_frontend() {
        return this.frontend;
    }

    public String get_url() {
        return this.url;
    }

    public String get_icon_path() {
        return this.icon;
    }

    public String get_help_path() {
        return this.help;
    }

    public String get_class_name() {
        return is_embedded ? this.class_name : null;
    }

    public String get_cmd() {
        return is_embedded ? null : this.class_name;
    }

    public String get_pwd() {
        return is_embedded ? null : this.pwd;
    }

    public ModuleEnvironment get_environment() {
        return this.environment;
    }

    /* Internal library functions (prototypes in module.h) */

    /*
     * _setup() Opens the global game module file
     * 
     * Returns: -1 on error 0 on success
     */
    // int _setup()
    // {
    // char *file;
    // char **ids;
    // char **games;
    // int i, j, count_types, count_modules, status;
    // GGZModule *module;
    //
    // if (mod_handle != -1) {
    // ggz_debug(GGZCORE_DBG_MODULE,
    // "module_setup() called twice");
    // return -1;
    // }
    //
    //
    // module_list = ggz_list_create(_compare, NULL,
    // _destroy, 0);
    // num_modules = 0;
    //
    // file = _conf_filename();
    // ggz_debug(GGZCORE_DBG_MODULE, "Reading %s", file);
    // mod_handle = ggz_conf_parse(file, GGZ_CONF_RDONLY);
    // /* Free up space taken by name */
    // ggz_free(file);
    //
    // if (mod_handle == -1) {
    // ggz_debug(GGZCORE_DBG_MODULE,
    // "Unable to load module conffile");
    // return -1;
    // }
    //
    // /* Read in list of supported gametypes */
    // status = ggz_conf_read_list(mod_handle, "Games", "*Engines*",
    // &count_types, &games);
    // if (status < 0) {
    // ggz_debug(GGZCORE_DBG_MODULE, "Couldn't read engine list");
    // return -1;
    // }
    // ggz_debug(GGZCORE_DBG_MODULE,
    // "%d game engines supported", count_types);
    //
    // for (i = 0; i < count_types; i++) {
    // status = ggz_conf_read_list(mod_handle, "Games", games[i],
    // &count_modules, &ids);
    //
    //
    // ggz_debug(GGZCORE_DBG_MODULE,
    // "%d modules for %s", count_modules, games[i]);
    //
    // for (j = 0; j < count_modules; j++) {
    // module = _new();
    // _read(module, ids[j]);
    // _add(module);
    // ggz_debug(GGZCORE_DBG_MODULE, "Module %d: %s", j,
    // ids[j]);
    //
    // }
    //
    // _ggz_free_chars(ids);
    // }
    //
    // _ggz_free_chars(games);
    //
    // _list_print();
    //
    // return 0;
    // }
    public boolean is_embedded() {
        return is_embedded;
    }

    public Module(String name, String version, String prot_engine,
            String prot_version, String author, String frontend, String url,
            String class_name, String pwd, String icon_path, String help_path,
            boolean is_embedded) {
        this.name = name;
        this.version = version;
        this.prot_engine = prot_engine;
        this.prot_version = prot_version;
        this.author = author;
        this.frontend = frontend;
        this.url = url;
        /* this.path = exec_path; */
        this.class_name = class_name;
        this.pwd = pwd;
        this.icon = icon_path;
        this.help = help_path;
        this.is_embedded = is_embedded;
    }

    private Module(String name, String prot_engine, String prot_version,
            String frontend) {
        this.name = name;
        this.prot_engine = prot_engine;
        this.prot_version = prot_version;
        this.frontend = frontend;
    }

    private static void add(Module module) {
        module_list.add(module);
    }

    // static char *_conf_filename()
    // {
    // char *filename;
    // int new_len;
    //
    // /* Allow for extra slash and newline when concatenating */
    // new_len = strlen(GGZCONFDIR) + strlen(GGZ_MOD_RC) + 2;
    // filename = ggz_malloc(new_len);
    //
    // strcpy(filename, GGZCONFDIR);
    // strcat(filename, "/");
    // strcat(filename, GGZ_MOD_RC);
    //
    // return filename;
    // }

    // static void _read(GGZModule * mod, char *id)
    // {
    // int argc;
    // char *environment;
    // /* FIXME: check for errors on all of these */
    //
    // /* Note: the memory allocated here is freed in _free */
    // mod->name = ggz_conf_read_string(mod_handle, id, "Name", NULL);
    // mod->version =
    // ggz_conf_read_string(mod_handle, id, "Version", NULL);
    // mod->prot_engine =
    // ggz_conf_read_string(mod_handle, id, "ProtocolEngine", NULL);
    // mod->prot_version =
    // ggz_conf_read_string(mod_handle, id, "ProtocolVersion", NULL);
    // ggz_conf_read_list(mod_handle, id, "SupportedGames", &argc,
    // &mod->games);
    // mod->author = ggz_conf_read_string(mod_handle, id, "Author", NULL);
    // mod->frontend =
    // ggz_conf_read_string(mod_handle, id, "Frontend", NULL);
    // mod->url = ggz_conf_read_string(mod_handle, id, "Homepage", NULL);
    // ggz_conf_read_list(mod_handle, id, "CommandLine", &argc,
    // &mod->argv);
    // mod->icon = ggz_conf_read_string(mod_handle, id, "IconPath", NULL);
    // mod->help = ggz_conf_read_string(mod_handle, id, "HelpPath", NULL);
    //
    // environment =
    // ggz_conf_read_string(mod_handle, id, "Environment", NULL);
    // if (!environment)
    // mod->environment = GGZ_ENVIRONMENT_XWINDOW;
    // else if (!ggz_strcmp(environment, "xwindow"))
    // mod->environment = GGZ_ENVIRONMENT_XWINDOW;
    // else if (!ggz_strcmp(environment, "xfullscreen"))
    // mod->environment = GGZ_ENVIRONMENT_XFULLSCREEN;
    // else if (!ggz_strcmp(environment, "passive"))
    // mod->environment = GGZ_ENVIRONMENT_PASSIVE;
    // else if (!ggz_strcmp(environment, "console"))
    // mod->environment = GGZ_ENVIRONMENT_CONSOLE;
    // else
    // mod->environment = GGZ_ENVIRONMENT_XWINDOW;
    // if (environment)
    // ggz_free(environment);
    // }

    // static void _print( )
    // {
    // int i = 0;
    //
    // ggz_debug(GGZCORE_DBG_MODULE, "Name: %s", this.name);
    // ggz_debug(GGZCORE_DBG_MODULE, "Version: %s", this.version);
    // ggz_debug(GGZCORE_DBG_MODULE, "ProtocolEngine: %s",
    // this.prot_engine);
    // ggz_debug(GGZCORE_DBG_MODULE, "ProtocolVersion: %s",
    // this.prot_version);
    // if (this.games)
    // while (this.games[i]) {
    // ggz_debug(GGZCORE_DBG_MODULE, "Game[%d]: %s", i,
    // this.games[i]);
    // ++i;
    // }
    //
    // ggz_debug(GGZCORE_DBG_MODULE, "Author: %s", this.author);
    // ggz_debug(GGZCORE_DBG_MODULE, "Frontend: %s", this.frontend);
    // ggz_debug(GGZCORE_DBG_MODULE, "URL: %s", this.url);
    // ggz_debug(GGZCORE_DBG_MODULE, "Icon: %s", this.icon);
    // ggz_debug(GGZCORE_DBG_MODULE, "Help: %s", this.help);
    // while (this.argv && this.argv[i]) {
    // ggz_debug(GGZCORE_DBG_MODULE, "Argv[%d]: %s", i,
    // this.argv[i]);
    // ++i;
    // }
    // }

    // static void _list_print()
    // {
    // GGZListEntry *cur;
    //
    // for (cur = ggz_list_head(module_list); cur;
    // cur = ggz_list_next(cur))
    // _print(ggz_list_get_data(cur));
    // }

    /* Match game module by 'name', 'prot_engine', 'prot_version' */
    // static int _compare( void *p, void *q)
    // {
    // int compare;
    // GGZModule *pmod = p, *qmod = q;
    //
    // compare = ggz_strcmp(pmod->name, qmod->name);
    // if (compare != 0)
    // return compare;
    //
    // compare = ggz_strcmp(pmod->prot_engine, qmod->prot_engine);
    // if (compare != 0)
    // return compare;
    //
    // compare = ggz_strcmp(pmod->prot_version, qmod->prot_version);
    // if (compare != 0)
    // return compare;
    //
    // compare = ggz_strcmp(pmod->frontend, qmod->frontend);
    //
    // return compare;
    // }
    public boolean equals(Object o) {
        boolean equals;
        Module module2 = (Module) o;

        equals = (o != null) && (o instanceof Module);
        if (!equals)
            return false;

        equals = name.equals(module2.name);
        if (!equals)
            return false;

        equals = prot_engine.equals(module2.prot_engine);
        if (!equals)
            return false;

        equals = prot_version.equals(module2.prot_version);
        if (!equals)
            return false;

        equals = frontend.equals(module2.frontend);

        return equals;
    }

    /**
     * This is here only because the Java API says we should implement
     * hashCode() if we implement equals().
     */
    public int hashCode() {
        return this.name.hashCode();
    }
}
