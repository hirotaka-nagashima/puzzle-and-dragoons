//-----------------------------------------------------------------------------
// Copyright (c) 2014 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "board.h"
#include <ctime>    // time()
#include <cstdlib>  // srand(), rand()
#include <queue>

const int Board::kConnectionMinNum = 3;
const int Board::kMaxCombos = kSize / kConnectionMinNum;
const int Board::k4Directions[4] = {-kArrayWidth, -1, +1, +kArrayWidth};

void Board::Score::Add(const Score &score) {
  sum_orbs += score.sum_orbs;
  sum_combos += score.sum_combos;
  for (int i = 0; i < kNumAttributes; ++i) {
    num_orbs[i] += score.num_orbs[i];
    num_combos[i] += score.num_combos[i];
  }
}

void Board::Initialize() {
  // Set random seed.
  // You should use a constant for debugging.
  srand(static_cast<unsigned int>(time(NULL)));

  // Initialize board randomly.
  for (int i = 0; i < kArraySize; ++i) {
    int y = i / kArrayWidth;
    int x = i % kArrayWidth;
    if ((y == 0 || y == kArrayHeight - 1) ||
        (x == 0 || x == kArrayWidth - 1)) {
      // Place a sentinel.
      set_board(i, kOutside);
    } else {
      // Place a orb.
      set_board(i, rand() % kNumAttributes);
    }
  }

  // Vanish connected orbs first.
  Board prev_board;
  do {
    prev_board = *this;
    VanishOrbs();
    DropOrbs();
  } while (!Equals(prev_board));
}

Board::Score Board::VanishOrbs() {
  Board next_board = *this;

  // Create a map for saving vanished orbs.
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      int id = GetId(y, x);
      int attribute = board(id);

      // Check whether there are orbs having
      // current orb's attribute on right and bottom side.
      for (int i = 2; i < 4; ++i) {
        // Count the number of connected orbs of
        // same attribute each other.
        int j = 1;
        for (j = 1; board(id + k4Directions[i] * j) == attribute; ++j) {}

        // Vanish connected orbs.
        if (j < kConnectionMinNum)
          continue;
        for (--j; 0 <= j; --j) {
          int dest = id + k4Directions[i] * j;
          next_board.set_board(dest, kTemp);
        }
      }
    }
  }

  Score information = {0};
  std::queue<int> connected_orbs;

  // Get information about vanished orbs.
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      int id = GetId(y, x);
      int attribute = board(id);
      if (kTemp != next_board.board(id))
        continue;

      // If the current orb is going to be vanished,
      // trace connected orbs of the same attribute
      // to get information about them.
      ++information.sum_combos;
      ++information.num_combos[attribute];

      // Trace connected orbs of the same attribute.
      connected_orbs.push(id);
      next_board.set_board(id, kNone);
      do {
        int temp_id = connected_orbs.front();
        connected_orbs.pop();
        ++information.sum_orbs;
        ++information.num_orbs[attribute];

        // Search for 4 directions recursively.
        for (int i = 0; i < 4; ++i) {
          int dest = temp_id + k4Directions[i];
          if (kTemp == next_board.board(dest) &&
              attribute == board(dest)) {
            connected_orbs.push(dest);
            next_board.set_board(dest, kNone);
          }
        }
      } while (!connected_orbs.empty());
    }
  }

  // Vanish orbs.
  for (int i = 0; i < kArraySize; ++i)
    set_board(i, next_board.board(i));

  return information;
}

void Board::DropOrbs() {
  // Drop orbs first if there are empties.
  for (int y = kHeight - 1; 0 <= y; --y) {
    for (int x = 0; x < kWidth; ++x) {
      int current = GetId(y, x);
      if (kNone != board(current))
        continue;

      // If a orb is not existed at current position,
      // search for a orb from current position to top.
      int j;
      for (j = current - kArrayWidth;
           board(j) == kNone;
           j -= kArrayWidth) {}

      // Drop a orb above the current position.
      if (board(j) != kOutside)
        Swap(current, j);
    }
  }

  // Add new orbs into the empties.
  AddNewOrbs();
}

void Board::MoveOrb(int direction, int src) {
  int dest = src + direction;
  Swap(src, dest);
}

void Board::Swap(int id_1, int id_2) {
  int board_id_1 = board(id_1);
  set_board(id_1, board_[id_2]);
  set_board(id_2, board_id_1);
}

bool Board::Equals(const Board &target) const {
  for (int i = 0; i < kArraySize; ++i) {
    if (target.board(i) != board(i))
      return false;
  }
  return true;
}

int Board::CalculateMaxCombos() const {
  // Count the number of orbs each attribute.
  int num_orbs[Board::kNumAttributes] = {0};
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x)
      ++num_orbs[board(y, x)];
  }

  // Calculate the maximum number of combos in current board.
  int max_combos = 0;
  for (int i = 0; i < Board::kNumAttributes; ++i)
    max_combos += num_orbs[i] / 3;

  return max_combos;
}

int Board::Evaluate() const {
  // Get a score.
  Board copy_board = *this;
  Score score = copy_board.VanishOrbs();

  // Get each parameters.
  int num_orbs_on_edge = copy_board.CountNumOrbsOnEdge();
  int perimeter = copy_board.CalculatePerimeter();
  int farthest_distance = copy_board.MeasureFarthestOrbsDistance();

  // Weight each parameters.
  int evaluation =
      score.sum_combos * 10000 -
      num_orbs_on_edge * 300 -
      farthest_distance * 300 -
      perimeter;

  return evaluation;
}

int Board::GetId(int y, int x) const {
  return (x + 1) + (y + 1) * kArrayWidth;
}

int Board::CalculatePerimeter() const {
  int perimeter = 0;
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      int id = GetId(y, x);
      if (kNone == board(id)) {
        // Search for 4 directions.
        for (int i = 0; i < 4; ++i) {
          int dest = id + k4Directions[i];
          int dest_orb = board(dest);
          if (kNone != dest_orb)
            perimeter++;
        }
      }
    }
  }
  return perimeter;
}

int Board::MeasureFarthestOrbsDistance() const {
  // Find the first orb and last one.
  int first_orb = 0;
  int last_orb = 0;
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      int id = GetId(y, x);
      if (!IsOrb(id))
        continue;
      if (first_orb == 0)
        first_orb = id;
      last_orb = id;
    }
  }

  // Calculate the Manhattan distance between them.
  int difference = last_orb - first_orb;
  int difference_y =
    difference / Board::kArrayWidth - 1;
  int difference_x =
    difference % Board::kArrayWidth - 1;
  int farthest_distance = difference_y + difference_x;
  return farthest_distance;
}

int Board::CountNumOrbsOnEdge() const {
  int num_orbs_on_edge = 0;
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      int id = GetId(y, x);
      if (IsOrb(id) && IsEdge(y, x))
        num_orbs_on_edge++;
    }
  }
  return num_orbs_on_edge;
}

void Board::AddNewOrbs() {
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      int id = GetId(y, x);
      if (kNone == board(id))
        set_board(id, rand() % kNumAttributes);
    }
  }
}

bool Board::IsOrb(int id) const {
  return 0 <= board(id);
}

bool Board::IsEdge(int y, int x) const {
  return y == 0 || x == 0 || y == kHeight - 1 || x == kWidth - 1;
}