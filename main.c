#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#define star '*'

char* timestr(struct tm *t, char* time){
    sprintf(time, "%02d-%02d-%04d %02d:%02d:%02d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);
    return time;
}
int calculatePos(int height, int width){
    return (height - width) / 2;
}
//desenez mascota vorbareata a jocului
void drawMascot(){
    char greetings[5][51] = {"  hey there bud! let's play!", "what a nice day to play 2048", 
    "          sup buddy?       ", " howdy partner! let's play! ", "  aye fam wanna play innit"};
    /* selectez ecranul principal 
    si deschid fisierul unde se afla desenul cu mascota
    */
    touchwin(stdscr);
    FILE *file = fopen("mascot.txt", "r");
    if (file == NULL){
        printf("Error in opening the specified file.\n");
        endwin();
        return;
    }
    
    int y = 1; 
    int x = 2;
    int c;
    
    while ((c = fgetc(file)) != EOF) {
        mvaddch(y, x, c);
        x++;
        //daca linia e un newline, trec la urmatorul rand
        if (c == '\n') { 
            y++;
            x = 2; //resetez coloana
        }
    }
    fclose(file);
    srand(time(NULL));
    int i = rand() % 5;
    mvwprintw(stdscr, 2, 3, greetings[i]);
    refresh();
}
//afisez meniul jocului
void setupScreen(int height, int width, char* title, char *ng, char *res, char *q){
    //setez culoarea backroundului
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    
    drawMascot();

    /*desenez meniul jocului la jumatatea ecranului si centrez fiecare cuvant
    in functie de lungimea ecranului si dimenasiunea textului, apoi adaug * in dreptul
    fiecarei optiuni atat in stanga cat si in dreapta
    */
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
//desenez tabelul jocului
void drawTable(WINDOW *window, int table[4][4]){
    //imi initialzez toate combinatiile de culori pentru fiecare celula
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_BLACK);

    init_pair(7, COLOR_BLACK, COLOR_WHITE);
    init_pair(8, COLOR_RED, COLOR_WHITE);
    init_pair(9, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(10, COLOR_YELLOW, COLOR_WHITE);
    init_pair(11, COLOR_GREEN, COLOR_WHITE);
    int height;
    int width;
    int i;
    getmaxyx(window, height, width);
    int squareSize = width / 6;
    int y = (height / 2) - (squareSize / 2); 
    int x = (width / 2) - (squareSize * 2); 

    // desenez liile exterioare are patratului
    for (i = 0; i < squareSize + 1; i++) {
        mvwaddch(window, y + i, x, ACS_VLINE);
        mvwaddch(window, y + i, x + squareSize * 4, ACS_VLINE);
    }
    mvwhline(window, y, x, ACS_HLINE, squareSize * 4 + 1);
    mvwhline(window, y + squareSize, x, ACS_HLINE, squareSize * 4 + 1);

    // desenez celulele 
    for (int i = 1; i < 4; i++) {
        mvwhline(window, y + i * squareSize / 4, x, ACS_HLINE, squareSize * 4);
        mvwvline(window, y, x + i * squareSize, ACS_VLINE, squareSize);
    }
    // trec prin matrice si pun in celulele corespunzatoare tablei numerele colorate
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (table[i][j] != 0) {
                switch (table[i][j]){
                case 2:
                    wattron(window, COLOR_PAIR(1));
                    break;
                case 4:
                    wattron(window, COLOR_PAIR(2));
                    break;
                case 8:
                    wattron(window, COLOR_PAIR(3));
                    break;
                case 16:
                    wattron(window, COLOR_PAIR(4));
                    break;
                case 32:
                    wattron(window, COLOR_PAIR(5));
                    break;
                case 64:
                    wattron(window, COLOR_PAIR(6));
                    break;
                case 128:
                    wattron(window, COLOR_PAIR(7));
                    break;
                case 256:
                    wattron(window, COLOR_PAIR(8));
                    break;
                case 512:
                    wattron(window, COLOR_PAIR(9));
                    break;
                case 1024:
                    wattron(window, COLOR_PAIR(10));
                    break;
                case 2048:
                    wattron(window, COLOR_PAIR(11));
                    break;
                default:
                    wattroff(window, A_COLOR);
                    break;
                }
                mvwprintw(window, (y + i * squareSize / 4) + 1, x + j * squareSize + 7, "%d", table[i][j]);
                wattroff(window, A_COLOR); // Turn off color after printing each number
            }
        }
    }
    wrefresh(window); // Refresh the window to show the table
}

void moveCell(int table[4][4]){
    int c;
    while ((c = getch()) != 'q'){
        //do stuff
    }
    if (tolower(c) == 'q'){
        //merg in fereastra principala si ii dau refresh
        touchwin(stdscr);
        wrefresh(stdscr);
    }
}

void continueGame(WINDOW* gameWindow, int *score, int table[4][4]){
    touchwin(gameWindow); //merg in fereastra de joc
    wrefresh(gameWindow); //dau un refresh la fereastra
    drawTable(gameWindow, table); //redesenez tabelul pentru a nu pierde culorile celulelor
    wrefresh(gameWindow);
    moveCell(table);
}

WINDOW* NewGame(int height, int width, int *score, int table[4][4]){
    //initializez o noua fereastra de joc
    WINDOW *gameWindow = newwin(height, width, 0, 0);
    //obtin ora si data curente pentru afisare
    char t[20];
    time_t now = time(NULL);
    struct tm *rawtime = localtime(&now);
    //desenez un contur cu linie alba
    box(gameWindow, 0, 0); 
    //control panel-ul
    mvwprintw(gameWindow, 1, width / 2 - 2, "2048"); // Print a message in the new window
    mvwprintw(gameWindow, 2, (width / 2) - strlen("2048") / 2 - 2, "Score: %d", score);
    mvwprintw(gameWindow, height - 4, (width / 2) - 9.5, "%s", timestr(rawtime, t));
    
    //desenez tabla de joc
    drawTable(gameWindow, table);
    wrefresh(gameWindow);
    
    moveCell(table);
    return gameWindow;
}

void initTable(int table[4][4]){
    //initializez toate valorile matricei cu 0 pentru un fresh start
    int i;
    int j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            table[i][j] = 0;
        }
    }
    /*
    voi alege random 2 indexuri in care voi pune o valoare random dintre 2
    si 4 si verific daca nu cumva indexii sunt egali pentru ca daca da
    voi da skip si voi alege altii
    */
    int cntr = 0;
    while (cntr != 2) {
        int x = rand() % 4;
        int y = rand() % 4;
        if (x != y){
            table[x][y] = (rand() % 2 + 1) * 2;
            cntr++;
        }
        else
            continue;
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
    char t[20];
    char messages[5][51] = {"let's play again ASAP!", "where are you going? :(", 
    "hey bud! shall we continue?", "come on bud.. one more!", "arghrhgrhgrrhgrghrrghr"};
    int k;
    srand(time(NULL));

    WINDOW* mainWindow = initscr(); //initializez fereastra de joc
    getmaxyx(mainWindow, height, width);
    clear(); /* Se șterge ecranul */
    noecho(); /* Se inhibă afișarea caracterelor introduse de la tastatură */
    cbreak(); /* Caracterele introduse sunt citite imediat - fără 'buffering' */
    curs_set(FALSE); /* Se ascunde cursorul */	
    keypad(stdscr, TRUE); //dau enable la keypad, adica tastele mai speciale

    setupScreen(height, width, title, ng, res, q); //afisez ecranul de pornire
    
    WINDOW* gameWindow = NULL; 
    while (1) {
        switch (selectOption(height, width, title, ng, res, q)) {
        /* daca a fost selectat New Game, verfic daca deja un joc e inceput
        si daca da, il sterg, altfel voi initializa o tabla de joc noua
        si voi porni jocul, ca mai apoi sa imi redesenez tabla de joc
        pentru cand voi iesi din fereastra noua creata 
        */
        case 1:
            if (gameWindow != NULL){
                delwin(gameWindow);
            }
            initTable(table);
            gameWindow = NewGame(height, width, score, table);
            setupScreen(height, width, title, ng, res, q);
            for (k = 3; k < 31; k++)
                mvaddch(2, k, ' ');
            k = rand() % 5;
            mvaddstr(2, 3, messages[k]);
            break;
        /* daca Resume e selectat, verific daca am deja un joc inceput
        daca acesta e inceput il voi continua si voi updata data si ora
        daca nu, transmit un mesaj mascotei
        */
        case 2:
            if (gameWindow != NULL) {
                continueGame(gameWindow, score, table);
                time_t now = time(NULL);
                struct tm *rawtime = localtime(&now);
                mvwprintw(gameWindow, height - 4, (width / 2) - 9.5, "%s", timestr(rawtime, t));
                setupScreen(height, width, title, ng, res, q);
                for (k = 3; k < 31; k++)
                mvaddch(2, k, ' ');
                k = rand() % 5;
                mvaddstr(2, 3, messages[k]);
                break;
            } else {
                setupScreen(height, width, title, ng, res, q);
                mvaddstr(2, 3, " sorry bud, you can't resume ");
                break;
            }
        /*
        pentru quit inchid orice fereastra e deschisa si ies din program
        */
        case 3:
            if (gameWindow != NULL) {
                delwin(gameWindow);
            }
            endwin();
            return 0;
        }
    }
    endwin();
    return 0;
}
