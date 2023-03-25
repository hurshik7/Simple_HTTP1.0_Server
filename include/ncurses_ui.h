#ifndef HTTP_SERVER_NCURSES_UI_H
#define HTTP_SERVER_NCURSES_UI_H


#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>


void print_menu(WINDOW* menu_win, int highlight);
WINDOW* create_menu_window(void);
int navigate_menu(WINDOW* menu_win);
void print_title(int startx);
void init_ncurses(void);
void view_database_wrapper(void);


#endif //HTTP_SERVER_NCURSES_UI_H

