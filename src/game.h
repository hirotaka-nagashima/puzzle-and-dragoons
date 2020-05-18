//-----------------------------------------------------------------------------
// Copyright (c) 2014 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef PUZZLE_AND_DRAGONS_GAME_H_
#define PUZZLE_AND_DRAGONS_GAME_H_

#include "board.h"
#include "graphic.h"

class Game {
public:
  void Initialize();
  void Terminate();
  // A player can play the puzzle.
  void Play();
  // The ai continues to solve puzzle automatically.
  void SolveAuto();

private:
  Board::Score VanishOrbs();

  Board board_;
  Graphic graphic_;
};

#endif  // PUZZLE_AND_DRAGONS_GAME_H_