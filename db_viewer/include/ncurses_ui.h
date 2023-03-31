#ifndef HTTP_SERVER_NCURSES_UI_H
#define HTTP_SERVER_NCURSES_UI_H


#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>


#define TITLE "HTTP Server DB Viewer (POST requests)"


/**
 * Prints a menu.
 * @param menu_win a WINDOW pointer
 * @param highlight an integer
 */
void print_menu(WINDOW* menu_win, int highlight);

/**
 * Create a menu window.
 * @return a WINDOW pointer
 */
WINDOW* create_menu_window(void);

/**
 * Get an user's choice.
 * @param menu_win a WINDOW pointer
 * @return an integer represents the user's choice
 */
int navigate_menu(WINDOW* menu_win);

/**
 * Prints the title on the ncurses window
 * @param startx an integer represents the start position of the x
 */
void print_title(int startx);

/**
 * Initiate ncurses ui.
 */
void init_ncurses(void);

/**
 * A wrapper function for print_post_request_data()
 * @param path a string represents the path of the db file
 */
void view_database_wrapper(char* path);

/**
 * Prints a box in a window.
 * @param box_win WINDOW pointer
 * @param highlight an integer
 */
void print_box_choices(WINDOW *box_win, int highlight);

/**
 * Get an user's choice, 1. Refresh or 2. Back.
 * @param box_win a WINDOW pointer
 * @return an integer represents the user's choice
 */
int navigate_box(WINDOW *box_win);

/**
 * Create a box window.
 * @return a WINDOW pointer
 */
WINDOW* create_box_window(void);


#endif //HTTP_SERVER_NCURSES_UI_H

