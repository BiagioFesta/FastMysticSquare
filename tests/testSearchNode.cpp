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
#include <SearchNode.hpp>

namespace kpuzzle4::testing {

TEST(SearchNode, InitialStateConstruction) {
  constexpr State kState = 0x0fedcba987654321;
  constexpr SearchNode::Direction kDirection = SearchNode::Direction::RIGHT;
  constexpr SearchNode::Cost_t kCost = 42;

  const SearchNode aSearchNode{kState, kDirection, kCost, 0};
  ASSERT_EQ(aSearchNode.getState(), kState);
  ASSERT_EQ(aSearchNode.getLastMove(), kDirection);
  ASSERT_EQ(aSearchNode.getCost2Here(), kCost);
  ASSERT_EQ(aSearchNode.getCounterPath(), 0);
}

TEST(SearchNode, moveLeft) {
  static const SearchNode aNodeMovable = State{0x0fedcba987654321};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveLeft(&aNewNode), 15);
  ASSERT_EQ(aNewNode.getCost2Here(), 1);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::LEFT);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'L');
}

TEST(SearchNode, moveLeftMaskOn) {
  static const SearchNode aNodeMovable = State{0x0fedcba987654321};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveLeft(&aNewNode, 0xf000000000000000), 15);
  ASSERT_EQ(aNewNode.getCost2Here(), 1);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::LEFT);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'L');
}

TEST(SearchNode, moveLeftMaskOff) {
  static const SearchNode aNodeMovable = State{0x0fedcba987654321};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveLeft(&aNewNode, 0x0fffffffffffffff), 15);
  ASSERT_EQ(aNewNode.getCost2Here(), 0);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::LEFT);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'L');
}

TEST(SearchNode, moveRight) {
  static const SearchNode aNodeMovable = State{0xf0edcba987654321};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveRight(&aNewNode), 15);
  ASSERT_EQ(aNewNode.getCost2Here(), 1);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::RIGHT);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'R');
}

TEST(SearchNode, moveRightMaskOn) {
  static const SearchNode aNodeMovable = State{0xf0edcba987654321};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveRight(&aNewNode, 0xf000000000000000), 15);
  ASSERT_EQ(aNewNode.getCost2Here(), 1);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::RIGHT);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'R');
}

TEST(SearchNode, moveRightMaskOff) {
  static const SearchNode aNodeMovable = State{0xf0edcba987654321};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveRight(&aNewNode, 0x0fffffffffffffff), 15);
  ASSERT_EQ(aNewNode.getCost2Here(), 0);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::RIGHT);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'R');
}

TEST(SearchNode, moveUp) {
  static const SearchNode aNodeMovable = State{0x0fedcba987654321};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveUp(&aNewNode), 0xc);
  ASSERT_EQ(aNewNode.getCost2Here(), 1);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::UP);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'U');
}

TEST(SearchNode, moveUpMaskOn) {
  static const SearchNode aNodeMovable = State{0x0fedcba987654321};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveUp(&aNewNode, 0x000f000000000000), 0xc);
  ASSERT_EQ(aNewNode.getCost2Here(), 1);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::UP);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'U');
}

TEST(SearchNode, moveUpMaskOff) {
  static const SearchNode aNodeMovable = State{0x0fedcba987654321};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveUp(&aNewNode, 0xfff0ffffffffffff), 0xc);
  ASSERT_EQ(aNewNode.getCost2Here(), 0);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::UP);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'U');
}

TEST(SearchNode, moveDown) {
  static const SearchNode aNodeMovable = State{0xfedcba9876543210};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveDown(&aNewNode), 0x4);
  ASSERT_EQ(aNewNode.getCost2Here(), 1);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::DOWN);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'D');
}

TEST(SearchNode, moveDownMaskOn) {
  static const SearchNode aNodeMovable = State{0xfedcba9876543210};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveDown(&aNewNode, 0x00000000000f0000), 0x4);
  ASSERT_EQ(aNewNode.getCost2Here(), 1);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::DOWN);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'D');
}

TEST(SearchNode, moveDownMaskOff) {
  static const SearchNode aNodeMovable = State{0xfedcba9876543210};

  SearchNode aNewNode;
  ASSERT_EQ(aNodeMovable.moveDown(&aNewNode, 0xfffffffffff0ffff), 0x4);
  ASSERT_EQ(aNewNode.getCost2Here(), 0);
  ASSERT_EQ(aNewNode.getLastMove(), SearchNode::Direction::DOWN);
  ASSERT_EQ(aNewNode.getCounterPath(), 1);
  ASSERT_EQ(aNewNode.getPath2Here()[0], 'D');
}

TEST(SearchNode, hashWithMaskSame) {
  static const SearchNode::Mask_t kMask = 0xff00000000000000;
  static const SearchNode aSearchNodeA = State{0xfedcba9876543210};
  static const SearchNode aSearchNodeB = State{0xfedcba9876543201};

  ASSERT_EQ(aSearchNodeA.getHashWithMask(kMask),
            aSearchNodeB.getHashWithMask(kMask));
}

TEST(SearchNode, hashWithMaskDifferent) {
  static const SearchNode::Mask_t kMask = 0xff00000000000000;
  static const SearchNode aSearchNodeA = State{0xfedcba9876543210};
  static const SearchNode aSearchNodeB = State{0xefdcba9876543210};

  ASSERT_NE(aSearchNodeA.getHashWithMask(kMask),
            aSearchNodeB.getHashWithMask(kMask));
}

}  // namespace kpuzzle4::testing
