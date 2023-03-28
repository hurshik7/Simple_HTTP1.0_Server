#include "ncurses_ui.h"
#include "my_ndbm.h"


void print_menu(WINDOW *menu_win, int highlight)
{
    int x, y, i;
    const char *choices[] = {
            "1. View database",
            "2. Quit",
    };

    x = 2;
    y = 2;

    box(menu_win, 0, 0);

    for (i = 0; i < 2; i++) {
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
                    choice = 2;
                }
                break;
            case KEY_DOWN:
                choice++;
                if (choice > 2) {
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
    mvprintw(1, startx, TITLE);
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

void view_database_wrapper(char* path)
{
    clear();
    refresh();

    printw("Database contents:\n\n");
    refresh();

    DBM *db = open_post_request_db(path);
    if (db != NULL) {
        print_post_request_data(db);
    }

    WINDOW* box_win = create_box_window();
    keypad(box_win, TRUE);
    int box_choice;
    while (1) {
        box_choice = navigate_box(box_win);
        if (box_choice == 1) { // Refresh
            dbm_close(db);
            clear();
            refresh();
            db = open_post_request_db(path);
            if (db != NULL) {
                print_post_request_data(db);
            }
        } else if (box_choice == 2) { // Back
            break;
        }
    }

    delwin(box_win);
    dbm_close(db);
    getch();
    clear();
    refresh();
}

void print_box_choices(WINDOW *box_win, int highlight)
{
    static const char *box_choices[] = {
            "1. Refresh",
            "2. Back",
    };

    int x, y, i;

    x = 2;
    y = 1;

    box(box_win, 0, 0);

    for (i = 0; i < 2; i++) {
        if (highlight == i + 1) {
            wattron(box_win, A_REVERSE);
            mvwprintw(box_win, y, x, "%s", box_choices[i]);
            wattroff(box_win, A_REVERSE);
        } else {
            mvwprintw(box_win, y, x, "%s", box_choices[i]);
        }
        y++;
    }

    wrefresh(box_win);
}

int navigate_box(WINDOW *box_win)
{
    int choice = 1;
    int key;

    while (1) {
        print_box_choices(box_win, choice);
        key = wgetch(box_win);
        switch (key) {
            case KEY_UP:
                choice--;
                if (choice < 1) {
                    choice = 2;
                }
                break;
            case KEY_DOWN:
                choice++;
                if (choice > 2) {
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

WINDOW* create_box_window(void)
{
    int box_height = 4;
    int box_width = 16;
    int box_startx = (COLS - box_width) / 2;
    int box_starty = LINES - (LINES * 10 / 100) - 2;
    WINDOW *box_win = newwin(box_height, box_width, box_starty, box_startx);
    return box_win;
}

