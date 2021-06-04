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
#include <PatternDB.hpp>
#include <sstream>

namespace kpuzzle4::testing {

using Mask_t = SearchNode::Mask_t;
using Cost_t = SearchNode::Cost_t;

template <Mask_t... Masks>
class PatternDBTest : public PatternDB<Masks...> {
 public:
  using Parent_t = PatternDB<Masks...>;

  using Parent_t::checkAllPartitionsHasZero;
  using Parent_t::checkPartitionsAreTotal;
  using Parent_t::checkPartitionsDisjoint;
  using Parent_t::computeIndicesOfIndex;
  using Parent_t::computeSizeOfTableCost;
  using Parent_t::countEnabledField;
  using Parent_t::getPartitionIndexOfTileIndex;
  using Parent_t::hash2index;
};

TEST(PatternDB, countEnabledField) {
  constexpr Mask_t kDummyMask = 0x0;
  using Test = std::pair<Mask_t, int>;

  constexpr Test aTestCases[] = {{0x0, 0}, {0xF, 1}, {0xFF, 2}, {0xF0F, 2}};

  for (const auto& [kMask, kCountExpt] : aTestCases) {
    ASSERT_EQ(PatternDBTest<kDummyMask>::countEnabledField(kMask), kCountExpt);
  }
}

TEST(PatternDB, getPartitionIndexOfTileIndex) {
  const PatternDBTest<0xF0F0F0F0F0F0F0FF, 0x0F0F0F0F0F0F0F0F> aPatternDB;

  ASSERT_TRUE(aPatternDB.isValidPartitions());

  for (int i = 1; i < State::kNumTiles; ++i) {
    const int aPartitionIndex = aPatternDB.getPartitionIndexOfTileIndex(i);

    ASSERT_EQ(aPartitionIndex, !(i % 2));
  }
}

TEST(PatternDB, getPartitionIndexOfTileIndexInvalid) {
  const PatternDBTest<0xF0FFFFFFFFFFFFFF> aPatternDB;

  for (int i = 1; i < State::kNumTiles; ++i) {
    const int aPartitionIndex = aPatternDB.getPartitionIndexOfTileIndex(i);

    ASSERT_EQ(aPartitionIndex, i != 14 ? 0 : -1);
  }
}

TEST(PatternDB, computeIndicesOfIndex) {
  const PatternDBTest<0xFFFFFFFF0000000F, 0x00000000FFFFFFFF> aPatternDBA;
  const PatternDBTest<0xF0F0F0F0F0F0F0FF, 0x0F0F0F0F0F0F0F0F> aPatternDBB;
  const PatternDBTest<0xFF0000000000000F, 0x00FFFFFFFFFFFFFF> aPatternDBC;

  ASSERT_EQ(aPatternDBA.computeIndicesOfIndex(), 0x0123456701234560);
  ASSERT_EQ(aPatternDBB.computeIndicesOfIndex(), 0x0011223344556670);
  ASSERT_EQ(aPatternDBC.computeIndicesOfIndex(), 0x010123456789abc0);
}

TEST(PatternDB, hash2index) {
  static constexpr int kNumTests = 2048;
  constexpr Mask_t kMask = 0xFF0000000000000F;
  constexpr Mask_t kMaskNoZero = kMask ^ 0xF;
  const PatternDBTest<kMask> aPatternDB;

  for (int i = 0; i < kNumTests; ++i) {
    const SearchNode aNodeA = State::generateValidRandState(i);
    const SearchNode aNodeB = State::generateValidRandState(i + kNumTests);
    const auto aHashA = aNodeA.getHashWithMask(kMask);
    const auto aHashB = aNodeB.getHashWithMask(kMask);
    const auto aIndexA = aPatternDB.hash2index(aHashA);
    const auto aIndexB = aPatternDB.hash2index(aHashB);

    if ((aHashA & kMaskNoZero) == (aHashB & kMaskNoZero)) {
      ASSERT_EQ(aIndexA, aIndexB) << "Test Case i: " << i;
    } else {
      ASSERT_NE(aIndexA, aIndexB) << "Test Case i: " << i;
    }
  }
}

TEST(PatternDB, checkDisjoint) {
  PatternDBTest<0xFFFFFFFF0000000F, 0x00000000FFFFFFFF> aPatDBValid;
  PatternDBTest<0xF0F0F0F0F0F0F0FF, 0x0F0FFF0F0F0F0F0F> aPatDBNotValid;

  ASSERT_TRUE(aPatDBValid.checkPartitionsDisjoint());
  ASSERT_TRUE(aPatDBValid.isValidPartitions());

  ASSERT_FALSE(aPatDBNotValid.checkPartitionsDisjoint());
  ASSERT_FALSE(aPatDBNotValid.isValidPartitions());
}

TEST(PatternDB, checkZeroEnabled) {
  PatternDBTest<0xFFFFFFFF0000000F, 0x00000000FFFFFFFF> aPatDBValid;
  PatternDBTest<0xF0F0F0F0F0F0F0F0, 0x0F0F0F0F0F0F0F0F> aPatDBNotValid;

  ASSERT_TRUE(aPatDBValid.checkAllPartitionsHasZero());
  ASSERT_TRUE(aPatDBValid.isValidPartitions());

  ASSERT_FALSE(aPatDBNotValid.checkAllPartitionsHasZero());
  ASSERT_FALSE(aPatDBNotValid.isValidPartitions());
}

TEST(PatternDB, checkTotal) {
  PatternDBTest<0xFFFFFFFF0000000F, 0x00000000FFFFFFFF> aPatDBValid;
  PatternDBTest<0xF0F0F0F0F0F0F0FF, 0x0F0F0F0F000F0F0F> aPatDBNotValid;

  ASSERT_TRUE(aPatDBValid.checkPartitionsAreTotal());
  ASSERT_TRUE(aPatDBValid.isValidPartitions());

  ASSERT_FALSE(aPatDBNotValid.checkPartitionsAreTotal());
  ASSERT_FALSE(aPatDBNotValid.isValidPartitions());
}

TEST(PatternDB, isValid) {
  using PatternDBNotDis = PatternDB<0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF>;
  using PatternDBNotZero = PatternDB<0xFFFFFFF00000000F, 0x0000000FFFFFFFF0>;
  using PatternDBNotTotal = PatternDB<0xFFFFFFF00000000F, 0x0000000FFFFFFF0F>;

  bool isValid;

  isValid = PatternDBNotDis::isValidPartitions();
  ASSERT_FALSE(isValid);

  isValid = PatternDBNotZero::isValidPartitions();
  ASSERT_FALSE(isValid);

  isValid = PatternDBNotTotal::isValidPartitions();
  ASSERT_FALSE(isValid);
}

TEST(PatternDB, checkSizeIndices) {
  static constexpr int kNumTests = 2048;
  constexpr Mask_t kMaskA = 0xFF0000000000000F;
  constexpr Mask_t kMaskB = 0x00000000FFFFFFFF;
  const PatternDBTest<kMaskA, kMaskB> aPatternDB;

  for (int i = 0; i < kNumTests; ++i) {
    const SearchNode aNodeA = State::generateValidRandState(i);
    const SearchNode aNodeB = State::generateValidRandState(i + kNumTests);
    const auto aHashA = aNodeA.getHashWithMask(kMaskA);
    const auto aHashB = aNodeB.getHashWithMask(kMaskB);
    const auto aIndexA = aPatternDB.hash2index(aHashA);
    const auto aIndexB = aPatternDB.hash2index(aHashB);

    ASSERT_LE(aIndexA, 0xFFFFFFFF);
    ASSERT_LE(aIndexB, 0xFFFFFFF);
  }
}

TEST(PatternDB, computeSizeOfTableCost) {
  const PatternDBTest<0xFFFFFFFF0000000F,
                      0x00000000FFFFFFFF,
                      0xF,
                      0xFF,
                      0xFFF,
                      0xFF0FF>
      aPatternDB;

  for (const auto aPartition : aPatternDB.getMaskPartitions()) {
    const int kSizePartition = aPatternDB.countEnabledField(aPartition) - 1;
    ASSERT_GE(kSizePartition, 0);

    std::uint64_t aRightShiftedMask = 0x0;
    for (int i = 0; i < kSizePartition; ++i) {
      aRightShiftedMask <<= 4;
      aRightShiftedMask |= 0xf;
    }

    ASSERT_EQ(aPatternDB.computeSizeOfTableCost(aPartition),
              aRightShiftedMask + 1);
  }
}

TEST(PatternDB, generateDB) {
  static constexpr Mask_t kMask = 0xF00000000000000F;
  static constexpr int kExpectedTableCostSize = 16;
  PatternDBTest<kMask> aPatternDB;
  aPatternDB.generate();

  const auto& aCostTable = aPatternDB.getCostTable(0);
  ASSERT_EQ(aCostTable.size(), kExpectedTableCostSize);
  for (int i = 0; i < kExpectedTableCostSize; ++i) {
    ASSERT_GE(aCostTable[i], 0);
    ASSERT_LE(aCostTable[i], SearchNode::kMaxPath);
  }
}

TEST(PatternDB, getCost) {
  static constexpr Mask_t kMask = 0xF00000000000000F;
  PatternDBTest<kMask> aPatternDB;
  aPatternDB.generate();

  for (int i = 0; i < State::kNumTiles; ++i) {
    State::StateConfiguration_t aConfiguration = 0x0;
    for (int j = 0; j < State::kNumTiles; ++j) {
      const int kIndexF = State::kNumTiles - i - 1;
      if (j == kIndexF) {
        aConfiguration |= static_cast<std::uint64_t>(0xF) << (j << 2);
      } else if (j < kIndexF) {
        aConfiguration |= static_cast<std::uint64_t>(j) << (j << 2);
      } else {
        aConfiguration |= static_cast<std::uint64_t>(j - 1) << (j << 2);
      }
    }

    const Cost_t kExpectedCost =
        std::abs(((State::kNumTiles - i - 1) % State::kSize) - 2) +
        std::abs(((State::kNumTiles - i - 1) / State::kSize) - 3);

    const State aState{aConfiguration};
    ASSERT_EQ(aPatternDB.getCost(aState), kExpectedCost);
  }
}

TEST(PatternDB, serializeAndDeserialize) {
  static constexpr Mask_t kMask = 0xF00000000000000F;

  PatternDB<kMask> aPatternDB;
  aPatternDB.generate();

  std::stringstream aSs;
  aPatternDB.serialize(&aSs);

  PatternDB<kMask> aPatternDBLoad;
  aSs.seekg(std::ios_base::beg);
  aPatternDBLoad.deserialize(&aSs);

  ASSERT_EQ(aPatternDB.getCostTable(0).size(),
            aPatternDBLoad.getCostTable(0).size());
}

TEST(PatternDB, serializeAndDeserializeIncompatible) {
  using PatternDBOriginal = PatternDB<0xF00000000000000F>;
  using PatternDBTwoPartitions =
      PatternDB<0xF00000000000000F, 0x0FFFFFFFFFFFFFFF>;
  using PatternDBDiffModel = PatternDB<0xFF0000000000000F>;

  std::stringstream aSs;
  ASSERT_THROW(PatternDBOriginal{}.deserialize(&aSs), std::exception);
  aSs.clear();

  PatternDBOriginal aPatternDB;
  aPatternDB.generate();
  aPatternDB.serialize(&aSs);

  aSs.seekg(std::ios_base::beg);
  ASSERT_THROW(PatternDBTwoPartitions{}.deserialize(&aSs), std::exception);

  aSs.seekg(std::ios_base::beg);
  ASSERT_THROW(PatternDBDiffModel{}.deserialize(&aSs), std::exception);
}

TEST(PatternDB, incompleteStream) {
  constexpr std::uint64_t kMask = 0xF00000000000000F;
  constexpr std::int32_t kNumPartitions = 1;
  constexpr std::uint64_t kSizeTable = 1;
  using PatternDB = PatternDB<kMask>;

  std::stringstream aSs;
  ASSERT_THROW(PatternDB{}.deserialize(&aSs), std::exception);
  aSs.clear();

  aSs.write(reinterpret_cast<const char*>(&kNumPartitions),
            sizeof(kNumPartitions));
  ASSERT_THROW(PatternDB{}.deserialize(&aSs), std::exception);
  aSs.clear();

  aSs.seekg(std::ios_base::beg);
  aSs.write(reinterpret_cast<const char*>(&kMask), sizeof(kMask));
  ASSERT_THROW(PatternDB{}.deserialize(&aSs), std::exception);
  aSs.clear();

  aSs.seekg(std::ios_base::beg);
  aSs.write(reinterpret_cast<const char*>(&kSizeTable), sizeof(kSizeTable));
  ASSERT_THROW(PatternDB{}.deserialize(&aSs), std::exception);
  aSs.clear();
}

}  // namespace kpuzzle4::testing
