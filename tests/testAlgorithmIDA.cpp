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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <AlgorithmIDA.hpp>

namespace kpuzzle4::testing {

class HeuristicFunction {
 public:
  MOCK_CONST_METHOD1(computeCost, int(const State&));

  int operator()(const State& iState) const { return computeCost(iState); }
};

TEST(AlgorithmIDA, FromFinal) {
  using ::testing::Return;

  const State aState = State::generateSortedState();

  HeuristicFunction aHeuristicFunction;
  EXPECT_CALL(aHeuristicFunction, computeCost(aState))
      .Times(1)
      .WillOnce(Return(0));

  AlgorithmIDA aAlgorithmIDA;
  ASSERT_TRUE(
      aAlgorithmIDA.findSolution(aState, aHeuristicFunction)._solutionFound);

  ASSERT_EQ(aAlgorithmIDA.getExploredNodes(), 1);
  ASSERT_EQ(aAlgorithmIDA.getCurrentMaxDepth(), 0);
  ASSERT_EQ(aAlgorithmIDA.getSolutionLength(), 0);
}

TEST(AlgorithmIDA, TwoSteps) {
  using ::testing::_;
  using ::testing::Return;

  State aState = State::generateSortedState();
  aState.moveLeft(&aState);
  aState.moveUp(&aState);

  HeuristicFunction aHeuristicFunction;
  EXPECT_CALL(aHeuristicFunction, computeCost(_)).WillRepeatedly(Return(0));

  AlgorithmIDA aAlgorithmIDA;
  ASSERT_TRUE(
      aAlgorithmIDA.findSolution(aState, aHeuristicFunction)._solutionFound);

  ASSERT_GT(aAlgorithmIDA.getExploredNodes(), 1);
  ASSERT_GT(aAlgorithmIDA.getCurrentMaxDepth(), 1);
  ASSERT_EQ(aAlgorithmIDA.getSolutionLength(), 2);
  ASSERT_EQ(aAlgorithmIDA.getSolutionPath()[0], 'D');
  ASSERT_EQ(aAlgorithmIDA.getSolutionPath()[1], 'R');
}

TEST(AlgorithmIDA, ImpossibleCase) {
  using ::testing::_;
  using ::testing::Return;

  static constexpr State::StateConfiguration_t kImpossibleConfiguration =
      0x0efdcba987654321;
  static constexpr State kUnsolvableState{kImpossibleConfiguration};

  HeuristicFunction aHeuristicFunction;
  EXPECT_CALL(aHeuristicFunction, computeCost(_))
      .WillRepeatedly(Return(AlgorithmIDA::kTotalDepthLimit));

  AlgorithmIDA aAlgorithmIDA;
  ASSERT_FALSE(aAlgorithmIDA.findSolution(kUnsolvableState, aHeuristicFunction)
                   ._solutionFound);
}

}  // namespace kpuzzle4::testing
