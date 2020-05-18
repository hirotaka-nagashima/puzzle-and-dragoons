//-----------------------------------------------------------------------------
// Copyright (c) 2014 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef PUZZLE_AND_DRAGONS_AI_H_
#define PUZZLE_AND_DRAGONS_AI_H_

#include <map>
#include <vector>

class Board;

class Ai {
public:
  struct Route {
    int size() const { return directions.size(); }

    int begin_id;
    std::map<int, int> directions;
  };

  Route GetBestRoute(const Board &original_board) const;

private:
  // Depth to simulate moving per part.
  // This is main factor of accuracy and thinking time.
  static const int kPartSearchingDepth;
  // The number of positions of orbs to start moving.
  static const int kMaxStartingPositions;

  int SearchForRoute(int phase, int num_times, int current_id,
                     int prev_direction, int best_evaluation,
                     Board *original_board, Route *route) const;
  std::vector<int> DetermineStarts(const Board &board) const;
};

#endif  // PUZZLE_AND_DRAGONS_AI_H_