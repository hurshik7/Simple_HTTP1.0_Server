#ifndef HTTP_SERVER_NCURSES_UI_H
#define HTTP_SERVER_NCURSES_UI_H


#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>


#define TITLE "HTTP Server DB Viewer (POST requests)"


void print_menu(WINDOW* menu_win, int highlight);
WINDOW* create_menu_window(void);
int navigate_menu(WINDOW* menu_win);
void print_title(int startx);
void init_ncurses(void);
void view_database_wrapper(const char* path);


#endif //HTTP_SERVER_NCURSES_UI_H

