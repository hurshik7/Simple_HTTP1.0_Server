#include "option_handler.h"
#include "server.h"
#include "ncurses_ui.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define ONE_MILLISECOND (1000)


volatile bool server_running;
pthread_t server_thread;
pthread_mutex_t screen_lock;


int main(int argc, char* argv[])
{
    // initiate options, and parse command line argument
    struct options opts;
    options_init(&opts);
    if (parse_arguments(argc, argv, &opts) == EXIT_FAILURE) {
        exit(EXIT_FAILURE);                                              // NOLINT(concurrency-mt-unsafe)
    }

    if (signal(SIGCHLD, (void (*)(int)) reap) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);                                             // NOLINT(concurrency-mt-unsafe)
    }

    init_ncurses();
    WINDOW* menu_win = create_menu_window();
    keypad(menu_win, TRUE);
    int choice;
    bool quit = false;

    pthread_mutex_init(&screen_lock, NULL);
    int startx = (COLS - 18) / 2;

    // Display menu
    while (!quit) {
        print_title(startx);
        choice = navigate_menu(menu_win);

        switch (choice) {
            case 1:
                if (!server_running) {
                    server_running = true;
                    pthread_create(&server_thread, NULL, run_server_thread, (void *) &opts);
                    while (server_running) {
                        int ch = getch();
                        if (ch == 'q') {
                            server_running = false;
                        }
                        usleep(ONE_MILLISECOND); // Sleep for 1ms to avoid busy waiting
                    }
                    pthread_join(server_thread, NULL);
                }
                break;
            case 2:
                view_database_wrapper();
                break;
            case 3:
                quit = true;
                break;
            default:
                break;
        }

    }

    // Clean up
    pthread_mutex_destroy(&screen_lock);
    clrtoeol();
    refresh();
    endwin();
    return 0;
}

