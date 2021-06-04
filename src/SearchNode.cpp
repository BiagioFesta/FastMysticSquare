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
#include "SearchNode.hpp"

namespace kpuzzle4 {

SearchNode::SearchNode(State iState, Direction iLastMove, Cost_t iCost2Here, int iCounterPath) noexcept
    : _state(std::move(iState)), _lastMove(iLastMove), _cost2Here(iCost2Here), _counterPath(iCounterPath) {}

int SearchNode::moveLeft(SearchNode* oSearchNode, const Mask_t iMask) const noexcept {
  const int aTileMoved = _state.moveLeft(&oSearchNode->_state);

  if (aTileMoved != -1) {
    oSearchNode->_lastMove = Direction::LEFT;
    oSearchNode->_cost2Here = (iMask >> (aTileMoved << 2)) & 0x1 ? _cost2Here + 1 : _cost2Here;
    oSearchNode->_counterPath = _counterPath + 1;
    oSearchNode->_path2Here = _path2Here;
    oSearchNode->_path2Here[_counterPath] = 'L';
  }

  return aTileMoved;
}

int SearchNode::moveRight(SearchNode* oSearchNode, const Mask_t iMask) const noexcept {
  const int aTileMoved = _state.moveRight(&oSearchNode->_state);

  if (aTileMoved != -1) {
    oSearchNode->_lastMove = Direction::RIGHT;
    oSearchNode->_cost2Here = (iMask >> (aTileMoved << 2)) & 0x1 ? _cost2Here + 1 : _cost2Here;
    oSearchNode->_counterPath = _counterPath + 1;
    oSearchNode->_path2Here = _path2Here;
    oSearchNode->_path2Here[_counterPath] = 'R';
  }

  return aTileMoved;
}

int SearchNode::moveDown(SearchNode* oSearchNode, const Mask_t iMask) const noexcept {
  const int aTileMoved = _state.moveDown(&oSearchNode->_state);

  if (aTileMoved != -1) {
    oSearchNode->_lastMove = Direction::DOWN;
    oSearchNode->_cost2Here = (iMask >> (aTileMoved << 2)) & 0x1 ? _cost2Here + 1 : _cost2Here;
    oSearchNode->_counterPath = _counterPath + 1;
    oSearchNode->_path2Here = _path2Here;
    oSearchNode->_path2Here[_counterPath] = 'D';
  }

  return aTileMoved;
}

int SearchNode::moveUp(SearchNode* oSearchNode, const Mask_t iMask) const noexcept {
  const int aTileMoved = _state.moveUp(&oSearchNode->_state);

  if (aTileMoved != -1) {
    oSearchNode->_lastMove = Direction::UP;
    oSearchNode->_cost2Here = (iMask >> (aTileMoved << 2)) & 0x1 ? _cost2Here + 1 : _cost2Here;
    oSearchNode->_counterPath = _counterPath + 1;
    oSearchNode->_path2Here = _path2Here;
    oSearchNode->_path2Here[_counterPath] = 'U';
  }

  return aTileMoved;
}

}  // namespace kpuzzle4
