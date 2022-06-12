/*
TODO
clean up code
Use proper C++
Think of better config file format
switch to https://github.com/dmikushin/tray
*/
#include <iostream>
#include <fstream>
#include <string.h>

#include "lib/systemdvars.h"
#include "lib/util.h"

#ifdef _WIN32
#define TRAY_WINAPI 1
#endif

#ifdef __linux__
#define TRAY_APPINDICATOR 1
#endif

#include "tray/tray.h"

#if TRAY_APPINDICATOR
#define TRAY_ICON "indicator-messages"
#elif TRAY_WINAPI
#define TRAY_ICON "icon.ico"
#endif





// GLOBALS
struct tray *g_tray;
char s_tray_icon[0x100];
char s_dash[0x100];
char s_preferences[0x100];
char s_preference_reboot[0x100];
char s_quit[0x100];
std::vector<std::string> g_entries{};
size_t g_selected_entry{0};
struct tray_menu *g_preferences_menu{NULL};
bool g_reboot_immediately{false};


void setup_strings() {
    strcpy(s_tray_icon, TRAY_ICON);
    strcpy(s_dash, "-");
    strcpy(s_preferences, "Preferences");
    strcpy(s_preference_reboot, "Reboot immediately after clicking entry");
    strcpy(s_quit, "Quit");
}


void select_entry_cb(struct tray_menu *item) {
    g_tray->menu[g_selected_entry].checked = 0;
    for (size_t i = 0; i < g_entries.size(); i++) {
        if (&g_tray->menu[i] == item) {
            g_selected_entry = i;
        }
    }
    g_tray->menu[g_selected_entry].checked = 1;

    // set systemd entry
    if (!setNextSystemdBootEntry(std::string(g_tray->menu[g_selected_entry].text))) {
        fprintf(stderr, "Failed to set next systemd-boot entry\n");
    } else {
        // reboot optionally
        if (g_reboot_immediately) {
            if(!reboot()) {
                fprintf(stderr, "Failed to reboot\n");
            }
        }
    }
	tray_update(g_tray);
}

void reboot_preference_cb(struct tray_menu *item) {
    g_reboot_immediately = !g_reboot_immediately;
    g_preferences_menu[0].checked = (int)g_reboot_immediately;
	tray_update(g_tray);
}

void quit_cb(struct tray_menu *item) {
  tray_exit();
}




void load_config() {
    std::ifstream conffile(get_config_file_path());
    if (conffile.is_open()) {
        conffile >> g_reboot_immediately;
        conffile.close();
    } else {
        g_reboot_immediately = false;
    }
}

void save_config() {
    std::ofstream conffile(get_config_file_path());
    if (!conffile.is_open()) {
        fprintf(stderr, "Failed to write config file\n");
    }
    conffile << g_reboot_immediately;
}

void setup_tray() {
    /*
    Setup preferences menu with entries:
     * Reboot Immediately?
     * NULL
    */
    g_preferences_menu = (struct tray_menu *)calloc(sizeof(struct tray_menu), 2);
    g_preferences_menu[0] = (struct tray_menu){.text = s_preference_reboot, .checked = (int)g_reboot_immediately, .cb = reboot_preference_cb};
    g_preferences_menu[1] = (struct tray_menu){.text = NULL};

    /*
    Setup main tray and menu with entries:
     * systemd_boot entry 1
     * ...
     * systemd_boot entry N
     * -
     * Preferences
     * -
     * Quit
     * NULL
    */
    g_entries = getSystemdBootEntries();
    if (g_entries.size() == 0) {
      std::cout << getLastErrorAsString() << std::endl;
      exit(1);
    }

    g_tray = (struct tray *)calloc(sizeof(struct tray), 1);
    if (!g_tray) {
        fprintf(stderr, "Memory error");
        exit(1);
    }
    g_tray->icon = s_tray_icon;

    // setup menu
    size_t num_items = g_entries.size() + 5;
    g_tray->menu = (struct tray_menu *)calloc(sizeof(struct tray_menu), num_items);

    for (size_t i = 0; i < g_entries.size(); i++) {
        g_tray->menu[i] = (struct tray_menu){g_entries[i].data(), 0, 0, select_entry_cb, NULL};
    }
    g_tray->menu[g_entries.size() + 0] = (struct tray_menu){.text = s_dash};
    g_tray->menu[g_entries.size() + 1] = (struct tray_menu){.text = s_preferences, .submenu = g_preferences_menu};
    g_tray->menu[g_entries.size() + 2] = (struct tray_menu){.text = s_dash};
    g_tray->menu[g_entries.size() + 3] = (struct tray_menu){.text = s_quit, .cb = quit_cb};
    g_tray->menu[g_entries.size() + 4] = (struct tray_menu){.text = NULL};
}

void destroy_tray() {
    free(g_preferences_menu);
    free(g_tray);
}



int main() {
    setup_strings();

    getPriviliges();
    load_config();

    setup_tray();

    tray_init(g_tray);
    while (tray_loop(1) == 0);
    tray_exit();

    destroy_tray();

    save_config();
    return 0;
}
