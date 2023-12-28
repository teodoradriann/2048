#include <string.h>
#include <ctype.h>
#include <curses.h>
#include <time.h>
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
    if (c == 'q'){
        return 3;
    }
    return 0;
}

void drawTable(WINDOW *window){
    int height, width;
    getmaxyx(window, height, width);
    int y = height / 2 - 6; // Start drawing the table in the middle of the window
    int x = (width / 2) - width / 4;
    wmove(window, y, x);
    // Draw the top border of the table
    whline(window, ACS_HLINE, 2 * (width / 4));

    wrefresh(window); // Refresh the window to show the table
}

WINDOW* NewGame(int height, int width, int *score){
    WINDOW *gameWindow = newwin(height, width, 0, 0);
    char t[20];
    int c;
    time_t now = time(NULL);
    struct tm *rawtime = localtime(&now);
    //printf("%s\n", timestr(rawtime, t));
    box(gameWindow, 0, 0); // Draw a box around the window
    mvwprintw(gameWindow, 1, width / 2 - 2, "2048"); // Print a message in the new window
    mvwprintw(gameWindow, 2, (width / 2) - strlen("2048") / 2 - 2, "Score: %d", score);
    
    drawTable(gameWindow);
    
    wrefresh(gameWindow); // Refresh the window
    while ((c = getch()) != 'q'){
        //do stuff
    }
    if (c == 'q'){
        touchwin(stdscr);
        wrefresh(stdscr);
    }
    return gameWindow;
}

int main(){
	int width;
    int height;
    char* title = "2048 made by Adrian";
    char* ng = "New Game";
    char* res = "Resume";
    char* q = "Quit";
    int *score = 0;

    WINDOW *mainWindow = initscr(); //initializez fereastra de joc
    raw();
    getmaxyx(mainWindow, height, width);
    clear(); //sterg ecranul
    noecho();
    cbreak();
    curs_set(FALSE);
    keypad(stdscr, TRUE);

    setupScreen(height, width, title, ng, res, q);
    WINDOW *gameWindow = NULL;
    while(1) {
        switch(selectOption(height, width, title, ng, res, q)){
        case 1:
            gameWindow = NewGame(height, width, score);
            setupScreen(height, width, title, ng, res, q);
            break;
        case 2: 
            if (gameWindow != NULL){
                touchwin(gameWindow);
                wrefresh(gameWindow);
            }
            else {
                int msgLenght = strlen(" hey there bud! let's play!  ");
                int i;
                for (i = 0; i < msgLenght; i++)
                    mvaddch(2, 3 + i, ' ');
                mvaddstr(2, 3, " sorry bud, you can't resume");
                refresh();
            }
            break;
        case 3:
            endwin();
            return 0;
        }
    }
    endwin();
	return 0;
}