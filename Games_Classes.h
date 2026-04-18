/**
 * @file Games_Classes.h
 * @brief Declarations for multiple board game classes and their UIs used in the project.
 *
 * @details
 * This header contains class declarations for a variety of board games (Pyramid,
 * Numerical Tic-Tac-Toe, Memory board and supporting UI classes).  
 * Many boards expose small helpers for AI,
 * minimax evaluation, and convenience getters/setters used by the UI and AI implementations.
 *
 * @note Designed to be used with the project's `BoardGame_Classes.h` which defines the
 * base `Board<T>`, `UI<T>`, `Player<T>` and `Move<T>` templates/structures.
 *
 * @warning Keep declarations in sync with the implementations provided in `Games_Classes.cpp`.
 *
 * @author Project
 * @version 1.0
 * @date 2025-12-10
 */

#ifndef Games_CLASSES_H
#define Games_CLASSES_H

#include "BoardGame_Classes.h"
#include <vector>
#include <string>
#include <utility>
#include <queue>
#include <tuple>
#include <limits>
#include <unordered_set>
#include <fstream>

using namespace std;

//==================================================== 1.4x4 Tic-Tac-Toe Classes =====================================================
/**
 * @class FourByFour_Board
 * @brief A 4x4 variant where tokens are moved (not placed); initial tokens are pre-seeded.
 *
 * @details The board permits moving an existing token to an adjacent empty cell. Several
 * helpers are provided to enumerate and apply moves for AI searches.
 */
class FourByFour_Board : public Board<char> {
private:
  char blank_symbol = ' ';

  /// Position chosen (by UI) for the token that will be moved.
  pair<int, int> choice_token_toMove_position{ 0, 0 };

public:
  FourByFour_Board();

  bool update_board(Move<char>* move) override;
  bool is_win(Player<char>* player) override;
  bool is_lose(Player<char>*) override { return false; };
  bool is_draw(Player<char>* player) override;
  bool game_is_over(Player<char>* player) override;

  /**
   * @brief Prompt / obtain token to move for a human player.
   * @param player Player who is moving a token.
   * @return Pair {row,col} chosen token position.
   */
  pair<int, int> get_token_place_forH(Player<char>* player);

  /**
   * @brief Pick a token position that belongs to the computer player.
   * @param player Computer player.
   * @return Pair {row,col} chosen token position.
   */
  pair<int, int> get_token_place_forC(Player<char>* player);

  // AI helper ----------------------------------------------------------------

  /**
   * @brief Collect positions of all tokens belonging to `sym`.
   * @param sym Symbol to collect (e.g. 'X' or 'O').
   * @return Vector of positions {row,col}.
   */
  vector<pair<int, int>> get_tokens_positions(char sym) const;

  /**
   * @brief Generate all legal moves for `sym` as (from -> to) pairs.
   * @param sym Symbol to generate moves for.
   * @return Vector of move pairs: { {from_r,from_c} , {to_r,to_c} }.
   */
  vector<pair<pair<int, int>, pair<int, int>>> generate_moves_for(char sym) const;

  /**
   * @brief Apply a `from->to` move pair to the board (used by minimax simulation).
   * @param mv Move pair description.
   */
  void apply_move_pair(const pair<pair<int, int>, pair<int, int>>& mv);

  /**
   * @brief Undo a previously applied `from->to` pair (must match apply_move_pair).
   * @param mv Move pair that was applied.
   */
  void undo_move_pair(const pair<pair<int, int>, pair<int, int>>& mv);

  /** @brief Raw cell getter (const). */
  char get_cell_raw(int r, int c) const { return board[r][c]; }

  /** @brief Raw cell setter. */
  void set_cell_raw(int r, int c, char v) { board[r][c] = v; }

  /**
   * @brief Set the chosen token position to be moved (used by UI prior to update_board).
   * @param p Pair {row,col}.
   */
  void set_choice_token_position(pair<int, int> p) { choice_token_toMove_position = p; }

  /**
   * @brief Basic heuristic evaluation of the board for symbol `sym`.
   * @param sym Symbol to evaluate for.
   * @return Evaluation integer: positive favours `sym`.
   */
  int evaluate_board_for(char sym) const;

  /**
   * @brief Minimax function signature for FourByFour (forward declaration).
   * @param board Pointer to FourByFour_Board being searched.
   * @param depth Current depth.
   * @param maxDepth Maximum depth to search.
   * @param maximizingPlayer True when maximizing.
   * @param maximizingSym Symbol for maximizing player.
   * @param minimizingSym Symbol for minimizing player.
   * @param alpha Alpha value.
   * @param beta Beta value.
   * @return Evaluation score.
   */
  int minimax_fourbyfour(FourByFour_Board* board, int depth, int maxDepth, bool maximizingPlayer, char maximizingSym, char minimizingSym, int alpha, int beta);

  /**
   * @brief Find best move on 4x4 board for `player_symbol`.
   * @param board Board pointer.
   * @param player_symbol Symbol to search moves for.
   * @param maxDepth Maximum search depth (default 6).
   * @return Move pair {from, to} describing the chosen move.
   */
  pair<pair<int, int>, pair<int, int>> find_best_move_fourbyfour(FourByFour_Board* board, char player_symbol, int maxDepth = 6);

};

// fourbyfour UI class --------------------------------------------------------

class FourByFour_UI : public UI<char> {
public:
  FourByFour_UI();
  ~FourByFour_UI() {};

  Player<char>* create_player(string& name, char symbol, PlayerType type);

  Player<char>** setup_players() override;

  /**
   * @brief Obtain the move for the player (handles selecting token and destination).
   * @param player Player requesting a move.
   * @return Move<char>* containing destination cell (from cell stored inside board object).
   */
  Move<char>* get_move(Player<char>* player);
};

//=================================================== 2.Pyramid Tic-Tac-Toe Classes ===================================================

/**
 * @class Pyramid_Board
 * @brief Pyramid-shaped board (3 rows but varying columns per row).
 *
 * @details Valid coordinates:
 *  - row 0: column 2 only
 *  - row 1: columns 1..3
 *  - row 2: columns 0..4
 *
 * Methods provided are small and suitable for AI search helpers implemented in .cpp.
 */
class Pyramid_Board : public Board<char> {
private:
  char blank_symbol = ' ';
public:
  Pyramid_Board();
  bool update_board(Move<char>* move) override;
  bool is_win(Player<char>* player) override;
  bool is_lose(Player<char>*) override { return false; };
  bool is_draw(Player<char>* player) override;
  bool game_is_over(Player<char>* player) override;

  /**
   * @brief Set a particular cell's value (used by AI to backtrack moves).
   * @param r Row index.
   * @param c Column index.
   * @param v Value to set.
   */
  void set_cell_value(int r, int c, char v) {
    board[r][c] = v;
  }

  /**
   * @brief True if the given pyramid cell is empty.
   * @param r Row index.
   * @param c Column index.
   * @return true when the cell contains a space.
   */
  bool is_empty_cell(int r, int c) const {
    return board[r][c] == ' ';
  }

};

class Pyramid_UI : public UI<char> {
public:
  Pyramid_UI();
  ~Pyramid_UI() {};

  Player<char>* create_player(string& name, char symbol, PlayerType type);
  Player<char>** setup_players() override;
  Move<char>* get_move(Player<char>* player);
  void display_board_matrix(const vector<vector<char>>& matrix) const override;
};

// AI helpers for Pyramid -----------------------------------------------------

bool is_valid_pyramid_cell(int r, int c);
bool is_board_full_pyramid(Pyramid_Board* board);
int minimax_pyramid(Pyramid_Board* board, int depth, bool is_maximizing_player,
  char maximizing_player_symbol, char minimizing_player_symbol);
pair<int, int> find_best_move_pyramid(Pyramid_Board* board, char player_symbol);

//=================================================== 3.Numerical Tic-Tac-Toe Classes ===================================================
/**
 * @class Numerical_Board
 * @brief A number-placement Tic-Tac-Toe where rows/columns/diagonals summing to 15 win.
 *
 * @details One player places odd numbers {1,3,5,7,9}, the other places even numbers {2,4,6,8}.
 * The board records used numbers to prevent reuse.
 */
class Numerical_Board : public Board<int> {
private:
  int blank_symbol = 0;
  /// Numbers already used on the board.
  vector<int> used_numbers;

  /**
   * @brief Check whether three integers (non-zero) form the magic sum 15.
   * @param a First integer.
   * @param b Second integer.
   * @param c Third integer.
   * @return true if a,b,c non-zero and a+b+c == 15.
   */
  bool check_line_sum(int a, int b, int c);

public:
  Numerical_Board();

  bool update_board(Move<int>* move) override;
  bool is_win(Player<int>* player) override;
  bool is_lose(Player<int>*) override { return false; };
  bool is_draw(Player<int>* player) override;
  bool game_is_over(Player<int>* player) override;

  /**
   * @brief Test whether given number has already been used on the board.
   * @param num Number to test.
   * @return true if `num` is in used_numbers.
   */
  bool is_number_used(int num);
};


/**
 * @class Numerical_UI
 * @brief UI for Numerical Tic-Tac-Toe managing odd/even number assignment and input.
 */
class Numerical_UI : public UI<int> {
private:
  /**
   * @brief Cast the player's board pointer to Numerical_Board for convenience.
   * @param player Player whose board will be cast.
   * @return Numerical_Board* or nullptr on bad cast.
   */
  Numerical_Board* get_Numerical_board(Player<int>* player) {
    return dynamic_cast<Numerical_Board*>(player->get_board_ptr());
  }

public:
  Numerical_UI();
  ~Numerical_UI() {};

  Player<int>** setup_players() override;
  Player<int>* create_player(string& name, int symbol, PlayerType type) override;
  Move<int>* get_move(Player<int>* player) override;

  /**
   * @brief Prompt human to choose an unused number from their allowed set.
   * @param player The human player selecting a number.
   * @return The chosen number.
   */
  int get_human_number_choice(Player<int>* player);

  /**
   * @brief Computer selects an unused number (randomly) from its allowed set.
   * @param player Computer player.
   * @return Chosen number or -1 if none remain.
   */
  int get_computer_number_choice(Player<int>* player);
};

//============================================ 4.Obstacles Tic-Tac-Toe Classes ==================================================

/**
 * @class Obstacles_Board
 * @brief 6x6 board variant that periodically adds obstacles ('#') while players place tokens.
 *
 * @details After every two moves the board places additional obstacles at random empty positions.
 */
class Obstacles_Board : public Board<char> {
private:
  char blank_symbol = ' ';

public:
  Obstacles_Board();

  /**
   * @brief Apply a player move to the board (places a token and may add obstacles).
   * @param move Move pointer to apply.
   * @return true if move applied; false on invalid.
   */
  bool update_board(Move<char>* move) override;

  /**
   * @brief Check win condition (4-in-a-row) for passed player.
   * @param player Player to test.
   * @return true on win.
   */
  bool is_win(Player<char>* player) override;

  bool is_lose(Player<char>* player) override { return false; };
  bool is_draw(Player<char>* player) override;
  bool game_is_over(Player<char>* player) override;

  /**
   * @brief Place an obstacle character ('#') into a random empty cell on the board.
   * @note May be called multiple times; does nothing when no empty cells available.
   */
  void add_obstacle();

  /**
   * @brief Get a raw cell value.
   * @param x Row.
   * @param y Column.
   * @return Character at the provided coordinates.
   */
  char get_cell(int x, int y) {
    return board[x][y];
  }
};

class Obstacles_UI : public UI<char> {
public:
  Obstacles_UI();
  ~Obstacles_UI() {};

  Player<char>* create_player(string& name, char symbol, PlayerType type) override;
  Move<char>* get_move(Player<char>* player) override;
  void display_board_matrix(const vector<vector<char>>& matrix) const override;
};

//=================================================== 5.Memory_Board Classes ===================================================

/**
 * @class Memory_Board
 * @brief A variant that stores explicit move histories for each player and detects sequences.
 *
 * @details Memory_Board records (x,y) coordinates of moves belonging to each player and
 * uses those vectors to check for winning patterns.
 */
class Memory_Board : public Board<char> {
private:
  char blank_symbol = ' ';

  /// Moves by player controlling 'X'.
  vector<pair<int, int>> p1_moves;   // stores (x,y) for 'X'
  /// Moves by player controlling 'O'.
  vector<pair<int, int>> p2_moves;   // stores (x,y) for 'O'

  /**
   * @brief Check whether a collection of moves contains a winning line.
   * @param moves Vector of coordinate pairs placed by a player.
   * @return true if the collection contains a winning triple.
   */
  bool check_line(const vector<pair<int, int>>& moves);

public:
  Memory_Board();

  bool update_board(Move<char>* move) override;
  bool is_win(Player<char>* player) override;
  bool is_lose(Player<char>* player) override { return false; }
  bool is_draw(Player<char>* player) override;
  bool game_is_over(Player<char>* player) override;
};

class Memory_UI : public UI<char> {
public:
  Memory_UI();
  ~Memory_UI() {};

  Player<char>* create_player(string& name, char symbol, PlayerType type) override;
  Move<char>* get_move(Player<char>* player) override;
  void display_board_matrix(const vector<vector<char>>& matrix) const override;
};

#endif // Games_CLASSES_H
  
