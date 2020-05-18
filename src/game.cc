//-----------------------------------------------------------------------------
// Copyright (c) 2014 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "game.h"
#include "ai.h"

void Game::Initialize() {
  board_.Initialize();
  graphic_.Initialize();
}

void Game::Terminate() {
  graphic_.Terminate();
}

void Game::Play() {
  bool is_moving = false;
  while (true) {
    // Move orbs following the mouse cursor.
    graphic_.DisplayBoard(board_);
    graphic_.MoveOrbs(&is_moving, &board_);

    // If the cursor move has stopped, display result.
    if (is_moving)
      continue;
    int max_combos = board_.CalculateMaxCombos();
    Board::Score score = VanishOrbs();
    graphic_.DisplayResult(score, max_combos);
    graphic_.WaitClick();
  }
}

void Game::SolveAuto() {
  Ai ai;
  while (true) {
    // Calculate a route for solving a puzzle.
    graphic_.DisplayBoard(board_);
    Ai::Route route = ai.GetBestRoute(board_);

    // Move orbs along the route.
    int current_position = route.begin_id;
    for (int i = 0; i < route.size(); ++i) {
      // Check whether this is the end of the route.
      int direction = route.directions[i];
      if (0 == direction)
        break;

      // Move orbs.
      board_.MoveOrb(direction, current_position);
      current_position += direction;
      graphic_.DisplayBoard(board_, current_position);
      graphic_.Sleep(20);
    }
    graphic_.Sleep(200);

    // Vanish connecting orbs and display result.
    int max_combos = board_.CalculateMaxCombos();
    Board::Score score = VanishOrbs();
    graphic_.DisplayResult(score, max_combos);
    graphic_.Sleep(1500);
  }
}

Board::Score Game::VanishOrbs() {
  Board::Score score = {0};

  // Continue to vanish and drop orbs untill nothing is changed.
  Board prev_board;
  do {
    // Vanish orbs.
    prev_board = board_;
    score.Add(board_.VanishOrbs());
    graphic_.DisplayBoard(board_);
    graphic_.Sleep(500);

    // Drop orbs.
    board_.DropOrbs();
    graphic_.DisplayBoard(board_);
    graphic_.Sleep(300);
  } while (!board_.Equals(prev_board));

  return score;
}
