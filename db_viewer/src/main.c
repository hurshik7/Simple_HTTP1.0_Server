#include "ncurses_ui.h"
#include "my_ndbm.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


int main(void)
{
    char path[PATH_MAX] = { '\0', };
    char* cwd = getcwd(path, PATH_MAX);
    if (cwd == NULL) {
        perror("getcwd");
    }
    char* ptr = strstr(path, "db_viewer");
    ptr += strlen("db_viewer");
    *ptr = '/';
    *(++ptr) = '\0';
    strcat(path, "db/");
    strcat(path, DB_NAME);

    init_ncurses();
    WINDOW* menu_win = create_menu_window();
    keypad(menu_win, TRUE);
    int choice;
    bool quit = false;

    int startx = (COLS - (int) strlen(TITLE)) / 2;

    // Display menu
    while (!quit) {
        print_title(startx);
        choice = navigate_menu(menu_win);

        switch (choice) {
            case 1:
                view_database_wrapper(path);
                break;
            case 2:
                quit = true;
                break;
            default:
                break;
        }

    }

    // Clean up
    clrtoeol();
    refresh();
    endwin();
    return 0;
}
