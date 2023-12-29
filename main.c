#include <ctype.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define star '*'
#define FOREVER 1

char *timestr(struct tm *t, char *time) {
    sprintf(time, "%02d-%02d-%04d %02d:%02d:%02d", t->tm_mday, t->tm_mon + 1,
            t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);
    return time;
}

int calculatePos(int height, int width) { return (height - width) / 2; }

void resetTable(int table[4][4]) {
    int i;
    int j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            table[i][j] = 0;
        }
    }
}

void initTable(int table[4][4]) {
    // initializez toate valorile matricei cu 0 pentru un fresh start
    resetTable(table);
    /*
    voi alege random 2 indexuri in care voi pune o valoare random dintre 2
    si 4 si verific daca nu cumva indexii sunt egali pentru ca daca da
    voi da skip si voi alege altii
    */
    int cntr = 0;
    srand(time(NULL));
    while (cntr != 2) {
        int x = rand() % 4;
        int y = rand() % 4;
        if (x != y) {
            table[x][y] = (rand() % 2 + 1) * 2;
            cntr++;
        } else
            continue;
    }
}
// desenez mascota vorbareata a jocului
void drawMascot() {
    char greetings[5][51] = {
        "  hey there bud! let's play!", "what a nice day to play 2048",
        "          sup buddy?       ", " howdy partner! let's play! ",
        "  aye fam wanna play innit"};
    /* selectez ecranul principal
    si deschid fisierul unde se afla desenul cu mascota
    */
    touchwin(stdscr);
    FILE *file = fopen("mascot.txt", "r");
    if (file == NULL) {
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
        // daca linia e un newline, trec la urmatorul rand
        if (c == '\n') {
            y++;
            x = 2;  // resetez coloana
        }
    }
    fclose(file);
    srand(time(NULL));
    int i = rand() % 5;
    mvwprintw(stdscr, 2, 3, greetings[i]);
    refresh();
}
// afisez meniul jocului
void setupScreen(char *title, char *ng, char *res, char *q) {
    // setez culoarea backroundului
    int height;
    int width;
    getmaxyx(stdscr, height, width);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    drawMascot();

    /*desenez meniul jocului la jumatatea ecranului si centrez fiecare cuvant
    in functie de lungimea ecranului si dimenasiunea textului, apoi adaug * in
    dreptul fiecarei optiuni atat in stanga cat si in dreapta
    */
    mvaddstr(height / 2 - 3, calculatePos(width, strlen(title)), title);
    mvaddstr(height / 2 - 2, calculatePos(width, strlen(ng)), ng);
    mvaddstr(height / 2 - 1, calculatePos(width, strlen(res)), res);
    mvaddstr(height / 2, calculatePos(width, strlen(q)), q);
    mvaddch(height / 2 - 2, calculatePos(width, strlen(ng)) - 2, star);
    mvaddch(height / 2 - 2, calculatePos(width, strlen(ng)) + strlen(ng) + 1,
            star);
    refresh();
    attroff(COLOR_PAIR(1));
}
// selectez o optiune si returnez valoarea selectata
int selectOption(char *title, char *ng, char *res, char *q) {
    int height;
    int width;
    getmaxyx(stdscr, height, width);
    int option = 1;
    int row = height / 2 - 2;
    int c;
    while ((c = getch()) != 'q') {
        switch (c) {
            case KEY_UP:
                if (row > height / 2 - 2) {
                    if (row == height / 2 - 1) {
                        mvaddch(row, calculatePos(width, strlen(res)) - 2, ' ');
                        mvaddch(
                            row,
                            calculatePos(width, strlen(res)) + strlen(res) + 1,
                            ' ');
                    } else if (row == height / 2) {
                        mvaddch(row, calculatePos(width, strlen(q)) - 2, ' ');
                        mvaddch(row,
                                calculatePos(width, strlen(q)) + strlen(q) + 1,
                                ' ');
                    }
                    row--;  // Move up
                    option--;
                }
                break;
            case KEY_DOWN:
                if (row < height / 2) {
                    if (row == height / 2 - 2) {
                        mvaddch(row, calculatePos(width, strlen(ng)) - 2, ' ');
                        mvaddch(
                            row,
                            calculatePos(width, strlen(ng)) + strlen(ng) + 1,
                            ' ');
                    } else if (row == height / 2 - 1) {
                        mvaddch(row, calculatePos(width, strlen(res)) - 2, ' ');
                        mvaddch(
                            row,
                            calculatePos(width, strlen(res)) + strlen(res) + 1,
                            ' ');
                    }
                    row++;  // Move down
                    option++;
                }
                break;
            case '\n':
                return option;
                break;
        }
        if (row == height / 2 - 2) {
            mvaddch(row, calculatePos(width, strlen(ng)) - 2, star);
            mvaddch(row, calculatePos(width, strlen(ng)) + strlen(ng) + 1,
                    star);
        } else if (row == height / 2 - 1) {
            mvaddch(row, calculatePos(width, strlen(res)) - 2, star);
            mvaddch(row, calculatePos(width, strlen(res)) + strlen(res) + 1,
                    star);
        } else if (row == height / 2) {
            mvaddch(row, calculatePos(width, strlen(q)) - 2, star);
            mvaddch(row, calculatePos(width, strlen(q)) + strlen(q) + 1, star);
        }
        refresh();  // Refresh the screen to show the new position
    }
    return 0;
}

void updateTable(WINDOW *gameWindow, int table[4][4]) {
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
    getmaxyx(gameWindow, height, width);
    int i;
    int j;
    int size = 16;
    int y = (height / 2) - (size / 2);
    int x = (width / 2) - (size * 2);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (table[i][j] != 0) {
                switch (table[i][j]) {
                    case 2:
                        wattron(gameWindow, COLOR_PAIR(1));
                        break;
                    case 4:
                        wattron(gameWindow, COLOR_PAIR(2));
                        break;
                    case 8:
                        wattron(gameWindow, COLOR_PAIR(3));
                        break;
                    case 16:
                        wattron(gameWindow, COLOR_PAIR(4));
                        break;
                    case 32:
                        wattron(gameWindow, COLOR_PAIR(5));
                        break;
                    case 64:
                        wattron(gameWindow, COLOR_PAIR(6));
                        break;
                    case 128:
                        wattron(gameWindow, COLOR_PAIR(7));
                        break;
                    case 256:
                        wattron(gameWindow, COLOR_PAIR(8));
                        break;
                    case 512:
                        wattron(gameWindow, COLOR_PAIR(9));
                        break;
                    case 1024:
                        wattron(gameWindow, COLOR_PAIR(10));
                        break;
                    case 2048:
                        wattron(gameWindow, COLOR_PAIR(11));
                        break;
                    default:
                        wattroff(gameWindow, A_COLOR);
                        break;
                }
                mvwprintw(gameWindow, (y + i * size / 4) + 2, x + j * size + 8,
                          "%d", table[i][j]);
                wattroff(gameWindow,
                         A_COLOR);  // Turn off color after printing each number
            } else {
                mvwprintw(gameWindow, (y + i * size / 4) + 2, x + j * size + 1,
                          "           ");
            }
        }
    }
    wrefresh(gameWindow);
}
// desenez tabelul jocului
void drawTable(WINDOW *gameWindow, int table[4][4]) {
    // imi initialzez toate combinatiile de culori pentru fiecare celula
    int height;
    int width;
    int i;
    getmaxyx(gameWindow, height, width);
    int size = 16;
    int y = (height / 2) - (size / 2);
    int x = (width / 2) - (size * 2);

    // desenez liile exterioare are patratului
    for (i = 0; i < size + 1; i++) {
        mvwaddch(gameWindow, y + i, x, '|');
        mvwaddch(gameWindow, y + i, x + size * 4, '|');
    }
    mvwhline(gameWindow, y, x, '-', size * 4 + 1);
    mvwhline(gameWindow, y + size, x, '-', size * 4 + 1);

    // desenez celulele
    for (i = 1; i < 4; i++) {
        mvwhline(gameWindow, y + i * size / 4, x + 0.5, '-', size * 4);
        mvwvline(gameWindow, y, x + i * size, '|', size);
    }
    wattron(gameWindow, COLOR_PAIR(7));
    mvwaddstr(gameWindow, height - 2, 2.33,
              " q - go back | w - move up | a - move left | s - move down | d "
              "- move right ");
    wattroff(gameWindow, COLOR_PAIR(8));

    updateTable(gameWindow, table);
    wrefresh(gameWindow);  // Refresh the window to show the table
}

void addRandomNumber(int table[4][4]) {
    int i;
    int j;
    int emptyCells = 0;
    for (i = 0; i < 4; i++){
        for (j = 0; j < 4; j++){
            if (table[i][j] == 0)
                emptyCells++;
        }
    }
    if (emptyCells == 0)
        return;
    int cntr = 0;
    srand(time(NULL));
    while (cntr != 1) {
        int x = rand() % 4;
        int y = rand() % 4;
        if (table[x][y] == 0) {
            table[x][y] = (rand() % 2 + 1) * 2;
            cntr++;
        } else
            continue;
    }
}

int isGameOver(int table[4][4]) {
    int i;
    int j;
    // verific daca apare 2048 in vreo celula
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (table[i][j] == 2048) {
                return 1;
            }
        }
    }

    // verific daca mai am celule ramase, daca da inseamna ca jocul nu s-a terminat
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (table[i][j] == 0) {
                return 0;
            }
        }
    }

    // verific daca am celule asemenea pe linii
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            if (table[i][j] == table[i][j + 1]) {
                return 0; 
            }
        }
    }

    // verific daca am celule asemenea pe coloane
    for (j = 0; j < 4; j++) {
        for (i = 0; i < 3; i++) {
            if (table[i][j] == table[i + 1][j]) {
                return 0; 
            }
        }
    }
    
    //jocul s-a terminat daca am ajuns aici
    return 1; 
}

void moveCell(WINDOW **gameWindow, int table[4][4], int *score) {
    int c;
    int i, j, k, p;
    int wMoved, aMoved, sMoved, dMoved;
    int gameOver = 0;
    int width;
    int height;
    getmaxyx(*gameWindow, height, width);
    while ((c = getch()) != 'q' && !gameOver) {
        switch (c) {
            case 'w':
                wMoved = 0;
                for (i = 0; i < 4; i++) {
                    for (j = 0; j < 4; j++) {
                        if (table[i][j] == 0) {
                            for (k = i + 1; k < 4; k++) {
                                if (table[k][j] != 0) {
                                    for (p = k + 1; p < 4; p++) {
                                        if (table[k][j] == table[p][j]) {
                                            table[k][j] += table[p][j];
                                            *score += table[k][j];
                                            table[p][j] = 0;
                                            wMoved = 1;
                                            break;
                                        }
                                    }
                                    if (table[i][j] != table[k][j]) {
                                        wMoved = 1;
                                    }
                                    table[i][j] = table[k][j];
                                    table[k][j] = 0;
                                    break;
                                }
                            }
                        } else if (table[i][j] != 0) {
                            for (k = i + 1; k < 4; k++) {
                                if (table[i][j] == table[k][j]) {
                                    table[i][j] += table[k][j];
                                    *score += table[i][j];
                                    table[k][j] = 0;
                                    wMoved = 1;
                                    break;
                                } else
                                    break;
                            }
                        }
                    }
                }
                if (wMoved) {
                    addRandomNumber(table);
                    updateTable(*gameWindow, table);
                    mvwprintw(*gameWindow, 2,
                              (width / 2) - strlen("2048") / 2 - 2, "Score: %d",
                              *score);
                    wrefresh(*gameWindow);
                }
                break;
            case 'a':
                aMoved = 0;
                for (j = 0; j < 4; j++) {
                    for (i = 0; i < 4; i++) {
                        if (table[j][i] == 0) {
                            for (k = i + 1; k < 4; k++) {
                                if (table[j][k] != 0) {
                                    for (p = k + 1; p < 4; p++) {
                                        if (table[j][k] == table[j][p]) {
                                            table[j][k] += table[j][p];
                                            *score += table[j][k];
                                            table[j][p] = 0;
                                            aMoved = 1;
                                            break;
                                        }
                                    }
                                    if (table[j][i] != table[j][k]) {
                                        aMoved = 1;
                                    }
                                    table[j][i] = table[j][k];
                                    table[j][k] = 0;
                                    break;
                                }
                            }
                        } else {
                            for (k = i + 1; k < 4; k++) {
                                if (table[j][k] != 0) {
                                    if (table[j][i] == table[j][k]) {
                                        table[j][i] += table[j][k];
                                        *score += table[j][i];
                                        table[j][k] = 0;
                                        aMoved = 1;
                                        break;
                                    } else {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                if (aMoved) {
                    addRandomNumber(table);
                    updateTable(*gameWindow, table);
                    mvwprintw(*gameWindow, 2,
                              (width / 2) - strlen("2048") / 2 - 2, "Score: %d",
                              *score);
                    wrefresh(*gameWindow);
                }
                break;
            case 's':
                sMoved = 0;
                for (i = 3; i >= 0; i--) {
                    for (j = 0; j < 4; j++) {
                        if (table[i][j] == 0) {
                            for (k = i - 1; k >= 0; k--) {
                                if (table[k][j] != 0) {
                                    for (p = k - 1; p >= 0; p--) {
                                        if (table[k][j] == table[p][j]) {
                                            table[k][j] += table[p][j];
                                            *score += table[k][j];
                                            table[p][j] = 0;
                                            sMoved = 1;
                                            break;
                                        }
                                    }
                                    if (table[i][j] != table[k][j]) {
                                        sMoved = 1;
                                    }
                                    table[i][j] = table[k][j];
                                    table[k][j] = 0;
                                    break;
                                }
                            }
                        } else {
                            for (k = i - 1; k >= 0; k--) {
                                if (table[k][j] != 0) {
                                    if (table[i][j] == table[k][j]) {
                                        table[i][j] += table[k][j];
                                        *score += table[i][j];
                                        table[k][j] = 0;
                                        sMoved = 1;
                                        break;
                                    } else {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                if (sMoved) {
                    addRandomNumber(table);
                    updateTable(*gameWindow, table);
                    mvwprintw(*gameWindow, 2,
                              (width / 2) - strlen("2048") / 2 - 2, "Score: %d",
                              *score);
                    wrefresh(*gameWindow);
                }
                break;
            case 'd':
                dMoved = 0;
                for (j = 3; j >= 0; j--) {
                    for (i = 0; i < 4; i++) {
                        if (table[j][i] == 0) {
                            for (k = i - 1; k >= 0; k--) {
                                if (table[j][k] != 0) {
                                    for (p = k - 1; p >= 0; p--) {
                                        if (table[j][k] == table[j][p]) {
                                            table[j][k] += table[j][p];
                                            *score += table[j][k];
                                            table[j][p] = 0;
                                            dMoved = 1;
                                            break;
                                        }
                                    }
                                    if (table[j][i] != table[j][k]) {
                                        dMoved = 1;
                                    }
                                    table[j][i] = table[j][k];
                                    table[j][k] = 0;
                                    break;
                                }
                            }
                        } else {
                            for (k = i - 1; k >= 0; k--) {
                                if (table[j][k] != 0) {
                                    if (table[j][i] == table[j][k]) {
                                        table[j][i] += table[j][k];
                                        *score += table[j][i];
                                        table[j][k] = 0;
                                        dMoved = 1;
                                        break;
                                    } else {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                if (dMoved) {
                    addRandomNumber(table);
                    updateTable(*gameWindow, table);
                    mvwprintw(*gameWindow, 2,
                              (width / 2) - strlen("2048") / 2 - 2, "Score: %d",
                              *score);
                    wrefresh(*gameWindow);
                }
                break;
            default:
                break;
        }
        gameOver = isGameOver(table);
        if (gameOver) {
            mvwprintw(*gameWindow, height - 1,
                      (width / 2) - strlen("GAME OVER!") / 2, "GAME OVER!");
            wrefresh(*gameWindow);
            getch();
            delwin(*gameWindow);
            *gameWindow = NULL;
        }
    }
    if (tolower(c) == 'q') {
        // merg in fereastra principala si ii dau refresh
        touchwin(stdscr);
        wrefresh(stdscr);
    }
}

void continueGame(WINDOW **gameWindow, int *score, int table[4][4]) {
    if (*gameWindow == NULL){
        return;
    } else {
        touchwin(*gameWindow);  // merg in fereastra de joc
        wrefresh(*gameWindow);  // dau un refresh la fereastra
        updateTable(*gameWindow, table);  // redesenez celulele pentru a nu le pierde culoarea
        wrefresh(*gameWindow);
        moveCell(gameWindow, table, score);
    }
}

WINDOW *newGame(int *score, int table[4][4]) {
    // initializez o noua fereastra de joc
    int height;
    int width;
    *score = 0;
    getmaxyx(stdscr, height, width);
    WINDOW *gameWindow = NULL;
    gameWindow = newwin(height, width, 0, 0);
    // obtin ora si data curente pentru afisare
    char t[20];
    time_t now = time(NULL);
    struct tm *rawtime = localtime(&now);
    // desenez un contur cu linie alba
    box(gameWindow, 0, 0);
    // control panel-ul
    mvwprintw(gameWindow, 1, width / 2 - 2, "2048");
    mvwprintw(gameWindow, 2, (width / 2) - strlen("2048") / 2 - 2, "Score: %d",
              *score);
    mvwprintw(gameWindow, 3, (width / 2) - 9.5, "%s", timestr(rawtime, t));

    // desenez tabla de joc
    drawTable(gameWindow, table);
    wrefresh(gameWindow);

    moveCell(&gameWindow, table, score);
    return gameWindow;
}

int main() {
    int width;
    int height;
    char *title = "2048";
    char *ng = "New Game";
    char *res = "Resume";
    char *q = "Quit";
    int *score = (int *)malloc(sizeof(int));
    *score = 0;
    int table[4][4] = {0};
    char t[20];
    char messages[5][51] = {"let's play again ASAP!", "where are you going? :(",
                            "hey bud! shall we continue?",
                            "come on bud.. one more!",
                            "arghrhgrhgrrhgrghrrghr"};
    int k;
    srand(time(NULL));

    WINDOW *mainWindow = initscr();  // initializez fereastra meniului principal
    WINDOW *gameWindow = NULL;       // initializez fereastra jocului

    getmaxyx(mainWindow, height, width);
    clear();  /* Se șterge ecranul */
    noecho(); /* Se inhibă afișarea caracterelor introduse de la tastatură */
    cbreak(); /* Caracterele introduse sunt citite imediat - fără 'buffering' */
    curs_set(FALSE);       /* Se ascunde cursorul */
    keypad(stdscr, TRUE);  // dau enable la keypad, adica tastele mai speciale

    setupScreen(title, ng, res, q);  // afisez ecranul de pornire

    while (FOREVER) {
        switch (selectOption(title, ng, res, q)) {
            /* daca a fost selectat New Game, verfic daca deja un joc e inceput
            si daca da, il sterg, altfel voi initializa o tabla de joc noua
            si voi porni jocul, ca mai apoi sa imi redesenez tabla de joc
            pentru cand voi iesi din fereastra noua creata
            */
            case 1:
                if (gameWindow != NULL) {
                    delwin(gameWindow);
                    gameWindow = NULL;
                }
                initTable(table);
                gameWindow = newGame(score, table);
                setupScreen(title, ng, res, q);
                for (k = 3; k < 31; k++) mvaddch(2, k, ' ');
                k = rand() % 5;
                mvaddstr(2, 3, messages[k]);
                break;
            /* daca Resume e selectat, verific daca am deja un joc inceput
            daca acesta e inceput il voi continua si voi updata data si ora
            daca nu, transmit un mesaj mascotei
            */
            case 2:
                if (gameWindow != NULL) {
                    continueGame(&gameWindow, score, table);
                    time_t now = time(NULL);
                    struct tm *rawtime = localtime(&now);
                    mvwprintw(gameWindow, 3, (width / 2) - 9.5, "%s",
                              timestr(rawtime, t));
                    setupScreen(title, ng, res, q);
                    for (k = 3; k < 31; k++) mvaddch(2, k, ' ');
                    k = rand() % 5;
                    mvaddstr(2, 3, messages[k]);
                    break;
                } else {
                    setupScreen(title, ng, res, q);
                    mvaddstr(2, 3, " sorry bud, you can't resume ");
                    break;
                }
            /*
            pentru quit inchid orice fereastra e deschisa si ies din program
            */
            case 3:
                if (gameWindow != NULL) {
                    delwin(gameWindow);
                    gameWindow = NULL;
                }
                endwin();
                return 0;
        }
    }
    free(score);
    endwin();
    return 0;
}