//-----------------------------------------------------------------------------
// Copyright (c) 2014 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "ai.h"
#include <algorithm>  // std::min()
#include "board.h"

const int Ai::kPartSearchingDepth = 10;
const int Ai::kMaxStartingPositions = 6;

Ai::Route Ai::GetBestRoute(const Board &board_original) const {
  // Determine orbs to be started to move.
  std::vector<int> starts = DetermineStarts(board_original);

  // Search for the best route.
  Route best_route;
  int best_score = INT_MIN;
  for (int i = 0; i < static_cast<int>(starts.size()); ++i) {
    // Each start to be moved.
    Board board = board_original;
    Route route = {0};
    route.begin_id = starts[i];
    int score = INT_MIN;
    int current_position = route.begin_id;

    // Search for the route until the score isn't changed.
    for (int phase = 1, num_moves = 0;; ++phase) {  // Each phase.
      // Search for the route.
      int prev_score = score;
      score = SearchForRoute(
          phase, num_moves, current_position,
          0, score,
          &board, &route);
      if (score - prev_score == 0)
        break;

      // Move orbs along the route.
      for (; num_moves < kPartSearchingDepth * phase; ++num_moves) {
        board.MoveOrb(route.directions[num_moves], current_position);
        current_position += route.directions[num_moves];
      }
    }

    // Update the best score.
    if (best_score < score) {
      best_score = score;
      best_route = route;
    }
  }

  return best_route;
}

int Ai::SearchForRoute(int phase, int num_times, int current_id,
                       int prev_direction, int best_evaluation,
                       Board *board, Route *route) const {
  if (kPartSearchingDepth * phase <= num_times)
    return board->Evaluate();

  // Find the best direction each scenes.
  for (int i = 0; i < 4; ++i) {
    // If the current direction is valid.
    int dest = current_id + Board::k4Directions[i];
    if (Board::kOutside == board->board(dest) ||
        Board::k4Directions[i] == prev_direction) {
      continue;
    }

    // Move an orb in the direction.
    board->MoveOrb(Board::k4Directions[i], current_id);

    // Search for a route.
    int evaluation = SearchForRoute(
        phase, num_times + 1, dest,
        -Board::k4Directions[i], best_evaluation,
        board, route);

    // Restore to previous board.
    board->MoveOrb(-Board::k4Directions[i], dest);

    // Compare the past highest score and the current one.
    if (best_evaluation < evaluation) {
      best_evaluation = evaluation;
      route->directions[num_times] = Board::k4Directions[i];
    }
  }

  return best_evaluation;
}

std::vector<int> Ai::DetermineStarts(const Board &board) const {
  // Calculate the number of extra orbs each attribute
  // to evaluate positions to be started to move after.
  int num_orbs[Board::kNumAttributes] = {0};
  for (int y = 0; y < Board::kHeight; ++y) {
    for (int x = 0; x < Board::kWidth; ++x)
      ++num_orbs[board.board(y, x)];
  }
  int num_extra_orbs[Board::kNumAttributes];
  for (int i = 0; i < Board::kNumAttributes; ++i)
    num_extra_orbs[i] = num_orbs[i] % 3;

  // Determine the best position to start move.
  std::vector<int> starts;
  std::vector<int> evaluations;
  for (int y = 0; y < Board::kHeight; ++y) {
    for (int x = 0; x < Board::kWidth; ++x) {
      int id = board.GetId(y, x);
      int attribute = board.board(id);

      // Evaluate the current position.
      int temp_num_extra_orbs = num_extra_orbs[attribute];
      int distance_to_edges = std::min(y, (Board::kHeight - 1) - y) +
                              std::min(x, (Board::kWidth - 1) - x);
      int evaluation = 100 * temp_num_extra_orbs - distance_to_edges;

      // Update deleting the id whose evaluation is minimum.
      starts.push_back(id);
      evaluations.push_back(evaluation);
      if (starts.size() <= kMaxStartingPositions)
        continue;
      int deleted_id;
      for (int i = 0, min_evaluation = INT_MAX;
           i < kMaxStartingPositions; ++i) {
        if (evaluations[i] < min_evaluation) {
          min_evaluation = evaluations[i];
          deleted_id = i;
        }
      }
      starts.erase(starts.begin() + deleted_id);
      evaluations.erase(evaluations.begin() + deleted_id);
    }
  }

  return starts;
}