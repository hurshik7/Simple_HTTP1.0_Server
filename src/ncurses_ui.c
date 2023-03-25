#include "ncurses_ui.h"
#include <pthread.h>


extern bool server_running;
extern pthread_t server_thread;
extern pthread_mutex_t screen_lock;


void print_menu(WINDOW *menu_win, int highlight)
{
    int x, y, i;
    char *choices[] = {
            "1. Run server",
            "2. View database",
            "3. Quit",
    };

    x = 2;
    y = 2;

    box(menu_win, 0, 0);

    for (i = 0; i < 3; i++) {
        if (highlight == i + 1) {
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        } else {
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        }
        y++;
    }

    y++;
    mvwprintw(menu_win, y, x, "Created by Shik Hur, Benny Chao (BCIT CST COMP4981 Project)");
    wrefresh(menu_win);
}

WINDOW* create_menu_window(void)
{
    int menu_height = 9;
    int menu_width = 70;
    int menu_startx = (COLS - menu_width) / 2;
    int menu_starty = (LINES - menu_height) / 2;
    WINDOW *menu_win = newwin(menu_height, menu_width, menu_starty, menu_startx);
    return menu_win;
}

int navigate_menu(WINDOW *menu_win)
{
    int choice = 1;
    int key;

    while (1) {
        print_menu(menu_win, choice);

        key = wgetch(menu_win);
        switch (key) {
            case KEY_UP:
                choice--;
                if (choice < 1) {
                    choice = 3;
                }
                break;
            case KEY_DOWN:
                choice++;
                if (choice > 3) {
                    choice = 1;
                }
                break;
            case 10:  // Enter key
                return choice;
            default:
                break;
        }
    }
}

void print_title(int startx)
{
    attron(A_BOLD);
    mvprintw(1, startx, "Simple HTTP Server");
    attroff(A_BOLD);
    refresh();
}

void init_ncurses(void)
{
    // Initialize ncurses
    initscr();
    clear();
    noecho();
    cbreak();
}

void view_database_wrapper(void)
{
    // Clear the screen
    clear();
    refresh();

    // Display database contents
    printw("Database contents:\n");
    // Print your database contents here
    printw("Example database content...\n");

    // Refresh the screen to show the database contents
    refresh();

    // Wait for a key press to close the database view
    getch();

    // Clear the screen again
    clear();
    refresh();
}
