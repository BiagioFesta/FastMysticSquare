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
#include <DistanceManhattan.hpp>

namespace kpuzzle4::testing {

TEST(DistanceManhattan, ZeroDistance) {
  State aStateA = State::generateSortedState();
  State aStateB = aStateA;

  ASSERT_EQ(DistanceManhattan::computeDistance(aStateA, aStateB), 0);
}

TEST(DistanceManhattan, DistanceOneMove) {
  State aStateA = State::generateSortedState();

  State aStateB;
  ASSERT_NE(aStateA.moveLeft(&aStateB), -1);

  ASSERT_LE(DistanceManhattan::computeDistance(aStateA, aStateB), 1);
}

TEST(DistanceManhattan, DistanceTwoMove) {
  State aStateA = State::generateSortedState();

  State aStateB;
  ASSERT_NE(aStateA.moveLeft(&aStateB), -1);
  ASSERT_NE(aStateB.moveLeft(&aStateB), -1);

  ASSERT_LE(DistanceManhattan::computeDistance(aStateA, aStateB), 2);
}

TEST(DistanceManhattan, DistanceThreeMove) {
  State aStateA = State::generateSortedState();

  State aStateB;
  ASSERT_NE(aStateA.moveLeft(&aStateB), -1);
  ASSERT_NE(aStateB.moveLeft(&aStateB), -1);
  ASSERT_NE(aStateB.moveUp(&aStateB), -1);

  ASSERT_EQ(aStateB.getStateConfiguration(), 0xfeadcb0987654321);

  ASSERT_LE(DistanceManhattan::computeDistance(aStateA, aStateB), 3);
}

TEST(DistanceManhattan, Symmetric) {
  State aStateA = State::generateSortedState();

  State aStateB;
  ASSERT_NE(aStateA.moveLeft(&aStateB), -1);

  ASSERT_EQ(DistanceManhattan::computeDistance(aStateA, aStateB),
            DistanceManhattan::computeDistance(aStateB, aStateA));
}

TEST(DistanceManhattan, DistanceWithFinal) {
  const State aState = State::generateSortedState();
  ASSERT_EQ(DistanceManhattan::computeDistanceWithFinal(aState), 0);
}

}  // namespace kpuzzle4::testing
