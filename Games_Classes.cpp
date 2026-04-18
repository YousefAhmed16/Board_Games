#include <iostream>
#include <cstdlib> // for rand() 
#include <algorithm> // for find()
#include <utility> // for std::pair
#include <queue> 
#include "Games_Classes.h"
#include <chrono>

using namespace std;


//================================================= 1.4x4 Board Game ===========================================================

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

//=============================================== 2.Pyramid_Board Implementation======================================================


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

//=============================================== 3.Numerical_Board Implementation==============================================

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

//============================================ 4.Obstacles Tic-Tac-Toe Implementation ==================================================

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

//================================================ 5.Memory_Board Implementation ===========================================================
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

