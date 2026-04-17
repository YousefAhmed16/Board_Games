/**
 * @file Games_Classes.h
 * @brief Declarations for multiple board game classes and their UIs used in the project.
 *
 * @details
 * This header contains class declarations for a variety of board games (SUS, Four-in-a-Row,
 * Five-by-Five Tic-Tac-Toe, Word board, Inverse/misère Tic-Tac-Toe, Diamond, Pyramid,
 * Numerical Tic-Tac-Toe, Obstacles, Infinity Tic-Tac-Toe, Ultimate (mini + big board),
 * Memory board and supporting UI classes).  Many boards expose small helpers for AI,
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

//=================================================== 1.SUS Classes ===================================================
/**
 * @class SUS_Board
 * @brief Board implementation for "SUS" game (3x3 variant where the pattern S-U-S scores).
 *
 * @details
 * The SUS game awards points when the sequence 'S' 'U' 'S' appears in a row/column/diagonal.
 * This class stores per-player scores, provides helpers used by the AI, and implements the
 * Board<char> interface functions so it can be used by the UI and game loop.
 *
 * @note The SUS board uses '-' as the blank symbol.
 */
class SUS_Board : public Board<char> {
private:
  char blank_symbol = '-';

  /// Score for player controlling 'S' (player 1).
  int score_P1 = 0; // score for symbol 'S'
  /// Score for player controlling 'U' (player 2).
  int score_P2 = 0; // score for symbol 'U'

  /**
   * @brief Check whether three values correspond to the pattern 'S' 'U' 'S'.
   * @param a First cell value.
   * @param b Second cell value.
   * @param c Third cell value.
   * @return True if a=='S' && b=='U' && c=='S', otherwise false.
   */
  bool check_line(char a, char b, char c);

  /**
   * @brief Count total 'S-U-S' sequences currently on the 3x3 board.
   * @return Number of S-U-S sequences (rows, columns, diagonals combined).
   * @note Used by AI evaluation & scoring when simulating moves.
   */
  int count_SUS_sequences();

public:
  /**
   * @brief Construct a SUS_Board and initialize the 3x3 storage.
   * @details Initializes the underlying board cells to the blank symbol '-' and
   * sets move counters / scores to zero.
   */
  SUS_Board();

  /**
   * @brief Place a move on the board (implements Board<T>::update_board).
   * @param move Pointer to the move to apply. Ownership semantics follow project conventions.
   * @return true if the move was valid and applied; false otherwise.
   */
  bool update_board(Move<char>* move) override; // note: does NOT delete move

  /**
   * @brief Determine whether the passed player has won.
   * @param player Player to test (their symbol is used).
   * @return true if the player's score is strictly greater when the game is over.
   */
  bool is_win(Player<char>* player) override;

  /**
   * @brief Determine whether the passed player has lost.
   * @param player Unused / present to satisfy interface.
   * @return Implementation-defined (SUS uses separate win/draw semantics).
   */
  bool is_lose(Player<char>*) override;

  /**
   * @brief Determine whether the game is a draw for the passed player.
   * @param player Player to test.
   * @return true if the board is filled and both players have equal scores.
   */
  bool is_draw(Player<char>* player) override;

  /**
   * @brief Whether the game is over (terminal).
   * @param player Player to test (unused in many implementations, kept for interface).
   * @return true if the game reached a terminal condition.
   */
  bool game_is_over(Player<char>* player) override;

  /**
   * @brief Read-only cell getter (non-const on purpose to match some internal usage).
   * @param x Row index (0-based).
   * @param y Column index (0-based).
   * @return Character stored at board[x][y].
   * @note Caller must ensure indices are in valid range.
   */
  char get_cell(int x, int y) {
    return board[x][y];
  }

  /**
   * @brief Test whether a given pyramid cell is empty (SUS-specific blank symbol).
   * @param r Row index.
   * @param c Column index.
   * @return true if cell equals the SUS blank symbol '-'.
   */
  bool is_empty_cell(int r, int c) const {
    return board[r][c] == '-';
  }

  /** @brief Get current score for player 1 ('S'). */
  int get_score_P1() const { return score_P1; }
  /** @brief Get current score for player 2 ('U'). */
  int get_score_P2() const { return score_P2; }
  /** @brief Get number of moves applied to the board. */
  int get_n_moves() const { return n_moves; }
};


/**
 * @class SUS_UI
 * @brief UI for SUS game: responsible for player creation and move selection.
 *
 * @details The UI constructs players and provides easy/medium/hard moves (AI)
 * implemented in the corresponding .cpp file.
 */
class SUS_UI : public UI<char> {
private:
  /**
   * @brief Helper to cast a player's board pointer to SUS_Board.
   * @param player Player whose board will be cast.
   * @return Pointer to SUS_Board or nullptr on bad cast.
   */
  SUS_Board* get_SUS_board(Player<char>* player) {
    return dynamic_cast<SUS_Board*>(player->get_board_ptr());
  }

public:
  SUS_UI();
  ~SUS_UI() {}

  /**
   * @brief Build and return configured players for SUS game.
   * @return Array of two Player<char>* pointers (caller owns).
   */
  Player<char>** setup_players() override;

  /**
   * @brief Create a player instance.
   * @param name Player name string.
   * @param symbol Character symbol assigned to the player (e.g. 'S' or 'U').
   * @param type PlayerType enum indicating human/computer.
   * @return Pointer to newly created Player<char>.
   */
  Player<char>* create_player(string& name, char symbol, PlayerType type) override;

  /**
   * @brief Obtain a move for the given player (from stdin or AI).
   * @param player Player requiring a move.
   * @return Pointer to a Move<char> instance describing the chosen move.
   */
  Move<char>* get_move(Player<char>* player) override;
};


// AI helper signatures for SUS (defined/implemented in .cpp)

/**
 * @brief Validate whether a position is valid for SUS board coordinates.
 * @param r Row index.
 * @param c Column index.
 * @return true when the indices correspond to a valid SUS cell.
 */
bool is_valid_SUS_cell(int r, int c);

/**
 * @brief Test whether SUS board has no empty valid cells left.
 * @param board Pointer to SUS_Board.
 * @return true if board is full (no valid empties).
 */
bool is_board_full_SUS(SUS_Board* board);

/**
 * @brief Minimax evaluation function for SUS board.
 * @param board Target board to evaluate (simulations are performed in-place).
 * @param depth Current recursion depth.
 * @param is_maximizing_player true if the current node is maximizing.
 * @param maximizing_player_symbol Symbol used by maximizing player.
 * @param minimizing_player_symbol Symbol used by minimizing player.
 * @return Integer evaluation score.
 */
int minimax_SUS(SUS_Board* board, int depth, bool is_maximizing_player,
  char maximizing_player_symbol, char minimizing_player_symbol);

/**
 * @brief Find the best move for SUS using minimax.
 * @param board Board instance to search.
 * @param player_symbol Symbol for the AI player.
 * @return Pair {row, col} with best move, or {-1,-1} if none.
 */
pair<int, int> find_best_move_SUS(SUS_Board* board, char player_symbol);

/** @brief Active depth globals used by SUS minimax; set before calling AI helpers. */
extern int Max_depth;      /**< @brief Active depth used by generic minimax calls. */
extern int Max_depth_P1;   /**< @brief Depth used specifically for player 'S'. */
extern int Max_depth_P2;   /**< @brief Depth used specifically for player 'U'. */

//=================================================== 2.Four In A Row Classes =====================================================

/**
 * @class FourBoard
 * @brief Classic Connect-Four style board (typically 6 rows x 7 cols).
 *
 * @details Implements helpers required by minimax: listing legal moves, placing and undoing,
 * and a static evaluation method for AI. The underlying board is stored in Board<char>.
 */
class FourBoard : public Board<char> {
private:
  const char blank = '-';
  const int connectN = 4; // need 4 in a row

  /// Column heights (index -> number of tokens currently in that column).
  vector<int> col_heights;

public:
  /** @brief Construct a FourBoard with standard dimensions (6 x 7). */
  FourBoard(); // 6 x 7

  /**
   * @brief Place a move (drop a piece in a column).
   * @param move Move containing target column encoded as x/y depending on project convention.
   * @return true if move applied, false otherwise.
   */
  bool update_board(Move<char>* move) override;

  /**
   * @brief Check whether the passed player has a winning connect-N alignment.
   * @param player Player to check.
   * @return true when that player has connected four in a row.
   */
  bool is_win(Player<char>* player) override;

  /** @brief Implemented to satisfy interface, not used in many modes. */
  bool is_lose(Player<char>*) override { return false; }

  /**
   * @brief Determine if the game is a draw (board full, no win).
   * @param player Unused for draw detection.
   * @return true if draw.
   */
  bool is_draw(Player<char>* player) override;

  /**
   * @brief Game-over detector.
   * @param player Unused for state check.
   * @return true if the board is terminal (win or draw).
   */
  bool game_is_over(Player<char>* player) override;

  /** @brief Safe const accessor for a cell. */
  char get_cell(int r, int c) const { return board[r][c]; }
  /** @brief Return number of rows. */
  int get_rows_count() const { return rows; }
  /** @brief Return number of columns. */
  int get_cols_count() const { return columns; }

  // REQUIRED FOR MINIMAX ------------------------------------------------------
  /**
   * @brief List of legal columns that can be played (no overflow).
   * @return Vector of column indices available.
   */
  vector<int> get_possible_moves() const;

  /**
   * @brief Drop a piece into the specified column for simulation.
   * @param col Column index.
   * @param symbol Symbol to place.
   */
  void place_in_column(int col, char symbol);

  /**
   * @brief Undo the last piece placed in the specified column.
   * @param col Column index.
   */
  void undo_in_column(int col);

  /**
   * @brief Static evaluation of a board from AI / human perspective.
   * @param ai Symbol used by AI.
   * @param human Symbol used by human.
   * @return Integer score: positive favors AI, negative favors human.
   */
  int evaluate(char ai, char human) const;

  /** @brief Return whether current board is terminal (win or full). */
  bool is_terminal() const;

};

class FourUI : public UI<char> {
private:
  FourBoard* get_four_board(Player<char>* p) {
    return dynamic_cast<FourBoard*>(p->get_board_ptr());
  }

public:
  FourUI();
  ~FourUI() override {}

  Player<char>** setup_players() override;
  Player<char>* create_player(std::string& name, char symbol, PlayerType type) override;
  Move<char>* get_move(Player<char>* player) override;

  /**
   * @brief Nicely print the 2D matrix representing the board.
   * @param matrix Board matrix to display.
   */
  void display_board_matrix(const vector<vector<char>>& matrix) const override;

  /**
   * @brief Helper used by AI UI to show chosen column.
   * @param col Column index chosen by the computer.
   */
  void display_computer_move(int col) const {
    cout << "Computer played column: " << col << "\n";
  }
};

//=================================================== 3.FiveByFive_Board Classes ===================================================
/**
 * @class FiveByFive_Board
 * @brief 5x5 Tic-Tac-Toe variant where sequences of length 3 score.
 *
 * @details The board exposes helpers to count three-in-a-row sequences which are
 * useful both for normal gameplay and for AI search (minimax). Cells contain ' ' as blank.
 */
class FiveByFive_Board : public Board<char> {
private:
    char blank_symbol = ' ';

public:
    /** @brief Construct a 5x5 board initialized to blanks. */
    FiveByFive_Board();

    /**
     * @brief Count number of 3-in-a-row sequences belonging to `playersym`.
     * @param playersym Symbol to count sequences for (usually 'X' or 'O').
     * @return Count of horizontal, vertical and diagonal sequences of length 3.
     */
    int count_three_sequences(char playersym) const;

    // Board class functions ---------------------------------------------------
    /**
     * @brief Apply a move to the 5x5 board.
     * @param move Move pointer describing (x,y,symbol).
     * @return true if valid and applied.
     */
    bool update_board(Move<char>* move) override;

    /**
     * @brief Whether the passed player has more 3-in-a-row sequences than opponent.
     * @param player Player to check.
     * @return true if player has strictly more sequences when game over.
     */
    bool is_win(Player<char>* player) override;

    bool is_lose(Player<char>* player) override;
    bool is_draw(Player<char>* player) override;
    bool game_is_over(Player<char>* player) override;

    /** @brief Getter for raw cell value (non-const signature kept for compatibility). */
    char get_cell(int x, int y) {
        return board[x][y];
    }

    /**
     * @brief Set raw cell value (used by AI for simulations).
     * @param r Row index.
     * @param c Column index.
     * @param v Value to place.
     */
    void set_cell_value(int r, int c, char v) {
        board[r][c] = v;
    }

    /** @brief True when the specified cell is empty. */
    bool is_empty_cell(int r, int c) const {
        return board[r][c] == ' ';
    }
};

class FiveByFive_UI : public UI<char> {
public:
    FiveByFive_UI();
    ~FiveByFive_UI() {};

    Player<char>* create_player(string& name, char symbol, PlayerType type) override;
    Player<char>** setup_players() override;
    Move<char>* get_move(Player<char>* player) override;

    /**
     * @brief Display the 5x5 board matrix in friendly format.
     * @param matrix 2D vector of characters representing the board.
     */
    void display_board_matrix(const vector<vector<char>>& matrix) const override;
};

// AI Helper Functions for FiveByFive -----------------------------------------

/**
 * @brief Validate whether given (r,c) lies within the playable five-by-five cells.
 * @param r Row index.
 * @param c Column index.
 * @return true if valid playable cell.
 */
bool is_valid_fivebyfive_cell(int r, int c);

/**
 * @brief Determine whether there are any empty cells left on the given board.
 * @param board Pointer to FiveByFive_Board.
 * @return true if no empty spaces remain.
 */
bool is_board_full_fivebyfive(FiveByFive_Board* board);

/**
 * @brief Minimax routine with alpha-beta for FiveByFive.
 * @param board Board pointer to use for simulation (mutates and backtracks).
 * @param depth Current depth.
 * @param is_maximizing_player True when maximizing.
 * @param maximizing_player_symbol Symbol for maximizing player.
 * @param minimizing_player_symbol Symbol for minimizing player.
 * @param alpha Alpha pruning value.
 * @param beta Beta pruning value.
 * @return Evaluation integer.
 */
int minimax_fivebyfive(FiveByFive_Board* board, int depth, bool is_maximizing_player,
    char maximizing_player_symbol, char minimizing_player_symbol,
    int alpha, int beta);

/**
 * @brief Search for best move for FiveByFive using minimax.
 * @param board Board to inspect.
 * @param player_symbol Symbol of the AI player.
 * @return Pair {row, col} with best found move or {-1,-1}.
 */
pair<int, int> find_best_move_fivebyfive(FiveByFive_Board* board, char player_symbol);

/** @brief Global controlling search difficulty for FiveByFive AI. */
extern int MAX_DEPTH_FIVEBYFIVE;

//===================================================  4.Word_Board Classes (WITH AI) ===================================================

/**
 * @class Word_Board
 * @brief Board where players form words (dictionary-backed).
 *
 * @details The Word_Board stores a dictionary (unordered_set<string>) used to validate
 * formed words. The class exposes AI helpers used by minimax searches.
 */
class Word_Board : public Board<char> {
private:
    unordered_set<string> dictionary;
    char blank_symbol = ' ';

public:
    Word_Board();

    /**
     * @brief Check whether the string `w` is a valid dictionary word.
     * @param w Word to validate.
     * @return true if the dictionary contains `w`.
     */
    bool check_word(const string& w) const;

    bool update_board(Move<char>* move) override;
    bool is_win(Player<char>* player) override;
    bool is_lose(Player<char>* player) override { return false; }
    bool is_draw(Player<char>* player) override;
    bool game_is_over(Player<char>* player) override;

    /** @brief Get value of given cell. */
    char get_cell(int x, int y) {
        return board[x][y];
    }

    /** @brief Set a cell's value for AI simulations. */
    void set_cell_value(int r, int c, char v) {
        board[r][c] = v;
    }

    /** @brief True when given cell is empty (blank symbol). */
    bool is_empty_cell(int r, int c) const {
        return board[r][c] == ' ';
    }
};

class Word_UI : public UI<char> {
public:
    Word_UI();
    ~Word_UI() {};

    Player<char>* create_player(string& name, char symbol, PlayerType type) override;
    Player<char>** setup_players() override;
    Move<char>* get_move(Player<char>* player) override;
    void display_board_matrix(const vector<vector<char>>& matrix) const override;
};

// AI helper signatures for Word game ----------------------------------------

/**
 * @brief Validate word-mode cell coordinates.
 * @param r Row index.
 * @param c Column index.
 * @return true if valid.
 */
bool is_valid_word_cell(int r, int c);

/**
 * @brief Check if a Word_Board is full (no empties).
 * @param board Pointer to Word_Board.
 * @return true if full.
 */
bool is_board_full_word(Word_Board* board);

/**
 * @brief Minimax search for Word_Board (may produce char as well as coords).
 * @param board Board pointer to simulate.
 * @param depth Recursion depth.
 * @param is_maximizing_player True when maximizing.
 * @param maximizing_player_symbol Symbol used by maximizing player.
 * @param minimizing_player_symbol Symbol used by minimizing player.
 * @return Evaluation integer.
 */
int minimax_word(Word_Board* board, int depth, bool is_maximizing_player,
    char maximizing_player_symbol, char minimizing_player_symbol);

/**
 * @brief Find best move for Word_Board returning position + placed char.
 * @param board Target board.
 * @param player_symbol Symbol of AI player.
 * @return pair< pair<int,int>, char > as ((row,col), charToPlace).
 */
pair<pair<int, int>, char> find_best_move_word(Word_Board* board, char player_symbol);

/** @brief Global controlling search difficulty for Word AI. */
extern int MAX_DEPTH_WORD;

//==================================== 5.Inverse (misere) Tic-Tac-Toe Classes ===================================================

/**
 * @class Inverse_Board
 * @brief Misère Tic-Tac-Toe: a player tries NOT to create three-in-a-row.
 *
 * @details This implementation reverses winning condition: forming three of your symbols
 * is losing. The AI helpers operate using conventional minimax but invert evaluations
 * as required by the misère rule.
 */
class Inverse_Board : public Board<char> {
private:
    char blank_symbol = ' ';
public:
    Inverse_Board();
    bool update_board(Move<char>* move) override;
    bool is_win(Player<char>* player) override;
    bool is_lose(Player<char>* player) override;
    bool is_draw(Player<char>* player) override;
    bool game_is_over(Player<char>* player) override;

    /**
     * @brief Set cell value (used by AI to simulate moves).
     * @param r Row.
     * @param c Column.
     * @param v Symbol value.
     */
    void set_cell_value(int r, int c, char v) {
        board[r][c] = v;
    }

    /** @brief True if given cell is empty. */
    bool is_empty_cell(int r, int c) const {
        return board[r][c] == ' ';
    }

    /** @brief Const cell getter for inverse board. */
    char get_cell(int x, int y) const { return board[x][y]; }
};

class Inverse_UI : public UI<char> {
public:
    Inverse_UI();
    ~Inverse_UI() {};
    Player<char>* create_player(string& name, char symbol, PlayerType type) override;
    Player<char>** setup_players() override;
    Move<char>* get_move(Player<char>* player) override;
    void display_board_matrix(const vector<vector<char>>& matrix) const override;
};

// AI functions for inverse ---------------------------------------------------

/**
 * @brief Check whether the inverse board is full (no empty cells).
 * @param board Board pointer.
 * @return true if full.
 */
bool is_board_full_inverse(Inverse_Board* board);

/**
 * @brief Minimax for inverse Tic-Tac-Toe with alpha-beta pruning.
 * @param board Board pointer.
 * @param depth Current depth.
 * @param is_maximizing_player True when maximizing.
 * @param maximizing_player_symbol Symbol for maximizing player.
 * @param minimizing_player_symbol Symbol for minimizing player.
 * @param alpha Alpha pruning value.
 * @param beta Beta pruning value.
 * @return Evaluation score.
 */
int minimax_inverse(Inverse_Board* board, int depth, bool is_maximizing_player,
    char maximizing_player_symbol, char minimizing_player_symbol, int alpha, int beta);

/**
 * @brief Find best move (row,col) for inverse board AI.
 * @param board Board to examine.
 * @param player_symbol Symbol for AI player.
 * @return Pair {row, col} for chosen move or {-1,-1} if not found.
 */
pair<int, int> find_best_move_inverse(Inverse_Board* board, char player_symbol);

// ============================================= 6.Diamond Classes ========================================================

/**
 * @class Diamond_Board
 * @brief A diamond-shaped board variant exposing sequence-count helpers.
 *
 * @details Methods allow counting sequences of length three and four along approved
 * directions. Several utility functions are provided to support AI.
 */
class Diamond_Board : public Board<char> {
private:
    char blank_symbol = ' ';

public:
    Diamond_Board();

    bool update_board(Move<char>* move);
    bool is_win(Player<char>* player);
    bool is_lose(Player<char>* player);
    bool is_draw(Player<char>* player);
    bool game_is_over(Player<char>* player);

    /**
     * @brief Count 3-length sequences for playersym.
     * @param playersym Symbol to count for.
     * @return Count of sequences.
     */
    int count_three_sequences(char playersym) const;

    /**
     * @brief Count 4-length sequences for playersym.
     * @param playersym Symbol to count.
     * @return Count of sequences.
     */
    int count_four_sequences(char playersym) const;

    /**
     * @brief Validate whether a coordinate is within the diamond shape.
     * @param x Row index.
     * @param y Column index.
     * @return True if in the diamond playable area.
     */
    bool is_valid_diamond_cell(int x, int y) const;

    /**
     * @brief Check simple rectangular bounds (0..rows-1, 0..cols-1).
     * @param x Row.
     * @param y Column.
     * @return true if both indices are within the underlying matrix bounds.
     */
    bool in_bounds(int x, int y) const;

    /**
     * @brief Fill a boolean array indicating sequence availability in four primary directions.
     * @param sym Player symbol.
     * @param length Sequence length to test (3 or 4).
     * @param dirs Output array of 4 booleans (horizontal, vertical, diag-down-right, diag-up-right).
     */
    void sequence_directions(char sym, int length, bool dirs[4]) const;

    /** @brief Const raw cell getter. */
    char get_cell(int x, int y) const { return board[x][y]; }

    /** @brief Set raw cell (for AI simulations). */
    void set_cell_value(int r, int c, char v) {
        board[r][c] = v;
    }

    /** @brief True when cell is empty. */
    bool is_empty_cell(int r, int c) const {
        return board[r][c] == ' ';
    }
};

// ---------------------- Diamond_UI ----------------------

class Diamond_UI : public UI<char> {
public:
    Diamond_UI();
    ~Diamond_UI() {}

    Player<char>* create_player(string& name, char symbol, PlayerType type);
    Player<char>** setup_players() override;
    Move<char>* get_move(Player<char>* player);
    void display_board_matrix(const vector<vector<char>>& matrix) const;
};

// AI helpers for Diamond -----------------------------------------------------

bool is_board_full_diamond(Diamond_Board* board);
int minimax_diamond(Diamond_Board* board, int depth, bool is_maximizing_player,
    char maximizing_player_symbol, char minimizing_player_symbol, int alpha, int beta);
pair<int, int> find_best_move_diamond(Diamond_Board* board, char player_symbol);

//==================================================== 7.4x4 Tic-Tac-Toe Classes =====================================================
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

//=================================================== 8.Pyramid Tic-Tac-Toe Classes ===================================================

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

//=================================================== 9.Numerical Tic-Tac-Toe Classes ===================================================
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

//============================================ 10.Obstacles Tic-Tac-Toe Classes ==================================================

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

//============================================== 11.infinity Tic-Tac-Toe Classes ============================================

/**
 * @class infinity_Board
 * @brief 3x3 board in which moves "expire" after a fixed number of subsequent moves.
 *
 * @details Each time the move count is divisible by 3 the oldest move is removed (cell reset).
 */
class infinity_Board : public Board<char> {
private:
  char blank_symbol = ' ';
  /// FIFO move history storing applied (row,column) pairs so oldest removal is possible.
  queue<pair<int, int>> move_history;
public:
  infinity_Board();

  bool update_board(Move<char>* move) override;
  bool is_win(Player<char>* player) override;
  bool is_lose(Player<char>*) override { return false; };
  bool is_draw(Player<char>* player) override;
  bool game_is_over(Player<char>* player) override;
};

class infinity_UI : public UI<char> {
public:
  infinity_UI();
  ~infinity_UI() {};
  Move<char>* get_move(Player<char>* player);
};
// ============================================= 12.Ultimate Tic Tac Toe ===============================================
// MiniBoard & BigBoard used by UltimateUI/GameManager

/**
 * @class MiniBoard
 * @brief Small 3x3 board used as a tile inside the Ultimate (BigBoard) game.
 */
class MiniBoard {
private:
  char board[3][3];
  bool finished;
  char winner;

public:
  MiniBoard();

  /**
   * @brief Place a symbol on the mini-board.
   * @param x Row index (0..2).
   * @param y Column index (0..2).
   * @param symbol Symbol to place.
   * @return true if placement succeeded (cell empty and not finished).
   */
  bool place_move(int x, int y, char symbol);

  /** @brief True when all cells are occupied. */
  bool is_full();

  /** @brief True when the mini-board is finished (someone won or full). */
  bool is_finished();

  /** @brief Get the winner symbol of the mini-board (or 0/space if none). */
  char get_winner();

  /** @brief Get the cell value at (x,y). */
  char get_cell(int x, int y);

  /** @brief Nicely display formatted mini-board to stdout. */
  void display_formatted();
};

/**
 * @class BigBoard
 * @brief A 3x3 of MiniBoards representing the full Ultimate Tic-Tac-Toe board.
 */
class BigBoard {
private:
  MiniBoard* mini[3][3];
  char result[3][3];

public:
  BigBoard();

  /**
   * @brief Return pointer to the mini-board at (x,y).
   * @param x Bi-level row.
   * @param y Bi-level column.
   * @return MiniBoard* pointer.
   */
  MiniBoard* get_mini(int x, int y);

  /**
   * @brief True when the mini-board at (x,y) is available for play.
   * @param x Mini-board row.
   * @param y Mini-board column.
   * @return True if not finished and playable.
   */
  bool is_mini_available(int x, int y);

  /**
   * @brief Update the big board cell (result matrix) after a mini finishes.
   * @param x Mini-board row.
   * @param y Mini-board column.
   */
  void update_big_cell(int x, int y);

  /**
   * @brief Detect if the overall BigBoard is finished.
   * @return True if big board has a winner or no further moves.
   */
  bool is_big_finished();

  /**
   * @brief Return the big-board winner symbol when finished.
   * @return Winner symbol for the large game.
   */
  char big_winner();

  /** @brief Display the big board formatted to stdout. */
  void display_formatted_big();
};

/**
 * @class UltimateUI
 * @brief UI wrapper for Ultimate Tic-Tac-Toe; connects player input with BigBoard.
 */
class UltimateUI : public UI<char> {
private:
  BigBoard* board;

public:
  UltimateUI(BigBoard* b);

  Player<char>** setup_players() override;
  Move<char>* get_move(Player<char>* player) override;

  /**
   * @brief Ask the current player to choose a mini-board (big cell).
   * @param bx Out parameter for big-row.
   * @param by Out parameter for big-col.
   * @param player Pointer to the player being asked.
   */
  void ask_for_big_choice(int& bx, int& by, Player<char>* player);
};

/**
 * @class UltimateGame
 * @brief Manages the overall Ultimate Tic-Tac-Toe game loop using BigBoard and UltimateUI.
 */
class UltimateGame {
private:
  BigBoard* board;
  UltimateUI* ui;
  Player<char>* players[2];

public:
  UltimateGame();
  /** @brief Run the game loop until completion. */
  void run();
};

//=================================================== 13.Memory_Board Classes ===================================================

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
  
