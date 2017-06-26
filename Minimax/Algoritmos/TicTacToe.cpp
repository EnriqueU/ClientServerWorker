#include<bits/stdc++.h>
using namespace std;

char gridChar(int i) {
    switch(i) {
        case -1:
            return 'x';
        case 0:
            return ' ';
        case 1:
            return 'o';
    }
}

void draw(int b[9]) {
    printf(" %c | %c | %c\n",gridChar(b[0]),gridChar(b[1]),gridChar(b[2]));
    printf("---+---+---\n");
    printf(" %c | %c | %c\n",gridChar(b[3]),gridChar(b[4]),gridChar(b[5]));
    printf("---+---+---\n");
    printf(" %c | %c | %c\n",gridChar(b[6]),gridChar(b[7]),gridChar(b[8]));
}

int win(const int board[9]) {
    unsigned wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    int i;
    for(i = 0; i < 8; ++i) {
        if(board[wins[i][0]] != 0 &&
           board[wins[i][0]] == board[wins[i][1]] &&
           board[wins[i][0]] == board[wins[i][2]])
            return board[wins[i][2]];
    }
    return 0;
}

int minimax(int board[9], int player) {
    int winner = win(board);
    if(winner != 0) return winner*player;

    int move = -1;
    int score = -2;
    int i;
    for(i = 0; i < 9; ++i) {
        if(board[i] == 0) {
            board[i] = player;
            int thisScore = -minimax(board, player*-1);
            if(thisScore > score) {
                score = thisScore;
                move = i;
            }
            board[i] = 0;
        }
    }
    if(move == -1) return 0;
    return score;
}

void computerMoveO(int board[9]) {
    int move = -1;
    int score = -2;
    int i;
    for(i = 0; i < 9; ++i) {
        if(board[i] == 0) { // si esta vacio
            board[i] = 1;
            int tempScore = -minimax(board, -1);
            board[i] = 0;
            if(tempScore > score) {
                score = tempScore;
                move = i;
            }
        }
    }
    board[move] = 1;
}

void computerMoveX(int board[9]) {
    int move = -1;
    int score = -2;
    int i;
    for(i = 0; i < 9; ++i) {
        if(board[i] == 0) { // si esta vacio
            board[i] = -1;
            int tempScore = -minimax(board, 1);
            board[i] = 0;
            if(tempScore > score) {
                score = tempScore;
                move = i;
            }
        }
    }
    board[move] = -1;
}

void playerMove(int board[9]) {
    int move = 0;
    do {
        printf("\nInput move ([0..8]): ");
        scanf("%d", &move);
        printf("\n");
    } while (move >= 9 || move < 0 && board[move] == 0);
    board[move] = -1;
}

int main() {
    int board[9] = {0,0,0,0,0,0,0,0,0}, opc;
    cout << "<<<<<<<<<<<<<<<Menú>>>>>>>>>>>>>>>" << '\n';
    cout << "1. 2 jugadores PC" << endl;
    cout << "2. 1 jugador PC y tú" << endl;
    cin >> opc;
    unsigned turn;
    switch (opc) {
      case 1:
        for(turn = 0; turn < 9 && win(board) == 0; ++turn) {
            if( turn% 2 == 0){
              cout << endl << "PC 2" << endl;
              computerMoveO(board); // turno de la maquina
              draw(board);
            } else {
              cout << endl << "PC 1" << endl;
              computerMoveX(board); // turno de la maquina
              draw(board);
              //draw(board);
              //playerMove(board); // mi turno
            }
        }
      break;
      case 2:
        printf("Computadora: O, Tú: X\nJugador(1) o (2)? ");
        int player=0;
        scanf("%d",&player);
        if(player==1) draw(board);
        for(turn = 0; turn < 9 && win(board) == 0; ++turn) {
            if((turn+player) % 2 == 0){
              cout << endl << "PC 2" << endl;
              computerMoveO(board); // turno de la maquina
              draw(board);
            } else {
              cout << endl << "PC 1" << endl;
              playerMove(board); // mi turno
              draw(board);
            }
        }

    }
    switch(win(board)) {
        case 0:
            printf("Empate.\n");
            break;
        case 1:
            draw(board);
            printf("Perdiste.\n");
            break;
        case -1:
            printf("Ganaste!\n");
            break;
    }
}
