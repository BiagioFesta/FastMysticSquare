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
#include "State.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <random>

namespace kpuzzle4 {

int State::moveLeft(State* oState) const noexcept {
  if (_indexSpace % kSize == 0) return -1;

  oState->_data = _data;

  const int aPosTimes4 = _indexSpace << 2;
  const int aPosMinusOneTimes4 = (_indexSpace - 1) << 2;
  const std::uint64_t aSpace = (oState->_data >> aPosTimes4) & 0xF;
  const std::uint64_t aTile = (oState->_data >> aPosMinusOneTimes4) & 0xF;

  const std::uint64_t zeroBitTile = static_cast<std::uint64_t>(0xF) << aPosMinusOneTimes4;
  oState->_data &= ~zeroBitTile;
  oState->_data |= aTile << aPosTimes4;
  oState->_data |= aSpace << aPosMinusOneTimes4;
  oState->_indexSpace = _indexSpace - 1;
  oState->_tilesPositions = computePositionFromConfiguration(oState->_data);

  return static_cast<int>(aTile);
}

int State::moveRight(State* oState) const noexcept {
  const int aIndexSpacePlusOne = _indexSpace + 1;
  if (aIndexSpacePlusOne % kSize == 0) return -1;

  oState->_data = _data;

  const int aPosTimes4 = _indexSpace << 2;
  const int aPosPlusOneTimes4 = aIndexSpacePlusOne << 2;
  const std::uint64_t aSpace = (oState->_data >> aPosTimes4) & 0xF;
  const std::uint64_t aTile = (oState->_data >> aPosPlusOneTimes4) & 0xF;

  const std::uint64_t zeroBitTile = static_cast<std::uint64_t>(0xF) << aPosPlusOneTimes4;
  oState->_data &= ~zeroBitTile;
  oState->_data |= aTile << aPosTimes4;
  oState->_data |= aSpace << aPosPlusOneTimes4;
  oState->_indexSpace = aIndexSpacePlusOne;
  oState->_tilesPositions = computePositionFromConfiguration(oState->_data);

  return static_cast<int>(aTile);
}

int State::moveUp(State* oState) const noexcept {
  if (_indexSpace < kSize) return -1;

  oState->_data = _data;

  const int aPosTimes4 = _indexSpace << 2;
  const int aPosMinusDimTimes4 = (_indexSpace - kSize) << 2;
  const std::uint64_t aSpace = (oState->_data >> aPosTimes4) & 0xF;
  const std::uint64_t aTile = (oState->_data >> aPosMinusDimTimes4) & 0xF;

  const std::uint64_t zeroBitTile = static_cast<std::uint64_t>(0xF) << aPosMinusDimTimes4;
  oState->_data &= ~zeroBitTile;
  oState->_data |= aTile << aPosTimes4;
  oState->_data |= aSpace << aPosMinusDimTimes4;
  oState->_indexSpace = _indexSpace - kSize;
  oState->_tilesPositions = computePositionFromConfiguration(oState->_data);

  return static_cast<int>(aTile);
}

int State::moveDown(State* oState) const noexcept {
  if (_indexSpace >= kNumTiles - kSize) return -1;

  oState->_data = _data;

  const int aPosTimes4 = _indexSpace << 2;
  const int aPosPlusDimTimes4 = (_indexSpace + kSize) << 2;
  const std::uint64_t aSpace = (oState->_data >> aPosTimes4) & 0xF;
  const std::uint64_t aTile = (oState->_data >> aPosPlusDimTimes4) & 0xF;

  const std::uint64_t zeroBitTile = (std::uint64_t)0xF << aPosPlusDimTimes4;
  oState->_data &= ~zeroBitTile;
  oState->_data |= aTile << aPosTimes4;
  oState->_data |= aSpace << aPosPlusDimTimes4;
  oState->_indexSpace = _indexSpace + kSize;
  oState->_tilesPositions = computePositionFromConfiguration(oState->_data);

  return static_cast<int>(aTile);
}

State State::generateValidRandState(const std::uint64_t iSeed) noexcept {
  using RndEngine_t = std::mt19937_64;

  std::array<int, kNumTiles> aValuesTiles;
  for (int i = 0; i < kNumTiles; ++i) {
    aValuesTiles[i] = i;
  }

  std::shuffle(aValuesTiles.begin(), aValuesTiles.end(), RndEngine_t{iSeed});

  State aRndState{aValuesTiles};

  if (!aRndState.isSolveable()) {
    if (aValuesTiles[0] != kValueSpaceTile && aValuesTiles[1] != kValueSpaceTile) {
      std::swap(aValuesTiles[0], aValuesTiles[1]);

    } else {
      std::swap(aValuesTiles[2], aValuesTiles[3]);
    }

    aRndState = State{aValuesTiles};
  }

  assert(aRndState.isSolveable());
  assert(aRndState.isValid());
  return aRndState;
}

}  // namespace kpuzzle4
