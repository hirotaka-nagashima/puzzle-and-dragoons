//-----------------------------------------------------------------------------
// Copyright (c) 2014 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef PUZZLE_AND_DRAGONS_BOARD_H_
#define PUZZLE_AND_DRAGONS_BOARD_H_

class Board {
public:
  // The number of attributes of orbs.
  static const int kNumAttributes = 6;

  struct Score {
    void Add(const Score &a);

    int sum_orbs;
    int sum_combos;
    int num_orbs[kNumAttributes];
    int num_combos[kNumAttributes];
  };

  enum OrbAttributes {
    kTemp = -3,
    kNone = -2,
    kOutside = -1,
    kFire,
    kWood,
    kWater,
    kHeart,
    kLight,
    kDark,
    kPoison,
    kObstacle,
  };

  static const int kWidth = 6;
  static const int kHeight = 5;
  static const int kSize = kWidth * kHeight;
  static const int kArrayWidth = 1 + kWidth + 1;    // Including sentinels.
  static const int kArrayHeight = 1 + kHeight + 1;  // Including sentinels.
  static const int kArraySize = kArrayWidth * kArrayHeight;
  // The minimum number required to vanish orbs.
  static const int kConnectionMinNum;
  static const int kMaxCombos;
  static const int k4Directions[4];

  void Initialize();
  // Return information about vanished orbs.
  Score VanishOrbs();
  void DropOrbs();
  void MoveOrb(int direction, int src);
  void Swap(int id_1, int id_2);
  bool Equals(const Board &target) const;
  int CalculateMaxCombos() const;
  int Evaluate() const;
  int GetId(int y, int x) const;

  int board(int id) const { return board_[id]; }
  int board(int y, int x) const { return board(GetId(y, x)); }

protected:
  // For "evaluate()".
  int CalculatePerimeter() const;
  int MeasureFarthestOrbsDistance() const;
  int CountNumOrbsOnEdge() const;

private:
  void AddNewOrbs();
  bool IsOrb(int id) const;
  bool IsEdge(int y, int x) const;

  void set_board(int id, int attribute) { board_[id] = attribute; }
  void set_board(int y, int x, int attribute) {
    set_board(GetId(y, x), attribute);
  }

  int board_[kArraySize];
};

#endif  // PUZZLE_AND_DRAGONS_BOARD_H_