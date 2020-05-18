//-----------------------------------------------------------------------------
// Copyright (c) 2014 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef PUZZLE_AND_DRAGONS_GRAPHIC_H_
#define PUZZLE_AND_DRAGONS_GRAPHIC_H_

#include <SDL.h>
#include <SDL_image.h>  // Display images.
#include <SDL_ttf.h>    // Display texts.
#include "board.h"

class Graphic {
public:
  struct Point {
    int x, y;
  };

  void Initialize();
  void Terminate();
  void DisplayBoard(const Board &board, int current_position = 0);
  void DisplayResult(const Board::Score &score, int max_combos);
  void MoveOrbs(bool *is_moving, Board *board) const;
  void Sleep(const int duration) const;
  Point WaitClick() const;

private:
  static const int kImageSizeOrb;
  static const int kWidthWindow;
  static const int kHeightWindow;

  // Unit of a image must be 4 per row.
  void DrawGraph(SDL_Surface *image, int dest_x, int dest_y,
                 int image_id = 0, int image_width = 0, int image_height = 0);
  void DrawString(const char *text, int dest_x, int dest_y,
                  const SDL_Color &color);
  void CheckClose() const;
  void ClearScreen();
  void Display();

  SDL_Surface *video_surface;
  SDL_Surface *image_orb;
  SDL_Surface *image_orb_small;
  SDL_Surface *image_overlayed_orb_small;
  SDL_Surface *image_result;
  TTF_Font *font;
};

#endif  // PUZZLE_AND_DRAGONS_GRAPHIC_H_