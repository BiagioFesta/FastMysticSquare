/*
  Copyright (C) 2018  Biagio Festa

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include <gtest/gtest.h>
#include <State.hpp>

namespace kpuzzle4::testing {

TEST(State, sortedState) {
  State aState;
  aState = State::generateSortedState();

  ASSERT_EQ(aState.getStateConfiguration(), 0x0fedcba987654321);
  ASSERT_EQ(aState.getTilesPositions(), 0xedcba9876543210f);
  ASSERT_EQ(aState.getIndexSpace(), 15);
}

TEST(State, moveLeft) {
  const State aStateMovable = State{0x0fedcba987654321};
  const State aStateFixed = State{0xfed0cba987654321};

  State aNewState;
  ASSERT_EQ(aStateMovable.moveLeft(&aNewState), 15);
  ASSERT_NE(aStateMovable, aNewState);
  ASSERT_EQ(aNewState.getStateConfiguration(), 0xf0edcba987654321);
  ASSERT_EQ(aNewState.getTilesPositions(), 0xfdcba9876543210e);
  ASSERT_EQ(aNewState.getIndexSpace(), 14);

  ASSERT_EQ(aStateFixed.moveLeft(&aNewState), -1);
}

TEST(State, moveRight) {
  const State aStateMovable = State{0xf0edcba987654321};
  const State aStateFixed = State{0x0fedcba987654321};

  State aNewState;
  ASSERT_EQ(aStateMovable.moveRight(&aNewState), 15);
  ASSERT_NE(aStateMovable, aNewState);
  ASSERT_EQ(aNewState.getStateConfiguration(), 0x0fedcba987654321);
  ASSERT_EQ(aNewState.getTilesPositions(), 0xedcba9876543210f);
  ASSERT_EQ(aNewState.getIndexSpace(), 15);

  ASSERT_EQ(aStateFixed.moveRight(&aNewState), -1);
}

TEST(State, moveUp) {
  const State aStateMovable = State{0x0fedcba987654321};
  const State aStateFixed = State{0xfedcba9876543210};

  State aNewState;
  ASSERT_EQ(aStateMovable.moveUp(&aNewState), 0xc);
  ASSERT_NE(aStateMovable, aNewState);
  ASSERT_EQ(aNewState.getStateConfiguration(), 0xcfed0ba987654321);
  ASSERT_EQ(aNewState.getTilesPositions(), 0xedcfa9876543210b);
  ASSERT_EQ(aNewState.getIndexSpace(), 11);

  ASSERT_EQ(aStateFixed.moveUp(&aNewState), -1);
}

TEST(State, moveDown) {
  const State aStateMovable = State{0xfedcba9876543210};
  const State aStateFixed = State{0x0fedcba987654321};

  State aNewState;
  ASSERT_EQ(aStateMovable.moveDown(&aNewState), 0x4);
  ASSERT_NE(aStateMovable, aNewState);
  ASSERT_EQ(aNewState.getStateConfiguration(), 0xfedcba9876503214);
  ASSERT_EQ(aNewState.getTilesPositions(), 0xfedcba9876503214);
  ASSERT_EQ(aNewState.getIndexSpace(), 4);

  ASSERT_EQ(aStateFixed.moveDown(&aNewState), -1);
}

TEST(State, testInvertibilityX) {
  const State aOriginalState = State::generateSortedState();

  State aNewState;
  ASSERT_NE(aOriginalState.moveLeft(&aNewState), -1);
  ASSERT_NE(aOriginalState, aNewState);

  ASSERT_NE(aNewState.moveRight(&aNewState), -1);
  ASSERT_EQ(aOriginalState, aNewState);
}

TEST(State, testInvertibilityY) {
  const State aOriginalState = State::generateSortedState();

  State aNewState;
  ASSERT_NE(aOriginalState.moveUp(&aNewState), -1);
  ASSERT_NE(aOriginalState, aNewState);

  ASSERT_NE(aNewState.moveDown(&aNewState), -1);
  ASSERT_EQ(aOriginalState, aNewState);
}

TEST(State, invalidConfiguration) {
  State aInvalidState{0xffedcba987654321};

  ASSERT_EQ(aInvalidState.getIndexSpace(), -1);
}

TEST(State, buildFromSortedValues) {
  static constexpr State kSortedState = State::generateSortedState();

  std::array<int, State::kNumTiles> kValues;
  for (int i = 0; i < State::kNumTiles; ++i) {
    kValues[i] = i < State::kNumTilesMinusOne ? i + 1 : 0;
  }

  State aState{kValues};
  for (int i = 0; i < State::kNumTiles; ++i) {
    ASSERT_EQ(kValues[i], aState.getValueTileAt(i));
    ASSERT_EQ(kValues[i], kSortedState.getValueTileAt(i));
  }

  ASSERT_EQ(aState.getIndexSpace(), kSortedState.getIndexSpace());
  ASSERT_EQ(aState.getStateConfiguration(),
            kSortedState.getStateConfiguration());
  ASSERT_EQ(aState.getTilesPositions(), kSortedState.getTilesPositions());
}

TEST(State, sortedIsSolveble) {
  static constexpr State kSortedState = State::generateSortedState();
  ASSERT_TRUE(kSortedState.isSolveable());
}

TEST(State, sortedIsValid) {
  static constexpr State kSortedState = State::generateSortedState();
  ASSERT_TRUE(kSortedState.isValid());
}

TEST(State, solveableRandom) {
  static constexpr int kNumRandomToTry = 1024;

  for (int i = 0; i < kNumRandomToTry; ++i) {
    const State kRandomState = State::generateValidRandState(i);
    ASSERT_TRUE(kRandomState.isSolveable());
    ASSERT_TRUE(kRandomState.isValid());
  }
}

TEST(State, buildFromValuesDuplicate) {
  std::array<int, State::kNumTiles> kValues;
  for (int i = 0; i < State::kNumTiles; ++i) {
    kValues[i] = i < State::kNumTilesMinusOne ? i + 1 : 1;
  }

  State aState{kValues};
  ASSERT_FALSE(aState.isValid());
}

TEST(State, buildFromValuesWrongValues) {
  std::array<int, State::kNumTiles> kValues;
  for (int i = 0; i < State::kNumTiles; ++i) {
    kValues[i] = i * 2;
  }

  State aState{kValues};
  ASSERT_FALSE(aState.isValid());
}

TEST(State, hashWithMaskSame) {
  static const State::Mask_t kMask = 0xff00000000000000;
  static const State aStateA = State{0xfedcba9876543210};
  static const State aStateB = State{0xfedcba9876543201};

  ASSERT_EQ(aStateA.getHashWithMask(kMask), aStateB.getHashWithMask(kMask));
}

TEST(State, hashWithMaskDifferent) {
  static const State::Mask_t kMask = 0xff00000000000000;
  static const State aStateA = State{0xfedcba9876543210};
  static const State aStateB = State{0xefdcba9876543210};

  ASSERT_NE(aStateA.getHashWithMask(kMask), aStateB.getHashWithMask(kMask));
}

}  // namespace kpuzzle4::testing
