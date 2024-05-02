#include <bits/stdc++.h>
#include <windows.h>

void TextColor(int x)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, x);
}

// class Board 

class Board {
private:
    std::vector<char> squares;

public:
    Board();
    bool isFull() const;
    void makeMove(int position, char token);
    bool checkWin(char token) const;
    void displayBoard() const;
    bool isSquareEmpty(int position) const;
    char getSquare(int position) const;
};

// phat trien class Board

Board::Board() : squares(9, ' ') {}

bool Board::isFull() const {
    for (char square : squares) {
        if (square == ' ') {
            return false;
        }
    }
    return true;
}

void Board::makeMove(int position, char token) {
    if (position < 0 || position >= squares.size())
        throw std::out_of_range("Chon vi tri tu (1-9).");

    if (!isSquareEmpty(position))
        throw std::invalid_argument("Vi tri da dc chon.");

    squares[position] = token;
}

bool Board::checkWin(char token) const {
    const int LOGIC_WIN[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        {0, 4, 8}, {2, 4, 6}
    };

    for (const auto &combo : LOGIC_WIN) {
        if (squares[combo[0]] == token && squares[combo[1]] == token && squares[combo[2]] == token)
            return true;
    }
    return false;
}

void Board::displayBoard() const {
    std::cout << "\n";
    for (size_t i = 0; i < squares.size(); ++i) {
       if(isSquareEmpty(i)) std::cout << i + 1;
       else if(squares[i] == 'X'){
        TextColor(3);
        std::cout << squares[i];
        TextColor(7);
       }
       else {
        TextColor(12);
        std::cout << squares[i];
        TextColor(7);
       }
        if (i % 3 == 2) {
            if (i < squares.size() - 1) std::cout << "\n-----\n";
        } else {
            std::cout << "|";
        }
    }
    std::cout << "\n";
}

bool Board::isSquareEmpty(int position) const {
    return squares[position] == ' ';
}

char Board::getSquare(int position) const {
    return squares[position];
}

// class Player 

class Player {
public:
    char token;

    explicit Player(char token);
    virtual int getMove(const Board& board) = 0;
    char getToken() const;
};

// phat trien class Player 

Player::Player(char token) : token(token) {}

char Player::getToken() const { return token; }

// class HumanPlayer

class HumanPlayer : public Player {
public:
    using Player::Player;
    int getMove(const Board& board) override;
};

// phat trien class HumanPlayer

int HumanPlayer::getMove(const Board& board) {
    int move;

    while (true) {
        std::cout << " chon vi tri tu (1-9): ";
        std::cin >> move;
        move--; 

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Ban da nhap sai. Moi ban chon 1 vi tri tu (1-9).\n";
            continue;
        }

        while (move < 0 || move >= 9) {
            std::cout << "Ban phai chon 1 vi tri tu (1-9). Moi ban chon lai: ";
            std::cin >> move;
            move--;            
        }

        while (!board.isSquareEmpty(move)) {
            std::cout << "Vi tri do da dc chon. Moi ban chon lai 1 vi tri tu (1-9):";
            std::cin >> move;
            move--;            
        }

        return move;
    }
}

// class ComputerPlayer

class ComputerPlayer : public Player {
    int hardness;

public:
    ComputerPlayer(char token, int hardness);
    int minimax(const Board& board, int depth, int alpha, int beta, bool isMaximizing);
    int getMoveEasy(const Board& board);
    int getMoveMedium(const Board& board); 
    int getMoveHard(const Board& board);
    int getMove(const Board& board) override;
};

// phat trien class ComputerPlayer

ComputerPlayer::ComputerPlayer(char token, int hardness): hardness(hardness), Player(token) {}

// thuat toan minimax

int ComputerPlayer::minimax(const Board& board, int depth, int alpha, int beta, bool isMaximizing) {
    if (board.checkWin(token)) {
        return 1;
    } else if (board.checkWin((token == 'X') ? 'O' : 'X')) {
        return -1;
    } else if (board.isFull()) {
        return 0;
    }
	// cat tia beta
    if (isMaximizing) {
        int bestScore = -1000;
        for (int i = 0; i < 9; ++i) {
            if (board.isSquareEmpty(i)) {
                Board newBoard = board;
                newBoard.makeMove(i, token);
                int score = minimax(newBoard, depth + 1, alpha, beta, false);
                bestScore = std::max(score, bestScore);
                alpha = std::max(alpha, bestScore);
                if (beta <= alpha)
                    break; 
            }
        }
        return bestScore;
    } else { // cat tia alpha
        int bestScore = 1000;
        for (int i = 0; i < 9; ++i) {
            if (board.isSquareEmpty(i)) {
                Board newBoard = board;
                newBoard.makeMove(i, (token == 'X') ? 'O' : 'X');
                int score = minimax(newBoard, depth + 1, alpha, beta, true);
                bestScore = std::min(score, bestScore);
                beta = std::min(beta, bestScore);
                if (beta <= alpha)
                    break; 
            }
        }
        return bestScore;
    }
}

// che do Easy

int ComputerPlayer::getMoveEasy(const Board& board) {
    int move;
    srand(time(0));
    do {
        move = rand() % 9;
    } while (!board.isSquareEmpty(move));

    return move;
}

// che do Medium

int ComputerPlayer::getMoveMedium(const Board& board) {
    int move = 11;
    if (board.isSquareEmpty(4)) move = 4;
    else {
        // check hang ngang
        for (int i = 0; i < 9; i += 3) {
            if (board.getSquare(i) == board.getSquare(i + 1) && board.isSquareEmpty(i + 2) && !board.isSquareEmpty(i))
                move = i + 2;
            else if (board.getSquare(i + 1) == board.getSquare(i + 2) && board.isSquareEmpty(i) && !board.isSquareEmpty(i + 1))
                move = i;
            else if (board.getSquare(i) == board.getSquare(i + 2) && board.isSquareEmpty(i + 1) && !board.isSquareEmpty(i))
                move = i + 1;
        }
        // check hang doc
        if (move == 11) {
            for (int i = 0; i < 3; i ++) {
                if (board.getSquare(i) == board.getSquare(i + 3) && board.isSquareEmpty(i + 6) && !board.isSquareEmpty(i))
                    move = i + 6;
                else if (board.getSquare(i + 3) == board.getSquare(i + 6) && board.isSquareEmpty(i) && !board.isSquareEmpty(i + 3))
                    move = i;
                else if (board.getSquare(i) == board.getSquare(i + 6) && board.isSquareEmpty(i + 3) && !board.isSquareEmpty(i))
                    move = i + 3;
            }
        }
        // check duong cheo chinh
        if (move == 11) {
            if (board.getSquare(0) == board.getSquare(4) && board.isSquareEmpty(8)) move = 8;
            else if (board.getSquare(4) == board.getSquare(8) && board.isSquareEmpty(0)) move = 0;
        }
        // check duong cheo phu
        if (move == 11) {
            if (board.getSquare(2) == board.getSquare(4) && board.isSquareEmpty(6)) move = 6;
            else if (board.getSquare(4) == board.getSquare(6) && board.isSquareEmpty(2)) move = 2;
        }
    }

    if (move == 11) return getMoveEasy(board);

    else return move;
}

// che do Hard

int ComputerPlayer::getMoveHard(const Board& board) {
    int bestMove = -1;
    int bestScore = -1000;
    for (int i = 0; i < 9; ++i) {
        if (board.isSquareEmpty(i)) {
            Board newBoard = board;
            newBoard.makeMove(i, token);
            int score = minimax(newBoard, 0, -1000, 1000, false);
            if (score > bestScore) {
                bestScore = score;
                bestMove = i;
            }
        }
    }
    return bestMove;
}

// ham chon che do

int ComputerPlayer::getMove(const Board& board) {
    if (hardness == 1) return getMoveEasy(board);
    else if (hardness == 2) return getMoveMedium(board);
    else if (hardness == 3) return getMoveHard(board);
    else return getMoveEasy(board);
}

// class Game

class Game {
private:
    Board board;
    Player* player1;
    Player* player2;
    Player* currentPlayer;
    bool isPvP;

    void switchPlayer();

public:
    Game(Player* p1, Player* p2, bool isPvPMode);
    void play();
};

// phat trien class Game

Game::Game(Player* p1, Player* p2, bool isPvPMode) : player1(p1), player2(p2), currentPlayer(p1), isPvP(isPvPMode) {}

void Game::switchPlayer() {
    if (currentPlayer == player1) {
        currentPlayer = player2;
    } else {
        currentPlayer = player1;
    }
}

void Game::play() {
    TextColor(9);
    std::cout << (isPvP ? "PvP Mode" : "PvE Mode");
    TextColor(7);
    int n = 1;
    while (!board.isFull()) {
        board.displayBoard();
        if (!isPvP) {
            if (n % 2 != 0) std::cout << "Moi player " << currentPlayer->getToken();
            else std::cout << "Computer: ";
        } else std::cout << "Moi player " << currentPlayer->getToken();
        int move = currentPlayer->getMove(board);
        if (!isPvP && n % 2 == 0) std::cout << move;
        board.makeMove(move, currentPlayer->getToken());

        if (board.checkWin(currentPlayer->getToken())) {
            board.displayBoard();
            TextColor(2);
            std::cout << "Player " << currentPlayer->getToken() << " wins!\n";
            TextColor(7);
            return;
        }

        switchPlayer();
        n++;
    }
    TextColor(6);
    std::cout << "Hoa`!\n";
    TextColor(7);
}

void check(int &a, double b = 0.1) {
    while (a != 1 && a != 2 && a != b) {
        std::cout << "Khong co lua chon thu " << a << ". Moi ban chon lai: ";
        std::cin >> a;
    }
}

void pickMode(){
    std::cout << "Moi player 1 chon quan co (1 - ";
    TextColor(3);
    std::cout << "X";
    TextColor(7);
    std::cout << ", 2 - ";
    TextColor(12);
    std::cout << "O";
    TextColor(7);
    std::cout << "): ";
}

int main() {
    std::cout << "Chon game mode (1 - PvP, 2 - PvE): ";
    int gameMode;
    std::cin >> gameMode;    
    Player* p1;
    Player* p2;
    check(gameMode);
    if (gameMode == 1) {
        int chess;
        pickMode();
        std::cin >> chess;
        check(chess);
        char chess1, chess2;
        chess1 = (chess == 1) ? 'X' : 'O';
        chess2 = (chess == 1) ? 'O' : 'X';
        p1 = new HumanPlayer(chess1);
        p2 = new HumanPlayer(chess2);
    } else {
        int hardness;
        std::cout << "Chon computer mode(1 -  ";
        TextColor(10);
        std::cout << "easy";
        TextColor(7);
        std::cout << ", 2 - ";
        TextColor(6);
        std::cout << "medium";
        TextColor(7);
        std::cout << ", 3 - ";
        TextColor(4);
        std::cout << "hard";
        TextColor(7);
        std::cout << "): ";
        
        std::cin >> hardness;
        check(hardness, 3);
        int chess;
        pickMode();
        std::cin >> chess;
        check(chess);
        char chess1, chess2;
        chess1 = (chess == 1) ? 'X' : 'O';
        chess2 = (chess == 1) ? 'O' : 'X';
        p1 = new HumanPlayer(chess1);
        p2 = new ComputerPlayer(chess2, hardness);
    }
	
    char playAgain = 'y';
    while(playAgain == 'y' || playAgain == 'Y') {
        Game game(p1, p2, gameMode == 1);
        game.play();

        std::cout << "Ban muon choi lai? (y/n): " << std::endl;
        std::cin >> playAgain;
    }
    TextColor(206);
    std::cout << "Cam on ban da choi!" << std::endl;

    delete p1;
    delete p2;
	
	TextColor(15);
    return 0;
}

