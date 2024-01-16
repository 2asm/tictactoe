#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>

std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

enum GameState {
    INPROGRESS,
    XWON,
    OWON,
    DRAW,
};

// X plays first
enum Symbol {
    EMPTY,
    X,
    O,
};

std::string print_symbol(Symbol symbol) {
    switch (symbol) {
    case X:
        return "X";
    case O:
        return "O";
    case EMPTY:
        return " ";
    default:
        return " ";
    }
}

struct Cell {
    Symbol symbol;
    size_t x, y;
    // Cell() {}
    Cell(size_t _x = -1, size_t _y = -1, Symbol _type = EMPTY)
        : x(_x), y(_y), symbol(_type) {}
};

struct Board {
    size_t dim;
    std::vector<std::vector<Cell>> grid;
    Board() {}
    Board(size_t _dim): dim(_dim) {
        assert(_dim >= 3 and dim%2==1);
        grid = std::vector<std::vector<Cell>>(dim, std::vector<Cell>(dim));
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                grid[i][j] = Cell(i, j, EMPTY);
            }
        }
    }

    size_t GetDim() { return dim; }
    void SetSymbol(size_t x, size_t y, Symbol symbol) {
        assert(x >= 0 and x < dim);
        assert(y >= 0 and y < dim);
        assert(grid[x][y].symbol == EMPTY);
        grid[x][y].symbol = symbol;
    }

    void Print() {
        std::cout << std::endl;
        for (int i = 0; i < dim; i++) {

            std::cout << " ";
            for (int j = 0; j < dim; j++) {
                if (j > 0) {
                    std::cout << " │ " << print_symbol(grid[i][j].symbol) << " ";
                } else {
                    std::cout << "   " << print_symbol(grid[i][j].symbol);
                }
            }
            std::cout << "  ";

            std::cout << std::endl;

            std::cout << " ";
            if (i != dim - 1) {
                std::cout << " ";
                for (int j = 0; j < dim; j++) {
                    std::cout << "────";
                    if (j != dim - 1)
                        std::cout << "┼";
                }
                std::cout << " ";
            }
            std::cout << " ";

            std::cout << std::endl;
        }
    }

    GameState GetState() {
        if (chekcRows(X) or chekcCols(X) or checkDiagnals(X))
            return XWON;
        if (chekcRows(O) or chekcCols(O) or checkDiagnals(O))
            return OWON;
        int empty = 0;
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                empty += grid[i][j].symbol == EMPTY;
            }
        }
        if (empty == 0)
            return DRAW;
        return INPROGRESS;
    }

  private:
    bool chekcRows(Symbol symbol) {
        for (int i = 0; i < dim; i++) {
            bool ok = true;
            for (int j = 0; j < dim; j++) {
                ok &= grid[i][j].symbol == symbol;
            }
            if (ok)
                return true;
        }
        return false;
    }

    bool chekcCols(Symbol symbol) {
        for (int j = 0; j < dim; j++) {
            bool ok = true;
            for (int i = 0; i < dim; i++) {
                ok &= grid[i][j].symbol == symbol;
            }
            if (ok)
                return true;
        }
        return false;
    }

    bool checkDiagnals(Symbol symbol) {
        bool ok = true;
        for (int i = 0, j = 0; i < dim; i++, j++) {
            ok &= grid[i][j].symbol == symbol;
        }
        if (ok)
            return true;
        ok = true;
        for (int i = 0, j = dim - 1; i < dim; i++, j--) {
            ok &= grid[i][j].symbol == symbol;
        }
        return ok;
    }
};

struct Player {
    std::string name;
    Symbol symbol;
    bool bot;
    Player() {}
    Player(std::string _name, Symbol _symbol = EMPTY, bool _bot=false)
        : name(_name), symbol(_symbol), bot(_bot) {}
    std::pair<int,int> botMove(Board const& b) {
        std::vector<std::pair<int,int>> ps;
        for(int i=0;i<b.dim;i++) {
            for(int j=0;j<b.dim;j++){
                if(b.grid[i][j].symbol == EMPTY) {
                    ps.push_back(std::make_pair(i,j));
                }
            }
        }
        int ind = rng()%ps.size();
        return ps[ind];
    }
};

struct TicTacToe {
    Player p1, p2;
    Board board;
    TicTacToe() {}
    TicTacToe(size_t dim, std::string playerName) {
        board = Board(dim);
        bool x = rng() % 2;
        p1 = Player(playerName, x ? X : O);
        p2 = Player("bot", x ? O : X, true);
    }
    void Play() {
        Player current_player = (p1.symbol == X) ? p1 : p2;
        board.Print();
        while (true) {

            size_t x, y;
            std::cout << current_player.name << "(" << print_symbol(current_player.symbol) << ")" << " -> ";
            if(current_player.bot) {
                auto p = current_player.botMove(board);
                x = p.first;
                y = p.second;
                int t_factor = rng()%10;
                // microseconds
                usleep(t_factor*100'000);
                std::cout << x << " " << y << std::endl;
            } else {
                while(true) {
                    std::cin >> x >> y;
                    if(x<board.dim and x>=0 and y<board.dim and y>=0 and board.grid[x][y].symbol == EMPTY) break;
                    std::cout << "Error: 0<x<" << board.dim << " and 0<y<" << board.dim << " and cell should be empty."<< std::endl;
                    std::cout << current_player.name << "(" << print_symbol(current_player.symbol) << ")" << " -> ";
                }
            }
            board.SetSymbol(x, y, current_player.symbol);
            board.Print();

            switch (board.GetState()) {
            case INPROGRESS:
                break;
            case XWON:
                std::cout << current_player.name << " WON!!!" << std::endl;
                return;
            case OWON:
                std::cout << current_player.name << " WON!!!" << std::endl;
                return;
            case DRAW:
                std::cout << "DRAW!!!" << std::endl;
                return;
            }

            // turn player
            // swap
            if (current_player.symbol == p1.symbol)
                current_player = p2;
            else
                current_player = p1;
        }
    }
};

int main() {
    std::cout << "Tic Tac Toe with Random Bot!" << std::endl;
    TicTacToe tictac(3, "dilip");
    tictac.Play();
    return 0;
}
