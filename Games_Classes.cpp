#include <iostream>
#include <cstdlib> // for rand() 
#include <algorithm> // for find()
#include <utility> // for std::pair
#include <queue> 
#include "Games_Classes.h"
#include <chrono>

using namespace std;

//============================================== 1.SUS Implementation ==================================================
// define globals
int Max_depth = 4;
int Max_depth_P1 = 4;
int Max_depth_P2 = 4;

// ===============================
//       SUS_Board
// ===============================
SUS_Board::SUS_Board() : Board<char>(3, 3) {
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < columns; j++)
      board[i][j] = blank_symbol;
}

bool SUS_Board::check_line(char a, char b, char c) {
  return (a == 'S' && b == 'U' && c == 'S');
}

int SUS_Board::count_SUS_sequences() {
  int count = 0;

  // rows
  for (int i = 0; i < 3; i++)
    if (check_line(board[i][0], board[i][1], board[i][2]))
      count++;

  // columns
  for (int j = 0; j < 3; j++)
    if (check_line(board[0][j], board[1][j], board[2][j]))
      count++;

  // diagonals
  if (check_line(board[0][0], board[1][1], board[2][2]))
    count++;

  if (check_line(board[0][2], board[1][1], board[2][0]))
    count++;

  return count;
}

// update_board now DOES NOT delete move (caller is responsible)
bool SUS_Board::update_board(Move<char>* move) {
  int x = move->get_x();
  int y = move->get_y();
  char symbol = move->get_symbol();

  if (x < 0 || x >= rows || y < 0 || y >= columns)
    return false;

  if (board[x][y] != blank_symbol)
    return false;

  int old_count = count_SUS_sequences();

  board[x][y] = symbol;
  n_moves++;

  int new_count = count_SUS_sequences();
  int gained = new_count - old_count;

  if (gained > 0) {
    if (symbol == 'S')
      score_P1 += gained;
    else
      score_P2 += gained;
  }

  return true;
}

// Winner should be decided by scores after the board is full.
// Return true for given player only when board is full and that player's score > opponent.
bool SUS_Board::is_win(Player<char>* player) {
  if (n_moves < 9) return false; // don't declare winner until board is full

  char sym = player->get_symbol();
  if (sym == 'S')
    return score_P1 > score_P2;
  else
    return score_P2 > score_P1;
}

bool SUS_Board::is_lose(Player<char>* player) {
  if (n_moves < 9) return false;

  char sym = player->get_symbol();
  if (sym == 'S')
    return score_P1 < score_P2;
  else
    return score_P2 < score_P1;
}


// Draw only when board full AND scores equal
bool SUS_Board::is_draw(Player<char>* player) {
  return (n_moves == 9 && score_P1 == score_P2);
}

// game over when board full (you could add early termination logic if desired)
bool SUS_Board::game_is_over(Player<char>* player) {
  return (n_moves == 9);
}

// ----------------------- SUS AI Player -----------------------
bool is_valid_SUS_cell(int r, int c) {
  return (r >= 0 && r <= 2 && c >= 0 && c <= 2);
}

bool is_board_full_SUS(SUS_Board* board) {
  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c) {
      if (board->is_empty_cell(r, c)) {
        return false;
      }
    }
  }
  return true;
}

int minimax_SUS(SUS_Board* board, int depth, bool is_maximizing_player,
  char maximizing_player_symbol, char minimizing_player_symbol) {

  // use the global MAX_DEPTH set before calling minimax
  if (depth >= Max_depth) {
    // heuristic: return score difference from perspective of maximizing player
    int scoreDiff = board->get_score_P1() - board->get_score_P2();
    if (maximizing_player_symbol == 'S')
      return scoreDiff;
    else
      return -scoreDiff;
  }

  // terminal: full board -> final score
  if (is_board_full_SUS(board)) {
    int finalDiff = board->get_score_P1() - board->get_score_P2();
    return (maximizing_player_symbol == 'S') ? finalDiff : -finalDiff;
  }

  char current_sym = is_maximizing_player ? maximizing_player_symbol : minimizing_player_symbol;

  int best_eval = is_maximizing_player ? numeric_limits<int>::min() : numeric_limits<int>::max();

  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c) {
      if (board->is_empty_cell(r, c)) {
        // make a temporary copy of the board and apply update_board (which handles scoring)
        SUS_Board temp = *board; // copy current game state

        Move<char> temp_move(r, c, current_sym);
        temp.update_board(&temp_move);

        int evaluation = minimax_SUS(&temp, depth + 1, !is_maximizing_player, maximizing_player_symbol, minimizing_player_symbol);

        if (is_maximizing_player) {
          best_eval = max(best_eval, evaluation);
        }
        else {
          best_eval = min(best_eval, evaluation);
        }

        // backtracking not needed because we used a temp copy
      }
    }
  }
  return best_eval;
}

pair<int, int> find_best_move_SUS(SUS_Board* board, char player_symbol) {
  int best_val = numeric_limits<int>::min(); // - infinity
  pair<int, int> best_move = { -1, -1 };

  char maximizing_player_symbol = player_symbol; // AI player
  char minimizing_player_symbol = (player_symbol == 'S') ? 'U' : 'S'; // Human player

  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c) {
      // check valid cell
      if (board->is_empty_cell(r, c)) {

        // make temporary copy and apply move so scoring updates
        SUS_Board temp = *board;
        Move<char> temp_move(r, c, maximizing_player_symbol);
        temp.update_board(&temp_move);

        // call minimax to evaluate this move; next depth is 1 and next player is minimizing
        int saved_Max_Depth = Max_depth; // preserve in case
        int move_val = minimax_SUS(&temp, 1, false, maximizing_player_symbol, minimizing_player_symbol);
        Max_depth = saved_Max_Depth;

        // update best move if needed
        if (move_val > best_val) {
          best_val = move_val;
          best_move = { r, c };
        }
      }
    }
  }
  return best_move;
}

// ===============================
//           SUS_UI
// ===============================

SUS_UI::SUS_UI() : UI<char>("Welcome to SUS Game", 3) {}

Player<char>* SUS_UI::create_player(string& name, char symbol, PlayerType type) {
  cout << "Creating "
    << (type == PlayerType::HUMAN ? "human" : "computer")
    << " player: " << name << " (" << symbol << ")\n";

  return new Player<char>(name, symbol, type);
}

Player<char>** SUS_UI::setup_players() {

  auto get_com_level = []() {
    cout << "Select Computer Player Difficulty Level:\n";
    cout << "1. Easy\n";
    cout << "2. Medium\n";
    cout << "3. Hard\n";
    int level;
    while (true) {
      cout << "Enter your choice (1-3): ";
      cin >> level;
      if (level >= 1 && level <= 3) {
        return level;
      }
      else {
        cout << "Invalid choice. Please select a valid difficulty level.\n";
      }
    }
    };
  Player<char>** players = new Player<char>*[2];
  vector<string> types = { "Human", "Computer" };

  string n1 = get_player_name("Player 1 (S)");
  PlayerType t1 = get_player_type_choice("Player 1 (S)", types);
  players[0] = create_player(n1, 'S', t1);

  if (t1 == PlayerType::COMPUTER) {
    int level = get_com_level();
    if (level == 1) {
      cout << "Computer Player 1 (S) set to Easy level.\n";
      cout << "\n";
      Max_depth_P1 = 0;
    }
    else if (level == 2) {
      cout << "Computer Player 1 (S) set to Medium level.\n";
      cout << "\n";
      Max_depth_P1 = 3;
    }
    else {
      cout << "Computer Player 1 (S) set to Hard level.\n";
      cout << "\n";
      Max_depth_P1 = 7;
    }
  }

  string n2 = get_player_name("Player 2 (U)");
  PlayerType t2 = get_player_type_choice("Player 2 (U)", types);
  players[1] = create_player(n2, 'U', t2);

  if (t2 == PlayerType::COMPUTER) {
    int level = get_com_level();
    if (level == 1) {
      cout << "Computer Player 2 (U) set to Easy level.\n";
      cout << "\n";
      Max_depth_P2 = 0;
    }
    else if (level == 2) {
      cout << "Computer Player 2 (U) set to Medium level.\n";
      cout << "\n";
      Max_depth_P2 = 3;
    }
    else {
      cout << "Computer Player 2 (U) set to Hard level.\n";
      cout << "\n";
      Max_depth_P2 = 7;
    }
  }

  return players;
}

Move<char>* SUS_UI::get_move(Player<char>* player) {
  int x = -1, y = -1;

  if (player->get_type() == PlayerType::HUMAN) {
    cout << player->get_name() << " (" << player->get_symbol() << ") enter x y (0-2): ";
    while (true) {
      cin >> x >> y;
      if (x < 0 || x > 2 || y < 0 || y > 2) {
        cout << "Invalid move! Enter x y (0-2): ";
      }
      else if (get_SUS_board(player)->get_cell(x, y) != '-') {
        cout << "Cell already used! Choose another: ";
      }
      else break;
    }
  }
  else {
    // set active MAX_DEPTH depending on which symbol the AI plays
    if (player->get_symbol() == 'S') Max_depth = Max_depth_P1; else Max_depth = Max_depth_P2;

    SUS_Board* boardptr = get_SUS_board(player);
    pair<int, int> best_pos = find_best_move_SUS(boardptr, player->get_symbol());

    x = best_pos.first;
    y = best_pos.second;

    // fallback: if no move found (shouldn't happen), pick first empty
    if (x == -1) {
      for (int r = 0; r < 3 && x == -1; ++r) {
        for (int c = 0; c < 3 && x == -1; ++c) {
          if (boardptr->is_empty_cell(r, c)) {
            x = r; y = c;
          }
        }
      }
    }

    cout << "Computer plays at: " << x << " " << y << endl;
  }

  return new Move<char>(x, y, player->get_symbol());
}
// ============================================ 2.Four In A Row Implementation ===================================================

// -------------------- FourBoard --------------------

int Max_Depth_Four = 4;
int Max_Depth_Four1 = 4;
int Max_Depth_Four2 = 4;


FourBoard::FourBoard() : Board<char>(6, 7), col_heights(columns, 0) {
  for (int r = 0; r < rows; ++r)
    for (int c = 0; c < columns; ++c)
      board[r][c] = blank;
}

bool FourBoard::update_board(Move<char>* move) {
  if (!move) return false;
  int r = move->get_x();
  int c = move->get_y();
  char sym = move->get_symbol();

  // validate
  if (c < 0 || c >= columns) { delete move; return false; }
  if (r < 0 || r >= rows) { delete move; return false; }

  if (board[r][c] != blank) { delete move; return false; }

  board[r][c] = sym;
  col_heights[c]++;
  ++n_moves;

  delete move;
  return true;
}

// helper to check 4-in-a-row starting at (r,c) in direction dr,dc
static bool check_dir(const vector<vector<char>>& B, int rows, int cols, int r, int c, int dr, int dc, char sym, int need) {
  int cnt = 0;
  for (int k = 0; k < need; ++k) {
    int rr = r + dr * k;
    int cc = c + dc * k;
    if (rr < 0 || rr >= rows || cc < 0 || cc >= cols) return false;
    if (B[rr][cc] != sym) return false;
  }
  return true;
}

bool FourBoard::is_win(Player<char>* player) {
  if (!player) return false;
  char sym = player->get_symbol();
  // scan every cell as possible start of 4-in-a-row
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (board[r][c] != sym) continue;
      // horizontal (to right)
      if (check_dir(board, rows, columns, r, c, 0, 1, sym, connectN)) return true;
      // vertical (down)
      if (check_dir(board, rows, columns, r, c, 1, 0, sym, connectN)) return true;
      // diag down-right
      if (check_dir(board, rows, columns, r, c, 1, 1, sym, connectN)) return true;
      // diag down-left
      if (check_dir(board, rows, columns, r, c, 1, -1, sym, connectN)) return true;
    }
  }
  return false;
}

bool FourBoard::is_draw(Player<char>* /*player*/) {
  return (n_moves >= rows * columns);
}

bool FourBoard::game_is_over(Player<char>* player) {
  // game over when someone has 4 in a row (is_win called by GameManager on current player)
  // or when board full
  return is_draw(player);
}

// -------------------- FourBoard AI / Minimax --------------------

vector<int> FourBoard::get_possible_moves() const {
  vector<int> moves;
  for (int c = 0; c < columns; ++c)
    if (col_heights[c] < rows) moves.push_back(c);
  return moves;
}

void FourBoard::place_in_column(int col, char symbol) {
  int r = rows - col_heights[col] - 1;
  if (r >= 0) {
    board[r][col] = symbol;
    col_heights[col]++;
    n_moves++;
  }
}

void FourBoard::undo_in_column(int col) {
  if (col_heights[col] > 0) {
    int r = rows - col_heights[col];
    board[r][col] = blank;
    col_heights[col]--;
    n_moves--;
  }
}

int FourBoard::evaluate(char ai, char human) const {
  int score = 0;
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      char sym = board[r][c];
      if (sym == blank) continue;
      int multiplier = (sym == ai) ? 1 : -1;

      // horizontal
      if (c <= columns - 4) {
        int cnt = 0;
        for (int k = 0; k < 4; ++k) if (board[r][c + k] == sym) cnt++;
        score += multiplier * cnt;
      }
      // vertical
      if (r <= rows - 4) {
        int cnt = 0;
        for (int k = 0; k < 4; ++k) if (board[r + k][c] == sym) cnt++;
        score += multiplier * cnt;
      }
      // diag down-right
      if (r <= rows - 4 && c <= columns - 4) {
        int cnt = 0;
        for (int k = 0; k < 4; ++k) if (board[r + k][c + k] == sym) cnt++;
        score += multiplier * cnt;
      }
      // diag down-left
      if (r <= rows - 4 && c >= 3) {
        int cnt = 0;
        for (int k = 0; k < 4; ++k) if (board[r + k][c - k] == sym) cnt++;
        score += multiplier * cnt;
      }
    }
  }
  return score;
}

bool FourBoard::is_terminal() const {
  for (int r = 0; r < rows; ++r)
    for (int c = 0; c < columns; ++c) {
      char sym = board[r][c];
      if (sym == blank) continue;
      // check 4 directions
      if (c <= columns - 4 && board[r][c + 1] == sym && board[r][c + 2] == sym && board[r][c + 3] == sym) return true;
      if (r <= rows - 4 && board[r + 1][c] == sym && board[r + 2][c] == sym && board[r + 3][c] == sym) return true;
      if (r <= rows - 4 && c <= columns - 4 && board[r + 1][c + 1] == sym && board[r + 2][c + 2] == sym && board[r + 3][c + 3] == sym) return true;
      if (r <= rows - 4 && c >= 3 && board[r + 1][c - 1] == sym && board[r + 2][c - 2] == sym && board[r + 3][c - 3] == sym) return true;
    }
  for (int c = 0; c < columns; ++c) if (col_heights[c] < rows) return false;
  return true;
}

// -------------------- Minimax Helper --------------------
struct MoveScore {
  int col;
  int score;
};

// Minimax function
int minimax(FourBoard* board, int depth, bool maximizingPlayer, char ai, char human) {
  if (board->is_terminal() || depth == 0) {
    return board->evaluate(ai, human);
  }

  vector<int> moves = board->get_possible_moves();
  if (maximizingPlayer) {
    int maxEval = -10000;
    for (int col : moves) {
      board->place_in_column(col, ai);
      int eval = minimax(board, depth - 1, false, ai, human);
      board->undo_in_column(col);
      if (eval > maxEval) maxEval = eval;
    }
    return maxEval;
  }
  else {
    int minEval = 10000;
    for (int col : moves) {
      board->place_in_column(col, human);
      int eval = minimax(board, depth - 1, true, ai, human);
      board->undo_in_column(col);
      if (eval < minEval) minEval = eval;
    }
    return minEval;
  }
}

int find_best_move(FourBoard* board, char ai, char human) {
  vector<int> moves = board->get_possible_moves();
  int bestScore = -10000;
  int bestCol = moves[0];

  for (int col : moves) {
    board->place_in_column(col, ai);
    int score = minimax(board, Max_Depth_Four - 1, false, ai, human);
    board->undo_in_column(col);

    if (score > bestScore) {
      bestScore = score;
      bestCol = col;
    }
  }
  return bestCol;
}


// -------------------- FourUI --------------------

FourUI::FourUI() : UI<char>("Welcome to Four-in-a-Row", 3) {
  // seed RNG for computer moves
  std::srand(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));
}

Player<char>* FourUI::create_player(string& name, char symbol, PlayerType type) {
  cout << "Creating " << (type == PlayerType::HUMAN ? "human" : "computer")
    << " player: " << name << " (" << symbol << ")\n";
  return new Player<char>(name, symbol, type);
}

Player<char>** FourUI::setup_players() {

  auto get_com_level = []() {
    cout << "Select Computer Player Difficulty Level:\n";
    cout << "1. Easy\n";
    cout << "2. Medium\n";
    cout << "3. Hard\n";
    int level;
    while (true) {
      cout << "Enter your choice (1-3): ";
      cin >> level;
      if (level >= 1 && level <= 3) {
        return level;
      }
      else {
        cout << "Invalid choice. Please select a valid difficulty level.\n";
      }
    }
    };

  Player<char>** players = new Player<char>*[2];
  vector<string> types = { "Human", "Computer" };

  string n1 = get_player_name("Player 1 (X)");
  PlayerType t1 = get_player_type_choice("Player 1 (X)", types);
  players[0] = create_player(n1, 'X', t1);
  if (t1 == PlayerType::COMPUTER) {
    int level = get_com_level();
    if (level == 1) {
      cout << "Computer Player 1 (X) set to Easy level.\n";
      cout << "\n";
      Max_Depth_Four1 = 1;
    }
    else if (level == 2) {
      cout << "Computer Player 1 (X) set to Medium level.\n";
      cout << "\n";
      Max_Depth_Four1 = 3;
    }
    else {
      cout << "Computer Player 1 (X) set to Hard level.\n";
      cout << "\n";
      Max_Depth_Four1 = 5;
    }
  }

  string n2 = get_player_name("Player 2 (O)");
  PlayerType t2 = get_player_type_choice("Player 2 (O)", types);
  players[1] = create_player(n2, 'O', t2);
  if (t2 == PlayerType::COMPUTER) {
    int level = get_com_level();
    if (level == 1) {
      cout << "Computer Player 2 (O) set to Easy level.\n";
      cout << "\n";
      Max_Depth_Four2 = 1;
    }
    else if (level == 2) {
      cout << "Computer Player 2 (O) set to Medium level.\n";
      cout << "\n";
      Max_Depth_Four2 = 3;
    }
    else {
      cout << "Computer Player 2 (O) set to Hard level.\n";
      cout << "\n";
      Max_Depth_Four2 = 5;
    }
  }

  return players;
}

static int find_drop_row(FourBoard* b, int col) {
  // lowest available row is the largest row index with blank
  for (int r = b->get_rows_count() - 1; r >= 0; --r) {
    if (b->get_cell(r, col) == '-') return r;
  }
  return -1;
}

Move<char>* FourUI::get_move(Player<char>* player) {
  FourBoard* fb = get_four_board(player);
  if (!fb) return new Move<char>(0, 0, player->get_symbol()); // fallback

  int x = -1, y = -1;

  // Human: ask for column; Computer: choose random non-full column
  if (player->get_type() == PlayerType::HUMAN) {
    int col;
    cout << player->get_name() << " (" << player->get_symbol() << ") enter column (0-" << (fb->get_cols_count() - 1) << "): ";
    while (true) {
      if (!(cin >> col)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Enter column number: ";
        continue;
      }
      if (col < 0 || col >= fb->get_cols_count()) {
        cout << "Column out of range. Try again: ";
        continue;
      }
      int row = find_drop_row(fb, col);
      if (row == -1) {
        cout << "Column full. Choose another column: ";
        continue;
      }
      // return move at (row,col)
      return new Move<char>(row, col, player->get_symbol());
    }
  }
  else {
    // Computer move
    vector<int> possible = fb->get_possible_moves();
    if (possible.empty()) return new Move<char>(0, 0, player->get_symbol());

    int col;
    if (Max_Depth_Four == 1) {
      // Easy → random
      col = possible[rand() % possible.size()];
    }
    else {
      // Medium/Hard → minimax
      char ai = player->get_symbol();
      char human = (ai == 'X') ? 'O' : 'X';
      col = find_best_move(fb, ai, human);
    }

    int row = find_drop_row(fb, col);
    display_computer_move(col);
    return new Move<char>(row, col, player->get_symbol());
  }
}

void FourUI::display_board_matrix(const vector<vector<char>>& matrix) const {
  if (matrix.empty() || matrix[0].empty()) return;

  int rows = matrix.size();
  int cols = matrix[0].size();

  cout << "\n  ";
  for (int j = 0; j < cols; ++j)
    cout << setw(cell_width + 2) << j;
  cout << "\n   " << string((cell_width + 2) * cols, '-') << "\n";

  for (int i = 0; i < rows; ++i) {
    cout << setw(2) << i << " |";
    for (int j = 0; j < cols; ++j)
      cout << setw(cell_width) << matrix[i][j] << " |";
    cout << "\n   " << string((cell_width + 2) * cols, '-') << "\n";
  }
  cout << endl;
}


//============================================ 3.FiveByFive_Board Implementation ==================================================

// Global variable for FiveByFive AI difficulty
int MAX_DEPTH_FIVEBYFIVE = 3;

FiveByFive_Board::FiveByFive_Board() : Board<char>(5, 5) {
    // make 5*5 board with blank symbol ' '
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            board[i][j] = blank_symbol;
        }
    }
}

bool FiveByFive_Board::update_board(Move<char>* move) {
    int x = move->get_x();
    int y = move->get_y();
    char symbol = move->get_symbol();

    if (x < 0 || x >= rows || y < 0 || y >= columns) return false;
    if (board[x][y] != blank_symbol) return false;

    n_moves++;
    board[x][y] = symbol;
    return true;
}

// Count all three_sequences for a given symbol.
int FiveByFive_Board::count_three_sequences(char playersym) const {
    int counter = 0;

    // Horizontal
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j <= 2; ++j) {
            if (board[i][j] == playersym && board[i][j + 1] == playersym && board[i][j + 2] == playersym)
                counter++;
        }
    }

    // Vertical
    for (int i = 0; i <= 2; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (board[i][j] == playersym && board[i + 1][j] == playersym && board[i + 2][j] == playersym)
                counter++;
        }
    }

    // Diagonal down-right 
    for (int i = 0; i <= 2; ++i) {
        for (int j = 0; j <= 2; ++j) {
            if (board[i][j] == playersym && board[i + 1][j + 1] == playersym && board[i + 2][j + 2] == playersym)
                counter++;
        }
    }

    // Diagonal up-right 
    for (int i = 2; i < 5; ++i) {
        for (int j = 0; j <= 2; ++j) {
            if (board[i][j] == playersym && board[i - 1][j + 1] == playersym && board[i - 2][j + 2] == playersym)
                counter++;
        }
    }

    return counter;
}

// is_win: returns true if the game is over and the passed player has more 3-in-a-row sequences than the other player.
bool FiveByFive_Board::is_win(Player<char>* player) {
    if (!game_is_over(player)) return false;

    char playersym = player->get_symbol();
    char otherPlayersym = (playersym == 'X') ? 'O' : 'X';

    int player_counter = count_three_sequences(playersym);
    int otherPlayer_counter = count_three_sequences(otherPlayersym);

    return (player_counter > otherPlayer_counter);
}

bool FiveByFive_Board::is_lose(Player<char>* player) {
    if (!game_is_over(player)) return false;

    char playersym = player->get_symbol();
    char otherplayer = (playersym == 'X') ? 'O' : 'X';

    int player_counter = count_three_sequences(playersym);
    int otherplayer_count = count_three_sequences(otherplayer);

    return (otherplayer_count > player_counter);
}

// is_draw if n_moves == 24 and both players have equal sequence counts.
bool FiveByFive_Board::is_draw(Player<char>* player) {
    if (!game_is_over(player)) return false;

    char playersym = player->get_symbol();
    char otherplayersym = (playersym == 'X') ? 'O' : 'X';

    int player_counter = count_three_sequences(playersym);
    int otherplayer_counter = count_three_sequences(otherplayersym);

    return (player_counter == otherplayer_counter);
}

// game is over if n_moves = 24 
bool FiveByFive_Board::game_is_over(Player<char>* player) {
    return (n_moves == 24);
}

// ===============================
//    FiveByFive AI Functions
// ===============================

bool is_valid_fivebyfive_cell(int r, int c) {
    return (r >= 0 && r < 5 && c >= 0 && c < 5);
}

bool is_board_full_fivebyfive(FiveByFive_Board* board) {
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 5; ++c) {
            if (board->is_empty_cell(r, c)) {
                return false;
            }
        }
    }
    return true;
}

// Count potential 2-in-a-rows that can be completed to 3-in-a-row
int count_two_sequences_with_potential(FiveByFive_Board* board, char playersym) {
    int counter = 0;

    // Horizontal - check for 2 in a row with empty third cell
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j <= 2; ++j) {
            int count = 0;
            int empty_count = 0;

            if (board->get_cell(i, j) == playersym) count++;
            else if (board->get_cell(i, j) == ' ') empty_count++;

            if (board->get_cell(i, j + 1) == playersym) count++;
            else if (board->get_cell(i, j + 1) == ' ') empty_count++;

            if (board->get_cell(i, j + 2) == playersym) count++;
            else if (board->get_cell(i, j + 2) == ' ') empty_count++;

            // If exactly 2 of our symbols and 1 empty, it's a potential 3-in-a-row
            if (count == 2 && empty_count == 1) counter++;
        }
    }

    // Vertical
    for (int i = 0; i <= 2; ++i) {
        for (int j = 0; j < 5; ++j) {
            int count = 0;
            int empty_count = 0;

            if (board->get_cell(i, j) == playersym) count++;
            else if (board->get_cell(i, j) == ' ') empty_count++;

            if (board->get_cell(i + 1, j) == playersym) count++;
            else if (board->get_cell(i + 1, j) == ' ') empty_count++;

            if (board->get_cell(i + 2, j) == playersym) count++;
            else if (board->get_cell(i + 2, j) == ' ') empty_count++;

            if (count == 2 && empty_count == 1) counter++;
        }
    }

    // Diagonal down-right
    for (int i = 0; i <= 2; ++i) {
        for (int j = 0; j <= 2; ++j) {
            int count = 0;
            int empty_count = 0;

            if (board->get_cell(i, j) == playersym) count++;
            else if (board->get_cell(i, j) == ' ') empty_count++;

            if (board->get_cell(i + 1, j + 1) == playersym) count++;
            else if (board->get_cell(i + 1, j + 1) == ' ') empty_count++;

            if (board->get_cell(i + 2, j + 2) == playersym) count++;
            else if (board->get_cell(i + 2, j + 2) == ' ') empty_count++;

            if (count == 2 && empty_count == 1) counter++;
        }
    }

    // Diagonal up-right
    for (int i = 2; i < 5; ++i) {
        for (int j = 0; j <= 2; ++j) {
            int count = 0;
            int empty_count = 0;

            if (board->get_cell(i, j) == playersym) count++;
            else if (board->get_cell(i, j) == ' ') empty_count++;

            if (board->get_cell(i - 1, j + 1) == playersym) count++;
            else if (board->get_cell(i - 1, j + 1) == ' ') empty_count++;

            if (board->get_cell(i - 2, j + 2) == playersym) count++;
            else if (board->get_cell(i - 2, j + 2) == ' ') empty_count++;

            if (count == 2 && empty_count == 1) counter++;
        }
    }

    return counter;
}

// Find the cell that completes a 2-in-a-row to 3-in-a-row
pair<int, int> find_winning_move(FiveByFive_Board* board, char playersym) {
    // Horizontal
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j <= 2; ++j) {
            int count = 0;
            int empty_r = -1, empty_c = -1;

            for (int k = 0; k < 3; ++k) {
                if (board->get_cell(i, j + k) == playersym) {
                    count++;
                }
                else if (board->get_cell(i, j + k) == ' ') {
                    empty_r = i;
                    empty_c = j + k;
                }
            }

            if (count == 2 && empty_r != -1) {
                return { empty_r, empty_c };
            }
        }
    }

    // Vertical
    for (int i = 0; i <= 2; ++i) {
        for (int j = 0; j < 5; ++j) {
            int count = 0;
            int empty_r = -1, empty_c = -1;

            for (int k = 0; k < 3; ++k) {
                if (board->get_cell(i + k, j) == playersym) {
                    count++;
                }
                else if (board->get_cell(i + k, j) == ' ') {
                    empty_r = i + k;
                    empty_c = j;
                }
            }

            if (count == 2 && empty_r != -1) {
                return { empty_r, empty_c };
            }
        }
    }

    // Diagonal down-right
    for (int i = 0; i <= 2; ++i) {
        for (int j = 0; j <= 2; ++j) {
            int count = 0;
            int empty_r = -1, empty_c = -1;

            for (int k = 0; k < 3; ++k) {
                if (board->get_cell(i + k, j + k) == playersym) {
                    count++;
                }
                else if (board->get_cell(i + k, j + k) == ' ') {
                    empty_r = i + k;
                    empty_c = j + k;
                }
            }

            if (count == 2 && empty_r != -1) {
                return { empty_r, empty_c };
            }
        }
    }

    // Diagonal up-right
    for (int i = 2; i < 5; ++i) {
        for (int j = 0; j <= 2; ++j) {
            int count = 0;
            int empty_r = -1, empty_c = -1;

            for (int k = 0; k < 3; ++k) {
                if (board->get_cell(i - k, j + k) == playersym) {
                    count++;
                }
                else if (board->get_cell(i - k, j + k) == ' ') {
                    empty_r = i - k;
                    empty_c = j + k;
                }
            }

            if (count == 2 && empty_r != -1) {
                return { empty_r, empty_c };
            }
        }
    }

    return { -1, -1 }; // No winning move found
}

int minimax_fivebyfive(FiveByFive_Board* board, int depth, bool is_maximizing_player,
    char maximizing_player_symbol, char minimizing_player_symbol,
    int alpha, int beta) {

    // Depth limit reached - evaluate using heuristic
    if (depth >= MAX_DEPTH_FIVEBYFIVE) {
        // heuristic with 2-in-a-row potential
        int max_count = board->count_three_sequences(maximizing_player_symbol);
        int min_count = board->count_three_sequences(minimizing_player_symbol);
        int max_two = count_two_sequences_with_potential(board, maximizing_player_symbol);
        int min_two = count_two_sequences_with_potential(board, minimizing_player_symbol);

        // Heavily weight completed 3-in-a-rows, also value 2-in-a-rows with potential
        return (max_count * 100) + (max_two * 20) - (min_count * 50) - (min_two * 10);
    }

    // Base Case: Check if game is over (board full)
    if (board->get_n_moves() >= 24) {
        int max_count = board->count_three_sequences(maximizing_player_symbol);
        int min_count = board->count_three_sequences(minimizing_player_symbol);
        int max_two = count_two_sequences_with_potential(board, maximizing_player_symbol);
        int min_two = count_two_sequences_with_potential(board, minimizing_player_symbol);

        // Heavily weight completed 3-in-a-rows, also value 2-in-a-rows with potential
        return (max_count * 100) + (max_two * 20) - (min_count * 50) - (min_two * 10);
    }

    // Recursive minimax
    char current_sym = is_maximizing_player ? maximizing_player_symbol : minimizing_player_symbol;

    if (is_maximizing_player) {
        int best_eval = numeric_limits<int>::min();

        for (int r = 0; r < 5; ++r) {
            for (int c = 0; c < 5; ++c) {
                if (is_valid_fivebyfive_cell(r, c) && board->is_empty_cell(r, c)) {
                    // Make the temporary move
                    board->set_cell_value(r, c, current_sym);

                    // Recursive call
                    int evaluation = minimax_fivebyfive(board, depth + 1, false,
                        maximizing_player_symbol, minimizing_player_symbol,
                        alpha, beta);

                    // Backtrack the move
                    board->set_cell_value(r, c, ' ');

                    best_eval = max(best_eval, evaluation);
                    alpha = max(alpha, best_eval);

                    if (beta <= alpha) {
                        return best_eval;
                    }
                }
            }
        }
        return best_eval;
    }
    else {
        int best_eval = numeric_limits<int>::max();

        for (int r = 0; r < 5; ++r) {
            for (int c = 0; c < 5; ++c) {
                if (is_valid_fivebyfive_cell(r, c) && board->is_empty_cell(r, c)) {
                    // Make the temporary move
                    board->set_cell_value(r, c, current_sym);

                    // Recursive call
                    int evaluation = minimax_fivebyfive(board, depth + 1, true,
                        maximizing_player_symbol, minimizing_player_symbol,
                        alpha, beta);

                    // Backtrack the move
                    board->set_cell_value(r, c, ' ');

                    best_eval = min(best_eval, evaluation);
                    beta = min(beta, best_eval);

                    if (beta <= alpha) {
                        return best_eval;
                    }
                }
            }
        }
        return best_eval;
    }
}

pair<int, int> find_best_move_fivebyfive(FiveByFive_Board* board, char player_symbol) {
    // FIRST: Check if there's a winning move (complete a 2-in-a-row)
    pair<int, int> winning_move = find_winning_move(board, player_symbol);
    if (winning_move.first != -1) {
        return winning_move; // Immediately take the winning move!
    }

    // SECOND: Check if opponent has a winning move and block it
    char opponent_symbol = (player_symbol == 'X') ? 'O' : 'X';
    pair<int, int> blocking_move = find_winning_move(board, opponent_symbol);
    if (blocking_move.first != -1) {
        return blocking_move; // Block opponent's winning move
    }

    // THIRD: Use minimax to find the best strategic move
    int best_val = numeric_limits<int>::min();
    pair<int, int> best_move = { -1, -1 };

    char maximizing_player_symbol = player_symbol; // AI player
    char minimizing_player_symbol = (player_symbol == 'X') ? 'O' : 'X'; // Opponent

    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 5; ++c) {
            if (is_valid_fivebyfive_cell(r, c) && board->is_empty_cell(r, c)) {
                // Make the temporary move
                board->set_cell_value(r, c, maximizing_player_symbol);

                // Evaluate this move
                int move_val = minimax_fivebyfive(board, 1, false, maximizing_player_symbol, minimizing_player_symbol,
                    numeric_limits<int>::min(), numeric_limits<int>::max());

                // Backtrack the move
                board->set_cell_value(r, c, ' ');

                // Update best move
                if (move_val > best_val) {
                    best_val = move_val;
                    best_move = { r, c };
                }
            }
        }
    }

    return best_move;
}

//============================================ FiveByFive_UI ==================================================

FiveByFive_UI::FiveByFive_UI() : UI<char>("Welcome to 5x5 Tic-Tac-Toe", 3) {}

Player<char>* FiveByFive_UI::create_player(string& name, char symbol, PlayerType type) {
    cout << "Creating " << (type == PlayerType::HUMAN ? "human" : "computer")
        << " player: " << name << " (" << symbol << ")\n";
    return new Player<char>(name, symbol, type);
}

Player<char>** FiveByFive_UI::setup_players() {
    auto get_computer_level = []() {
        cout << "Select Computer Player Difficulty Level:\n";
        cout << "1. Easy\n";
        cout << "2. Medium\n";
        cout << "3. Hard\n";
        int level;
        while (true) {
            cout << "Enter your choice (1-3): ";
            cin >> level;
            if (level >= 1 && level <= 3) {
                return level;
            }
            else {
                cout << "Invalid choice. Please select a valid difficulty level.\n";
            }
        }
        };

    Player<char>** players = new Player<char>*[2];
    vector<string> type_options = { "Human", "Computer" };

    // Player 1 (X)
    string nameX = get_player_name("Player 1 (X)");
    PlayerType typeX = get_player_type_choice("Player 1 (X)", type_options);
    players[0] = create_player(nameX, 'X', typeX);

    if (typeX == PlayerType::COMPUTER) {
        int level = get_computer_level();
        if (level == 1) {
            cout << "Computer Player 1 (X) set to Easy level.\n\n";
            MAX_DEPTH_FIVEBYFIVE = 1;
        }
        else if (level == 2) {
            cout << "Computer Player 1 (X) set to Medium level.\n\n";
            MAX_DEPTH_FIVEBYFIVE = 2;
        }
        else {
            cout << "Computer Player 1 (X) set to Hard level.\n\n";
            MAX_DEPTH_FIVEBYFIVE = 3;
        }
    }

    // Player 2 (O)
    string nameO = get_player_name("Player 2 (O)");
    PlayerType typeO = get_player_type_choice("Player 2 (O)", type_options);
    players[1] = create_player(nameO, 'O', typeO);

    if (typeO == PlayerType::COMPUTER) {
        int level = get_computer_level();
        if (level == 1) {
            cout << "Computer Player 2 (O) set to Easy level.\n\n";
            MAX_DEPTH_FIVEBYFIVE = 1;
        }
        else if (level == 2) {
            cout << "Computer Player 2 (O) set to Medium level.\n\n";
            MAX_DEPTH_FIVEBYFIVE = 2;
        }
        else {
            cout << "Computer Player 2 (O) set to Hard level.\n\n";
            MAX_DEPTH_FIVEBYFIVE = 3;
        }
    }

    return players;
}

Move<char>* FiveByFive_UI::get_move(Player<char>* player) {
    int x, y;
    auto boardptr = dynamic_cast<FiveByFive_Board*>(player->get_board_ptr());

    if (player->get_type() == PlayerType::HUMAN) {
        cout << "\nPlease enter your move x (0 to 4) and y (0 to 4): ";
        while (true) {
            cin >> x >> y;
            if (x < 0 || x > 4 || y < 0 || y > 4) {
                cout << "Please enter a Valid move x and y (0 to 4): ";
                continue;
            }
            if (boardptr->get_cell(x, y) != ' ') {
                cout << "This cell is already used, choose another cell: ";
                continue;
            }
            break;
        }
    }
    else {
        // AI move using minimax
        pair<int, int> best_pos = find_best_move_fivebyfive(boardptr, player->get_symbol());
        x = best_pos.first;
        y = best_pos.second;

        // Fallback to random if no move found
        if (x == -1) {
            do {
                x = rand() % 5;
                y = rand() % 5;
            } while (boardptr->get_cell(x, y) != ' ');
        }

        cout << "Computer chooses: (" << x << ", " << y << ")" << endl;
    }

    return new Move<char>(x, y, player->get_symbol());
}

// Drawing 5*5 board
void FiveByFive_UI::display_board_matrix(const vector<vector<char>>& matrix) const {
    if (matrix.empty() || matrix[0].empty()) return;

    cout << "\n    ";
    for (int j = 0; j < 5; ++j)
        cout << setw(cell_width + 1) << j;
    cout << "\n   " << string((cell_width + 2) * 5, '-') << "\n";

    for (int i = 0; i < 5; ++i) {
        cout << setw(2) << i << " |";
        for (int j = 0; j < 5; ++j)
            cout << setw(cell_width) << matrix[i][j] << " |";
        cout << "\n   " << string((cell_width + 2) * 5, '-') << "\n";
    }
    cout << endl;
}
//============================================ 4.Word_Board Implementation ==================================================

Word_Board::Word_Board() : Board<char>(3, 3) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            board[i][j] = blank_symbol;
        }
    }

    // Load dictionary from dic.txt
    ifstream file("dic.txt");
    if (!file.is_open()) {
        return;
    }

    string w;
    while (file >> w) {
        // Convert to uppercase
        for (char& c : w) c = toupper(c);
        dictionary.insert(w);
    }
    file.close();
}

bool Word_Board::update_board(Move<char>* move) {
    int x = move->get_x();
    int y = move->get_y();
    char letter = (char)toupper(move->get_symbol());

    if (x < 0 || x >= rows || y < 0 || y >= columns) return false;
    if (board[x][y] != blank_symbol) return false;

    board[x][y] = letter;
    n_moves++;
    return true;
}

// Check dictionary membership
bool Word_Board::check_word(const string& w) const {
    return (dictionary.find(w) != dictionary.end());
}

// is_win: returns true if any row, column, or diagonal forms a valid word
bool Word_Board::is_win(Player<char>* player) {
    // Check rows
    for (int i = 0; i < 3; ++i) {
        string row;
        for (int j = 0; j < 3; ++j) {
            row += board[i][j];
        }
        if (row.find(blank_symbol) == string::npos) {
            if (check_word(row)) return true;       // forward
            reverse(row.begin(), row.end());
            if (check_word(row)) return true;       // backward
        }
    }
    // Check columns
    for (int j = 0; j < 3; ++j) {
        string col;
        for (int i = 0; i < 3; ++i) {
            col += board[i][j];
        }
        if (col.find(blank_symbol) == string::npos) {
            if (check_word(col)) return true;
            reverse(col.begin(), col.end());
            if (check_word(col)) return true;
        }
    }
    // Check main diagonal
    string diag1;
    for (int i = 0; i < 3; ++i) {
        diag1 += board[i][i];
    }

    if (diag1.find(blank_symbol) == string::npos) {
        if (check_word(diag1)) return true;
        reverse(diag1.begin(), diag1.end());
        if (check_word(diag1)) return true;
    }
    // Check anti-diagonal
    string diag2;
    for (int i = 0; i < 3; ++i) {
        diag2 += board[i][2 - i];
    }

    if (diag2.find(blank_symbol) == string::npos) {
        if (check_word(diag2)) return true;
        reverse(diag2.begin(), diag2.end());
        if (check_word(diag2)) return true;
    }

    return false;
}

// is_draw if n_moves == 9 and no winner
bool Word_Board::is_draw(Player<char>* /*player*/) {
    return (n_moves == 9 && !is_win(nullptr));
}

// game is over if n_moves = 9 or a word was formed
bool Word_Board::game_is_over(Player<char>* /*player*/) {
    return is_win(nullptr) || (n_moves == 9);
}

//--------------------------- Word AI Player Implementation ------------------------------- 

int MAX_DEPTH_WORD = 3; // Global variable for minimax depth

bool is_valid_word_cell(int r, int c) {
    return (r >= 0 && r < 3 && c >= 0 && c < 3);
}

bool is_board_full_word(Word_Board* board) {
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (board->get_cell(r, c) == ' ') {
                return false;
            }
        }
    }
    return true;
}

// Helper: Get smart letter choices for a specific position
vector<char> get_smart_letters(Word_Board* board, int r, int c) {
    vector<char> letters;
    const char common[] = "ETAOINSHRDLCUMWFGYPBVKJXQZ"; // Frequency order

    // Get current board state
    string rows[3], cols[3], diag1, diag2;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            rows[i] += board->get_cell(i, j);
            cols[j] += board->get_cell(i, j);
        }
        diag1 += board->get_cell(i, i);
        diag2 += board->get_cell(i, 2 - i);
    }

    // Prioritize letters that could complete words
    for (char letter : common) {
        // Test if this letter helps any line containing position (r,c)
        string test_row = rows[r];
        test_row[c] = letter;

        string test_col = cols[c];
        test_col[r] = letter;

        // Quick check: if 2 letters already placed in line, prioritize completing
        int row_filled = 0, col_filled = 0;
        for (int i = 0; i < 3; i++) {
            if (rows[r][i] != ' ') row_filled++;
            if (cols[c][i] != ' ') col_filled++;
        }

        if (row_filled >= 2 || col_filled >= 2) {
            letters.push_back(letter);
            if (letters.size() >= 8) break; 
        }
    }

    
    if (letters.empty()) {
        for (int i = 0; i < 6; i++) letters.push_back(common[i]);
    }

    return letters;
}

// Evaluation function: checks how close we are to forming words
int evaluate_word_board(Word_Board* board) {
    int score = 0;

    // Helper to check if a partial line could become a word
    auto check_potential = [&](const string& line) -> int {
        if (line.length() != 3) return 0;

        int blanks = 0;
        for (char c : line) if (c == ' ') blanks++;

        // If complete, check if it's a word
        if (blanks == 0) {
            if (board->check_word(line)) return 100; // Win!
            string rev = line;
            reverse(rev.begin(), rev.end());
            if (board->check_word(rev)) return 100; // Win!
            return 0;
        }

        // Check if we can complete this to a word 
        if (blanks == 1) {
            // Only try vowels and common consonants for speed
            const char quick_test[] = "AEIORSTLN";
            for (char letter : quick_test) {
                string test = line;
                for (int i = 0; i < 3; i++) {
                    if (test[i] == ' ') {
                        test[i] = letter;
                        if (board->check_word(test)) return 20;
                        string rev = test;
                        reverse(rev.begin(), rev.end());
                        if (board->check_word(rev)) return 20;
                        break;
                    }
                }
            }
        }

        return 0;
        };

    // Check all rows
    for (int i = 0; i < 3; i++) {
        string row;
        for (int j = 0; j < 3; j++) row += board->get_cell(i, j);
        score += check_potential(row);
    }

    // Check all columns
    for (int j = 0; j < 3; j++) {
        string col;
        for (int i = 0; i < 3; i++) col += board->get_cell(i, j);
        score += check_potential(col);
    }

    // Check diagonals
    string diag1, diag2;
    for (int i = 0; i < 3; i++) {
        diag1 += board->get_cell(i, i);
        diag2 += board->get_cell(i, 2 - i);
    }
    score += check_potential(diag1);
    score += check_potential(diag2);

    return score;
}

// Minimax algorithm for Word Tic-Tac-Toe 
int minimax_word(Word_Board* board, int depth, bool is_maximizing_player,
    char maximizing_player_symbol, char minimizing_player_symbol)
{
    if (depth >= MAX_DEPTH_WORD)
        return 0;

    // Terminal states:
    if (board->is_win(nullptr)) {
        return is_maximizing_player ? -10 : 10;
    }
    if (board->is_draw(nullptr)) {
        return 0;
    }

    if (is_maximizing_player) {
        int best_score = -99999;

        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                if (board->is_empty_cell(r, c)) {
                    for (char letter = 'A'; letter <= 'Z'; letter++) {

                        board->set_cell_value(r, c, letter);

                        int score = minimax_word(
                            board, depth + 1, false,
                            maximizing_player_symbol, minimizing_player_symbol
                        );

                        board->set_cell_value(r, c, ' ');

                        best_score = max(best_score, score);
                    }
                }
            }
        }
        return best_score;
    }
    else {
        int best_score = 99999;

        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                if (board->is_empty_cell(r, c)) {
                    for (char letter = 'A'; letter <= 'Z'; letter++) {

                        board->set_cell_value(r, c, letter);

                        int score = minimax_word(
                            board, depth + 1, true,
                            maximizing_player_symbol, minimizing_player_symbol
                        );

                        board->set_cell_value(r, c, ' ');

                        best_score = min(best_score, score);
                    }
                }
            }
        }
        return best_score;
    }
}

// Find best move for AI
pair<pair<int, int>, char> find_best_move_word(Word_Board* board, char player_symbol) {
    int best_score = -99999;
    pair<int, int> best_move = { -1, -1 };
    char best_letter = 'A';

    char opponent_symbol = (player_symbol == 'X') ? 'O' : 'X';

    // Try every empty cell
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (!board->is_empty_cell(r, c)) continue;

            // Try every letter A-Z
            for (char letter = 'A'; letter <= 'Z'; letter++) {

                board->set_cell_value(r, c, letter);

                int score = minimax_word(board, 0, false,
                    player_symbol, opponent_symbol);

                board->set_cell_value(r, c, ' '); // undo move

                // maximize score
                if (score > best_score) {
                    best_score = score;
                    best_move = { r, c };
                    best_letter = letter;
                }
            }
        }
    }

    return { best_move, best_letter };
}

//============================================ Word_UI Implementation ==================================================

Word_UI::Word_UI() : UI<char>("Welcome to Word Tic-Tac-Toe", 3) {}

Player<char>* Word_UI::create_player(string& name, char symbol, PlayerType type) {
    cout << "Creating " << (type == PlayerType::HUMAN ? "human" : "computer")
        << " player: " << name << "\n";
    return new Player<char>(name, symbol, type);
}

Player<char>** Word_UI::setup_players() {
    auto get_computer_level = []() {
        cout << "Select Computer Player Difficulty Level:\n";
        cout << "1. Easy\n";
        cout << "2. Medium\n";
        cout << "3. Hard\n";
        int level;
        while (true) {
            cout << "Enter your choice (1-3): ";
            cin >> level;
            if (level >= 1 && level <= 3) {
                return level;
            }
            else {
                cout << "Invalid choice. Please select a valid difficulty level.\n";
            }
        }
        };

    Player<char>** players = new Player<char>*[2];
    vector<string> type_options = { "Human", "Computer" };

    // Player 1
    string name1 = get_player_name("Player 1");
    PlayerType type1 = get_player_type_choice("Player 1", type_options);
    players[0] = create_player(name1, 'X', type1);

    if (type1 == PlayerType::COMPUTER) {
        int level = get_computer_level();
        if (level == 1) {
            cout << "Computer Player 1 set to Easy level.\n\n";
            MAX_DEPTH_WORD = 1; 
        }
        else if (level == 2) {
            cout << "Computer Player 1 set to Medium level.\n\n";
            MAX_DEPTH_WORD = 1; 
        }
        else {
            cout << "Computer Player 1 set to Hard level.\n\n";
            MAX_DEPTH_WORD = 1; // Same depth for all - difference is in evaluation quality
        }
    }

    // Player 2
    string name2 = get_player_name("Player 2");
    PlayerType type2 = get_player_type_choice("Player 2", type_options);
    players[1] = create_player(name2, 'O', type2);

    if (type2 == PlayerType::COMPUTER) {
        int level = get_computer_level();
        if (level == 1) {
            cout << "Computer Player 2 set to Easy level.\n\n";
            MAX_DEPTH_WORD = 1;
        }
        else if (level == 2) {
            cout << "Computer Player 2 set to Medium level.\n\n";
            MAX_DEPTH_WORD = 1;
        }
        else {
            cout << "Computer Player 2 set to Hard level.\n\n";
            MAX_DEPTH_WORD = 1; // All levels use depth 1 for instant response
        }
    }

    return players;
}

Move<char>* Word_UI::get_move(Player<char>* player) {
    int x = 0, y = 0;
    char letter = 'A';
    Board<char>* boardPtr = player->get_board_ptr();
    Word_Board* word_board = dynamic_cast<Word_Board*>(boardPtr);

    if (player->get_type() == PlayerType::HUMAN) {
        // Get letter
        cout << "Enter a letter (A to Z): ";
        while (true) {
            cin >> letter;
            letter = (char)toupper((unsigned char)letter);

            if (!isalpha((unsigned char)letter)) {
                cout << "Invalid letter. Please enter a letter (A to Z): ";
                cin.clear();
                continue;
            }
            break;
        }

        // Get position
        cout << "Please enter your move x (0 to 2) and y (0 to 2): ";
        while (true) {
            cin >> x >> y;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Invalid input. Enter numbers only: ";
                continue;
            }

            if (x < 0 || x > 2 || y < 0 || y > 2) {
                cout << "Please enter a Valid move x and y (0 to 2): ";
                continue;
            }
            if (boardPtr->get_cell(x, y) != ' ') {
                cout << "This cell is already used, choose another cell: ";
                continue;
            }
            break;
        }
    }
    else {
        // Computer move using AI
        auto best_move = find_best_move_word(word_board, player->get_symbol());
        x = best_move.first.first;
        y = best_move.first.second;
        letter = best_move.second;

        // Fallback if AI didn't find a move (shouldn't happen)
        if (x == -1) {
            vector<pair<int, int>> empty_cells;
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    if (boardPtr->get_cell(r, c) == ' ') {
                        empty_cells.push_back({ r, c });
                    }
                }
            }
            if (!empty_cells.empty()) {
                int idx = rand() % empty_cells.size();
                x = empty_cells[idx].first;
                y = empty_cells[idx].second;
                letter = 'A' + (rand() % 26);
            }
        }

        cout << "Computer chooses letter '" << letter
            << "' at (" << x << ", " << y << ")\n";
    }

    return new Move<char>(x, y, letter);
}

void Word_UI::display_board_matrix(const vector<vector<char>>& matrix) const {
    cout << "\n    0   1   2\n";
    cout << "   -----------\n";
    for (int i = 0; i < 3; ++i) {
        cout << i << " | ";
        for (int j = 0; j < 3; ++j) {
            char c = matrix[i][j];
            if (c == '\0') c = ' ';
            cout << c << " | ";
        }
        cout << "\n   -----------\n";
    }
}

//=================================== 5.Inverse Board ============================================
Inverse_Board::Inverse_Board() : Board<char>(3, 3) {
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < columns; ++j)
            board[i][j] = blank_symbol;
}

bool Inverse_Board::update_board(Move<char>* move) {
    int x = move->get_x();
    int y = move->get_y();
    char symbol = move->get_symbol();
    if (x < 0 || x >= rows || y < 0 || y >= columns) return false;
    if (board[x][y] != blank_symbol) return false;
    board[x][y] = symbol;
    n_moves++;
    return true;
}

bool Inverse_Board::is_win(Player<char>* player) {
    // the player cannot win directly
    return false;
}

bool Inverse_Board::is_lose(Player<char>* player) {
    char s = player->get_symbol();
    // rows
    for (int i = 0; i < 3; ++i)
        if (board[i][0] == s && board[i][1] == s && board[i][2] == s) return true;
    // columns
    for (int j = 0; j < 3; ++j)
        if (board[0][j] == s && board[1][j] == s && board[2][j] == s) return true;
    // diagonals
    if (board[0][0] == s && board[1][1] == s && board[2][2] == s) return true;
    if (board[0][2] == s && board[1][1] == s && board[2][0] == s) return true;

    return false;
}

bool Inverse_Board::is_draw(Player<char>* player) {
    if (is_lose(player)) return false;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (board[i][j] == blank_symbol) return false;
    return true;
}

bool Inverse_Board::game_is_over(Player<char>* player) {
    return is_lose(player) || is_draw(player);
}

//--------------------------- Inverse AI Player ------------------------------- 

int MAX_DEPTH_INVERSE; // minimax search depth

bool is_board_full_inverse(Inverse_Board* board) {
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (board->is_empty_cell(r, c)) {
                return false;
            }
        }
    }
    return true;
}

int count_in_line_inverse(Inverse_Board* board, int positions[][2], int count, char symbol) {
    int total = 0;
    for (int i = 0; i < count; i++) {
        if (board->get_cell(positions[i][0], positions[i][1]) == symbol) {
            total++;
        }
    }
    return total;
}


int evaluate_board_inverse(Inverse_Board* board, char ai_symbol, char opponent_symbol) {
    Player<char> ai_player("TempAI", ai_symbol, PlayerType::COMPUTER);
    ai_player.set_board_ptr(board);

    Player<char> opp_player("TempOpp", opponent_symbol, PlayerType::HUMAN);
    opp_player.set_board_ptr(board);

    if (board->is_lose(&ai_player)) {
        return -1000; 
    }

    if (board->is_lose(&opp_player)) {
        return 1000; 
    }

    int score = 0;

    // all winning lines 
    int lines[8][3][2] = {
      {{0,0}, {0,1}, {0,2}}, // row 0
      {{1,0}, {1,1}, {1,2}}, // row 1
      {{2,0}, {2,1}, {2,2}}, // row 2
      {{0,0}, {1,0}, {2,0}}, // col 0
      {{0,1}, {1,1}, {2,1}}, // col 1
      {{0,2}, {1,2}, {2,2}}, // col 2
      {{0,0}, {1,1}, {2,2}}, // main diagonal
      {{0,2}, {1,1}, {2,0}}  // anti-diagonal
    };

    for (int i = 0; i < 8; i++) {
        int ai_count = count_in_line_inverse(board, lines[i], 3, ai_symbol);
        int opp_count = count_in_line_inverse(board, lines[i], 3, opponent_symbol);
        int empty_count = count_in_line_inverse(board, lines[i], 3, ' ');

       
        if (opp_count == 2 && empty_count == 1) {
            score += 150;
        }

        if (opp_count == 1 && ai_count == 0 && empty_count == 2) {
            score += 30; 
        }

        if (ai_count == 2 && empty_count == 1) {
            score -= 200; 
        }

        if (ai_count == 1 && opp_count == 0 && empty_count == 2) {
            score -= 20; 
        }

       
        if (ai_count > 0 && opp_count > 0) {
            score += 10; 
        }
    }

    if (board->get_cell(1, 1) == ai_symbol) {
        score += 5;
    }

    return score;
}

int minimax_inverse(Inverse_Board* board, int depth, bool is_maximizing_player,
    char maximizing_player_symbol, char minimizing_player_symbol, int alpha, int beta) {

    if (depth >= MAX_DEPTH_INVERSE) {
        return evaluate_board_inverse(board, maximizing_player_symbol, minimizing_player_symbol);
    }

    
    Player<char> max_player("TempAI", maximizing_player_symbol, PlayerType::COMPUTER);
    max_player.set_board_ptr(board);

    if (board->is_lose(&max_player)) {
        return -1000 + depth; 
    }

    Player<char> min_player("TempHuman", minimizing_player_symbol, PlayerType::HUMAN);
    min_player.set_board_ptr(board);

    if (board->is_lose(&min_player)) {
        return 1000 - depth; 
    }

    if (is_board_full_inverse(board)) {
        return 0; 
    }

    char current_sym = is_maximizing_player ? maximizing_player_symbol : minimizing_player_symbol;

    if (is_maximizing_player) {
        int max_eval = numeric_limits<int>::min();

        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                if (board->is_empty_cell(r, c)) {
                    board->set_cell_value(r, c, current_sym);

                    int eval = minimax_inverse(board, depth + 1, false,
                        maximizing_player_symbol, minimizing_player_symbol, alpha, beta);

                    board->set_cell_value(r, c, ' ');

                    max_eval = max(max_eval, eval);
                    alpha = max(alpha, eval);

                    if (beta <= alpha) {
                        return max_eval; // Alpha-beta pruning
                    }
                }
            }
        }
        return max_eval;
    }
    else {
        int min_eval = numeric_limits<int>::max();

        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                if (board->is_empty_cell(r, c)) {
                    board->set_cell_value(r, c, current_sym);

                    int eval = minimax_inverse(board, depth + 1, true,
                        maximizing_player_symbol, minimizing_player_symbol, alpha, beta);

                    board->set_cell_value(r, c, ' ');

                    min_eval = min(min_eval, eval);
                    beta = min(beta, eval);

                    if (beta <= alpha) {
                        return min_eval; // Alpha-beta pruning
                    }
                }
            }
        }
        return min_eval;
    }
}

pair<int, int> find_best_move_inverse(Inverse_Board* board, char player_symbol) {
    int best_val = numeric_limits<int>::min();
    pair<int, int> best_move = { -1, -1 };

    char maximizing_player_symbol = player_symbol;
    char minimizing_player_symbol = (player_symbol == 'X') ? 'O' : 'X';

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (board->is_empty_cell(r, c)) {
                board->set_cell_value(r, c, maximizing_player_symbol);

                int move_val = minimax_inverse(board, 0, false, maximizing_player_symbol,
                    minimizing_player_symbol,
                    numeric_limits<int>::min(),
                    numeric_limits<int>::max());

                board->set_cell_value(r, c, ' ');

                if (move_val > best_val) {
                    best_val = move_val;
                    best_move = { r, c };
                }
            }
        }
    }
    return best_move;
}

//--- Inverse_UI ---

Inverse_UI::Inverse_UI() : UI<char>("Welcome to Inverse Tic-Tac-Toe", 3) {}

Player<char>* Inverse_UI::create_player(string& name, char symbol, PlayerType type) {
    cout << "Creating " << (type == PlayerType::HUMAN ? "human" : "computer")
        << " player: " << name << " (" << symbol << ")\n";
    cout << "\n";
    return new Player<char>(name, symbol, type);
}

Player<char>** Inverse_UI::setup_players() {
    auto get_com_level = []() {
        cout << "Select Computer Player Difficulty Level:\n";
        cout << "1. Easy\n";
        cout << "2. Medium\n";
        cout << "3. Hard\n";
        int level;
        while (true) {
            cout << "Enter your choice (1-3): ";
            cin >> level;
            if (level >= 1 && level <= 3) {
                return level;
            }
            else {
                cout << "Invalid choice. Please select a valid difficulty level.\n";
            }
        }
        };

    Player<char>** players = new Player<char>*[2];
    vector<string> type_options = { "Human", "Computer" };

    // player 1
    string nameX = get_player_name("Player 1 (X)");
    PlayerType typeX = get_player_type_choice("Player 1 (X)", type_options);
    players[0] = create_player(nameX, 'X', typeX);
    if (typeX == PlayerType::COMPUTER) {
        int level = get_com_level();
        if (level == 1) {
            cout << "Computer Player 1 (X) set to Easy level.\n";
            cout << "\n";
            MAX_DEPTH_INVERSE = 2;
        }
        else if (level == 2) {
            cout << "Computer Player 1 (X) set to Medium level.\n";
            cout << "\n";
            MAX_DEPTH_INVERSE = 5;
        }
        else {
            cout << "Computer Player 1 (X) set to Hard level.\n";
            cout << "\n";
            MAX_DEPTH_INVERSE = 9;
        }
    }

    // player 2
    string nameO = get_player_name("Player 2 (O)");
    PlayerType typeO = get_player_type_choice("Player 2 (O)", type_options);
    players[1] = create_player(nameO, 'O', typeO);
    if (typeO == PlayerType::COMPUTER) {
        int level = get_com_level();
        if (level == 1) {
            cout << "Computer Player 2 (O) set to Easy level.\n";
            cout << "\n";
            MAX_DEPTH_INVERSE = 2;
        }
        else if (level == 2) {
            cout << "Computer Player 2 (O) set to Medium level.\n";
            cout << "\n";
            MAX_DEPTH_INVERSE = 5;
        }
        else {
            cout << "Computer Player 2 (O) set to Hard level.\n";
            cout << "\n";
            MAX_DEPTH_INVERSE = 9;
        }
    }
    return players;
}

Move<char>* Inverse_UI::get_move(Player<char>* player) {
    int x, y;
    Board<char>* bp = player->get_board_ptr();
    Inverse_Board* ib = dynamic_cast<Inverse_Board*>(bp);

    if (player->get_type() == PlayerType::COMPUTER) {


        // minimax AI
        pair<int, int> best_pos = find_best_move_inverse(ib, player->get_symbol());
        x = best_pos.first;
        y = best_pos.second;

        cout << player->get_name() << " (" << player->get_symbol() << ") plays: " << x << " " << y << endl;
        return new Move<char>(x, y, player->get_symbol());
    }

    cout << player->get_name() << " (" << player->get_symbol() << ") enter row and column (0-2): ";
    cin >> x >> y;
    return new Move<char>(x, y, player->get_symbol());
}

void Inverse_UI::display_board_matrix(const vector<vector<char>>& matrix) const {
    cout << "\nInverse Tic-Tac-Toe Board:\n\n";

    // Print column labels
    cout << "    0   1   2\n";
    cout << "  -------------\n";

    for (int i = 0; i < 3; i++) {
        cout << i << " | ";
        for (int j = 0; j < 3; j++) {
            cout << matrix[i][j];
            if (j < 3) cout << " | ";
        }
        cout << "\n";
        if (i < 3) cout << "  -------------\n";
    }
    cout << "\n";
}

// ================================ 6.Diamond_Board Implementation (7x7) ==================================================
/*
allowed coordinates for 7x7 diamond:
(0,3)
(1,2),(1,3),(1,4)
(2,1),(2,2),(2,3),(2,4),(2,5)
(3,0),(3,1),(3,2),(3,3),(3,4),(3,5),(3,6)
(4,1),(4,2),(4,3),(4,4),(4,5)
(5,2),(5,3),(5,4)
(6,3)
*/

/*
winning condition:
one 3-in-a-row line
&&
one 4-in-a-row line
in different directions
*/


// ---------------------- Diamond_Board ----------------------
Diamond_Board::Diamond_Board() : Board<char>(7, 7) {
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            board[i][j] = blank_symbol;
}

bool Diamond_Board::in_bounds(int x, int y) const {
    return x >= 0 && x < rows && y >= 0 && y < columns;
}

bool Diamond_Board::is_valid_diamond_cell(int x, int y) const {
    if (!in_bounds(x, y)) return false;
    if (x == 0 || x == 6) return y == 3;
    if (x == 1 || x == 5) return y >= 2 && y <= 4;
    if (x == 2 || x == 4) return y >= 1 && y <= 5;
    if (x == 3) return y >= 0 && y <= 6;
    return false;
}

bool Diamond_Board::update_board(Move<char>* move) {
    int x = move->get_x();
    int y = move->get_y();
    char s = move->get_symbol();
    if (!in_bounds(x, y) || !is_valid_diamond_cell(x, y) || board[x][y] != blank_symbol) return false;
    board[x][y] = s;
    n_moves++;
    return true;
}

int Diamond_Board::count_three_sequences(char sym) const {
    int count = 0;
    int dx[4] = { 0,1,1,1 }, dy[4] = { 1,0,1,-1 };
    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < columns; y++) {
            if (!is_valid_diamond_cell(x, y) || board[x][y] != sym) continue;
            for (int d = 0; d < 4; d++) {
                int x1 = x + dx[d], y1 = y + dy[d];
                int x2 = x + 2 * dx[d], y2 = y + 2 * dy[d];
                if (in_bounds(x1, y1) && in_bounds(x2, y2) &&
                    is_valid_diamond_cell(x1, y1) && is_valid_diamond_cell(x2, y2) &&
                    board[x1][y1] == sym && board[x2][y2] == sym)
                    count++;
            }
        }
    }
    return count;
}

int Diamond_Board::count_four_sequences(char sym) const {
    int count = 0;
    int dx[4] = { 0,1,1,1 }, dy[4] = { 1,0,1,-1 };
    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < columns; y++) {
            if (!is_valid_diamond_cell(x, y) || board[x][y] != sym) continue;
            for (int d = 0; d < 4; d++) {
                bool ok = true;
                for (int k = 1; k < 4; k++) {
                    int nx = x + k * dx[d], ny = y + k * dy[d];
                    if (!in_bounds(nx, ny) || !is_valid_diamond_cell(nx, ny) || board[nx][ny] != sym) {
                        ok = false; break;
                    }
                }
                if (ok) count++;
            }
        }
    }
    return count;
}

void Diamond_Board::sequence_directions(char sym, int length, bool dirs[4]) const {
    int dx[4] = { 0, 1, 1, 1 };  // horizontal, vertical, diag1, diag2
    int dy[4] = { 1, 0, 1, -1 };

    //init. directions to false
    for (int i = 0; i < 4; i++) dirs[i] = false;

    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < columns; y++) {
            if (!is_valid_diamond_cell(x, y) || board[x][y] != sym) continue;

            for (int d = 0; d < 4; d++) {
                bool ok = true;
                for (int k = 1; k < length; k++) {
                    int nx = x + k * dx[d], ny = y + k * dy[d];
                    if (!in_bounds(nx, ny) || !is_valid_diamond_cell(nx, ny) || board[nx][ny] != sym) {
                        ok = false;
                        break;
                    }
                }
                if (ok) dirs[d] = true;
            }
        }
    }
}

// player wins?
bool Diamond_Board::is_win(Player<char>* player) {
    char sym = player->get_symbol();
    bool dirs3[4], dirs4[4];
    sequence_directions(sym, 3, dirs3);
    sequence_directions(sym, 4, dirs4);

    for (int d3 = 0; d3 < 4; d3++) {
        if (!dirs3[d3]) continue;
        for (int d4 = 0; d4 < 4; d4++) {
            if (!dirs4[d4]) continue;
            if (d3 != d4) return true; // different directions
        }
    }
    return false;
}
bool Diamond_Board::is_lose(Player<char>*) { return false; }

bool Diamond_Board::is_draw(Player<char>*) {
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            if (is_valid_diamond_cell(i, j) && board[i][j] == blank_symbol)
                return false;
    return true;
}

bool Diamond_Board::game_is_over(Player<char>* player) {
    return is_win(player) || is_draw(player);
}

//--------------------------- Diamond AI Player ------------------------------- 

int MAX_DEPTH_DIAMOND; // depth for minimax search

bool is_board_full_diamond(Diamond_Board* board) {
    for (int r = 0; r < 7; ++r) {
        for (int c = 0; c < 7; ++c) {
            if (board->is_valid_diamond_cell(r, c) && board->is_empty_cell(r, c)) {
                return false;
            }
        }
    }
    return true;
}

//AI eval func.
int evaluate_board_diamond(Diamond_Board* board, char ai_symbol, char opponent_symbol) {
    Player<char> ai_player("TempAI", ai_symbol, PlayerType::COMPUTER);
    ai_player.set_board_ptr(board);

    Player<char> opp_player("TempOpp", opponent_symbol, PlayerType::HUMAN);
    opp_player.set_board_ptr(board);

    // Check for wins
    if (board->is_win(&ai_player)) return 1000;
    if (board->is_win(&opp_player)) return -1000;

    // Count sequences
    int ai_three = board->count_three_sequences(ai_symbol);
    int ai_four = board->count_four_sequences(ai_symbol);
    int opp_three = board->count_three_sequences(opponent_symbol);
    int opp_four = board->count_four_sequences(opponent_symbol);

    // Check directions 
    bool ai_dirs3[4], ai_dirs4[4];
    bool opp_dirs3[4], opp_dirs4[4];
    board->sequence_directions(ai_symbol, 3, ai_dirs3);
    board->sequence_directions(ai_symbol, 4, ai_dirs4);
    board->sequence_directions(opponent_symbol, 3, opp_dirs3);
    board->sequence_directions(opponent_symbol, 4, opp_dirs4);

    // Count different directions  for AI 
    int ai_three_dirs = 0, ai_four_dirs = 0;
    int opp_three_dirs = 0, opp_four_dirs = 0;
    for (int i = 0; i < 4; i++) {
        if (ai_dirs3[i]) ai_three_dirs++;
        if (ai_dirs4[i]) ai_four_dirs++;
        if (opp_dirs3[i]) opp_three_dirs++;
        if (opp_dirs4[i]) opp_four_dirs++;
    }


    int score = 0;


    score += ai_three_dirs * 30;
    score += ai_four_dirs * 80;

    if (ai_three_dirs > 0 && ai_four_dirs > 0) {
        score += 200;
    }


    score -= opp_three_dirs * 35;
    score -= opp_four_dirs * 90;

    if (opp_three_dirs > 0 && opp_four_dirs > 0) {
        score -= 250;
    }


    score += ai_three * 5;
    score += ai_four * 15;
    score -= opp_three * 6;
    score -= opp_four * 18;

    return score;
}

int minimax_diamond(Diamond_Board* board, int depth, bool is_maximizing_player,
    char maximizing_player_symbol, char minimizing_player_symbol, int alpha, int beta) {

    if (depth >= MAX_DEPTH_DIAMOND) {
        return evaluate_board_diamond(board, maximizing_player_symbol, minimizing_player_symbol);
    }

    Player<char> max_player("TempAI", maximizing_player_symbol, PlayerType::COMPUTER);
    max_player.set_board_ptr(board);

    if (board->is_win(&max_player)) {
        return 1000 - depth;
    }

    Player<char> min_player("TempHuman", minimizing_player_symbol, PlayerType::HUMAN);
    min_player.set_board_ptr(board);

    if (board->is_win(&min_player)) {
        return -1000 + depth;
    }

    if (is_board_full_diamond(board)) {
        return 0; // draw
    }

    char current_sym = is_maximizing_player ? maximizing_player_symbol : minimizing_player_symbol;

    if (is_maximizing_player) {
        int max_eval = numeric_limits<int>::min();

        for (int r = 0; r < 7; ++r) {
            for (int c = 0; c < 7; ++c) {
                if (board->is_valid_diamond_cell(r, c) && board->is_empty_cell(r, c)) {
                    board->set_cell_value(r, c, current_sym);

                    int eval = minimax_diamond(board, depth + 1, false,
                        maximizing_player_symbol, minimizing_player_symbol, alpha, beta);

                    board->set_cell_value(r, c, ' ');

                    max_eval = max(max_eval, eval);
                    alpha = max(alpha, eval);

                    if (beta <= alpha) {
                        return max_eval;
                    }
                }
            }
        }
        return max_eval;
    }
    else {
        int min_eval = numeric_limits<int>::max();

        for (int r = 0; r < 7; ++r) {
            for (int c = 0; c < 7; ++c) {
                if (board->is_valid_diamond_cell(r, c) && board->is_empty_cell(r, c)) {
                    board->set_cell_value(r, c, current_sym);

                    int eval = minimax_diamond(board, depth + 1, true,
                        maximizing_player_symbol, minimizing_player_symbol, alpha, beta);

                    board->set_cell_value(r, c, ' ');

                    min_eval = min(min_eval, eval);
                    beta = min(beta, eval);

                    if (beta <= alpha) {
                        return min_eval;
                    }
                }
            }
        }
        return min_eval;
    }
}

pair<int, int> find_best_move_diamond(Diamond_Board* board, char player_symbol) {
    int best_val = numeric_limits<int>::min();
    pair<int, int> best_move = { -1, -1 };

    char maximizing_player_symbol = player_symbol;
    char minimizing_player_symbol = (player_symbol == 'X') ? 'O' : 'X';

    for (int r = 0; r < 7; ++r) {
        for (int c = 0; c < 7; ++c) {
            if (board->is_valid_diamond_cell(r, c) && board->is_empty_cell(r, c)) {
                board->set_cell_value(r, c, maximizing_player_symbol);

                int move_val = minimax_diamond(board, 0, false, maximizing_player_symbol,
                    minimizing_player_symbol,
                    numeric_limits<int>::min(),
                    numeric_limits<int>::max());

                board->set_cell_value(r, c, ' ');

                if (move_val > best_val) {
                    best_val = move_val;
                    best_move = { r, c };
                }
            }
        }
    }
    return best_move;
}

// ---------------------- Diamond_UI ----------------------

Diamond_UI::Diamond_UI() : UI<char>("Welcome to Diamond Tic-Tac-Toe", 3) {}

Player<char>* Diamond_UI::create_player(string& name, char symbol, PlayerType type) {
    cout << "Creating " << (type == PlayerType::HUMAN ? "human" : "computer")
        << " player: " << name << " (" << symbol << ")\n";
    cout << "\n";
    return new Player<char>(name, symbol, type);
}

Player<char>** Diamond_UI::setup_players() {
    auto get_com_level = []() {
        cout << "Select Computer Player Difficulty Level:\n";
        cout << "1. Easy\n";
        cout << "2. Medium\n";
        cout << "3. Hard\n";
        int level;
        while (true) {
            cout << "Enter your choice (1-3): ";
            cin >> level;
            if (level >= 1 && level <= 3) {
                return level;
            }
            else {
                cout << "Invalid choice. Please select a valid difficulty level.\n";
            }
        }
        };

    Player<char>** players = new Player<char>*[2];
    vector<string> type_options = { "Human", "Computer" };

    // player 1
    string nameX = get_player_name("Player 1 (X)");
    PlayerType typeX = get_player_type_choice("Player 1 (X)", type_options);
    players[0] = create_player(nameX, 'X', typeX);
    if (typeX == PlayerType::COMPUTER) {
        int level = get_com_level();
        if (level == 1) {
            cout << "Computer Player 1 (X) set to Easy level.\n";
            cout << "\n";
            MAX_DEPTH_DIAMOND = 2;
        }
        else if (level == 2) {
            cout << "Computer Player 1 (X) set to Medium level.\n";
            cout << "\n";
            MAX_DEPTH_DIAMOND = 4;
        }
        else {
            cout << "Computer Player 1 (X) set to Hard level.\n";
            cout << "\n";
            MAX_DEPTH_DIAMOND = 8;
        }
    }

    // player 2
    string nameO = get_player_name("Player 2 (O)");
    PlayerType typeO = get_player_type_choice("Player 2 (O)", type_options);
    players[1] = create_player(nameO, 'O', typeO);
    if (typeO == PlayerType::COMPUTER) {
        int level = get_com_level();
        if (level == 1) {
            cout << "Computer Player 2 (O) set to Easy level.\n";
            cout << "\n";
            MAX_DEPTH_DIAMOND = 2;
        }
        else if (level == 2) {
            cout << "Computer Player 2 (O) set to Medium level.\n";
            cout << "\n";
            MAX_DEPTH_DIAMOND = 4;
        }
        else {
            cout << "Computer Player 2 (O) set to Hard level.\n";
            cout << "\n";
            MAX_DEPTH_DIAMOND = 8;
        }
    }
    return players;
}

static void flush_stdin_line() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

Move<char>* Diamond_UI::get_move(Player<char>* player) {
    int x = 0, y = 0;
    Board<char>* bp = player->get_board_ptr();
    Diamond_Board* db = dynamic_cast<Diamond_Board*>(bp);

    if (player->get_type() == PlayerType::COMPUTER) {

        // AI implementation 
        pair<int, int> best_pos = find_best_move_diamond(db, player->get_symbol());
        x = best_pos.first;
        y = best_pos.second;

        cout << player->get_name() << " (" << player->get_symbol() << ") plays: " << x << " " << y << endl;
        return new Move<char>(x, y, player->get_symbol());
    }

    while (true) {
        cout << player->get_name() << " (" << player->get_symbol() << ") enter x y: ";
        if (!(cin >> x >> y)) { cout << "Invalid input.\n"; flush_stdin_line(); continue; }
        if (x < 0 || x > 6 || y < 0 || y > 6) { cout << "Coordinates out of range.\n"; continue; }

        Board<char>* bp = player->get_board_ptr();
        Diamond_Board* db = dynamic_cast<Diamond_Board*>(bp);
        if (db) {
            if (!db->is_valid_diamond_cell(x, y)) { cout << "Not a diamond cell.\n"; continue; }
            if (db->get_cell(x, y) != ' ') { cout << "Cell occupied.\n"; continue; }
        }
        flush_stdin_line();
        break;
    }
    return new Move<char>(x, y, player->get_symbol());
}

void Diamond_UI::display_board_matrix(const vector<vector<char>>& m) const {
    cout << "\n";
    cout << "               3\n";
    cout << "             +---+\n";
    cout << "           0 | " << m[0][3] << " |\n";
    cout << "             +---+\n";
    cout << "         +---+---+---+\n";
    cout << "       1 | " << m[1][2] << " | " << m[1][3] << " | " << m[1][4] << " |\n";
    cout << "         +---+---+---+\n";
    cout << "     +---+---+---+---+---+\n";
    cout << "   2 | " << m[2][1] << " | " << m[2][2] << " | " << m[2][3] << " | " << m[2][4] << " | " << m[2][5] << " |\n";
    cout << "     +---+---+---+---+---+\n";
    cout << " +---+---+---+---+---+---+---+\n";
    cout << "3| " << m[3][0] << " | " << m[3][1] << " | " << m[3][2] << " | " << m[3][3] << " | " << m[3][4] << " | " << m[3][5] << " | " << m[3][6] << " |\n";
    cout << " +---+---+---+---+---+---+---+\n";
    cout << "     +---+---+---+---+---+\n";
    cout << "   4 | " << m[4][1] << " | " << m[4][2] << " | " << m[4][3] << " | " << m[4][4] << " | " << m[4][5] << " |\n";
    cout << "     +---+---+---+---+---+\n";
    cout << "         +---+---+---+\n";
    cout << "       5 | " << m[5][2] << " | " << m[5][3] << " | " << m[5][4] << " |\n";
    cout << "         +---+---+---+\n";
    cout << "             +---+\n";
    cout << "           6 | " << m[6][3] << " |\n";
    cout << "             +---+\n";
}




//================================================= 7.4x4 Board Game ===========================================================

//------FourByFour_Board imp------

FourByFour_Board::FourByFour_Board() : Board<char>(4, 4) {
  // player x tokens 
  board[0][1] = 'X';
  board[0][3] = 'X';
  board[3][0] = 'X';
  board[3][2] = 'X';
  // player O tokens
  board[0][0] = 'O';
  board[0][2] = 'O';
  board[3][1] = 'O';
  board[3][3] = 'O';

  // Empty cells
  for (int i = 1; i <= 2; i++) {
    for (int j = 0; j < columns; j++) {
      board[i][j] = blank_symbol;
    }
  }
}

bool FourByFour_Board::update_board(Move<char>* move) {
  int x = move->get_x();
  int y = move->get_y();
  char sym = move->get_symbol();

  // get the old position of the token that will move
  int old_x = choice_token_toMove_position.first;
  int old_y = choice_token_toMove_position.second;

  // make the old cell blank
  board[old_x][old_y] = blank_symbol;

  // make the move to the new position
  board[x][y] = sym;

  n_moves += 1;
  return true;
}

bool FourByFour_Board::is_win(Player<char>* player) {
    char sym = player->get_symbol();

    auto all_equal_sym = [&](char a, char b, char c) {
        return a == sym && b == sym && c == sym;
        };

    // rows and columns (check any 3-in-row that equals player's symbol)
    for (int i = 0; i < 4; ++i) {
        if (all_equal_sym(board[i][0], board[i][1], board[i][2])) return true;
        if (all_equal_sym(board[i][1], board[i][2], board[i][3])) return true;
        if (all_equal_sym(board[0][i], board[1][i], board[2][i])) return true;
        if (all_equal_sym(board[1][i], board[2][i], board[3][i])) return true;
    }

    // diagonals (all 3-length diagonals) — ensure they equal sym
    if (all_equal_sym(board[0][0], board[1][1], board[2][2])) return true;
    if (all_equal_sym(board[1][1], board[2][2], board[3][3])) return true;
    if (all_equal_sym(board[0][1], board[1][2], board[2][3])) return true;
    if (all_equal_sym(board[1][0], board[2][1], board[3][2])) return true;
    if (all_equal_sym(board[0][2], board[1][1], board[2][0])) return true;
    if (all_equal_sym(board[1][3], board[2][2], board[3][1])) return true;
    if (all_equal_sym(board[0][3], board[1][2], board[2][1])) return true;
    if (all_equal_sym(board[1][2], board[2][1], board[3][0])) return true;

    return false;
}


bool FourByFour_Board::is_draw(Player<char>* player) {
  return false;// no draw in this game
}

bool FourByFour_Board::game_is_over(Player<char>* player) {
  return is_win(player);// game is over only if any player win
}

pair<int, int> FourByFour_Board::get_token_place_forH(Player<char>* player) {
  int x, y;
  cout << "\n" << player->get_name() << ", enter the position of your token that you want to move (x and y from 0 to 3): ";
  while (true) {
    cin >> x >> y;
    if (x < 0 || x > 3 || y < 0 || y > 3) {// check valid position
      cout << "Please enter a Valid position for the token x and y (0 to 3): ";
      continue;
    }
    else if (player->get_board_ptr()->get_cell(x, y) != player->get_symbol()) {// check the token belong to the player
      cout << "Invalid choice. You must choose one of your own tokens. Please try again: ";
      continue;
    }
    else
      break;
  }

  // store the chosen token position that will move
  choice_token_toMove_position = make_pair(x, y);

  return choice_token_toMove_position;
}

pair<int, int> FourByFour_Board::get_token_place_forC(Player<char>* player) {
  int x, y;
  while (true) {
    x = rand() % 4;
    y = rand() % 4;
    if (player->get_board_ptr()->get_cell(x, y) == player->get_symbol()) {// check the token belong to the computer
      break;
    }
  }

  // store the chosen token position that will move
  choice_token_toMove_position = make_pair(x, y);

  return choice_token_toMove_position;
}

//===============
// Ai helpers 
//===============

int Max_depth_4x4;

// return positions of tokens of symbol sym
vector<pair<int, int>> FourByFour_Board::get_tokens_positions(char sym) const {
    vector<pair<int, int>> pos;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (board[r][c] == sym) pos.emplace_back(r, c);
        }
    }
    return pos;
}

// generate all legal moves as pairs: {from, to}
vector<pair<pair<int, int>, pair<int, int>>> FourByFour_Board::generate_moves_for(char sym) const {
    vector<pair<pair<int, int>, pair<int, int>>> moves;
    auto tokens = get_tokens_positions(sym);
    const int dr[4] = { -1, 1, 0, 0 };
    const int dc[4] = { 0, 0, -1, 1 };
    for (auto& p : tokens) {
        int r = p.first, c = p.second;
        for (int k = 0; k < 4; ++k) {
            int nr = r + dr[k], nc = c + dc[k];
            if (nr >= 0 && nr < 4 && nc >= 0 && nc < 4) {
                if (board[nr][nc] == blank_symbol) {
                    moves.push_back({ {r,c},{nr,nc} });
                }
            }
        }
    }
    return moves;
}

// apply the move (from -> to)
void FourByFour_Board::apply_move_pair(const std::pair<std::pair<int, int>, std::pair<int, int>>& mv) {
    int fr = mv.first.first, fc = mv.first.second;
    int tr = mv.second.first, tc = mv.second.second;
    char sym = board[fr][fc];
    board[fr][fc] = blank_symbol;
    board[tr][tc] = sym;
}

// undo the move
void FourByFour_Board::undo_move_pair(const std::pair<std::pair<int, int>, std::pair<int, int>>& mv) {
    int fr = mv.first.first, fc = mv.first.second;
    int tr = mv.second.first, tc = mv.second.second;
    char sym = board[tr][tc];
    board[tr][tc] = blank_symbol;
    board[fr][fc] = sym;
}


// evaluation: difference in count of 3-in-a-row sequences
int FourByFour_Board::evaluate_board_for(char sym) const {
    char other = (sym == 'X') ? 'O' : 'X';

    auto count_three = [&](char s)->int {
        int cnt = 0;
        auto all_equal = [&](char a, char b, char c) {
            return a == b && b == c && a != blank_symbol;
            };
        // rows
        for (int i = 0; i < 4; ++i) {
            if (all_equal(board[i][0], board[i][1], board[i][2])) cnt++;
            if (all_equal(board[i][1], board[i][2], board[i][3])) cnt++;
        }
        // cols
        for (int j = 0; j < 4; ++j) {
            if (all_equal(board[0][j], board[1][j], board[2][j])) cnt++;
            if (all_equal(board[1][j], board[2][j], board[3][j])) cnt++;
        }
        // diagonals (all 3-length diagonals)
        if (all_equal(board[0][0], board[1][1], board[2][2])) cnt++;
        if (all_equal(board[1][1], board[2][2], board[3][3])) cnt++;
        if (all_equal(board[0][1], board[1][2], board[2][3])) cnt++;
        if (all_equal(board[1][0], board[2][1], board[3][2])) cnt++;
        if (all_equal(board[0][2], board[1][1], board[2][0])) cnt++;
        if (all_equal(board[1][3], board[2][2], board[3][1])) cnt++;
        if (all_equal(board[0][3], board[1][2], board[2][1])) cnt++;
        if (all_equal(board[1][2], board[2][1], board[3][0])) cnt++;
        return cnt;
        };

    int my_count = count_three(sym);
    int opp_count = count_three(other);

    return my_count - opp_count;
}

// minimax with alpha-beta and depth limit
int FourByFour_Board::minimax_fourbyfour(FourByFour_Board* board, int depth, int maxDepth, bool maximizingPlayer, char maximizingSym, char minimizingSym, int alpha, int beta) {
    // terminal checks: if someone has a win or depth limit reached
    Player<char> maxP("tempMax", maximizingSym, PlayerType::COMPUTER); maxP.set_board_ptr(board);
    Player<char> minP("tempMin", minimizingSym, PlayerType::HUMAN); minP.set_board_ptr(board);

    if (board->is_win(&maxP)) return 1000 - depth; // prefer faster win
    if (board->is_win(&minP)) return depth - 1000; // avoid loss
    if (depth >= maxDepth) {
        // return heuristic evaluation
        return board->evaluate_board_for(maximizingSym);
    }

    if (maximizingPlayer) {
        int best = numeric_limits<int>::min();
        auto moves = board->generate_moves_for(maximizingSym);
        if (moves.empty()) {
            // no moves -> evaluate
            return board->evaluate_board_for(maximizingSym);
        }
        for (auto& mv : moves) {
            board->apply_move_pair(mv);
            int val = minimax_fourbyfour(board, depth + 1, maxDepth, false, maximizingSym, minimizingSym, alpha, beta);
            board->undo_move_pair(mv);
            best = max(best, val);
            alpha = max(alpha, best);
            if (beta <= alpha) break; // beta cut-off
        }
        return best;
    }
    else {
        int best = numeric_limits<int>::max();
        auto moves = board->generate_moves_for(minimizingSym);
        if (moves.empty()) {
            return board->evaluate_board_for(maximizingSym);
        }
        for (auto& mv : moves) {
            board->apply_move_pair(mv);
            int val = minimax_fourbyfour(board, depth + 1, maxDepth, true, maximizingSym, minimizingSym, alpha, beta);
            board->undo_move_pair(mv);
            best = min(best, val);
            beta = min(beta, best);
            if (beta <= alpha) break;
        }
        return best;
    }
}

// find best move: returns pair {from, to}
pair<pair<int, int>,pair<int, int>> FourByFour_Board::find_best_move_fourbyfour(FourByFour_Board* board, char player_symbol, int maxDepth) {
    char opponent = (player_symbol == 'X') ? 'O' : 'X';
    int bestVal = numeric_limits<int>::min();
    vector<pair<pair<int, int>, pair<int, int>>> bestMoves;

    auto moves = board->generate_moves_for(player_symbol);
    if (moves.empty()) return { {-1,-1},{-1,-1} };

    for (auto& mv : moves) {
        board->apply_move_pair(mv);
        int moveVal = minimax_fourbyfour(board, 0, maxDepth, false, player_symbol, opponent, std::numeric_limits<int>::min(), numeric_limits<int>::max());
        board->undo_move_pair(mv);

        if (moveVal > bestVal) {
            bestVal = moveVal;
            bestMoves.clear();
            bestMoves.push_back(mv);
        }
        else if (moveVal == bestVal) {
            bestMoves.push_back(mv);
        }
    }

    // tie-break: prefer moves that move toward center (1,1),(1,2),(2,1),(2,2)
    auto prefer_score = [](const pair<pair<int, int>, pair<int, int>>& mv) {
        int tr = mv.second.first, tc = mv.second.second;
        int score = 0;
        // center preference
        if ((tr == 1 || tr == 2) && (tc == 1 || tc == 2)) score += 5;
        // closer to central columns/rows
        score += (2 - abs(1 - tr));
        score += (2 - abs(1 - tc));
        return score;
        };

    // pick best by preference
    auto chosen = bestMoves[0];
    int bestPref = prefer_score(chosen);
    for (size_t i = 1; i < bestMoves.size(); ++i) {
        int sc = prefer_score(bestMoves[i]);
        if (sc > bestPref) {
            bestPref = sc;
            chosen = bestMoves[i];
        }
    }

    return chosen;
}

//-------------------------------FourByFour_UI imp----------------------------------------

FourByFour_UI::FourByFour_UI() : UI<char>("Welcome to 4x4 Tic-Tac-Toe", 4) {}

Player<char>* FourByFour_UI::create_player(string& name, char symbol, PlayerType type) {
    // Create player based on type
    cout << "Creating " << (type == PlayerType::HUMAN ? "human" : "computer")
        << " player: " << name << " (" << symbol << ")\n";
    cout << "\n";

    return new Player<char>(name, symbol, type);
}

Player<char>** FourByFour_UI::setup_players() {
    auto git_com_level = []() {
        cout << "Select Computer Player Difficulty Level:\n";
        cout << "1. Easy\n";
        cout << "2. Medium\n";
        cout << "3. Hard\n";
        int level;
        while (true) {
            cout << "Enter your choice (1-3): ";
            cin >> level;
            if (level >= 1 && level <= 3) {
                return level;
            }
            else {
                cout << "Invalid choice. Please select a valid difficulty level.\n";
            }
        }
        };
    Player<char>** players = new Player<char>*[2];
    vector<string> type_options = { "Human", "Computer" };
    // player 1
    string nameX = get_player_name("Player 1 (X)");
    PlayerType typeX = get_player_type_choice("Player 1 (X)", type_options);
    players[0] = create_player(nameX, 'X', typeX);
    if (typeX == PlayerType::COMPUTER) {
        int level = git_com_level();
        if (level == 1) {
            cout << "Computer Player 1 (X) set to Easy level.\n";
            cout << "\n";
            Max_depth_4x4 = 1;
        }
        else if (level == 2) {
            cout << "Computer Player 1 (X) set to Medium level.\n";
            cout << "\n";
            Max_depth_4x4 = 4;
        }
        else {
            cout << "Computer Player 1 (X) set to Hard level.\n";
            cout << "\n";
            Max_depth_4x4 = 8;
        }
    }
    // player 2
    string nameO = get_player_name("Player 2 (O)");
    PlayerType typeO = get_player_type_choice("Player 2 (O)", type_options);
    players[1] = create_player(nameO, 'O', typeO);
    if (typeO == PlayerType::COMPUTER) {
        int level = git_com_level();
        if (level == 1) {
            cout << "Computer Player 2 (O) set to Easy level.\n";
            cout << "\n";
            Max_depth_4x4 = 1;
        }
        else if (level == 2) {
            cout << "Computer Player 2 (O) set to Medium level.\n";
            cout << "\n";
            Max_depth_4x4 = 5;
        }
        else {
            cout << "Computer Player 2 (O) set to Hard level.\n";
            cout << "\n";
            Max_depth_4x4 = 8;
        }
    }
    return players;
}

Move<char>* FourByFour_UI::get_move(Player<char>* player) {
  int x, y;
  pair<int, int> token_position;// position of the token that will move

  // get pointer to the fourbyfour board not the main board class
  auto fourByfour_board_ptr = dynamic_cast<FourByFour_Board*>(player->get_board_ptr());

  // get the token position that will move and store it in token_position
  if (player->get_type() == PlayerType::HUMAN) {
    token_position = fourByfour_board_ptr->get_token_place_forH(player);
    cout << "\nPlease enter the adjacent empty cell to move to (x and y from 0 to 3): ";
    while (true) {
        cin >> x >> y;

        if (x < 0 || x > 3 || y < 0 || y > 3) {// check valid position
            cout << "Invalid move. Please enter x and y (0 to 3): ";
            continue;
        }
        else if (fourByfour_board_ptr->get_cell(x, y) != ' ') {// check the cell is empty
            cout << "Invalid move. This cell is not empty, choose another cell: ";
            continue;
        }
        bool is_adjacent = (abs(x - token_position.first) == 1 && y == token_position.second) ||
            (abs(y - token_position.second) == 1 && x == token_position.first);

        if (is_adjacent) {// check the cell is adjacent to the token old position
            break;
        }
        else {// not adjacent
            cout << "Invalid move. You can only move to adjacent (horizontal or vertical) empty cells: ";
            continue;
        }

    }
  }
  else {
      // Use AI to pick best move
      auto best_move = fourByfour_board_ptr->find_best_move_fourbyfour(fourByfour_board_ptr, player->get_symbol(), Max_depth_4x4);
      auto from = best_move.first;
      auto to = best_move.second;

      // set the token position so update_board can use it
      fourByfour_board_ptr->set_choice_token_position(from);

      x = to.first;
      y = to.second;

      display_message("Computer moves token from (" + to_string(from.first) + "," + to_string(from.second) + ") to (" + to_string(x) + "," + to_string(y) + ")");
  }

  return new Move<char>(x, y, player->get_symbol());
}

//=============================================== 8.Pyramid_Board Implementation======================================================


/// 
/// the Board look like this: 
///               ---  
///            0 |   |
///           -----------
/// 	   1 |   |   |   |
///       -------------------
///    2 |   |   |   |   |   |
///       -------------------
///        0   1   2   3   4 


//------pyramid board functions -----

Pyramid_Board::Pyramid_Board() : Board<char>(3, 5) {// make 3*5 board his cells = ' '
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      board[i][j] = blank_symbol;
    }
  }
}

bool Pyramid_Board::update_board(Move<char>* move) {// ubdate board 
  int x = move->get_x();
  int y = move->get_y();
  char symbol = move->get_symbol();
  // cheak if the cell is empty 
  if (x < 0 || x > 2 || y < 0 || y > 4 || board[x][y] != blank_symbol) {
    return false;
  }
  // make the move 
  n_moves++;
  board[x][y] = symbol;
  return true;
}



// Games_Classes.cpp (in Pyramid_Board::is_win)
bool Pyramid_Board::is_win(Player<char>* player) {
  char sym = player->get_symbol();

  // The current player must have placed the symbols.

  // Check the column (0,2) (1,2) (2,2)
  if (board[0][2] == sym && board[1][2] == sym && board[2][2] == sym) {
    return true;
  }

  // Check the left diagonal (0,2) (1,1) (2,0)
  if (board[0][2] == sym && board[1][1] == sym && board[2][0] == sym) {
    return true;
  }

  // Check the right diagonal (0,2) (1,3) (2,4)
  if (board[0][2] == sym && board[1][3] == sym && board[2][4] == sym) {
    return true;
  }

  // Check the second row (1,1) (1,2) (1,3)
  if (board[1][1] == sym && board[1][2] == sym && board[1][3] == sym) {
    return true;
  }

  // Check the last row for three in a row:
  // (2,0), (2,1), (2,2) OR (2,1), (2,2), (2,3) OR (2,2), (2,3), (2,4)
  // The existing logic is complicated and wrong. Simplify to three valid horizontal lines:
  if ((board[2][0] == sym && board[2][1] == sym && board[2][2] == sym) ||
    (board[2][1] == sym && board[2][2] == sym && board[2][3] == sym) ||
    (board[2][2] == sym && board[2][3] == sym && board[2][4] == sym)) {
    return true;
  }

  return false;
}
bool Pyramid_Board::is_draw(Player<char>* player) {
  // draw if number of moves = 9 and no player win
  return (n_moves == 9 && !is_win(player));
}

bool Pyramid_Board::game_is_over(Player<char>* player) {
  // game is over if the moves = 9 or any player win 
  return is_win(player) || (n_moves == 9);
}

//--------------------------- pyramid ai player ------------------------------- 
bool is_valid_pyramid_cell(int r, int c) {
  return (r == 0 && c == 2) ||
    (r == 1 && c >= 1 && c <= 3) ||
    (r == 2 && c >= 0 && c <= 4);
}

bool is_board_full_pyramid(Pyramid_Board* board) {
  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 5; ++c) {
      if (is_valid_pyramid_cell(r, c) && board->is_empty_cell(r, c)) {
        return false;
      }
    }
  }
  return true;
}
int MAX_DEPTH; // 8 hard , 3 medium , 1 easy

int minimax_pyramid(Pyramid_Board* board, int depth, bool is_maximizing_player,
  char maximizing_player_symbol, char minimizing_player_symbol) {

  if (depth >= MAX_DEPTH) {
    return 0;   // neutral evaluation
  }


  // -------------------  (Base Case) -------------------

// make temporary players to check win conditions
  Player<char> max_player("TempAI", maximizing_player_symbol, PlayerType::COMPUTER);
  max_player.set_board_ptr(board);

  if (board->is_win(&max_player)) {
    return 10 - depth; // win for maximizing player with the lowest depth
  }

  Player<char> min_player("TempHuman", minimizing_player_symbol, PlayerType::HUMAN);
  min_player.set_board_ptr(board);

  if (board->is_win(&min_player)) {
    return depth - 10; // loss for maximizing player with the highest depth
  }

  if (is_board_full_pyramid(board)) {
    return 0; // draw
  }

  // -------------------- maximizing & minimizing --------------------

  char current_sym = is_maximizing_player ? maximizing_player_symbol : minimizing_player_symbol;

  // make best_eval = - or + infinity based on the player type
  int best_eval = is_maximizing_player ? numeric_limits<int>::min() : numeric_limits<int>::max();

  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 5; ++c) {
      if (is_valid_pyramid_cell(r, c) && board->is_empty_cell(r, c)) {
        // make the temp move
        board->set_cell_value(r, c, current_sym);

        // call minimax recursively for the next depth and opposite player
        int evaluation = minimax_pyramid(board, depth + 1, !is_maximizing_player, maximizing_player_symbol, minimizing_player_symbol);

        // modify best_eval based on the player type
        if (is_maximizing_player) {
          best_eval = max(best_eval, evaluation);
        }
        else {
          best_eval = min(best_eval, evaluation);
        }

        // backtrack the move
        board->set_cell_value(r, c, ' ');
      }
    }
  }
  return best_eval;
}

pair<int, int> find_best_move_pyramid(Pyramid_Board* board, char player_symbol) {
  int best_val = numeric_limits<int>::min();// - infinity
  pair<int, int> best_move = { -1, -1 };

  char maximizing_player_symbol = player_symbol;// AI player
  char minimizing_player_symbol = (player_symbol == 'X') ? 'O' : 'X';// Human player

  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 5; ++c) {
      // check valid cell
      if (is_valid_pyramid_cell(r, c) && board->is_empty_cell(r, c)) {

        // make the temp move
        board->set_cell_value(r, c, maximizing_player_symbol);

        // call minimax to evaluate this move
        int move_val = minimax_pyramid(board, 0, false, maximizing_player_symbol, minimizing_player_symbol);

        // backtrack the move
        board->set_cell_value(r, c, ' ');

        // update best move if needed
        if (move_val > best_val) {
          best_val = move_val;
          best_move = { r, c };
        }
      }
    }
  }
  return best_move;
}

//--------------------------- pyramid board_UI -------------------------------

Pyramid_UI::Pyramid_UI() : UI<char>("Welcome to Pyramid Tic-Tac-Toe", 3) {}

Player<char>* Pyramid_UI::create_player(string& name, char symbol, PlayerType type) {
  // Create player based on type
  cout << "Creating " << (type == PlayerType::HUMAN ? "human" : "computer")
    << " player: " << name << " (" << symbol << ")\n";
  cout << "\n";

  return new Player<char>(name, symbol, type);
}

Player<char>** Pyramid_UI::setup_players() {
  auto git_com_level = []() {
    cout << "Select Computer Player Difficulty Level:\n";
    cout << "1. Easy\n";
    cout << "2. Medium\n";
    cout << "3. Hard\n";
    int level;
    while (true) {
      cout << "Enter your choice (1-3): ";
      cin >> level;
      if (level >= 1 && level <= 3) {
        return level;
      }
      else {
        cout << "Invalid choice. Please select a valid difficulty level.\n";
      }
    }
    };
  Player<char>** players = new Player<char>*[2];
  vector<string> type_options = { "Human", "Computer" };
  // player 1
  string nameX = get_player_name("Player 1 (X)");
  PlayerType typeX = get_player_type_choice("Player 1 (X)", type_options);
  players[0] = create_player(nameX, 'X', typeX);
  if (typeX == PlayerType::COMPUTER) {
    int level = git_com_level();
    if (level == 1) {
      cout << "Computer Player 1 (X) set to Easy level.\n";
      cout << "\n";
      MAX_DEPTH = 1;
    }
    else if (level == 2) {
      cout << "Computer Player 1 (X) set to Medium level.\n";
      cout << "\n";
      MAX_DEPTH = 4;
    }
    else {
      cout << "Computer Player 1 (X) set to Hard level.\n";
      cout << "\n";
      MAX_DEPTH = 12;
    }
  }
  // player 2
  string nameO = get_player_name("Player 2 (O)");
  PlayerType typeO = get_player_type_choice("Player 2 (O)", type_options);
  players[1] = create_player(nameO, 'O', typeO);
  if (typeO == PlayerType::COMPUTER) {
    int level = git_com_level();
    if (level == 1) {
      cout << "Computer Player 2 (O) set to Easy level.\n";
      cout << "\n";
      MAX_DEPTH = 1;
    }
    else if (level == 2) {
      cout << "Computer Player 2 (O) set to Medium level.\n";
      cout << "\n";
      MAX_DEPTH = 5;
    }
    else {
      cout << "Computer Player 2 (O) set to Hard level.\n";
      cout << "\n";
      MAX_DEPTH = 12;
    }
  }
  return players;
}

Move<char>* Pyramid_UI::get_move(Player<char>* player) {
  int x, y;
  auto boardptr = dynamic_cast<Pyramid_Board*>(player->get_board_ptr());

  if (player->get_type() == PlayerType::HUMAN) {
    while (true) {
      if (player->get_symbol() == 'X')
        cout << "\033[91m";
      else
        cout << "\033[94m";
      cout << "\nPlease enter your move x {0 to 2} and y (0 to 4): ";
      cout << "\033[0m";
      cout << "\n";
      cin >> x >> y;

      if (cin.fail()) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "\033[93m";
        cout << "Invalid input. Please enter two numbers." << endl;
        cout << "\033[0m";
        continue;
      }
      else if (x > 2 || x < 0 || y > 4 || y < 0) {
        cout << "\033[93m";
        cout << "Invalid input. Please enter valid coordinates." << endl;
        cout << "\033[0m";
        continue;
      }
      else if (x == 0 && y != 2) {
        cout << "\033[93m";
        cout << "Invalid input. Please enter valid coordinates." << endl;
        cout << "\033[0m";
        continue;
      }
      else if (x == 1 && (y < 1 || y > 3)) {
        cout << "\033[93m";
        cout << "Invalid input. Please enter valid coordinates." << endl;
        cout << "\033[0m";
        continue;
      }
      else if (x == 2 && (y < 0 || y > 4)) {
        cout << "\033[93m";
        cout << "Invalid input. Please enter valid coordinates." << endl;
        cout << "\033[0m";
        continue;
      }
      else if (player->get_board_ptr()->get_board_matrix()[x][y] != ' ') {
        cout << "\033[93m";
        cout << "Invalid input. Please enter a coordinates for Impty cell." << endl;
        cout << "\033[0m";
        continue;
      }
      else
        break;
    }
  }
  else {
    pair<int, int> best_pos = find_best_move_pyramid(boardptr, player->get_symbol());

    x = best_pos.first;
    y = best_pos.second;
  }
  return new Move<char>(x, y, player->get_symbol());
}

void Pyramid_UI::display_board_matrix(const vector<vector<char>>& matrix) const {
  if (matrix.empty() || matrix[0].empty()) return;

  auto color = [](char c) {
    if (c == 'X')
      return "\033[31m"; // Red for X
    else if (c == 'O')
      return "\033[34m"; // Blue for O
    else
      return "\033[0m";  // Reset
    };

  cout << "\n";
  cout << "                 ----\n";
  cout << "              0 |" << color(matrix[0][2]) << setw(cell_width) << matrix[0][2] << color(' ') << " |\n";
  cout << "            --------------\n";
  cout << " 	 1 |";
  for (int i = 1; i < 4; i++) {
    cout << color(matrix[1][i]) << setw(cell_width) << matrix[1][i] << color(' ') << " |";
  }
  cout << "\n";
  cout << "       ------------------------\n";
  cout << "    2 |";
  for (int i = 0; i < 5; i++) {
    cout << color(matrix[2][i]) << setw(cell_width) << matrix[2][i] << color(' ') << " |";
  }
  cout << "\n";
  cout << "       ------------------------\n";
  cout << "         0    1    2    3    4 \n";
  cout << endl;
}

//=============================================== 9.Numerical_Board Implementation==============================================

Numerical_Board::Numerical_Board() : Board<int>(3, 3) {// make 3*3 board his cells = 0
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      board[i][j] = blank_symbol;
    }
  }
}

bool Numerical_Board::check_line_sum(int a, int b, int c) {// check three numbers are = 15 or not and make sure that these numbers not = 0
  if (a != 0 && b != 0 && c != 0) {
    if (a + b + c == 15) {
      return true;
    }
  }
  return false;
}

bool Numerical_Board::is_number_used(int num) {// check the number is exist in used number or not 
  if (std::find(used_numbers.begin(), used_numbers.end(), num) != used_numbers.end()) {
    return true;
  }
  return false;
}

bool Numerical_Board::update_board(Move<int>* move) {// ubdate board 
  int x = move->get_x();
  int y = move->get_y();
  int number = move->get_symbol();

  // cheak if the cell is empty 
  if (x < 0 || x >= rows || y < 0 || y >= columns || board[x][y] != blank_symbol) {
    return false;
  }

  // check if the number used 
  if (is_number_used(number)) {// 1 3 5 7 9 
    return false;
  }

  // make the move and update the used_numbers 
  n_moves++;
  board[x][y] = number;
  used_numbers.push_back(number);

  return true;
}

bool Numerical_Board::is_win(Player<int>* player) {
  // cheak the 3 rows 
  for (int i = 0; i < rows; ++i) {
    if (check_line_sum(board[i][0], board[i][1], board[i][2])) return true;
  }

  // check the 3 colomns  
  for (int j = 0; j < columns; ++j) {
    if (check_line_sum(board[0][j], board[1][j], board[2][j])) return true;
  }

  // check the 2 diagonals 
  if (check_line_sum(board[0][0], board[1][1], board[2][2])) return true;
  if (check_line_sum(board[0][2], board[1][1], board[2][0])) return true;

  return false;
}

bool Numerical_Board::is_draw(Player<int>* player) {
  // draw if number of moves = 9 and no player win
  return (n_moves == 9 && !is_win(player));
}

bool Numerical_Board::game_is_over(Player<int>* player) {
  // game is over if the moves = 9 or any player win 
  return is_win(player) || (n_moves == 9);
}

// --- Numerical_UI Implementation ---

Numerical_UI::Numerical_UI() : UI<int>("Welcome to Numerical Tic-Tac-Toe", 3) {} // hello message

// setup player function 
Player<int>** Numerical_UI::setup_players() {
  Player<int>** players = new Player<int>*[2];
  vector<string> type_options = { "Human", "Computer" };

  // player 1 has odd numbers 
  string nameX = get_player_name("Player 1 (Odd)");
  PlayerType typeX = get_player_type_choice("Player 1 (Odd)", type_options);
  players[0] = create_player(nameX, 1, typeX);

  // player 2 has even numbers 
  string nameO = get_player_name("Player 2 (Even)");
  PlayerType typeO = get_player_type_choice("Player 2 (Even)", type_options);
  players[1] = create_player(nameO, 2, typeO);

  return players;
}

Player<int>* Numerical_UI::create_player(string& name, int symbol, PlayerType type) {
  string numbers_list; // valied numbers for the player 
  if (symbol == 1) {
    numbers_list = "Odd Numbers (1, 3, 5, 7, 9)";
  }
  else {
    numbers_list = "Even Numbers (2, 4, 6, 8)";
  }

  cout << "Creating " << (type == PlayerType::HUMAN ? "human" : "computer")
    << " player: " << name << " (" << numbers_list << ")\n";// cout creating player (number) player: (name) (list) 

  return new Player<int>(name, symbol, type);
}

// get the number from the player befor make move
int Numerical_UI::get_human_number_choice(Player<int>* player) {
  // get pointr to the board
  Numerical_Board* boradptr = get_Numerical_board(player);
  // check the player is odd or even 
  bool is_odd_player = (player->get_symbol() == 1);
  int num;

  string available_str;

  if (is_odd_player) {
    available_str = "1, 3, 5, 7, 9";
  }
  else {
    available_str = "2, 4, 6, 8";
  }
  while (true) {
    cout << "\n" << player->get_name() << ", choose an unused number (" << available_str << "): ";
    cin >> num;
    if (boradptr->is_number_used(num) || num > 9 || num < 1)
      continue;
    else
      break;
  }

  // make sure the player chooses from the assigned number (even/odd)

  if (is_odd_player) {
    while (num != 1 && num != 3 && num != 5 && num != 7 && num != 9) {
      cout << "Invalid choice. Please choose an unused odd number (1, 3, 5, 7, 9): ";
      cin >> num;
    }
  }
  else {
    while (num != 2 && num != 4 && num != 6 && num != 8) {
      cout << "Invalid choice. Please choose an unused even number (2, 4, 6, 8): ";
      cin >> num;
    }
  }
  return num;
}

// get the computer number choice  
int Numerical_UI::get_computer_number_choice(Player<int>* player) {
  Numerical_Board* board_ptr = get_Numerical_board(player);
  bool is_odd_player = (player->get_symbol() == 1);

  vector<int> available_numbers;

  // choose for avaliable numbers 
  if (is_odd_player) {
    available_numbers = { 1, 3, 5, 7, 9 };
  }
  else {
    available_numbers = { 2, 4, 6, 8 };
  }

  vector<int> unused_options;
  // save used numbers 
  for (int num : available_numbers) {
    if (!board_ptr->is_number_used(num)) {
      unused_options.push_back(num);
    }
  }

  if (unused_options.empty()) return -1;

  // choose a random number
  int rand_index = rand() % unused_options.size();
  int chosen_num = unused_options[rand_index];

  display_message("Computer chooses number: " + to_string(chosen_num));

  return chosen_num;
}

Move<int>* Numerical_UI::get_move(Player<int>* player) {
  int x, y, number;

  if (player->get_type() == PlayerType::HUMAN) {
    number = get_human_number_choice(player);
  }
  else {
    number = get_computer_number_choice(player);
  }


  if (player->get_type() == PlayerType::HUMAN) {
    cout << "\nPlease enter your move x and y (0 to 2) for number " << number << ": ";
    while (true) {
      cin >> x >> y;
      if (x < 0 || x > 2 || y < 0 || y > 2) {
        cout << "Please enter a Valid move x and y (0 to 2): ";
        continue;
      }
      else if (get_Numerical_board(player)->get_cell(x, y) != 0) {
        cout << "This cell is already used, choose another cell: ";
      }
      else
        break;
    }
  }
  else {
    while (true) { // valid computer move
      x = rand() % 3;
      y = rand() % 3;
      if (get_Numerical_board(player)->get_cell(x, y) == 0) {
        break;
      }
    }
  }

  Move<int>* move = new Move<int>(x, y, number);
  return move;
}

//============================================ 10.Obstacles Tic-Tac-Toe Implementation ==================================================

Obstacles_Board::Obstacles_Board() : Board<char>(6, 6) {
  // 6*6 board 
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      board[i][j] = blank_symbol;
    }
  }
}



bool Obstacles_Board::update_board(Move<char>* move) {
  int x = move->get_x();
  int y = move->get_y();
  char symbol = move->get_symbol();

  if (x < 0 || x >= rows || y < 0 || y >= columns) return false;
  if (board[x][y] != blank_symbol) return false;

  n_moves++;
  board[x][y] = symbol;

  // Add # after every 2 moves 
  if (n_moves % 2 == 0) {
    add_obstacle();
    add_obstacle();

  }

  return true;
}

void Obstacles_Board::add_obstacle() {
  std::vector<std::pair<int, int>> empty_cells;

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (board[i][j] == blank_symbol) {
        empty_cells.push_back({ i, j });
      }
    }
  }

  if (!empty_cells.empty()) {
    int rand_index = rand() % empty_cells.size();
    int ox = empty_cells[rand_index].first;
    int oy = empty_cells[rand_index].second;

    board[ox][oy] = '#';

  }
}



bool Obstacles_Board::is_win(Player<char>* player) {
  char s = player->get_symbol();

  // Horizontal
  for (int i = 0; i < 6; ++i) {
    for (int j = 0; j <= 6 - 4; ++j) {
      if (board[i][j] == s && board[i][j + 1] == s &&
        board[i][j + 2] == s && board[i][j + 3] == s)
        return true;
    }
  }

  // Vertical
  for (int i = 0; i <= 6 - 4; ++i) {
    for (int j = 0; j < 6; ++j) {
      if (board[i][j] == s && board[i + 1][j] == s &&
        board[i + 2][j] == s && board[i + 3][j] == s)
        return true;
    }
  }

  // Diagonal down-right
  for (int i = 0; i <= 6 - 4; ++i) {
    for (int j = 0; j <= 6 - 4; ++j) {
      if (board[i][j] == s && board[i + 1][j + 1] == s &&
        board[i + 2][j + 2] == s && board[i + 3][j + 3] == s)
        return true;
    }
  }

  // Diagonal up-right
  for (int i = 3; i < 6; ++i) {
    for (int j = 0; j <= 6 - 4; ++j) {
      if (board[i][j] == s && board[i - 1][j + 1] == s &&
        board[i - 2][j + 2] == s && board[i - 3][j + 3] == s)
        return true;
    }
  }

  return false;
}


bool Obstacles_Board::is_draw(Player<char>* player) {
  return (n_moves == 18 && !is_win(player));
}

bool Obstacles_Board::game_is_over(Player<char>* player) {
  return is_win(player) || is_draw(player);
}


//============================================ Obstacles Tic-Tac-Toe_UI==================================================

Obstacles_UI::Obstacles_UI() : UI<char>("Welcome to Obstacles Tic-Tac-Toe", 3) {}

// selection player type and name
Player<char>* Obstacles_UI::create_player(string& name, char symbol, PlayerType type) {
  cout << "Creating " << (type == PlayerType::HUMAN ? "human" : "computer")
    << " player: " << name << " (" << symbol << ")\n";

  return new Player<char>(name, symbol, type);
}


Move<char>* Obstacles_UI::get_move(Player<char>* player) {
  int x, y;
  Board<char>* boardPtr = player->get_board_ptr();

  if (player->get_type() == PlayerType::HUMAN) {
    cout << "\nPlease enter your move x (0 to 5) and y (0 to 5): ";
    while (true) {
      cin >> x >> y;
      if (x < 0 || x > 5 || y < 0 || y > 5) {
        cout << "Please enter a Valid move x and y (0 to 5): ";
        continue;
      }
      if (boardPtr->get_cell(x, y) != ' ') {
        cout << "This cell is already used, choose another cell: ";
        continue;
      }
      break;
    }
  }
  else {
    // computer: random empty cell
    while (true) {
      x = rand() % 6;
      y = rand() % 6;
      if (boardPtr->get_cell(x, y) == ' ') break;
    }
    display_message("Computer chooses: (" + to_string(x) + ", " + to_string(y) + ")");
  }

  return new Move<char>(x, y, player->get_symbol());
}

//drawing 6*6 board
void Obstacles_UI::display_board_matrix(const vector<vector<char>>& matrix) const {
  if (matrix.empty() || matrix[0].empty()) return;

  cout << "\n    ";
  for (int j = 0; j < 6; ++j)
    cout << setw(cell_width + 1) << j;
  cout << "\n   " << string((cell_width + 2) * 6, '-') << "\n";

  for (int i = 0; i < 6; ++i) {
    cout << setw(2) << i << " |";
    for (int j = 0; j < 6; ++j)
      cout << setw(cell_width) << matrix[i][j] << " |";
    cout << "\n   " << string((cell_width + 2) * 6, '-') << "\n";
  }
  cout << endl;
}

//================================================= 11.Infinity Tic Tac Toe===========================================================

//------Infinity_Board imp------

infinity_Board::infinity_Board() : Board<char>(3, 3) {// make 3*3 board his cells = ' '
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      board[i][j] = blank_symbol;
    }
  }
}

bool infinity_Board::update_board(Move<char>* move) {
  int x = move->get_x();
  int y = move->get_y();
  char sym = move->get_symbol();

  if (x < 0 || x >= rows || y < 0 || y >= columns || board[x][y] != blank_symbol)
    return false;

  board[x][y] = sym;

  move_history.push({ x, y });

  n_moves += 1;

  if (n_moves % 3 == 0) {
    pair<int, int> oldest = move_history.front();
    move_history.pop();

    board[oldest.first][oldest.second] = blank_symbol;
  }

  return true;
}

bool infinity_Board::is_win(Player<char>* player) {
  const char sym = player->get_symbol();

  auto all_equal = [&](char a, char b, char c) {
    return a == b && b == c && a != blank_symbol;
    };

  // Check rows and columns
  for (int i = 0; i < rows; ++i) {
    if ((all_equal(board[i][0], board[i][1], board[i][2]) && board[i][0] == sym) ||
      (all_equal(board[0][i], board[1][i], board[2][i]) && board[0][i] == sym))
      return true;
  }

  // Check diagonals
  if ((all_equal(board[0][0], board[1][1], board[2][2]) && board[1][1] == sym) ||
    (all_equal(board[0][2], board[1][1], board[2][0]) && board[1][1] == sym))
    return true;

  return false;
}

bool infinity_Board::is_draw(Player<char>* player) {
  return false;
}

bool infinity_Board::game_is_over(Player<char>* player) {
  return is_win(player);
}

//-------------------infinity_UI imp-----------------------

infinity_UI::infinity_UI() : UI<char>("Welcome to Infinity Tic-Tac-Toe", 3) {}


Move<char>* infinity_UI::get_move(Player<char>* player) {
  int x, y;

  if (player->get_type() == PlayerType::HUMAN) {
    cout << "\nPlease enter your move x and y (0 to 2): ";
    while (true) {
      cin >> x >> y;

      if (cin.fail()) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Invalid input. Please enter two numbers (0 to 2)." << endl;
        continue;
      }

      if (x < 0 || x > 2 || y < 0 || y > 2) {
        cout << "Please enter a Valid move x and y (0 to 2): ";
        continue;
      }

      else if (player->get_board_ptr()->get_cell(x, y) != ' ') {
        cout << "Cell is already taken. Please enter an empty cell: ";
        continue;
      }
      else {
        break;
      }
    }
  }
  else {
    while (true) {
      x = rand() % 3;
      y = rand() % 3;
      if (player->get_board_ptr()->get_cell(x, y) == ' ')
        break;
    }
  }
  return new Move<char>(x, y, player->get_symbol());
}

//====================================== 12.Ultimate Tic Tac Toe ===========================================
// =========================================================
//                   MiniBoard IMPLEMENTATION
// =========================================================

MiniBoard::MiniBoard() {
  finished = false;
  winner = '-';

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      board[i][j] = '-';
}

char MiniBoard::get_cell(int x, int y) {
  return board[x][y];
}

bool MiniBoard::is_full() {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (board[i][j] == '-') return false;
  return true;
}

bool MiniBoard::is_finished() {
  return finished;
}

char MiniBoard::get_winner() {
  return winner;
}

bool MiniBoard::place_move(int x, int y, char symbol) {
  if (finished) return false;
  if (x < 0 || x >= 3 || y < 0 || y >= 3) return false;
  if (board[x][y] != '-') return false;

  board[x][y] = symbol;

  // check win
  for (int i = 0; i < 3; i++)
    if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) {
      winner = symbol; finished = true; return true;
    }

  for (int j = 0; j < 3; j++)
    if (board[0][j] == symbol && board[1][j] == symbol && board[2][j] == symbol) {
      winner = symbol; finished = true; return true;
    }

  if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) {
    winner = symbol; finished = true; return true;
  }

  if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) {
    winner = symbol; finished = true; return true;
  }

  if (is_full()) finished = true;

  return true;
}

void MiniBoard::display_formatted() {
  cout << "\n     0   1   2\n";
  cout << "   -------------\n";
  for (int i = 0; i < 3; i++) {
    cout << " " << i << " |";
    for (int j = 0; j < 3; j++)
      cout << " " << board[i][j] << " |";
    cout << "\n   -------------\n";
  }
}
// ==================================================== Ultimate Tic Tac Toe ===========================================
// =========================================================
//                     BigBoard IMPLEMENTATION
// =========================================================

BigBoard::BigBoard() {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) {
      mini[i][j] = new MiniBoard();
      result[i][j] = '-';
    }
}

MiniBoard* BigBoard::get_mini(int x, int y) {
  return mini[x][y];
}

bool BigBoard::is_mini_available(int x, int y) {
  return result[x][y] == '-';  // طالما مش X ولا O ولا D يبقى لسه متلعبش
}


void BigBoard::update_big_cell(int x, int y) {
  MiniBoard* m = mini[x][y];
  if (m->get_winner() == 'X' || m->get_winner() == 'O')
    result[x][y] = m->get_winner();
  else if (m->is_finished())
    result[x][y] = 'D'; // Draw
}

bool BigBoard::is_big_finished() {
  if (big_winner() != '-') return true;

  // check all filled
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (result[i][j] == '-') return false;

  return true;
}

char BigBoard::big_winner() {
  // rows
  for (int i = 0; i < 3; i++)
    if (result[i][0] != '-' &&
      result[i][0] == result[i][1] &&
      result[i][1] == result[i][2])
      return result[i][0];

  // columns
  for (int j = 0; j < 3; j++)
    if (result[0][j] != '-' &&
      result[0][j] == result[1][j] &&
      result[1][j] == result[2][j])
      return result[0][j];

  // diagonals
  if (result[0][0] != '-' &&
    result[0][0] == result[1][1] &&
    result[1][1] == result[2][2])
    return result[0][0];

  if (result[0][2] != '-' &&
    result[0][2] == result[1][1] &&
    result[1][1] == result[2][0])
    return result[0][2];

  return '-';
}

void BigBoard::display_formatted_big() {
  cout << "\nBIG BOARD:\n";
  cout << "     0   1   2\n";
  cout << "   -------------\n";
  for (int i = 0; i < 3; i++) {
    cout << " " << i << " |";
    for (int j = 0; j < 3; j++)
      cout << " " << result[i][j] << " |";
    cout << "\n   -------------\n";
  }
}


// =========================================================
//                     UI IMPLEMENTATION
// =========================================================

UltimateUI::UltimateUI(BigBoard* b)
  : UI<char>("Ultimate Tic Tac Toe!", 3), board(b) {}

Player<char>** UltimateUI::setup_players() {
  Player<char>** p = new Player<char>*[2];
  vector<string> types = { "Human", "Computer" };

  string n1 = get_player_name("Player 1");
  PlayerType t1 = get_player_type_choice("Player 1", types);
  p[0] = new Player<char>(n1, 'X', t1);

  string n2 = get_player_name("Player 2");
  PlayerType t2 = get_player_type_choice("Player 2", types);
  p[1] = new Player<char>(n2, 'O', t2);

  return p;
}

// اختيار صندوق كبير صالح
void UltimateUI::ask_for_big_choice(int& bx, int& by, Player<char>* player) {
  while (true) {
    cout << player->get_name() << " choose BIG board (x y): ";
    cin >> bx >> by;

    if (bx < 0 || bx > 2 || by < 0 || by > 2) {
      cout << "Invalid big cell! Try again.\n";
      continue;
    }

    if (!board->is_mini_available(bx, by)) {
      cout << "This mini-board is finished. Choose another.\n";
      continue;
    }

    break;
  }
}

Move<char>* UltimateUI::get_move(Player<char>* player) {
  static int current_bx = -1, current_by = -1;   // فاكرين آخر صندوق
  MiniBoard* m = nullptr;

  // لو أول مرة أو الصندوق خلص
  if (current_bx == -1 || board->is_mini_available(current_bx, current_by) == false)
  {
    while (true) {
      cout << player->get_name() << " choose BIG board (0-2 0-2): ";
      cin >> current_bx >> current_by;

      if (current_bx < 0 || current_bx > 2 || current_by < 0 || current_by > 2) {
        cout << "Invalid big board coordinates. Try again.\n";
        continue;
      }

      if (!board->is_mini_available(current_bx, current_by)) {
        cout << "This big board is already finished. Pick another one.\n";
        continue;
      }
      break;
    }
  }

  m = board->get_mini(current_bx, current_by);

  cout << "\nYou are playing in BIG board (" << current_bx << "," << current_by << ")\n";
  m->display_formatted();

  int x, y;

  while (true) {
    cout << "Enter small board cell (0-2 0-2): ";
    cin >> x >> y;

    if (x < 0 || x > 2 || y < 0 || y > 2) {
      cout << "Invalid small cell. Try again.\n";
      continue;
    }

    if (!m->place_move(x, y, player->get_symbol())) {
      cout << "Cell is already taken or move not allowed.\n";
      continue;
    }

    break;
  }

  return new Move<char>(current_bx * 10 + x, current_by * 10 + y, player->get_symbol());
}



// =========================================================
//                     GAME IMPLEMENTATION
// =========================================================

UltimateGame::UltimateGame() {
  board = new BigBoard();
  ui = new UltimateUI(board);

  Player<char>** p = ui->setup_players();
  players[0] = p[0];
  players[1] = p[1];
  delete[] p;
}

void UltimateGame::run() {
  int turn = 0;

  while (!board->is_big_finished()) {
    Player<char>* p = players[turn % 2];

    cout << "\n========= " << p->get_name() << " (" << p->get_symbol() << ") =========\n";
    board->display_formatted_big();

    Move<char>* move = ui->get_move(p);

    int bx = move->get_x() / 10;
    int x = move->get_x() % 10;
    int by = move->get_y() / 10;
    int y = move->get_y() % 10;

    board->update_big_cell(bx, by);

    delete move;
    turn++;
  }

  board->display_formatted_big();

  char win = board->big_winner();
  if (win == '-')
    cout << "Game ended in a DRAW!\n";
  else
    cout << win << " wins the BIG BOARD!\n";
}

//================================================ 13.Memory_Board Implementation ===========================================================
Memory_Board::Memory_Board() : Board<char>(3, 3) {
  for (int i = 0;i < rows;i++)
    for (int j = 0;j < columns;j++)
      board[i][j] = blank_symbol;
}

bool Memory_Board::update_board(Move<char>* move) {
  int x = move->get_x();
  int y = move->get_y();

  if (x < 0 || x>2 || y < 0 || y>2) return false;
  if (board[x][y] != blank_symbol) return false;

  board[x][y] = '#';
  n_moves++;

  if (n_moves % 2 != 0)
    p1_moves.push_back({ x,y });   // odd -> player X
  else
    p2_moves.push_back({ x,y });   // even -> player O

  return true;
}


bool Memory_Board::check_line(const vector<pair<int, int>>& moves) {
  // if player has less than 3 moves, no need to check
  if (moves.size() < 3) return false;

  int rowCount[3] = { 0,0,0 };
  int colCount[3] = { 0,0,0 };
  int diag1 = 0; // (0,0),(1,1),(2,2)
  int diag2 = 0; // (0,2),(1,1),(2,0)

  for (auto& m : moves) {
    int r = m.first;
    int c = m.second;

    rowCount[r]++;     // count move in row r
    colCount[c]++;     // count move in column c

    if (r == c) diag1++;          // main diagonal
    if (r + c == 2) diag2++;      // anti diagonal
  }

  // If any row/column/diagonal reaches 3 -> win
  for (int i = 0;i < 3;i++)
    if (rowCount[i] == 3 || colCount[i] == 3 || diag1 == 3 || diag2 == 3) {
      return true;
    }
  return false;
}

bool Memory_Board::is_win(Player<char>* player) {
  if (player->get_symbol() == 'X') {
    return check_line(p1_moves);
  }
  else {
    return check_line(p2_moves);
  }
}

bool Memory_Board::is_draw(Player<char>* player) {
  return(n_moves == 9 && !is_win(player));
}

bool Memory_Board::game_is_over(Player<char>* player) {
  return is_win(player) || n_moves == 9;
}


//================================================ Memory_Board UI =====================================================================
Memory_UI::Memory_UI() : UI<char>("Welcome to 5x5 Tic-Tac-Toe", 3) {}

// selecting player type and name
Player<char>* Memory_UI::create_player(string& name, char symbol, PlayerType type) {
  cout << "Creating " << (type == PlayerType::HUMAN ? "human" : "computer")
    << " player: " << name << " (" << symbol << ")\n";

  return new Player<char>(name, symbol, type);
}


Move<char>* Memory_UI::get_move(Player<char>* player) {
  int x, y;
  Board<char>* boardPtr = player->get_board_ptr();

  if (player->get_type() == PlayerType::HUMAN) {
    cout << "\nPlease enter your move x (0 to 2) and y (0 to 2): ";
    while (true) {
      cin >> x >> y;
      if (x < 0 || x > 2 || y < 0 || y > 2) {
        cout << "Please enter a Valid move x and y (0 to 2): ";
        continue;
      }
      if (boardPtr->get_cell(x, y) != ' ') {
        cout << "This cell is already used, choose another cell: ";
        continue;
      }
      break;
    }
  }
  else {
    // computer: random empty cell
    while (true) {
      x = rand() % 3;
      y = rand() % 3;
      if (boardPtr->get_cell(x, y) == ' ') break;
    }
    display_message("Computer chooses: (" + to_string(x) + ", " + to_string(y) + ")");
  }

  return new Move<char>(x, y, player->get_symbol());
}


void Memory_UI::display_board_matrix(const vector< vector<char>>& matrix) const {
  cout << "\n    0   1   2\n";
  cout << "   -----------\n";
  for (int i = 0;i < 3;i++) {
    cout << i << " | ";
    for (int j = 0;j < 3;j++) cout << matrix[i][j] << " | ";
    cout << "\n   -----------\n";
  }
};

