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
#ifndef KPUZZLE4__DISTANCE_MANHATTAN__HPP
#define KPUZZLE4__DISTANCE_MANHATTAN__HPP
#include "SearchNode.hpp"
#include "State.hpp"

namespace kpuzzle4 {

class DistanceManhattan {
 public:
  using Cost_t = SearchNode::Cost_t;
  static constexpr State kFinalState = State::generateSortedState();

  //! \brief It computes the heuristic cost from two states.
  static Cost_t computeDistance(const State& iStateA, const State& iStateB) noexcept;

  //! \brief It computes the heuristic cost towards the final state.
  static Cost_t computeDistanceWithFinal(const State& iState) noexcept;
};

}  // namespace kpuzzle4

#endif  // KPUZZLE4__DISTANCE_MANHATTAN__HPP
