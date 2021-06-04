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
#ifndef KPUZZLE4__STATE__HPP
#define KPUZZLE4__STATE__HPP
#include <array>
#include <cassert>
#include <cstdint>

namespace kpuzzle4 {

class State {
 public:
  static constexpr int kSize = 4;  // Row Size
  static constexpr int kNumTiles = kSize * kSize;
  static constexpr int kNumTilesMinusOne = kNumTiles - 1;
  static constexpr int kValueSpaceTile = 0x0;

  using StateConfiguration_t = std::uint64_t;  // State Configuration
  using Mask_t = std::uint64_t;

  //! \brief Default Constructor.
  State() = default;

  //! \brief Constructs from a Configuration.
  constexpr State(StateConfiguration_t iStateConfiguration) noexcept;

  /*! \brief Constructs from an array of values.
   *  \note It does not check whether the created state will be solveable or
   *  valid.
   */
  explicit constexpr State(const std::array<int, kNumTiles>& iVectorValue) noexcept;

  //! \brief "Not equal" operator.
  constexpr bool operator!=(const State& iOthState) const noexcept;

  //! \brief "Equal" operator.
  constexpr bool operator==(const State& iOthState) const noexcept;

  //! \return whether the state is solveable or not.
  constexpr bool isSolveable() const noexcept;

  /*! \return whether the state is valid or not.
   *  \note a 'valid' state is the one which has all valid value tiles.
   */
  constexpr bool isValid() const noexcept;

  /*! \brief Tries to apply an action, moving the "Space" tile on the left.
   *  \param [out] oState   The output state if the movement is possible.
   *  \return The value of the tile swapped with the "Space" tile, otherise it
   *  returns -1.
   */
  int moveLeft(State* oState) const noexcept;

  /*! \brief Tries to apply an action, moving the "Space" tile on the right.
   *  \param [out] oState   The output state if the movement is possible.
   *  \return The value of the tile swapped with the "Space" tile, otherise it
   *  returns -1.
   */
  int moveRight(State* oState) const noexcept;

  /*! \brief Tries to apply an action, moving the "Space" tile toward down.
   *  \param [out] oState   The output state if the movement is possible.
   *  \return The value of the tile swapped with the "Space" tile, otherise it
   *  returns -1.
   */
  int moveDown(State* oState) const noexcept;

  /*! \brief Tries to apply an action, moving the "Space" tile torwad up.
   *  \param [out] oState   The output state if the movement is possible.
   *  \return The value of the tile swapped with the "Space" tile, otherise it
   *  returns -1.
   */
  int moveUp(State* oState) const noexcept;

  //! \return the configuration of the state.
  constexpr StateConfiguration_t getStateConfiguration() const noexcept;

  //! \return the index where the "Space" tile is locate.
  constexpr int getIndexSpace() const noexcept;

  //! \return the value of the tile at the specified index.
  constexpr int getValueTileAt(const int iIndex) const noexcept;

  //! \return a BitMask which specifies the index for each tile.
  constexpr std::uint64_t getTilesPositions() const noexcept;

  //! \return the Hash of the state applying a mask.
  constexpr std::uint64_t getHashWithMask(const Mask_t iMask) const noexcept;

  //! \brief Generates a sorted state.
  static constexpr State generateSortedState() noexcept;

  //! \brief Generates a random state (valid = solveable).
  static State generateValidRandState(const std::uint64_t iSeed) noexcept;

 private:
  StateConfiguration_t _data;
  int _indexSpace;
  std::uint64_t _tilesPositions;

  /*! \brief Compute the BitMask which specifies the index (position) for each
   *  tile in the input configuration.
   */
  static constexpr std::uint64_t computePositionFromConfiguration(StateConfiguration_t iState) noexcept;

  //! \return the index of the "Space" tile given an input configuration.
  static constexpr int findIndexSpace(StateConfiguration_t iState) noexcept;
};

constexpr State::State(StateConfiguration_t iStateConfiguration) noexcept
    : _data(iStateConfiguration),
      _indexSpace(findIndexSpace(iStateConfiguration)),
      _tilesPositions(computePositionFromConfiguration(iStateConfiguration)) {}

constexpr std::uint64_t State::computePositionFromConfiguration(StateConfiguration_t iState) noexcept {
  std::uint64_t aTilesPositions = 0;

  for (int i = kNumTilesMinusOne; i >= 0; --i) {
    const std::uint64_t aTile = (iState >> (i << 2)) & 0xF;
    aTilesPositions |= (static_cast<std::uint64_t>(i) << (aTile << 2));
  }

  return aTilesPositions;
}

constexpr int State::findIndexSpace(StateConfiguration_t iState) noexcept {
  for (int i = kNumTilesMinusOne; i >= 0; --i) {
    if (((iState >> (i << 2)) & 0xF) == kValueSpaceTile) {
      return i;
    }
  }

  return -1;
}

constexpr State State::generateSortedState() noexcept {
  constexpr StateConfiguration_t kSortedConfiguration = 0x0fedcba987654321;
  return State{kSortedConfiguration};
}

constexpr State::StateConfiguration_t State::getStateConfiguration() const noexcept {
  return _data;
}

constexpr int State::getIndexSpace() const noexcept {
  return _indexSpace;
}

constexpr std::uint64_t State::getTilesPositions() const noexcept {
  return _tilesPositions;
}

constexpr bool State::operator!=(const State& iOthState) const noexcept {
  return _data != iOthState._data;
}

constexpr bool State::operator==(const State& iOthState) const noexcept {
  return _data == iOthState._data;
}

constexpr State::State(const std::array<int, kNumTiles>& iVectorValue) noexcept
    : _data(0), _indexSpace(0), _tilesPositions(0) {
  for (int i = 0; i < kNumTiles; ++i) {
    _data |= static_cast<std::uint64_t>(iVectorValue[i]) << (i << 2);
  }

  _indexSpace = findIndexSpace(_data);
  _tilesPositions = computePositionFromConfiguration(_data);
}

constexpr bool State::isSolveable() const noexcept {
  assert(isValid() == true);

  int aInversionCounter = 0;

  for (int i = 0; i < kNumTiles; ++i) {
    const int aTileI = getValueTileAt(i);

    if (aTileI != kValueSpaceTile) {
      for (int j = i + 1; j < kNumTiles; ++j) {
        const int aTileJ = getValueTileAt(j);
        if (aTileJ != kValueSpaceTile && aTileJ < aTileI) ++aInversionCounter;
      }
    } else {
      aInversionCounter += 1 + i / kSize;
    }
  }

  return aInversionCounter & 0x1 ? false : true;
}

constexpr int State::getValueTileAt(const int iIndex) const noexcept {
  return static_cast<int>((_data >> (iIndex << 2)) & 0xf);
}

constexpr bool State::isValid() const noexcept {
  static_assert(kNumTiles <= 64);

  std::uint64_t aValuesCheck = 0x0;
  for (int i = 0; i < kNumTiles; ++i) {
    const int aValue = getValueTileAt(i);
    if (aValue > kNumTilesMinusOne) return false;

    if ((aValuesCheck >> aValue) & 0x1) {
      return false;
    }
    aValuesCheck |= static_cast<std::uint64_t>(0x1) << aValue;
  }

  return true;
}

constexpr std::uint64_t State::getHashWithMask(const Mask_t iMask) const noexcept {
  return _tilesPositions & iMask;
}

}  // namespace kpuzzle4

#endif  // KPUZZLE4__STATE__HPP
