#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_LEN 50
#define BUTTON_SIZE 120
#define PADDING 10

typedef struct {
    char name[MAX_LEN];
    char symbol;
    int score;
} Player;

typedef struct {
    Rectangle rect;
    char value;
} Cell;

// -------------------- GLOBALS --------------------
Player player1, player2;
Player* currentPlayer;
Cell board[3][3];
int moveCount = 0;
bool gameActive = false;
bool isAIMode = false;
char statusText[100] = "";
char lastWinner[MAX_LEN] = "";  // Store winner of last game

// GUI States
typedef enum { STATE_LOGIN, STATE_SIGNUP, STATE_MODE, STATE_PLAYER2, STATE_SYMBOLS, STATE_GAME, STATE_POSTGAME, STATE_HISTORY } GameState;
GameState state = STATE_LOGIN;

// Input fields
char loginUser[MAX_LEN] = "";
char loginPass[MAX_LEN] = "";

char signupUser[MAX_LEN] = "";
char signupPass[MAX_LEN] = "";
char signupConfirm[MAX_LEN] = "";

char player2Name[MAX_LEN] = "";

// Focused field
typedef enum { FIELD_NONE, FIELD_LOGIN_USER, FIELD_LOGIN_PASS,
               FIELD_SIGNUP_USER, FIELD_SIGNUP_PASS, FIELD_SIGNUP_CONFIRM,
               FIELD_PLAYER2_NAME, FIELD_SYMBOL_P1, FIELD_SYMBOL_P2 } ActiveField;
ActiveField activeField = FIELD_LOGIN_USER;

// Selected symbols
char symbolPlayer1 = 'X';
char symbolPlayer2 = 'O';

// -------------------- FUNCTIONS --------------------
void resetBoard() {
    moveCount = 0;
    gameActive = true;
    currentPlayer = &player1;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            board[i][j].value = ' ';
            board[i][j].rect.x = PADDING + j * (BUTTON_SIZE + PADDING);
            board[i][j].rect.y = 150 + i * (BUTTON_SIZE + PADDING);
            board[i][j].rect.width = BUTTON_SIZE;
            board[i][j].rect.height = BUTTON_SIZE;
        }
}

char checkWinner() {
    // Rows
    for (int i = 0; i < 3; i++)
        if (board[i][0].value != ' ' &&
            board[i][0].value == board[i][1].value &&
            board[i][1].value == board[i][2].value)
            return board[i][0].value;

    // Columns
    for (int i = 0; i < 3; i++)
        if (board[0][i].value != ' ' &&
            board[0][i].value == board[1][i].value &&
            board[1][i].value == board[2][i].value)
            return board[0][i].value;

    // Diagonals
    if (board[0][0].value != ' ' &&
        board[0][0].value == board[1][1].value &&
        board[1][1].value == board[2][2].value)
        return board[0][0].value;

    if (board[0][2].value != ' ' &&
        board[0][2].value == board[1][1].value &&
        board[1][1].value == board[2][0].value)
        return board[0][2].value;

    return ' ';
}

bool isBoardFull() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j].value == ' ')
                return false;
    return true;
}

// Draw Tic-Tac-Toe board (fixed text drawing)
void drawBoard() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            DrawRectangleRec(board[i][j].rect, LIGHTGRAY);
            DrawRectangleLinesEx(board[i][j].rect, 2, BLACK);
            if (board[i][j].value != ' ') {
                char s[2] = { board[i][j].value, '\0' };
                DrawText(s, (int)(board[i][j].rect.x + BUTTON_SIZE / 3),
                         (int)(board[i][j].rect.y + BUTTON_SIZE / 4), 60, BLACK);
            }
        }
}

// -------------------- Login/Signup Validation --------------------
bool loginValidate() {
    FILE* f = fopen("credentials.txt", "r");
    if (!f) return false;
    char user[MAX_LEN], pass[MAX_LEN];
    bool found = false;
    while (fscanf(f, "%s %s", user, pass) != EOF) {
        if (strcmp(user, loginUser) == 0 && strcmp(pass, loginPass) == 0) {
            found = true;
            strcpy(player1.name, loginUser);
            break;
        }
    }
    fclose(f);
    return found;
}

bool signupValidate() {
    if (strcmp(signupPass, signupConfirm) != 0) return false;
    FILE* f = fopen("credentials.txt", "a+");
    if (!f) return false;
    char user[MAX_LEN], pass[MAX_LEN];
    rewind(f);
    while (fscanf(f, "%s %s", user, pass) != EOF) {
        if (strcmp(user, signupUser) == 0) { fclose(f); return false; }
    }
    fprintf(f, "%s %s\n", signupUser, signupPass);
    fclose(f);
    return true;
}

// -------------------- Setup players after mode --------------------
void setupPlayersGUI() {
    player1.symbol = symbolPlayer1;

    if (isAIMode) {
        strcpy(player2.name, "Computer");
        player2.symbol = symbolPlayer2;
    } else {
        strcpy(player2.name, player2Name);
        player2.symbol = symbolPlayer2;
    }

    // Preserve scores across games only if you want. Resetting to 0 here for new match session:
    player1.score = 0;
    player2.score = 0;
    resetBoard();
}

// -------------------- Save game history --------------------
void saveHistory(const char* winner) {
    FILE* f = fopen("history.txt", "a");
    if (!f) return;
    fprintf(f, "%s %d\n", winner, moveCount);
    fclose(f);
}

// -------------------- Draw Player History --------------------
void drawHistory(const char* playerName) {
    ClearBackground(RAYWHITE);
    DrawText(TextFormat("%s's Wins:", playerName), 50, 20, 20, BLACK);

    FILE* f = fopen("history.txt", "r");
    if (!f) {
        DrawText("No history found.", 50, 60, 20, BLACK);
    } else {
        int y = 60;
        char winner[MAX_LEN];
        int moves;
        while (fscanf(f, "%s %d", winner, &moves) != EOF) {
            if (strcmp(winner, playerName) == 0) {
                DrawText(TextFormat("Won in %d moves", moves), 50, y, 20, BLACK);
                y += 30;
            }
        }
        fclose(f);
    }

    // Draw Back Button
    Rectangle backBtn = {50, 550, 150, 40};
    DrawRectangleRec(backBtn, LIGHTGRAY);
    DrawRectangleLinesEx(backBtn, 2, BLACK);
    DrawText("Back", backBtn.x + 45, backBtn.y + 10, 20, BLACK);

    // Check click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, backBtn)) {
            state = STATE_POSTGAME;  // Return to postgame window
        }
    }
}

// -------------------- Minimax AI (perfect) --------------------
int evaluateBoard(int depth) {
    char w = checkWinner();
    if (w == player2.symbol) return 10 - depth; // prefer faster win
    if (w == player1.symbol) return -10 + depth; // prefer slower loss
    return 0;
}

bool movesLeft() {
    return !isBoardFull();
}

int minimax(int depth, bool isMax) {
    char winner = checkWinner();
    if (winner == player2.symbol) return 10 - depth;
    if (winner == player1.symbol) return -10 + depth;
    if (!movesLeft()) return 0;

    if (isMax) {
        int best = -10000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j].value == ' ') {
                    board[i][j].value = player2.symbol;
                    int val = minimax(depth + 1, false);
                    board[i][j].value = ' ';
                    if (val > best) best = val;
                }
            }
        }
        return best;
    } else {
        int best = 10000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j].value == ' ') {
                    board[i][j].value = player1.symbol;
                    int val = minimax(depth + 1, true);
                    board[i][j].value = ' ';
                    if (val < best) best = val;
                }
            }
        }
        return best;
    }
}

// Perfect AI move using minimax
void makeAIMove() {
    if (!gameActive) return;

    int bestVal = -10000;
    int bestRow = -1, bestCol = -1;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j].value == ' ') {
                board[i][j].value = player2.symbol;
                int moveVal = minimax(0, false);
                board[i][j].value = ' ';

                if (moveVal > bestVal) {
                    bestRow = i;
                    bestCol = j;
                    bestVal = moveVal;
                }
            }
        }
    }

    if (bestRow != -1 && bestCol != -1) {
        board[bestRow][bestCol].value = player2.symbol;
        moveCount++;
        currentPlayer = &player1;
    }
}

// -------------------- MAIN --------------------
int main() {
    srand((unsigned int)time(NULL));
    InitWindow(500, 650, "Tic-Tac-Toe GUI");
    SetTargetFPS(60);

    // Initialize default player1 name to something until login
    strcpy(player1.name, "Player1");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Vector2 mouse = GetMousePosition();

        // -------------------- INPUT HANDLING --------------------
        // Read characters typed this frame
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125) {   // printable
                switch (activeField) {
                    case FIELD_LOGIN_USER:
                        if (strlen(loginUser) < MAX_LEN - 1) {
                            size_t len = strlen(loginUser);
                            loginUser[len] = (char)key;
                            loginUser[len + 1] = '\0';
                        }
                        break;
                    case FIELD_LOGIN_PASS:
                        if (strlen(loginPass) < MAX_LEN - 1) {
                            size_t len = strlen(loginPass);
                            loginPass[len] = (char)key;
                            loginPass[len + 1] = '\0';
                        }
                        break;
                    case FIELD_SIGNUP_USER:
                        if (strlen(signupUser) < MAX_LEN - 1) {
                            size_t len = strlen(signupUser);
                            signupUser[len] = (char)key;
                            signupUser[len + 1] = '\0';
                        }
                        break;
                    case FIELD_SIGNUP_PASS:
                        if (strlen(signupPass) < MAX_LEN - 1) {
                            size_t len = strlen(signupPass);
                            signupPass[len] = (char)key;
                            signupPass[len + 1] = '\0';
                        }
                        break;
                    case FIELD_SIGNUP_CONFIRM:
                        if (strlen(signupConfirm) < MAX_LEN - 1) {
                            size_t len = strlen(signupConfirm);
                            signupConfirm[len] = (char)key;
                            signupConfirm[len + 1] = '\0';
                        }
                        break;
                    case FIELD_PLAYER2_NAME:
                        if (strlen(player2Name) < MAX_LEN - 1) {
                            size_t len = strlen(player2Name);
                            player2Name[len] = (char)key;
                            player2Name[len + 1] = '\0';
                        }
                        break;
                    case FIELD_SYMBOL_P1:
                        symbolPlayer1 = (char)key;
                        break;
                    case FIELD_SYMBOL_P2:
                        symbolPlayer2 = (char)key;
                        break;
                    default:
                        break;
                }
            }
            key = GetCharPressed();
        }

        // Handle backspace to erase in active field
        if (IsKeyPressed(KEY_BACKSPACE)) {
            switch (activeField) {
                case FIELD_LOGIN_USER:
                    if (strlen(loginUser) > 0) loginUser[strlen(loginUser) - 1] = '\0';
                    break;
                case FIELD_LOGIN_PASS:
                    if (strlen(loginPass) > 0) loginPass[strlen(loginPass) - 1] = '\0';
                    break;
                case FIELD_SIGNUP_USER:
                    if (strlen(signupUser) > 0) signupUser[strlen(signupUser) - 1] = '\0';
                    break;
                case FIELD_SIGNUP_PASS:
                    if (strlen(signupPass) > 0) signupPass[strlen(signupPass) - 1] = '\0';
                    break;
                case FIELD_SIGNUP_CONFIRM:
                    if (strlen(signupConfirm) > 0) signupConfirm[strlen(signupConfirm) - 1] = '\0';
                    break;
                case FIELD_PLAYER2_NAME:
                    if (strlen(player2Name) > 0) player2Name[strlen(player2Name) - 1] = '\0';
                    break;
                default:
                    break;
            }
        }

        // -------------------- GUI STATES --------------------
        switch (state) {
            case STATE_LOGIN: {
                DrawText("Login (TAB to Signup)", 50, 20, 20, BLACK);

                // Labels
                DrawText("Username:", 50, 80, 20, BLACK);
                DrawText("Password:", 50, 150, 20, BLACK);

                Rectangle userBox = {50, 110, 200, 30};
                Rectangle passBox = {50, 180, 200, 30};
                DrawRectangleRec(userBox, activeField == FIELD_LOGIN_USER ? GRAY : LIGHTGRAY);
                DrawRectangleRec(passBox, activeField == FIELD_LOGIN_PASS ? GRAY : LIGHTGRAY);

                DrawText(loginUser, 55, 115, 20, BLACK);
                char masked[MAX_LEN] = "";
                for (int i = 0; i < (int)strlen(loginPass); i++) masked[i] = '*';
                masked[strlen(loginPass)] = '\0';
                DrawText(masked, 55, 185, 20, BLACK);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointRec(mouse, userBox)) activeField = FIELD_LOGIN_USER;
                    else if (CheckCollisionPointRec(mouse, passBox)) activeField = FIELD_LOGIN_PASS;
                }

                DrawText("Press ENTER to login", 50, 250, 15, DARKGRAY);
                DrawText(statusText, 50, 280, 20, RED);
                if (IsKeyPressed(KEY_TAB)) {
                    // Clear status and switch
                    strcpy(statusText, "");
                    state = STATE_SIGNUP;
                    activeField = FIELD_SIGNUP_USER;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    if (loginValidate()) {
                        state = STATE_MODE;
                        strcpy(statusText, "");
                    } else strcpy(statusText, "Invalid credentials!");
                }
                break;
            }

            case STATE_SIGNUP: {
                DrawText("Signup (TAB to Login)", 50, 20, 20, BLACK);

                // Labels
                DrawText("Username:", 50, 80, 20, BLACK);
                DrawText("Password:", 50, 150, 20, BLACK);
                DrawText("Confirm Password:", 50, 220, 20, BLACK);

                Rectangle sUserBox = {50, 110, 200, 30};
                Rectangle sPassBox = {50, 180, 200, 30};
                Rectangle sConfirmBox = {50, 250, 200, 30};
                DrawRectangleRec(sUserBox, activeField == FIELD_SIGNUP_USER ? GRAY : LIGHTGRAY);
                DrawRectangleRec(sPassBox, activeField == FIELD_SIGNUP_PASS ? GRAY : LIGHTGRAY);
                DrawRectangleRec(sConfirmBox, activeField == FIELD_SIGNUP_CONFIRM ? GRAY : LIGHTGRAY);

                DrawText(signupUser, 55, 115, 20, BLACK);
                char maskedPass[MAX_LEN] = "";
                for (int i = 0; i < (int)strlen(signupPass); i++) maskedPass[i] = '*';
                maskedPass[strlen(signupPass)] = '\0';
                DrawText(maskedPass, 55, 185, 20, BLACK);

                char maskedConfirm[MAX_LEN] = "";
                for (int i = 0; i < (int)strlen(signupConfirm); i++) maskedConfirm[i] = '*';
                maskedConfirm[strlen(signupConfirm)] = '\0';
                DrawText(maskedConfirm, 55, 255, 20, BLACK);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointRec(mouse, sUserBox)) activeField = FIELD_SIGNUP_USER;
                    else if (CheckCollisionPointRec(mouse, sPassBox)) activeField = FIELD_SIGNUP_PASS;
                    else if (CheckCollisionPointRec(mouse, sConfirmBox)) activeField = FIELD_SIGNUP_CONFIRM;
                }

                DrawText("Press ENTER to signup", 50, 300, 15, DARKGRAY);
                DrawText(statusText, 50, 330, 20, RED);
                if (IsKeyPressed(KEY_TAB)) {
                    strcpy(statusText, "");
                    state = STATE_LOGIN;
                    activeField = FIELD_LOGIN_USER;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    if (signupValidate()) {
                        strcpy(statusText, "Signup successful! Login.");
                        // move to login and prefill username
                        strcpy(loginUser, signupUser);
                        strcpy(signupUser, "");
                        strcpy(signupPass, "");
                        strcpy(signupConfirm, "");
                        state = STATE_LOGIN;
                        activeField = FIELD_LOGIN_PASS;
                    } else strcpy(statusText, "Signup failed! Exists or mismatch.");
                }
                break;
            }

            case STATE_MODE: {
                DrawText("Select Mode:", 50, 20, 20, BLACK);
                Rectangle friendBtn = {150, 100, 200, 50};
                Rectangle aiBtn = {150, 200, 200, 50};
                DrawRectangleRec(friendBtn, LIGHTGRAY); DrawText("Play with Friend", 160, 115, 20, BLACK);
                DrawRectangleRec(aiBtn, LIGHTGRAY); DrawText("Play with Computer", 160, 215, 20, BLACK);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointRec(mouse, friendBtn)) { isAIMode = false; state = STATE_PLAYER2; activeField = FIELD_PLAYER2_NAME; }
                    else if (CheckCollisionPointRec(mouse, aiBtn)) { isAIMode = true; state = STATE_SYMBOLS; activeField = FIELD_SYMBOL_P1; strcpy(player2Name, "Computer"); }
                }
                break;
            }

            case STATE_PLAYER2: {
                DrawText("Enter Player 2 Name:", 50, 20, 20, BLACK);
                Rectangle p2Box = {50, 60, 200, 30};
                DrawRectangleRec(p2Box, activeField == FIELD_PLAYER2_NAME ? GRAY : LIGHTGRAY);
                DrawText(player2Name, 55, 65, 20, BLACK);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) if (CheckCollisionPointRec(mouse, p2Box)) activeField = FIELD_PLAYER2_NAME;
                DrawText("Press ENTER when done", 50, 110, 15, DARKGRAY);
                if (IsKeyPressed(KEY_ENTER)) state = STATE_SYMBOLS;
                break;
            }

            case STATE_SYMBOLS: {
                DrawText("Choose Symbols:", 20, 20, 20, BLACK);
                Rectangle symP1Box = {200, 75, 50, 30};
                Rectangle symP2Box = {200, 115, 50, 30};
                DrawRectangleRec(symP1Box, activeField == FIELD_SYMBOL_P1 ? GRAY : LIGHTGRAY);
                DrawRectangleRec(symP2Box, activeField == FIELD_SYMBOL_P2 ? GRAY : LIGHTGRAY);
                char p1Text[2] = { symbolPlayer1, '\0' };
                char p2Text[2] = { symbolPlayer2, '\0' };
                DrawText(TextFormat("%s Symbol:", player1.name), 50, 80, 20, BLACK);
                DrawText(p1Text, symP1Box.x + 15, symP1Box.y + 5, 20, BLACK);
                DrawText(TextFormat("%s Symbol:", isAIMode ? "Computer" : player2Name), 50, 120, 20, BLACK);
                DrawText(p2Text, symP2Box.x + 15, symP2Box.y + 5, 20, BLACK);
                DrawText("Click box and type one character. ENTER to start.", 20, 160, 15, DARKGRAY);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointRec(mouse, symP1Box)) activeField = FIELD_SYMBOL_P1;
                    else if (CheckCollisionPointRec(mouse, symP2Box)) activeField = FIELD_SYMBOL_P2;
                    else activeField = FIELD_NONE;
                }

                // Characters handled earlier — change symbol if active
                if (IsKeyPressed(KEY_ENTER)) {
                    // Prevent same symbols
                    if (symbolPlayer1 == symbolPlayer2) {
                        strcpy(statusText, "Symbols must differ!");
                    } else {
                        setupPlayersGUI();
                        state = STATE_GAME;
                        strcpy(statusText, "");
                        activeField = FIELD_NONE;
                    }
                }
                DrawText(statusText, 20, 180, 18, RED);
                break;
            }

            case STATE_GAME: {
                DrawText(statusText, 20, 100, 20, BLACK);
                drawBoard();

                if (gameActive && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    for (int i = 0; i < 3; i++)
                        for (int j = 0; j < 3; j++) {
                            if (CheckCollisionPointRec(mouse, board[i][j].rect) && board[i][j].value == ' ') {
                                board[i][j].value = currentPlayer->symbol;
                                moveCount++;
                                char winner = checkWinner();
                                if (winner != ' ') {
                                    const char* winName = winner == player1.symbol ? player1.name : player2.name;
                                    sprintf(statusText, "%s wins!", winName);
                                    strcpy(lastWinner, winName);
                                    // update scores
                                    if (winner == player1.symbol) player1.score++;
                                    else if (winner == player2.symbol) player2.score++;
                                    saveHistory(winName);
                                    gameActive = false;
                                    state = STATE_POSTGAME;
                                } else if (isBoardFull()) {
                                    strcpy(statusText, "Draw!");
                                    strcpy(lastWinner, "Draw");
                                    saveHistory("Draw");
                                    gameActive = false;
                                    state = STATE_POSTGAME;
                                } else currentPlayer = currentPlayer == &player1 ? &player2 : &player1;
                            }
                        }
                }

                if (gameActive && isAIMode && currentPlayer == &player2) {
                    makeAIMove();
                    char winner = checkWinner();
                    if (winner != ' ') {
                        sprintf(statusText, "%s wins!", player2.name);
                        strcpy(lastWinner, player2.name);
                        // update score for computer
                        if (winner == player2.symbol) player2.score++;
                        else if (winner == player1.symbol) player1.score++;
                        saveHistory(player2.name);
                        gameActive = false;
                        state = STATE_POSTGAME;
                    } else if (isBoardFull()) {
                        strcpy(statusText, "Draw!");
                        strcpy(lastWinner, "Draw");
                        saveHistory("Draw");
                        gameActive = false;
                        state = STATE_POSTGAME;
                    } else currentPlayer = &player1;
                }
                break;
            }

            case STATE_POSTGAME: {
                DrawText("Game Over!", 50, 50, 20, BLACK);
                DrawText(statusText, 50, 90, 20, RED);

                DrawText(TextFormat("%s: %d", player1.name, player1.score), 50, 140, 20, BLACK);
                DrawText(TextFormat("%s: %d", player2.name, player2.score), 50, 170, 20, BLACK);

                Rectangle replayBtn = {100, 220, 300, 50};
                Rectangle historyBtn = {100, 300, 300, 50};
                DrawRectangleRec(replayBtn, LIGHTGRAY); DrawText("Play Again", 200, 235, 20, BLACK);
                DrawRectangleRec(historyBtn, LIGHTGRAY); DrawText("View History", 200, 315, 20, BLACK);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointRec(mouse, replayBtn)) {
                        resetBoard();
                        state = STATE_GAME;
                        strcpy(statusText, "");
                    } else if (CheckCollisionPointRec(mouse, historyBtn)) state = STATE_HISTORY;
                }
                break;
            }

            case STATE_HISTORY: {
                drawHistory(lastWinner);
                break;
            }

            default: break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
