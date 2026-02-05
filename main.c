#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    char name[50];
    int score;
    char symbol;
} Player;

typedef struct {
    char winner[50];
    int moves;
} GameHistory;

char board[3][3];
char currentPlayer;
Player player1, player2;
bool isAIMode = false;
int moveCount = 0;

void resetBoard(char (*b)[3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            b[i][j] = ' ';
        }
    }
}

void initialiseGame() {
    resetBoard(board);
    currentPlayer = player1.symbol;
    moveCount = 0;
}

void authentication() {
    char option[10];
    FILE *file;
    
    while(1) {
        printf("Login/SignUp: ");
        scanf("%s", option);
        
        if (strcmp(option, "Login") == 0 || strcmp(option, "SignUp") == 0) {
            break;
        }
        
        printf("Invalid input! Type Login or SignUp.\n");
    }

    file = fopen("credentials.txt", "a+");
    
    if (!file) {
        return;
    }

    char name[50], pass[50], conf[50], sN[50], sP[50];

    if (strcmp(option, "SignUp") == 0) {
        printf("Enter name: ");
        scanf("%s", name);
        
        bool exists = false;
        rewind(file);
        
        while (fscanf(file, "%s %s", sN, sP) != EOF) {
            if (strcmp(name, sN) == 0) {
                exists = true;
            }
        }
        
        if (exists) {
            printf("Username already exists.\n");
            fclose(file);
            exit(0);
        }
        
        printf("Enter password: ");
        scanf("%s", pass);
        printf("Confirm password: ");
        scanf("%s", conf);
        
        while (strcmp(pass, conf) != 0) {
            printf("Mismatch! Confirm again: ");
            scanf("%s", conf);
        }
        
        fprintf(file, "%s %s\n", name, pass);
        printf("Account created.\n");
        fclose(file);
        exit(0);
        
    } else {
        while(1) {
            printf("Enter username: ");
            scanf("%s", name);
            printf("Enter password: ");
            scanf("%s", pass);
            
            bool found = false;
            rewind(file);
            
            while (fscanf(file, "%s %s", sN, sP) != EOF) {
                if (strcmp(name, sN) == 0 && strcmp(pass, sP) == 0) {
                    found = true;
                }
            }
            
            if (found) {
                printf("Login successful.\n");
                strcpy(player1.name, name);
                break;
            } else {
                printf("Incorrect username or password. Try again.\n");
            }
        }
    }
    
    fclose(file);
}

void setupPlayers(Player *p1, Player *p2) {
    strcpy(p1->name, player1.name);
    printf("Enter symbol for %s: ", p1->name);
    scanf(" %c", &p1->symbol);
    p1->score = 0;
    
    if (!isAIMode) {
        printf("Enter Player 2 name: ");
        scanf("%s", p2->name);
        printf("Enter symbol: ");
        scanf(" %c", &p2->symbol);
        p2->score = 0;
    } else {
        strcpy(p2->name, "Computer");
        printf("Enter symbol for Computer: ");
        scanf(" %c", &p2->symbol);
        p2->score = 0;
    }
}

void displayBoard(char b[3][3]) {
    printf("\n");
    
    for (int i = 0; i < 3; i++) {
        printf(" %c | %c | %c\n", b[i][0], b[i][1], b[i][2]);
        
        if (i < 2) {
            printf("---|---|---\n");
        }
    }
}

bool isBoardFull(char b[][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (b[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

char checkWinner(char b[][3]) {
    for (int i = 0; i < 3; i++) {
        if (b[i][0] != ' ' && b[i][0] == b[i][1] && b[i][1] == b[i][2]) {
            return b[i][0];
        }
    }
    
    for (int i = 0; i < 3; i++) {
        if (b[0][i] != ' ' && b[0][i] == b[1][i] && b[1][i] == b[2][i]) {
            return b[0][i];
        }
    }
    
    if (b[0][0] != ' ' && b[0][0] == b[1][1] && b[1][1] == b[2][2]) {
        return b[0][0];
    }
    
    if (b[0][2] != ' ' && b[0][2] == b[1][1] && b[1][1] == b[2][0]) {
        return b[0][2];
    }
    
    return ' ';
}

bool makeMove(int pos, char b[][3], char player) {
    if (pos < 1 || pos > 9) {
        return false;
    }
    
    int r = (pos - 1) / 3;
    int c = (pos - 1) % 3;
    
    if (b[r][c] != ' ') {
        return false;
    }
    
    b[r][c] = player;
    moveCount++;
    return true;
}

int evaluate(char b[3][3]) {
    char w = checkWinner(b);
    
    if (w == player2.symbol) {
        return 10;
    }
    
    if (w == player1.symbol) {
        return -10;
    }
    
    return 0;
}

int minimax(char b[3][3], int depth, bool isMax) {
    int score = evaluate(b);
    
    if (score == 10) {
        return score - depth;
    }
    
    if (score == -10) {
        return score + depth;
    }
    
    if (isBoardFull(b)) {
        return 0;
    }
    
    if (isMax) {
        int best = -1000;
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (b[i][j] == ' ') {
                    b[i][j] = player2.symbol;
                    int val = minimax(b, depth + 1, false);
                    
                    if (best < val) {
                        best = val;
                    }
                    
                    b[i][j] = ' ';
                }
            }
        }
        
        return best;
        
    } else {
        int best = 1000;
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (b[i][j] == ' ') {
                    b[i][j] = player1.symbol;
                    int val = minimax(b, depth + 1, true);
                    
                    if (best > val) {
                        best = val;
                    }
                    
                    b[i][j] = ' ';
                }
            }
        }
        
        return best;
    }
}

int findBestMove() {
    int bestVal = -1000;
    int bestMove = -1;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = player2.symbol;
                int val = minimax(board, 0, false);
                board[i][j] = ' ';
                
                if (val > bestVal) {
                    bestVal = val;
                    bestMove = i * 3 + j + 1;
                }
            }
        }
    }
    
    return bestMove;
}

void saveHistory(char *winner) {
    FILE *f = fopen("history.txt", "a");
    
    if (!f) {
        return;
    }
    
    fprintf(f, "%s %d\n", winner, moveCount);
    fclose(f);
}

void displayUserHistory() {
    FILE *f = fopen("history.txt", "r");
    
    if (!f) {
        return;
    }
    
    char w[50];
    int m;
    printf("\nYour Wins:\n");
    
    while (fscanf(f, "%s %d", w, &m) != EOF) {
        if (strcmp(w, player1.name) == 0) {
            printf("Won in %d moves\n", m);
        }
    }
    
    fclose(f);
}

void playGame() {
    int pos;
    char w;
    player1.score = 0;
    player2.score = 0;
    initialiseGame();
    
    while(1) {
        displayBoard(board);
        
        if (currentPlayer == player1.symbol) {
            printf("%s's turn (%c): ", player1.name, player1.symbol);
            scanf("%d", &pos);
            
            if (!makeMove(pos, board, player1.symbol)) {
                continue;
            }
            
        } else {
            if (isAIMode) {
                pos = findBestMove();
                makeMove(pos, board, player2.symbol);
            } else {
                printf("%s's turn (%c): ", player2.name, player2.symbol);
                scanf("%d", &pos);
                
                if (!makeMove(pos, board, player2.symbol)) {
                    continue;
                }
            }
        }
        
        w = checkWinner(board);
        
        if (w != ' ') {
            displayBoard(board);
            
            if (w == player1.symbol) {
                printf("%s wins!\n", player1.name);
                player1.score++;
                saveHistory(player1.name);
            } else {
                printf("%s wins!\n", player2.name);
                player2.score++;
                
                if (!isAIMode) {
                    saveHistory(player2.name);
                }
            }
            
            break;
        }
        
        if (isBoardFull(board)) {
            displayBoard(board);
            printf("Draw!\n");
            saveHistory("Draw");
            break;
        }
        
        if (currentPlayer == player1.symbol) {
            currentPlayer = player2.symbol;
        } else {
            currentPlayer = player1.symbol;
        }
    }
    
    printf("\nGame Score:\n%s=%d\n%s=%d\n", player1.name, player1.score, player2.name, player2.score);
}

int main() {
    srand(time(NULL));
    authentication();
    
    int mode;
    
    while(1) {
        printf("1. Friend\n2. Computer\n");
        scanf("%d", &mode);
        
        if (mode == 1 || mode == 2) {
            break;
        }
        
        printf("Invalid input! Choose 1 or 2.\n");
    }
    
    if (mode == 2) {
        isAIMode = true;
    } else {
        isAIMode = false;
    }
    
    setupPlayers(&player1, &player2);
    
    char again = 'y';
    
    while (again == 'y' || again == 'Y') {
        playGame();
        
        while(1) {
            printf("\nPlay again? (y/n): ");
            scanf(" %c", &again);
            
            if (again == 'y' || again == 'Y' || again == 'n' || again == 'N') {
                break;
            }
            
            printf("Invalid input! Enter y or n.\n");
        }
        
        initialiseGame();
    }
    
    char h;
    
    while(1) {
        printf("\nDo you want to see your win history? (y/n): ");
        scanf(" %c", &h);
        
        if (h == 'y' || h == 'Y' || h == 'n' || h == 'N') {
            break;
        }
        
        printf("Invalid input! Enter y or n.\n");
    }
    
    if (h == 'y' || h == 'Y') {
        displayUserHistory();
    }
    
    return 0;
}
