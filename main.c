#include <string.h>
#include <ctype.h>
#include <curses.h>
#include <time.h>
#include <stdlib.h>
#define star '*'

char* timestr(struct tm *t, char* time){
    sprintf(time, "%02d-%02d-%04d %02d:%02d:%02d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);
    return time;
}
//calculez pozitia ????
int calculatePos(int height, int width){
    return (height - width) / 2;
}
void drawMascot(){
    touchwin(stdscr);
    FILE *file = fopen("asciiArt.txt", "r");
    if (file == NULL){
        printf("Error in opening the specified file.\n");
        endwin();
        return;
    }

    int y = 1; 
    int x = 2;
    int c;
    
    while ((c = fgetc(file)) != EOF) { // Read characters from the file until EOF
        mvaddch(y, x, c); // Print each character
        x++;
        if (c == '\n') { // If the character is a newline, move to the next row
            y++;
            x = 2; // Reset the column
        }
    }
    fclose(file);
    refresh();
}
//afisez meniul jocului
void setupScreen(int height, int width, char* title, char *ng, char *res, char *q){
    //setez culoarea backroundului
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    
    drawMascot();

    mvaddstr(height / 2 - 3, calculatePos(width, strlen(title)), title);
	mvaddstr(height / 2 - 2, calculatePos(width, strlen(ng)), ng);
	mvaddstr(height / 2 - 1, calculatePos(width, strlen(res)), res);
	mvaddstr(height / 2, calculatePos(width, strlen(q)), q);
    mvaddch(height / 2 - 2, calculatePos(width, strlen(ng)) - 2, star);
    mvaddch(height / 2 - 2, calculatePos(width, strlen(ng)) + strlen(ng) + 1, star);
    refresh();
    attroff(COLOR_PAIR(1));
}
//selectez o optiune si returnez valoarea selectata
int selectOption(int height, int width, char* title, char *ng, char *res, char *q){
    int option = 1;
    int row = height / 2 - 2;
    int c;
    while((c = getch()) != 'q') {
        switch(c) {
            case KEY_UP:
                if (row > height / 2 - 2) {
                    if (row == height / 2 - 1) {
                        mvaddch(row, calculatePos(width, strlen(res)) - 2, ' ');
                        mvaddch(row, calculatePos(width, strlen(res)) + strlen(res) + 1, ' ');
                    }
                    else if (row == height / 2){
                        mvaddch(row, calculatePos(width, strlen(q)) - 2, ' ');
                        mvaddch(row, calculatePos(width, strlen(q)) + strlen(q) + 1, ' ');
                    }
                    row--; // Move up
                    option--;
                }
                break;
            case KEY_DOWN:
                if (row < height / 2) {
                    if (row == height / 2 - 2){
                        mvaddch(row, calculatePos(width, strlen(ng)) - 2, ' ');
                        mvaddch(row, calculatePos(width, strlen(ng)) + strlen(ng) + 1, ' ');
                    }
                    else if (row == height / 2 - 1){
                        mvaddch(row, calculatePos(width, strlen(res)) - 2, ' ');
                        mvaddch(row, calculatePos(width, strlen(res)) + strlen(res) + 1, ' ');
                    }
                    row++; // Move down
                    option++;
                }
                break;
            case '\n':
                return option;
                break;
        }
        if (row == height / 2 - 2) {
            mvaddch(row, calculatePos(width, strlen(ng)) - 2, star);
            mvaddch(row, calculatePos(width, strlen(ng)) + strlen(ng) + 1, star);
        }
        else if (row == height / 2 - 1){
            mvaddch(row, calculatePos(width, strlen(res)) - 2, star);
            mvaddch(row, calculatePos(width, strlen(res)) + strlen(res) + 1, star);
        }
        else if (row == height / 2){
            mvaddch(row, calculatePos(width, strlen(q)) - 2, star);
            mvaddch(row, calculatePos(width, strlen(q)) + strlen(q) + 1, star);
        }
        refresh(); // Refresh the screen to show the new position
    }
    return 0;
}

void drawTable(WINDOW *window, int table[4][4]){
    int height, width;
    getmaxyx(window, height, width);
    int squareSize = width / 6; // Size of the square based on the width of the window
    int y = (height / 2) - (squareSize / 2); // Start drawing the table in the middle of the window
    int x = (width / 2) - (squareSize * 2); // Center the table horizontally

    // Draw the outer square
    for (int i = 0; i < squareSize + 1; i++) {
        mvwaddch(window, y + i, x, ACS_VLINE);
        mvwaddch(window, y + i, x + squareSize * 4, ACS_VLINE);
    }
    mvwhline(window, y, x, ACS_HLINE, squareSize * 4 + 1);
    mvwhline(window, y + squareSize, x, ACS_HLINE, squareSize * 4 + 1);

    // Draw the inner lines
    for (int i = 1; i < 4; i++) {
        mvwhline(window, y + i * squareSize / 4, x, ACS_HLINE, squareSize * 4);
        mvwvline(window, y, x + i * squareSize, ACS_VLINE, squareSize);
    }

    // Draw the values of the matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (table[i][j] != 0) {
                mvwprintw(window, y + i * squareSize / 4 + 1, x + j * squareSize + 1, "%d", table[i][j]);
            }
        }
    }

    wrefresh(window); // Refresh the window to show the table
}

void continueGame(WINDOW* game, int *score){
    touchwin(game);
    wrefresh(game);
    int c;
    while ((c = getch()) != 'q'){
        //do stuff
    }
    
    if (tolower(c) == 'q'){
        touchwin(stdscr);
        wrefresh(stdscr);
    }
}

WINDOW* NewGame(int height, int width, int *score, int table[4][4]){
    WINDOW *gameWindow = newwin(24, 80, 0, 0);
    char t[20];
    int c;
    time_t now = time(NULL);
    struct tm *rawtime = localtime(&now);
    //printf("%s\n", timestr(rawtime, t));
    box(gameWindow, 0, 0); // Draw a box around the window
    mvwprintw(gameWindow, 1, width / 2 - 2, "2048"); // Print a message in the new window
    mvwprintw(gameWindow, 2, (width / 2) - strlen("2048") / 2 - 2, "Score: %d", score);
    drawTable(gameWindow, table);
    wrefresh(gameWindow); // Refresh the window

    while ((c = getch()) != 'q'){
        //do stuff
    }
    
    if (tolower(c) == 'q'){
        touchwin(stdscr);
        wrefresh(stdscr);
    }
    return gameWindow;
}

void initTable(int table[4][4]){
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            table[i][j] = 0;
        }
    }
    int i;
    for (i = 0; i < 2; i++) {
        int x = rand() % 4;
        int y = rand() % 4;
        table[x][y] = (rand() % 2 + 1) * 2;
    }
}

int main() {
    int width;
    int height;
    char* title = "2048 made by Adrian";
    char* ng = "New Game";
    char* res = "Resume";
    char* q = "Quit";
    int* score = NULL;
    int table[4][4] = {0};

    WINDOW* mainWindow = initscr(); //initializez fereastra de joc
    getmaxyx(mainWindow, height, width);
    clear(); //sterg ecranul
    noecho();
    cbreak();
    curs_set(FALSE);
    keypad(stdscr, TRUE);

    setupScreen(height, width, title, ng, res, q);
    WINDOW* gameWindow = NULL;
    while (1) {
        switch (selectOption(height, width, title, ng, res, q)) {
        case 1:
            if (gameWindow != NULL){
                delwin(gameWindow);
            }
            initTable(table);
            gameWindow = NewGame(height, width, score, table);
            mvaddstr(2, 3, " hey there bud! let's play!  ");
            break;
        case 2:
            if (gameWindow != NULL) {
                continueGame(gameWindow, score);
                break;
            } else {
                mvaddstr(2, 3, " sorry bud, you can't resume ");
                refresh();
                break;
            }
        case 3:
            if (gameWindow != NULL) {
                delwin(gameWindow); // Delete the game window if it exists
            }
            endwin();
            return 0;
        }
    }
    endwin();
    return 0;
}
