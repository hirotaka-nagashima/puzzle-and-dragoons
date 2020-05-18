//-----------------------------------------------------------------------------
// Copyright (c) 2014 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "graphic.h"

const int Graphic::kImageSizeOrb = 90;
const int Graphic::kWidthWindow = Board::kWidth * kImageSizeOrb;
const int Graphic::kHeightWindow = Board::kHeight * kImageSizeOrb;

void Graphic::Initialize() {
  // Initialize the SDL.
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "ERROR: %s\n", SDL_GetError());
    exit(-1);
  }

  // Initialize a font service.
  if (TTF_Init() < 0) {
    fprintf(stderr, "ERROR: %s\n", TTF_GetError());
    exit(-1);
  }

  SDL_Surface *returned_value = SDL_SetVideoMode(
      kWidthWindow, kHeightWindow, 16, SDL_HWSURFACE);
  if (NULL == returned_value) {
    fprintf(stderr, "ERROR: %s\n", SDL_GetError());
    exit(-1);
  }

  SDL_WM_SetCaption("Puzzle & Dragons", "Puzzle & Dragons");
  video_surface = SDL_GetVideoSurface();

  // Load images.
  image_orb = IMG_Load("resources/orb.png");
  image_orb_small = IMG_Load("resources/orb_small.png");
  image_overlayed_orb_small = IMG_Load("resources/overlayed_orb_small.png");
  image_result = IMG_Load("resources/result.png");
  if (!image_orb || !image_result || !image_orb_small ||
      !image_overlayed_orb_small) {
    fprintf(stderr, "ERROR: %s\n", IMG_GetError());
    exit(-1);
  }

  // Load a font.
  font = TTF_OpenFont("resources/font.ttf", 40);
  if (!font) {
    fprintf(stderr, "ERROR: %s\n", TTF_GetError());
    exit(-1);
  }
}

void Graphic::Terminate() {
  TTF_CloseFont(font);

  SDL_FreeSurface(image_orb);
  SDL_FreeSurface(image_orb_small);
  SDL_FreeSurface(image_overlayed_orb_small);
  SDL_FreeSurface(image_result);
  SDL_FreeSurface(video_surface);

  TTF_Quit();

  SDL_Quit();
}

void Graphic::DisplayBoard(const Board &board, int current_position) {
  ClearScreen();

  // Draw orbs.
  for (int y = 0; y < Board::kHeight; ++y) {
    for (int x = 0; x < Board::kWidth; ++x) {
      int id = board.GetId(y, x);
      if (Board::kNone == board.board(id))
        continue;

      // Float a touched orb.
      double floating_ratio = 0.0;
      if (id == current_position)
        floating_ratio = 0.1;

      // Draw a orb.
      int dest_x = static_cast<int>(kImageSizeOrb * (x - floating_ratio));
      int dest_y = static_cast<int>(kImageSizeOrb * (y - floating_ratio));
      DrawGraph(image_orb, dest_x, dest_y, board.board(id),
                kImageSizeOrb, kImageSizeOrb);
    }
  }

  Display();
}

void Graphic::DisplayResult(const Board::Score &score, int max_combos) {
  // Convert given score to string.
  char score_string[30];
  sprintf_s(score_string, "%d/%d  COMBOS",
            score.sum_combos, max_combos);

  // Overlay a background.
  DrawGraph(image_result, 0, 0);
  // Draw the score.
  DrawString(score_string, 140, 200, {0x55, 0x55, 0x55});

  // Draw vanished orb's atributes.
  int base_x = kWidthWindow / 2 - Board::kNumAttributes * 30 / 2;
  for (int i = 0; i < Board::kNumAttributes; ++i) {
    int dest_x = base_x + i * 30;
    int dest_y = 280;
    SDL_Surface *orb_image = (1 <= score.num_combos[i]) ?
        image_orb_small : image_overlayed_orb_small;
    DrawGraph(orb_image, dest_x, dest_y, i, 25, 25);
  }

  Display();
}

void Graphic::MoveOrbs(bool *is_moving, Board *board) const {
  static enum IdStatus { kNone = -1 };
  static int past_id = kNone;
  SDL_Event event;

  while (true) {
    // Wait for a mouse event, click.
    SDL_WaitEvent(&event);
    if (event.type == SDL_QUIT) {
      exit(0);
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
      *is_moving = true;
      break;
    } else if (event.type == SDL_MOUSEBUTTONUP) {
      *is_moving = false;
      past_id = kNone;
      break;
    }

    // Move orbs while the mouse cursor is dragging.
    if (*is_moving) {
      // Get a position of the mouse.
      int x, y;
      SDL_GetMouseState(&x, &y);
      int mouse_position = (x / kImageSizeOrb + 1) +
          (y / kImageSizeOrb + 1) * Board::kArrayWidth;

      // Swap orbs as needed.
      if (past_id == kNone) {  // Need to initialize.
        past_id = mouse_position;
      } else if (mouse_position != past_id) {  // Any orb was moved.
        board->Swap(mouse_position, past_id);
        past_id = mouse_position;
      }
      break;
    }
  }
}

void Graphic::Sleep(const int duration) const {
  for (int i = 0; i < duration / 10; ++i) {
    SDL_Delay(10);
    CheckClose();
  }
}

Graphic::Point Graphic::WaitClick() const {
  SDL_Event event;
  while (true) {
    SDL_WaitEvent(&event);

    // Case that window was closed.
    if (event.type == SDL_QUIT)
      exit(0);

    // Case that mouse was clicked.
    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT) {
      Point point;
      point.x = event.button.x;
      point.y = event.button.y;
      return point;
    }
  }
}

void Graphic::DrawGraph(SDL_Surface *image, int dest_x, int dest_y,
                        int image_id, int image_width, int image_height) {
  SDL_Rect src, dest;
  src.x = (image_id % 4) * image_width;
  src.y = (image_id / 4) * image_height;
  src.w = (image_width == 0) ? image->w : image_width;
  src.h = (image_height == 0) ? image->h : image_height;
  dest.x = dest_x;
  dest.y = dest_y;
  SDL_BlitSurface(image, &src, video_surface, &dest);
}

void Graphic::DrawString(const char *text, int dest_x, int dest_y,
                         const SDL_Color &color) {
  SDL_Surface *temp_text = TTF_RenderUTF8_Blended(font, text, color);
  SDL_Rect src, dest;
  src.x = src.y = 0;
  src.w = temp_text->w;
  src.h = temp_text->h;
  dest.x = dest_x;
  dest.y = dest_y;
  SDL_BlitSurface(temp_text, &src, video_surface, &dest);
  SDL_FreeSurface(temp_text);
}

void Graphic::CheckClose() const {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // If a window was closed.
    if (event.type == SDL_QUIT)
      exit(0);
  }
}

void Graphic::ClearScreen() {
  // Draw a white plane.
  SDL_FillRect(video_surface, 0, 0xffff);
}

void Graphic::Display() {
  // Dump buffer.
  SDL_Flip(video_surface);
}