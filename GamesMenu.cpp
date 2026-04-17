/**
 * @file  GamesMenu.cpp
 * @brief Entry point for Games Menu.
 */

#include <iostream> 
#include <ctime>    
#include <limits>
#include <cstdlib>
#include "BoardGame_Classes.h"
#include "Games_Classes.h" 
using namespace std;


int GetTrueInput(int from, int to) {// function to get a valid game number
  int number;
  cin >> number;

  while (true) {
    if (cin.fail()) {
      cin.clear();
      cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      cout << "This option is not available, please try again." << endl;
      cin >> number;
    }
    else if (number < from || number > to) {
      cout << "This option is not available, please Enter a number from: " << from << " to " << to << endl;
      cin >> number;
    }
    else {
      break;
    }
  }
  return number;
}


int main() {
    while (true) {
        cout << endl;
        cout << "\033[96m";
        cout << "-----------------------\n";
        cout << "|      Games Menu      |\n";
        cout << "-----------------------\n";
        cout << endl;

        cout << "7. 4x4 Tic-Tac-Toe\n";
        cout << endl;

        cout << "8. Pyramid Tic-Tac-Toe\n";
        cout << endl;

        cout << "9. Numerical Tic Tac Toe\n";
        cout << endl;

        cout << "10. Obstacles Tic-Tac-Toe\n";
        cout << endl;

        cout << "\033[0m";

        cout << "Choose a number: " << endl;

        int number = GetTrueInput(7, 10);
        cout << "\n";

        switch (number) {
case 7: {
            srand(static_cast<unsigned int>(time(0)));
            UI<char>* fourbyfour_ui = new FourByFour_UI();
            Board<char>* fourbyfour_board = new FourByFour_Board();
            Player<char>** fourbyfour_players = fourbyfour_ui->setup_players();

			cout << "\033[93m";
            cout << "Rules: Players take turns moving one of their tokens to an adjacent (horizontal or vertical) empty square.\n"
                " Tokens cannot move diagonally or jump over other tokens.\n"
                " Winning Condition: The first player to align three of their tokens in a row, column, or diagonal wins.\n";
			cout << "\033[0m";

            GameManager<char> fourbyfour_game(fourbyfour_board, fourbyfour_players, fourbyfour_ui);
            fourbyfour_game.run();
            // --- Cleanup ---
            delete fourbyfour_board;
            for (int i = 0; i < 2; ++i) {
                delete fourbyfour_players[i];
            }
            delete[] fourbyfour_players;
            delete fourbyfour_ui;
            break;
        }

case 8: {
            srand(static_cast<unsigned int>(time(0)));
            UI<char>* pyramid_ui = new Pyramid_UI();
            Board<char>* pyramid_board = new Pyramid_Board();
            Player<char>** pyramid_players = pyramid_ui->setup_players();

            cout << "\033[93m";
            cout << " Rules:Players take turns placing their 'X' or 'O' marks in empty squares.\n";
            cout << " WinningCondition: The first player to align three of their marks horizontally, \n vertically, or diagonally wins.\n";
			cout << "\033[0m";

            GameManager<char> pyramid_game(pyramid_board, pyramid_players, pyramid_ui);
            pyramid_game.run();
            // --- Cleanup ---
            delete pyramid_board;
            for (int i = 0; i < 2; ++i) {
                delete pyramid_players[i];
            }
            delete[] pyramid_players;
            delete pyramid_ui;
            break;
        }

case 9: {
            srand(static_cast<unsigned int>(time(0)));

            UI<int>* game_ui = new Numerical_UI();

            Board<int>* numerical_board = new Numerical_Board();

            Player<int>** players = game_ui->setup_players();

            cout << "\033[93m";
            cout << " Gam Rules: \nPlayer 1 uses odd numbers (1, 3, 5, 7, 9) and Player 2 uses even numbers (2, 4, 6, 8).\n"
                " Players alternate placing one number in an empty cell, and each number can only be used once.\n"
                " Winning Condition: A player wins by placing three numbers in a row, column, or diagonal that add up to\n"
                " exactly 15. If all cells are filled and no line sums to 15, the game is a draw.\n";
			cout << "\033[0m";

            GameManager<int> numerical_game(numerical_board, players, game_ui);

            numerical_game.run();

            // --- Cleanup ---
            delete numerical_board;
            for (int i = 0; i < 2; ++i) {
                delete players[i];
            }
            delete[] players;
            delete game_ui;

            break;
        }

case 10: {
            srand(static_cast<unsigned int>(time(0)));
            UI<char>* Obstacle_ui = new Obstacles_UI();
            Board<char>* Obstacle_board = new Obstacles_Board();
            Player<char>** Obstacle_players = Obstacle_ui->setup_players();

			cout << "\033[93m";
            cout << " Game Rules:\n After every round (one turn for each player), two new obstacle cells are randomly added to the\n "
                " board. These cells cannot be used by either player.\n"
                " WinningCondition: The first player to align four of their marks in a row (horizontally, vertically, or\n "
                " diagonally) wins. The game is a draw if the board fills without a winner.\n";
			cout << "\033[0m";

            GameManager<char> Obstacle_game(Obstacle_board, Obstacle_players, Obstacle_ui);
            Obstacle_game.run();

            // --- Cleanup ---
            delete Obstacle_board;
            for (int i = 0; i < 2; ++i) {
                delete Obstacle_players[i];
            }
            delete[] Obstacle_players;
            delete Obstacle_ui;
            break;
        }

        default:
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}
