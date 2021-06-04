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
#ifndef KPUZZLE4__PATTERN_DB__HPP
#define KPUZZLE4__PATTERN_DB__HPP
#include <array>
#include <istream>
#include <ostream>
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <vector>
#include "SearchNode.hpp"

namespace kpuzzle4 {

template <SearchNode::Mask_t... Masks>
class PatternDB {
 public:
  static constexpr int kNumPartitions = sizeof...(Masks);

  using Mask_t = SearchNode::Mask_t;
  using Cost_t = SearchNode::Cost_t;
  using MaskPartitions_t = std::array<Mask_t, kNumPartitions>;
  using CostTable_t = std::vector<Cost_t>;

  //! \return the mask partitions model.
  static constexpr const MaskPartitions_t& getMaskPartitions() noexcept;

  //! \return whether the partition model is valid or not.
  static constexpr bool isValidPartitions() noexcept;

  /*! \brief It generates the cost tables for all partitions of Pattern.
   *  The time and space complexity depends on the size of Partitions.
   */
  void generate();

  //! \return the Cost Table of the i-th partition.
  const CostTable_t& getCostTable(const int iPartitionIndex) const noexcept;

  /*! \brief After the PatternDB has been generated is possible to compute an
   *  heuristic cost (distance from the sorted state) with this method.
   */
  Cost_t getCost(const State& iState) const noexcept;

  /*! \brief It serializes the content of the entire database into a output
   *  stream.
   */
  void serialize(std::ostream* oStream) const;

  /*! \brief It deserialies (load) the content of a input stream to construct
   *  the pattern database.
   *  \see serialize
   *  \throw std::runtime_error in case of file is not compatible or errors
   *  generated.
   */
  void deserialize(std::istream* iStream);

 protected:
  std::array<CostTable_t, kNumPartitions> _costTablePartitions;

  /*! \brief It counts how many tile are in the mask (partition).
   *  E.g., 0xFF00 -> 2
   *        0xF00F -> 2
   *        0xF000 -> 1
   */
  static constexpr int countEnabledField(const Mask_t iMask) noexcept;

  /*! \brief Given a tile value and partitions, it computes the partition-index
   *  in which the tile is enabled (the tile belongs to that partition).
   *  \param [in] iTileValue        The value of the tile to investigate.
   *                                (E.g. 0xf, 0x2, ...).
   *  \note 'iTileValue' cannot be 0 because it belogs to multiple partitions.
   *  \return the index of the partition or -1 in case not belong any partition.
   */
  static constexpr int getPartitionIndexOfTileIndex(const int iTileValue) noexcept;

  /*! \brief Given a partitions model it computes the index that each tile in
   *  the hash should assume in the index computation.
   *  E.g.
   *        Partition1: 0xFF00F    Partition2: 0x00FFF
   *                      ->   0x1010
   *
   *        Partition1: 0xF0F0F    Partition2: 0x0F0FF
   *                      ->   0x1100
   */
  static constexpr std::uint64_t computeIndicesOfIndex() noexcept;

  /*! \brief Checks whether all partitions are disjointed or not.
   *  \note the zero (the last 4 bits) are not considered.
   */
  static constexpr bool checkPartitionsDisjoint() noexcept;

  //! \brief Checks whether all partitions have zero active in the mask.
  static constexpr bool checkAllPartitionsHasZero() noexcept;

  /*! \brief Check whther all partitions are total:
   *  That is, the union of all partition is the universal set (no mask).
   */
  static constexpr bool checkPartitionsAreTotal() noexcept;

  /*! \brief "Compress" the hash generating a index to insert the node into the
   * cost table.
   */
  static constexpr int hash2index(const std::uint64_t iHash) noexcept;

  //! \return the size (in terms of number of element) of the table for a mask.
  static constexpr std::uint64_t computeSizeOfTableCost(const Mask_t iMask) noexcept;

  /*! \brief Generated all possible cost, filling the Cost Table.
   */
  static void bfs(const int iIndexPartition, CostTable_t* oCostTable);

  static constexpr MaskPartitions_t sMaskPartitions = {Masks...};
  static constexpr std::uint64_t sIndicesForValues = computeIndicesOfIndex();
};

template <SearchNode::Mask_t... Mask>
constexpr int PatternDB<Mask...>::countEnabledField(const Mask_t iMask) noexcept {
  int aCounter = 0;

  for (int i = 0; i < State::kNumTiles; ++i) {
    assert(((iMask >> (i << 2)) & 0xF) == 0xF || ((iMask >> (i << 2)) & 0xF) == 0x0);

    aCounter += (iMask >> (i << 2)) & 0x1;
  }

  return aCounter;
}

template <SearchNode::Mask_t... Mask>
constexpr int PatternDB<Mask...>::getPartitionIndexOfTileIndex(const int iTileValue) noexcept {
  assert(iTileValue != 0);

  for (int i = 0; i < kNumPartitions; ++i) {
    if ((sMaskPartitions[i] >> (iTileValue << 2)) & 0xF) return i;
  }

  return -1;
}

template <SearchNode::Mask_t... Mask>
constexpr std::uint64_t PatternDB<Mask...>::computeIndicesOfIndex() noexcept {
  std::array<std::uint64_t, kNumPartitions> aCounters = {};
  std::uint64_t aIndicesOfIndex = 0;

  for (int i = State::kNumTilesMinusOne; i > 0; --i) {
    const int aPartitionIndex = getPartitionIndexOfTileIndex(i);
    if (aPartitionIndex != -1) {
      assert(aCounters[aPartitionIndex] <= 0xF);

      aIndicesOfIndex |= aCounters[aPartitionIndex] << (i << 2);
      ++aCounters[aPartitionIndex];
    }
  }

  return aIndicesOfIndex;
}

template <SearchNode::Mask_t... Mask>
constexpr bool PatternDB<Mask...>::isValidPartitions() noexcept {
  return checkPartitionsDisjoint() && checkAllPartitionsHasZero() && checkPartitionsAreTotal();
}

template <SearchNode::Mask_t... Mask>
constexpr bool PatternDB<Mask...>::checkPartitionsDisjoint() noexcept {
  constexpr int kNumPartitionsMinusOne = kNumPartitions - 1;

  for (int i = 0; i < kNumPartitionsMinusOne; ++i) {
    for (int j = i + 1; j < kNumPartitions; ++j) {
      if ((sMaskPartitions[i] & sMaskPartitions[j]) != 0xF) return false;
    }
  }

  return true;
}

template <SearchNode::Mask_t... Mask>
constexpr bool PatternDB<Mask...>::checkAllPartitionsHasZero() noexcept {
  for (int i = 0; i < kNumPartitions; ++i) {
    if ((sMaskPartitions[i] & 0xF) != 0xF) return false;
  }
  return true;
}

template <SearchNode::Mask_t... Mask>
constexpr bool PatternDB<Mask...>::checkPartitionsAreTotal() noexcept {
  for (int i = State::kNumTilesMinusOne; i > 0; --i) {
    if (getPartitionIndexOfTileIndex(i) == -1) return false;
  }
  return true;
}

template <SearchNode::Mask_t... Mask>
constexpr int PatternDB<Mask...>::hash2index(const std::uint64_t iHash) noexcept {
  int aIndex = 0;

  for (int i = State::kNumTilesMinusOne; i > 0; --i) {
    const int kHashValueOfI = static_cast<int>((iHash >> (i << 2)) & 0xF);
    assert(kHashValueOfI >= 0);
    assert(kHashValueOfI < State::kNumTiles);

    const int kShift = ((sIndicesForValues >> (i << 2) & 0xF) << 2);
    aIndex |= kHashValueOfI << kShift;
  }

  return aIndex;
}

template <SearchNode::Mask_t... Mask>
constexpr const typename PatternDB<Mask...>::MaskPartitions_t& PatternDB<Mask...>::getMaskPartitions() noexcept {
  return sMaskPartitions;
}

template <SearchNode::Mask_t... Mask>
constexpr std::uint64_t PatternDB<Mask...>::computeSizeOfTableCost(const Mask_t iMask) noexcept {
  const int kSizeMask = countEnabledField(iMask) - 1;
  return static_cast<std::uint64_t>(1) << (kSizeMask * 4);
}

template <SearchNode::Mask_t... Mask>
void PatternDB<Mask...>::generate() {
  for (int i = 0; i < kNumPartitions; ++i) {
    bfs(i, &_costTablePartitions[i]);
  }
}

template <SearchNode::Mask_t... Mask>
void PatternDB<Mask...>::bfs(const int iIndexPartition, CostTable_t* oCostTable) {
  assert(iIndexPartition < kNumPartitions);
  using OpenList_t = std::queue<SearchNode>;
  using CloseList_t = std::unordered_set<std::uint64_t>;
  static constexpr SearchNode kSortedNode = State::generateSortedState();
  static constexpr auto kMaxCost = std::numeric_limits<Cost_t>::max();

  const Mask_t kMaskPartition = sMaskPartitions[iIndexPartition];

  oCostTable->resize(computeSizeOfTableCost(kMaskPartition), kMaxCost);
  OpenList_t aOpenList;
  CloseList_t aCloseList;
  SearchNode aChildNode;

  aOpenList.emplace(kSortedNode);
  while (!aOpenList.empty()) {
    const auto aCurrentNode = std::move(aOpenList.front());
    aOpenList.pop();
    const std::uint64_t aHash = aCurrentNode.getHashWithMask(kMaskPartition);

    if (aCloseList.count(aHash) == 0) {
      aCloseList.insert(aHash);
      const int aIndex = hash2index(aHash);
      const Cost_t aCost = aCurrentNode.getCost2Here();
      assert(aIndex < static_cast<int>(oCostTable->size()));

      (*oCostTable)[aIndex] = std::min(aCost, (*oCostTable)[aIndex]);

      if (aCurrentNode.getLastMove() != SearchNode::Direction::RIGHT &&
          aCurrentNode.moveLeft(&aChildNode, kMaskPartition) != -1) {
        aOpenList.push(std::move(aChildNode));
      }
      if (aCurrentNode.getLastMove() != SearchNode::Direction::LEFT &&
          aCurrentNode.moveRight(&aChildNode, kMaskPartition) != -1) {
        aOpenList.push(std::move(aChildNode));
      }
      if (aCurrentNode.getLastMove() != SearchNode::Direction::UP &&
          aCurrentNode.moveDown(&aChildNode, kMaskPartition) != -1) {
        aOpenList.push(std::move(aChildNode));
      }
      if (aCurrentNode.getLastMove() != SearchNode::Direction::DOWN &&
          aCurrentNode.moveUp(&aChildNode, kMaskPartition) != -1) {
        aOpenList.push(std::move(aChildNode));
      }
    }
  }
}

template <SearchNode::Mask_t... Mask>
const typename PatternDB<Mask...>::CostTable_t& PatternDB<Mask...>::getCostTable(
    const int iPartitionIndex) const noexcept {
  assert(iPartitionIndex < kNumPartitions);
  return _costTablePartitions[iPartitionIndex];
}

template <SearchNode::Mask_t... Mask>
typename PatternDB<Mask...>::Cost_t PatternDB<Mask...>::getCost(const State& iState) const noexcept {
  Cost_t aCost = 0;

  for (int i = 0; i < kNumPartitions; ++i) {
    const std::uint64_t aMaskedHash = iState.getHashWithMask(sMaskPartitions[i]);
    const auto aIndex = hash2index(aMaskedHash);
    assert(aIndex < static_cast<int>(_costTablePartitions[i].size()));
    assert(_costTablePartitions[i][aIndex] >= 0);
    assert(_costTablePartitions[i][aIndex] <= SearchNode::kMaxPath);

    aCost += _costTablePartitions[i][aIndex];
  }

  return aCost;
}

template <SearchNode::Mask_t... Mask>
void PatternDB<Mask...>::serialize(std::ostream* oStream) const {
  const std::int32_t aNumPartitions = static_cast<std::int32_t>(kNumPartitions);
  oStream->write(reinterpret_cast<const char*>(&aNumPartitions), sizeof(aNumPartitions));

  for (int i = 0; i < kNumPartitions; ++i) {
    const std::uint64_t aMask = static_cast<std::uint64_t>(sMaskPartitions[i]);
    oStream->write(reinterpret_cast<const char*>(&aMask), sizeof(aMask));
  }

  for (int i = 0; i < kNumPartitions; ++i) {
    const std::uint64_t aSizeTable = _costTablePartitions[i].size();
    oStream->write(reinterpret_cast<const char*>(&(aSizeTable)), sizeof(aSizeTable));

    for (std::uint64_t j = 0; j < aSizeTable; ++j) {
      const std::uint8_t aCost = _costTablePartitions[i][j];
      oStream->write(reinterpret_cast<const char*>(&(aCost)), sizeof(aCost));
    }
  }
}

template <SearchNode::Mask_t... Mask>
void PatternDB<Mask...>::deserialize(std::istream* iStream) {
  std::int32_t aNumPartitions;
  iStream->read(reinterpret_cast<char*>(&aNumPartitions), sizeof(aNumPartitions));
  if (iStream->gcount() != sizeof(aNumPartitions)) {
    throw std::runtime_error("PatternDB File is not valid");
  }
  if (aNumPartitions != kNumPartitions) {
    throw std::runtime_error("PatternDB File has different number of partitions");
  }

  for (int i = 0; i < kNumPartitions; ++i) {
    std::uint64_t aMask;
    iStream->read(reinterpret_cast<char*>(&aMask), sizeof(aMask));
    if (iStream->gcount() != sizeof(aMask)) {
      throw std::runtime_error("PatternDB File is not valid");
    }
    if (aMask != sMaskPartitions[i]) {
      throw std::runtime_error("PatternDB File has different partitions model");
    }
  }

  for (int i = 0; i < kNumPartitions; ++i) {
    std::uint64_t aSizeTable;
    iStream->read(reinterpret_cast<char*>(&(aSizeTable)), sizeof(aSizeTable));
    if (iStream->gcount() != sizeof(aSizeTable)) {
      throw std::runtime_error("PatternDB File is not valid");
    }
    _costTablePartitions[i].resize(aSizeTable);

    for (std::uint64_t j = 0; j < aSizeTable; ++j) {
      std::uint8_t aCost;
      iStream->read(reinterpret_cast<char*>(&(aCost)), sizeof(aCost));
      if (iStream->gcount() != sizeof(aCost)) {
        throw std::runtime_error("PatternDB File is not valid");
      }
      _costTablePartitions[i][j] = aCost;
    }
  }
}

}  // namespace kpuzzle4

#endif  // KPUZZLE4__PATTERN_DB__HPP
