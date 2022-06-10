#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>
#include <ncurses.h>
#include <string.h>
#include <signal.h>

// srandom(time(NULL));

int height, width, real_width;

void life(int grid[height][width], int new_grid[height][width]);

void randomize_grid(int grid[height][width]);
void zero_grid(int grid[height][width]);
void zero_grid_borders(int grid[height][width]);
void print_grid(int grid[height][width]);

void superimpose_pattern(int grid[height][width], int pattern_height, int pattern_width, int pattern[pattern_height][pattern_width]);
void initialize_grid(int grid[height][width]);
char* menu();
void print_greeting();

int main() {
    
    // for printing unicode characters
    setlocale(LC_ALL, "");   

    initscr();
    noecho();

    // hide cursor
    curs_set(0);

    // enable arrow keys
    keypad(stdscr, true);

    getmaxyx(stdscr, height, real_width);

    // to account for borders
    height -= 2;
    real_width -= 2;

    width = real_width / 2;

    // 2 grids to alternate between them
    int grid[height][width];
    int alt_grid[height][width];

    zero_grid_borders(grid);
    zero_grid_borders(alt_grid);

    int input;
    unsigned long counter = 0;


    while (1) {

        border(0, 0, 0, 0, 0, 0, 0, 0);

        initialize_grid(grid);

        // so that getch doesn't pause and wait for an input
        nodelay(stdscr, true);

        // print the initial grid first
        mvaddch(0, 1, '0');
        print_grid(grid);
        usleep(500000);

        counter = 0;

        while (1) {

            input = getch();

            // go back to the main menu
            if (input == 'b') {
                clear();
                break;
            }

            switch (input) {

                case 'q':
                    endwin();
                    exit(0);

                default:
                    break;
            }

            if (counter % 2 == 0) {
                
                life(grid, alt_grid);
                print_grid(alt_grid);
            }
            else {

                life(alt_grid, grid);
                print_grid(grid);
            }

            counter++;
            mvprintw(0, 1, "%d", counter);

            usleep(500000);
        }
    }
}

void life(int grid[height][width], int new_grid[height][width]) {

    int ones_count;

    for (int row = 1; row < height - 1; row++) {
        
        for (int col = 1; col < width - 1; col++) {

            // adding up the values surrounding the element
            ones_count += grid[row-1][col-1] + grid[row-1][col] + grid[row-1][col+1];
            ones_count += grid[row][col-1] + grid[row][col+1];
            ones_count += grid[row+1][col-1] + grid[row+1][col] + grid[row+1][col+1];

            if (grid[row][col] == 1) {

                if (ones_count == 2 || ones_count == 3)
                    new_grid[row][col] = 1;
                else
                    new_grid[row][col] = 0;
            }
            else {

                if (ones_count == 3)
                    new_grid[row][col] = 1;
                else
                    new_grid[row][col] = 0;
            }
            
            ones_count = 0;
        }
    }
}

void randomize_grid(int grid[height][width]) {

    for (int row = 1; row < height - 1; row++)
        for (int col = 1; col < width - 1; col++)
            grid[row][col] = random() % 2;
}

// prints the grid with some nice formatting
void print_grid(int grid[height][width]) {

    for (int row = 0; row < height; row++)
        for (int col = 0; col < width; col++)
            mvprintw(1 + row, 1 + 2*col, "%s", grid[row][col] ? "██" : "  ");

    refresh();
}

// fills the grid's borders with zeroes
void zero_grid_borders(int grid[height][width]) {

    // top & bottom borders
    for (int col = 0; col < width; col++) {

        grid[0][col] = 0;
        grid[height - 1][col] = 0;
    }

    // left & right borders
    for (int row = 0; row < height; row++) {

        grid[row][0] = 0;
        grid[row][width - 1] = 0;
    }
}

void zero_grid(int grid[height][width]) {

    for (int row = 0; row < height; row++)
        for (int col = 0; col < width; col++)
            grid[row][col] = 0;
}

void initialize_grid(int grid[height][width]) {

    char* options[] = {"Random", "10 Cell Line", "Glider", "xkcd's RIP Conway"};
    int options_num = sizeof(options) / sizeof(options[0]);

    char* selection = menu(options, options_num);
    if (strcmp(selection, "Random") == 0) {

        randomize_grid(grid);
        return;
    }
    else {

        zero_grid(grid);

        if (strcmp(selection, "10 Cell Line") == 0) {

            int pattern[1][10] = {
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            };

            superimpose_pattern(grid, 1, 10, pattern);
        }
        else if (strcmp(selection, "Glider") == 0) {

            int pattern[3][3] = {
                {0, 0, 1},
                {1, 0, 1},
                {0, 1, 1}
            };

            superimpose_pattern(grid, 3, 3, pattern);
        }
        else if (strcmp(selection, "xkcd's RIP Conway") == 0) {

            int pattern[9][7] = {
                {0, 0, 1, 1, 1, 0, 0},
                {0, 0, 1, 0, 1, 0, 0},
                {0, 0, 1, 0, 1, 0, 0},
                {0, 0, 0, 1, 0, 0, 0},
                {1, 0, 1, 1, 1, 0, 0},
                {0, 1, 0, 1, 0, 1, 0},
                {0, 0, 0, 1, 0, 0, 1},
                {0, 0, 1, 0, 1, 0, 0},
                {0, 0, 1, 0, 1, 0, 0}
            };

            superimpose_pattern(grid, 9, 7, pattern);
        }

    }
}

void superimpose_pattern(int grid[height][width], int pattern_height, int pattern_width, int pattern[pattern_height][pattern_width]) {

        for (int row = 0; row < pattern_height; row++) {
            for (int col = 0; col < pattern_width; col++) {

                int y_pos = ((height - pattern_height) / 2) + row;
                int x_pos = ((width - pattern_width) / 2) + col;

                grid[y_pos][x_pos] = pattern[row][col];
            }
        }
}

char* menu(char* options[], int options_num) {

    print_greeting();

    int choice = 0;
    int input;

    int y_pos, x_pos;

    while (1) {

        for (int i = 0; i < options_num; i++) {

            y_pos = (height / 2) + 2*i;
            x_pos = (real_width - strlen(options[i])) / 2;

            // selection character
            mvaddch(y_pos, x_pos - 2, i == choice ? ACS_RARROW : ' ');

            mvprintw(y_pos, x_pos, "%s", options[i]);
        }

        input = getch();

        switch (input) {

            case KEY_UP:
                choice--;

                if (choice < 0)
                    choice = options_num - 1;

                break;

            case KEY_DOWN:
                choice++;

                if (choice > options_num - 1)
                    choice = 0;

                break;

            case '\n':
                return options[choice];

            case 'q':
                endwin();
                exit(0);

            default:
                break;
        }

        refresh();
    }
}

void print_greeting() {

    char* greeting[] = {
     "           Conway's Game Of           ",
     "                                      ",
     "                  ██ ██   ████        ",
     "                  ██      ██          ",
     "███████   █████   ██ ██ ██████  █████ ",
     " ██   ██ ██   ██  ██ ██   ██   ██   ██",
     " ██   ██ ██       ██ ██   ██   ███████",
     " ██   ██ ██   ██  ██ ██   ██   ██     ",
     "███   ██  █████  ███ ██   ██    ██████"
    };

    int greeting_lines = sizeof(greeting) / sizeof(greeting[0]);

    int x_pos = (real_width - strlen(greeting[0])) / 2;
    int y_pos = height / 4;

    for (int i = 0; i < greeting_lines; i++)
        mvprintw(y_pos + i, x_pos, "%s", greeting[i]);
}
