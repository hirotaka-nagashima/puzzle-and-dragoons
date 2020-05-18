//-----------------------------------------------------------------------------
// Copyright (c) 2014 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------
// 2016/11/03: Reconstructed with C++.
// 2014/11/08: Project was created.
//-----------------------------------------------------------------------------

#include "game.h"

// NOTE: Comment out below to use SDL in VS2015 or later versions.
// https://stackoverflow.com/questions/30412951/
FILE _iob[] = {*stdin, *stdout, *stderr};
extern "C" FILE * __cdecl __iob_func() {
  return _iob;
}

// NOTE: When we use SDL, "SDLInitialize()" and "SDLFinalize()"
// must be called from "main()". Moreover, "main()" must be
// "int main(int argc, char *argv[])" and return "0".
int main(int argc, char *argv[]) {
  Game game;
  game.Initialize();
  game.SolveAuto();
  game.Terminate();
  return 0;
}