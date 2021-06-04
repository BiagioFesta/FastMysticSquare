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
#ifndef KPUZZLE4__SEARCH_NODE__HPP
#define KPUZZLE4__SEARCH_NODE__HPP
#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include "State.hpp"

namespace kpuzzle4 {

class SearchNode {
 public:
  using Cost_t = std::int8_t;
  using Mask_t = State::Mask_t;

  static constexpr Mask_t kNoMask = 0xFFFFFFFFFFFFFFFF;
  static constexpr int kMaxPath = 80;

  enum class Direction { NONE, LEFT, RIGHT, DOWN, UP };

  using Path_t = std::array<char, kMaxPath>;

  //! \brief Default constructor.
  SearchNode() = default;

  //! \brief Constructs the node starting from an initial state.
  constexpr SearchNode(State iState) noexcept;

  /*! \brief Constructs the node from a intermediate state.
   *  \param [in]   The state to be stored in the node.
   *  \param [in]   The last moved done in order to obtain this new node.
   *  \param [in]   The cost in order to reach this new node.
   *  \param [in]   The length (counter) of the path so far.
   */
  SearchNode(State iState, Direction iLastMove, Cost_t iCost2Here, int iCounterPath) noexcept;

  //! \return the state stored in the node.
  constexpr const State& getState() const noexcept {
    return _state;
  }

  //! \return the last moved done to reach the node.
  constexpr Direction getLastMove() const noexcept {
    return _lastMove;
  }

  //! \return the cost to reach the node.
  constexpr Cost_t getCost2Here() const noexcept {
    return _cost2Here;
  }

  //! \return the length of the path so far.
  constexpr int getCounterPath() const noexcept {
    return _counterPath;
  }

  //! \return the path (moves) to reath the node.
  constexpr const Path_t& getPath2Here() const noexcept {
    return _path2Here;
  };

  /*! \brief Computes the hash of the of node (its state representation).
   *  The hash is conditioned by the mask.
   *  In other workds, the tiles of the state not enabled by the mask are not
   *  counted in the hash computation.
   */
  constexpr std::uint64_t getHashWithMask(const Mask_t iMask) const noexcept;

  /*! \brief Tries to apply an action, moving the "Space" tile on the left.
   *  \param [out] oSearchNode   The output node if the movement is possible.
   *  \param [in]  iMask         The mask of tiles to be considered for the
   *                             cost.
   *  \note The mask is specified for each tile. If the tile is enabled (0xF)
   *  then moving that tile will increase the cost. If the tile is disabled
   *  (0x0) the moving that tile will keep the same cost.
   *  \return The value of the tile swapped with the "Space" tile, otherise it
   *  returns -1.
   */
  int moveLeft(SearchNode* oSearchNode, const Mask_t iMask = kNoMask) const noexcept;

  /*! \brief Tries to apply an action, moving the "Space" tile on the right.
   *  \param [out] oSearchNode   The output node if the movement is possible.
   *  \param [in]  iMask         The mask of tiles to be considered for the
   *                             cost.
   *  \note The mask is specified for each tile. If the tile is enabled (0xF)
   *  then moving that tile will increase the cost. If the tile is disabled
   *  (0x0) the moving that tile will keep the same cost.
   *  \return The value of the tile swapped with the "Space" tile, otherise it
   *  returns -1.
   */
  int moveRight(SearchNode* oSearchNode, const Mask_t iMask = kNoMask) const noexcept;

  /*! \brief Tries to apply an action, moving the "Space" tile towards up.
   *  \param [out] oSearchNode   The output node if the movement is possible.
   *  \param [in]  iMask         The mask of tiles to be considered for the
   *                             cost.
   *  \note The mask is specified for each tile. If the tile is enabled (0xF)
   *  then moving that tile will increase the cost. If the tile is disabled
   *  (0x0) the moving that tile will keep the same cost.
   *  \return The value of the tile swapped with the "Space" tile, otherise it
   *  returns -1.
   */
  int moveDown(SearchNode* oSearchNode, const Mask_t iMask = kNoMask) const noexcept;

  /*! \brief Tries to apply an action, moving the "Space" tile to the bottom.
   *  \param [out] oSearchNode   The output node if the movement is possible.
   *  \param [in]  iMask         The mask of tiles to be considered for the
   *                             cost.
   *  \note The mask is specified for each tile. If the tile is enabled (0xF)
   *  then moving that tile will increase the cost. If the tile is disabled
   *  (0x0) the moving that tile will keep the same cost.
   *  \return The value of the tile swapped with the "Space" tile, otherise it
   *  returns -1.
   */
  int moveUp(SearchNode* oSearchNode, const Mask_t iMask = kNoMask) const noexcept;

 private:
  static_assert(std::is_arithmetic_v<Cost_t>);
  static_assert(kMaxPath <= std::numeric_limits<Cost_t>::max());

  State _state;
  Direction _lastMove;
  Cost_t _cost2Here;
  int _counterPath;
  Path_t _path2Here;
};

constexpr SearchNode::SearchNode(State iState) noexcept
    : _state(std::move(iState)), _lastMove(Direction::NONE), _cost2Here(0), _counterPath(0), _path2Here() {}

constexpr std::uint64_t SearchNode::getHashWithMask(const Mask_t iMask) const noexcept {
  return _state.getHashWithMask(iMask);
}

}  // namespace kpuzzle4

#endif  // KPUZZLE4__SEARCH_NODE__HPP
