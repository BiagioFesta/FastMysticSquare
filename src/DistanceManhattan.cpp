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
#include "DistanceManhattan.hpp"
#include <cmath>
#include <type_traits>

namespace kpuzzle4 {

DistanceManhattan::Cost_t DistanceManhattan::computeDistance(const State& iStateA, const State& iStateB) noexcept {
  static_assert(std::is_arithmetic_v<Cost_t>);

  Cost_t aCost = 0;

  for (int i = State::kNumTilesMinusOne; i >= 0; --i) {
    const std::uint64_t aPosTimes4 = i << 2;
    const int aStateATile = (iStateA.getStateConfiguration() >> aPosTimes4) & 0xF;
    const int aStateBTile = (iStateB.getStateConfiguration() >> aPosTimes4) & 0xF;

    if (aStateATile != 0 && aStateATile != aStateBTile) {
      const int aStateAPosition = (iStateA.getTilesPositions() >> (aStateATile << 2)) & 0xF;
      const int aStateBPosition = (iStateB.getTilesPositions() >> (aStateATile << 2)) & 0xF;

      const int aStateAX = aStateAPosition / State::kSize;
      const int aStateBX = aStateBPosition / State::kSize;
      const int aStateAY = aStateAPosition % State::kSize;
      const int aStateBY = aStateBPosition % State::kSize;

      const int aValueX = std::abs(aStateAX - aStateBX);
      const int aValueY = std::abs(aStateAY - aStateBY);

      aCost += (aValueX + aValueY);
    }
  }

  return aCost;
}

DistanceManhattan::Cost_t DistanceManhattan::computeDistanceWithFinal(const State& iState) noexcept {
  return computeDistance(iState, kFinalState);
}

}  // namespace kpuzzle4
