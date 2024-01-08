#include <ctype.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STAR '*'
#define FOREVER 1

char *timestr(struct tm *t, char *time) {
    sprintf(time, "%02d-%02d-%04d %02d:%02d:%02d", t->tm_mday, t->tm_mon + 1,
            t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);
    return time;
}

int max(int a, int b, int c, int d) {
    int max = a;
    if (b > max) {
        max = b;
    }
    if (c > max) {
        max = c;
    }
    if (d > max) {
        max = d;
    }
    return max;
}

int calculatePos(int height, int width) { return (height - width) / 2; }

// resetez tabelul la valorea nula
void resetTable(int table[4][4]) {
    int i;
    int j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            table[i][j] = 0;
        }
    }
}

// initiez tabelul de joc
void initTable(int table[4][4]) {
    // initializez toate valorile matricei cu 0 pentru un fresh start
    resetTable(table);
    /*
    voi alege random 2 indexuri in care voi pune o valoare random dintre 2
    si 4
    */
    int cntr = 0;
    srand(time(NULL));
    while (cntr != 2) {
        int x = rand() % 4;
        int y = rand() % 4;
        if (table[x][y] == 0) {
            table[x][y] = (rand() % 2 + 1) * 2;
            cntr++;
        } else
            continue;
    }
}

// desenez mascota vorbareata a jocului
void drawBuddy() {
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
void mainScreen(char *title, char *ng, char *res, char *load, char *q) {
    // setez culoarea backroundului
    int height;
    int width;
    getmaxyx(stdscr, height, width);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    drawBuddy();

    /*desenez meniul jocului la jumatatea ecranului si centrez fiecare cuvant
    in functie de lungimea ecranului si dimenasiunea textului, apoi adaug * in
    dreptul primei optiuni atat in stanga cat si in dreapta
    */
    mvaddstr(height / 2 - 4, calculatePos(width, strlen(title)), title);
    mvaddstr(height / 2 - 3, calculatePos(width, strlen(ng)), ng);
    mvaddstr(height / 2 - 2, calculatePos(width, strlen(res)), res);
    mvaddstr(height / 2 - 1, calculatePos(width, strlen(load)), load);
    mvaddstr(height / 2, calculatePos(width, strlen(q)), q);
    mvaddch(height / 2 - 3, calculatePos(width, strlen(ng)) - 2, STAR);
    mvaddch(height / 2 - 3, calculatePos(width, strlen(ng)) + strlen(ng) + 1,
            STAR);
    mvaddstr(
        height - 1, width / 2 - 33,
        "the game automatically saves the current game when you press quit");
    refresh();
    attroff(COLOR_PAIR(1));
}

// selectez o optiune si returnez valoarea selectata
int select(char *title, char *ng, char *res, char *load, char *q) {
    int height;
    int width;
    getmaxyx(stdscr, height, width);
    int option = 1;
    int row = height / 2 - 3;
    int c;
    while ((c = getch()) != 'q') {
        switch (c) {
            case KEY_UP:
                if (row > height / 2 - 3) {
                    if (row == height / 2 - 2) {
                        mvaddch(row, calculatePos(width, strlen(res)) - 2, ' ');
                        mvaddch(
                            row,
                            calculatePos(width, strlen(res)) + strlen(res) + 1,
                            ' ');
                    } else if (row == height / 2 - 1) {
                        mvaddch(row, calculatePos(width, strlen(load)) - 2,
                                ' ');
                        mvaddch(row,
                                calculatePos(width, strlen(load)) +
                                    strlen(load) + 1,
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
                    if (row == height / 2 - 3) {
                        mvaddch(row, calculatePos(width, strlen(ng)) - 2, ' ');
                        mvaddch(
                            row,
                            calculatePos(width, strlen(ng)) + strlen(ng) + 1,
                            ' ');
                    } else if (row == height / 2 - 2) {
                        mvaddch(row, calculatePos(width, strlen(res)) - 2, ' ');
                        mvaddch(
                            row,
                            calculatePos(width, strlen(res)) + strlen(res) + 1,
                            ' ');
                    } else if (row == height / 2 - 1) {
                        mvaddch(row, calculatePos(width, strlen(load)) - 2,
                                ' ');
                        mvaddch(row,
                                calculatePos(width, strlen(load)) +
                                    strlen(load) + 1,
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
        if (row == height / 2 - 3) {
            mvaddch(row, calculatePos(width, strlen(ng)) - 2, STAR);
            mvaddch(row, calculatePos(width, strlen(ng)) + strlen(ng) + 1,
                    STAR);
        } else if (row == height / 2 - 2) {
            mvaddch(row, calculatePos(width, strlen(res)) - 2, STAR);
            mvaddch(row, calculatePos(width, strlen(res)) + strlen(res) + 1,
                    STAR);
        } else if (row == height / 2 - 1) {
            mvaddch(row, calculatePos(width, strlen(load)) - 2, STAR);
            mvaddch(row, calculatePos(width, strlen(load)) + strlen(load) + 1,
                    STAR);
        } else if (row == height / 2) {
            mvaddch(row, calculatePos(width, strlen(q)) - 2, STAR);
            mvaddch(row, calculatePos(width, strlen(q)) + strlen(q) + 1, STAR);
        }
        refresh();  // Refresh the screen to show the new position
    }
    return 0;
}

// updatez tabelul de joc
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
                // sterg casuta pentru a afisa noul numar
                mvwprintw(gameWindow, (y + i * size / 4) + 2, x + j * size + 8,
                          "    ");
                // selectez culoarea pentru fiecare numar
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
                // afisez noul numar
                mvwprintw(gameWindow, (y + i * size / 4) + 2, x + j * size + 8,
                          "%d", table[i][j]);
                wattroff(gameWindow, A_COLOR);  // sterg culoarea
            } else {
                mvwprintw(gameWindow, (y + i * size / 4) + 2, x + j * size + 8,
                          "    ");
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

// adaug un numar random pe tabla de joc
void addRandomNumber(int table[4][4]) {
    int i;
    int j;
    // verific cate celule sunt goale pentru a sti daca mai am loc pe tabla
    int emptyCells = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (table[i][j] == 0) emptyCells++;
        }
    }
    if (emptyCells == 0) return;
    // daca mai am loc pe tabla aleg 2 indici random si pun 2 sau 4 in celula
    // respectiva
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

// verific daca jocul e over sau nu
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

    // verific daca mai am celule ramase, daca da inseamna ca jocul nu s-a
    // terminat
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

    // jocul s-a terminat daca am ajuns aici
    return 1;
}

// mut celulele in sus
void moveUp(int table[4][4], int *score, int *wMoved) {
    *wMoved = 0;
    int i, j, k, p;
    //merg prin matrice pentru a afla unde gasesc prima 
    //celula libera sau ocupata, cat mai sus posibil
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            //daca am gasit o celula libera, voi incepe sa caut sub 
            //ea numere cu for-ul K
            if (table[i][j] == 0) {
                for (k = i + 1; k < 4; k++) {
                    //daca gasesc o celula sub 0 care are un numar in ea
                    //incep sa caut iar sub ea sa vad daca mai sunt celule asemenea
                    if (table[k][j] != 0) {
                        for (p = k + 1; p < 4; p++) {
                            //daca am gasit o celula asemenea, o adun la cea initiala
                            //iar apoi modific scorul si marchez celula din urma cu 0
                            //pentru ca s-a contopit cu cea de deasupra ei
                            if (table[k][j] == table[p][j]) {
                                table[k][j] += table[p][j];
                                *score += table[k][j];
                                table[p][j] = 0;
                                *wMoved = 1;
                            }
                        }
                        //daca nu gasesc nicio celula asemenea cu K pe care 
                        //sa o contopesc atunci pur si simplu mut celula K
                        //pe linia I care era 0 si sterg celula K din pozitia
                        //veche
                        if (table[i][j] != table[k][j]) {
                            *wMoved = 1;
                        }
                        table[i][j] = table[k][j];
                        table[k][j] = 0;
                        break;
                    }
                }
                //daca o celula initiala nu e 0 caut sub ea orice celula K diferita de 0
                //asemenea cu I pentru a o contopi cu aceasta
            } else if (table[i][j] != 0) {
                for (k = i + 1; k < 4; k++) {
                    if (table[k][j] != 0) {
                        if (table[i][j] == table[k][j]) {
                            table[i][j] += table[k][j];
                            *score += table[i][j];
                            table[k][j] = 0;
                            *wMoved = 1;
                        }
                        break;
                    }
                }
            }
        }
    }
}

// algoritmii de mai jos sunt la fel ca cel cu W doar ca parcug matricea
// in sensuri diferite pentru fiecare directe
void moveDown(int table[4][4], int *score, int *sMoved) {
    int i, j, k, p;
    *sMoved = 0;
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
                                *sMoved = 1;
                            }
                        }
                        if (table[i][j] != table[k][j]) {
                            *sMoved = 1;
                        }
                        table[i][j] = table[k][j];
                        table[k][j] = 0;
                        break;
                    }
                }
            } else if (table[i][j] != 0) {
                for (k = i - 1; k >= 0; k--) {
                    if (table[k][j] != 0) {
                        if (table[i][j] == table[k][j]) {
                            table[i][j] += table[k][j];
                            *score += table[i][j];
                            table[k][j] = 0;
                            *sMoved = 1;
                        }
                        break;
                    }
                }
            }
        }
    }
}

void moveLeft(int table[4][4], int *score, int *aMoved) {
    int i, j, k, p;
    *aMoved = 0;
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
                                *aMoved = 1;
                            }
                        }
                        if (table[j][i] != table[j][k]) {
                            *aMoved = 1;
                        }
                        table[j][i] = table[j][k];
                        table[j][k] = 0;
                        break;
                    }
                }
            } else if (table[j][i] != 0) {
                for (k = i + 1; k < 4; k++) {
                    if (table[j][k] != 0) {
                        if (table[j][i] == table[j][k]) {
                            table[j][i] += table[j][k];
                            *score += table[j][i];
                            table[j][k] = 0;
                            *aMoved = 1;
                        }
                        break;
                    }
                }
            }
        }
    }
}

void moveRight(int table[4][4], int *score, int *dMoved) {
    int i, j, k, p;
    *dMoved = 0;
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
                                *dMoved = 1;
                            }
                        }
                        if (table[j][i] != table[j][k]) {
                            *dMoved = 1;
                        }
                        table[j][i] = table[j][k];
                        table[j][k] = 0;
                        break;
                    }
                }
            } else if (table[j][i] != 0) {
                for (k = i - 1; k >= 0; k--) {
                    if (table[j][k] != 0) {
                        if (table[j][i] == table[j][k]) {
                            table[j][i] += table[j][k];
                            *score += table[j][i];
                            table[j][k] = 0;
                            *dMoved = 1;
                        }
                        break;
                    }
                }
            }
        }
    }
}

// returnez numarul maxim de celule nulle
int numberOfNullCells(int table[4][4]) {
    int i;
    int j;
    int cntr = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (table[i][j] == 0) cntr++;
        }
    }
    return cntr;
}

// returnez cea mai buna mutare posibila dpdv al celulelor eliberate
int returnTheBestMove(int table[4][4]) {
    // imi iau o copie a tabelului initial pe care voi face mutarile
    int copyTable[4][4];
    int i;
    int max = -666;
    int freeCells;
    int bestMove = 0;
    int score = 0;
    // 4 mutari
    for (i = 1; i <= 4; i++) {
        // copiez la fiecare mutare matricea initiala in copie
        memcpy(copyTable, table, 16 * sizeof(int));
        int moved = 0;
        switch (i) {
            case 1:
                moveUp(copyTable, &score, &moved);
                break;
            case 2:
                moveLeft(copyTable, &score, &moved);
                break;
            case 3:
                moveDown(copyTable, &score, &moved);
                break;
            case 4:
                moveRight(copyTable, &score, &moved);
                break;
            default:
                break;
        }
        // aflu numarul de celule specific mutarii
        freeCells = numberOfNullCells(copyTable);
        // daca e mai mare atunci mutarea respectiva are numarul maxim de celule
        // goale si o retin
        if (freeCells > max) {
            max = freeCells;
            bestMove = i;
        }
    }
    return bestMove;
}

// functia care se ocupa de mutatul celulelor pe tabla de joc
void moveCells(WINDOW **gameWindow, int table[4][4], int *score) {
    int c;
    int gameOver = 0;
    int width;
    int height;
    int waitFor = 200;  // 20 de secunde
    int elapsedTime = 0;
    getmaxyx(*gameWindow, height, width);

    timeout(100);  // dupa o secunda de asteptare getch va returna ERR
    nodelay(*gameWindow, TRUE);  // functia getch va returna ERR dupa o secunda
                                 // in care nu exista input

    while (FOREVER) {
        c = getch();
        // daca avem input de la utilizator mutam celulele in mod corespunzator
        if (c != ERR) {
            int moved = 0;
            elapsedTime = 0;
            switch (c) {
                case 'w':
                    moveUp(table, score, &moved);
                    break;
                case 'a':
                    moveLeft(table, score, &moved);
                    break;
                case 's':
                    moveDown(table, score, &moved);
                    break;
                case 'd':
                    moveRight(table, score, &moved);
                    break;
                default:
                    break;
            }
            if (moved) {
                // A valid move was performed
                addRandomNumber(table);
                updateTable(*gameWindow, table);
                mvwprintw(*gameWindow, 2, (width / 2) - strlen("2048") / 2 - 2,
                          "Score: %d", *score);
                wrefresh(*gameWindow);
                elapsedTime = 0;
            }
            // daca s-a mutat vreo celula punem un numar random pe tabla
            // si modificam scorul
            // daca nu avem input de la utilizator, vom incrementa in fiecare
            // secunda timpul care a trecut
        } else {
            elapsedTime++;
            // daca timpul care a trecut e mai mare sau egal cu
            // timpul pe care trebuia sa il asteptam vom afla cea mai buna
            // mutare si vom muta acolo.
            // daca nu exista o mutare care sa creasca cel mai mult scorul
            // atunci nu vom face nimic si vom astepta decizia utilizatorului
            if (elapsedTime >= waitFor) {
                int move = returnTheBestMove(table);
                int moved = 0;
                switch (move) {
                    case 1:
                        moveUp(table, score, &moved);
                        break;
                    case 2:
                        moveLeft(table, score, &moved);
                        break;
                    case 3:
                        moveDown(table, score, &moved);
                        break;
                    case 4:
                        moveRight(table, score, &moved);
                        break;
                }
                if (moved) {
                    addRandomNumber(table);
                    updateTable(*gameWindow, table);
                    mvwprintw(*gameWindow, 2,
                              (width / 2) - strlen("2048") / 2 - 2, "Score: %d",
                              *score);
                    wrefresh(*gameWindow);
                }
                elapsedTime = 0;
            }
        }
        // verificam daca jocul s-a terminat sau nu
        gameOver = isGameOver(table);
        if (gameOver) {
            mvwprintw(*gameWindow, height - 1,
                      (width / 2) - strlen("GAME OVER!") / 2, "GAME OVER!");
            wrefresh(*gameWindow);
            nodelay(*gameWindow, FALSE);
            getchar();
            delwin(*gameWindow);
            *gameWindow = NULL;
            break;  // ies din loop cand e game over
        }
    }
    if (tolower(c) == 'q') {
        // merg in fereastra principala si ii dau refresh
        touchwin(stdscr);
        wrefresh(stdscr);
    }
}

// functia care continua jocul
void continueGame(WINDOW **gameWindow, int *score, int table[4][4]) {
    /* daca fereastra nu e NULL inseamna ca am un joc in desfasurare pe care
    pot sa il reiau, daca e NULL nu fac nimic
    */
    if (*gameWindow == NULL) {
        return;
    } else {
        touchwin(*gameWindow);  // merg in fereastra de joc
        wrefresh(*gameWindow);  // dau un refresh la fereastra
        updateTable(
            *gameWindow,
            table);  // redesenez celulele pentru a nu le pierde culoarea
        wrefresh(*gameWindow);                // dau un refresh
        moveCells(gameWindow, table, score);  // continui jocul
    }
}

// functia prin care salvez jocul
void saveGame(int *score, int table[4][4]) {
    // deschid un fisier save in care sa pun datele jocului
    FILE *save = fopen("save.txt", "w");
    if (save == NULL) {
        return;
    }
    // salvez matricea curenta si scorul
    int i;
    int j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            fprintf(save, "%d ", table[i][j]);
        }
        fprintf(save, "\n");
    }
    fprintf(save, "%d\n", *score);
    fclose(save);
}

// functia care da load la joc
WINDOW *loadGame(int *score, int table[4][4]) {
    // functia loadGame e practic la fel ca newGame doar ca citeste valorile
    // salvate din fisier si initializeaza tabla de joc cu ele
    // initializez o noua fereastra de joc
    int height;
    int width;
    FILE *saveFile = fopen("save.txt", "r");
    if (saveFile == NULL) {
        return NULL;
    }
    // citesc matricea din fisier
    int i;
    int j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            fscanf(saveFile, "%d", &table[i][j]);
        }
    }
    fscanf(saveFile, "%d", score);  // citesc scorul
    getmaxyx(stdscr, height, width);
    if (isGameOver(table)) {
        return NULL;
    }
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
    drawTable(gameWindow, table);
    wrefresh(gameWindow);

    // incep jocul
    moveCells(&gameWindow, table, score);
    return gameWindow;
}

// functia care incepe un joc nou
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

    // incep jocul
    moveCells(&gameWindow, table, score);
    return gameWindow;
}

int main() {
    int width;
    char *title = "2048";
    char *ng = "New Game";
    char *res = "Resume";
    char *load = "Load";
    char *q = "Quit";
    int *score = (int *)malloc(sizeof(int));
    *score = 0;
    int table[4][4] = {0};
    char t[20];
    char messages[4][51] = {"let's play again ASAP!", "where are you going? :(",
                            "hey bud! shall we continue?",
                            "come on bud.. one more!"};
    int k;
    srand(time(NULL));

    WINDOW *mainWindow = initscr();  // initializez fereastra meniului principal
    WINDOW *gameWindow = NULL;       // initializez fereastra jocului

    width = getmaxx(mainWindow);
    clear();  /* Se șterge ecranul */
    noecho(); /* Se inhibă afișarea caracterelor introduse de la tastatură */
    cbreak(); /* Caracterele introduse sunt citite imediat - fără 'buffering' */
    curs_set(FALSE);       /* Se ascunde cursorul */
    keypad(stdscr, TRUE);  // dau enable la keypad, adica tastele mai speciale

    mainScreen(title, ng, res, load, q);  // afisez ecranul de pornire

    while (FOREVER) {
        switch (select(title, ng, res, load, q)) {
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
                mainScreen(title, ng, res, load, q);
                // sterg mesajul curent si adaug un mesaj random mascotei
                mvprintw(2, 3, "                             ");
                k = rand() % 4;
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
                    mainScreen(title, ng, res, load, q);
                    mvprintw(2, 3, "                             ");
                    k = rand() % 4;
                    mvaddstr(2, 3, messages[k]);
                    break;
                } else {
                    mainScreen(title, ng, res, load, q);
                    mvaddstr(2, 3, " sorry bud, you can't resume ");
                    break;
                }
            /* daca Load e selectat, verific daca am un joc inceput
            daca un nou joc e inceput, nu pot sa dau load la cel vechi
            pentru ca l-as suprascrie pe acesta. daca nu e inceput
            atunci dau load la vechiul joc
            */
            case 3:
                if (gameWindow != NULL) {
                    mainScreen(title, ng, res, load, q);
                    mvaddstr(2, 3, "sorry bro you started a game");
                    break;
                } else {
                    gameWindow = loadGame(score, table);
                    if (gameWindow == NULL) {
                        mainScreen(title, ng, res, load, q);
                        mvprintw(2, 3, "                             ");
                        mvaddstr(2, 3, " the last game was over homie");
                        break;
                    }
                    mainScreen(title, ng, res, load, q);
                    // sterg mesajul curent si adaug un mesaj random mascotei
                    mvprintw(2, 3, "                             ");
                    k = rand() % 4;
                    mvaddstr(2, 3, messages[k]);
                    break;
                }
            /*
            pentru quit inchid orice fereastra e deschisa salvez jocul curent
            si ies din program
            */
            case 4:
                if (gameWindow != NULL) {
                    delwin(gameWindow);
                    gameWindow = NULL;
                }
                saveGame(score, table);
                endwin();
                return 0;
        }
    }
    free(score);
    endwin();
    return 0;
}