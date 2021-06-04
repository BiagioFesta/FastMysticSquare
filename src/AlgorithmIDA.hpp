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
#ifndef KPUZZLE4__ALGORITHM_IDA__HPP
#define KPUZZLE4__ALGORITHM_IDA__HPP
#include <chrono>
#include <stack>
#include <utility>
#include "SearchNode.hpp"
#include "State.hpp"

namespace kpuzzle4 {

class AlgorithmIDA {
 public:
  /*! \brief For the 15-puzzle, lengths of optimal solutions range from 0 to 80
   *  single-tile moves.
   */
  static constexpr int kTotalDepthLimit = SearchNode::kMaxPath;
  using Duration_t = std::chrono::milliseconds;
  using Clock_t = std::chrono::steady_clock;

  using Path_t = SearchNode::Path_t;

  struct SolverResult_t {
    bool _solutionFound;
    Duration_t _timeElapsed;

    SolverResult_t() = default;
    SolverResult_t(bool iSolutionFound, Duration_t iTimeElapsed);
  };

  /*! \brief Find a solution to the kpuzzle4 problem with IDA Algorithm.
   *  \param [in] iStartingState   The initial state of the problem.
   *  \param [in] iHeuristicFn     The heuristic function which compute the cost
   *                               to the final state given a internal state.
   *  \return true if a solution has been found.
   *  \template HeuristicFn is a function type with signature: int(const
   *  State&).
   */
  template <typename HeuristicFn>
  SolverResult_t findSolution(const State& iStartingState, HeuristicFn&& iHeuristicFn);

  //! \return the current MaxDepth in the DFS exploration.
  int getCurrentMaxDepth() const noexcept {
    return _maxCurrentDepth;
  }

  //! \return the number of nodes explored during DFS exploration.
  long long getExploredNodes() const noexcept {
    return _nodeExplored;
  }

  //! \return the length of the solution path.
  int getSolutionLength() const noexcept {
    return _solutionLengthPath;
  }

  //! \retrun the solution path.
  const Path_t& getSolutionPath() const noexcept {
    return _solutionPath;
  }

 private:
  static_assert(kTotalDepthLimit <= SearchNode::kMaxPath);

  int _maxCurrentDepth = 0;
  long long _nodeExplored = 0ll;
  int _solutionLengthPath = 0;
  Path_t _solutionPath;

  template <typename HeuristicFn>
  bool limitedDepthSearch(const State& iStartingState, HeuristicFn&& iHeuristicFn);
};

template <typename HeuristicFn>
AlgorithmIDA::SolverResult_t AlgorithmIDA::findSolution(const State& iStartingState, HeuristicFn&& iHeuristicFn) {
  const auto aTimeStart = Clock_t::now();

  _maxCurrentDepth = iHeuristicFn(iStartingState);
  _nodeExplored = 0ll;
  _solutionLengthPath = 0;
  bool aSolutionFound = false;

  while (_maxCurrentDepth <= kTotalDepthLimit && aSolutionFound == false) {
    aSolutionFound = limitedDepthSearch(iStartingState, std::forward<HeuristicFn>(iHeuristicFn));

    if (aSolutionFound == false) {
      _maxCurrentDepth += 2;
    }
  }

  const auto aTimeStop = Clock_t::now();

  return {aSolutionFound, std::chrono::duration_cast<Duration_t>(aTimeStop - aTimeStart)};
}

template <typename HeuristicFn>
bool AlgorithmIDA::limitedDepthSearch(const State& iStartingState, HeuristicFn&& iHeuristicFn) {
  using OpenList = std::stack<SearchNode>;
  static constexpr State kFinalState = State::generateSortedState();

  OpenList aOpenList;
  aOpenList.emplace(iStartingState);

  while (!aOpenList.empty()) {
    ++_nodeExplored;
    const SearchNode aCurrentNode = std::move(aOpenList.top());
    aOpenList.pop();

    if (aCurrentNode.getState() == kFinalState) {
      _solutionLengthPath = aCurrentNode.getCounterPath();
      _solutionPath = aCurrentNode.getPath2Here();
      return true;
    }

    const int aHeuristicCost = iHeuristicFn(aCurrentNode.getState());
    const int aCostHere = aCurrentNode.getCost2Here();

    if (aCostHere + aHeuristicCost <= _maxCurrentDepth) {
      const auto aLastMove = aCurrentNode.getLastMove();

      SearchNode aChildNode;

      if (aLastMove != SearchNode::Direction::RIGHT && aCurrentNode.moveLeft(&aChildNode) != -1) {
        aOpenList.push(std::move(aChildNode));
      }
      if (aLastMove != SearchNode::Direction::LEFT && aCurrentNode.moveRight(&aChildNode) != -1) {
        aOpenList.push(std::move(aChildNode));
      }
      if (aLastMove != SearchNode::Direction::UP && aCurrentNode.moveDown(&aChildNode) != -1) {
        aOpenList.push(std::move(aChildNode));
      }
      if (aLastMove != SearchNode::Direction::DOWN && aCurrentNode.moveUp(&aChildNode) != -1) {
        aOpenList.push(std::move(aChildNode));
      }
    }
  }

  return false;
}

AlgorithmIDA::SolverResult_t::SolverResult_t(bool iSolutionFound, Duration_t iTimeElapsed)
    : _solutionFound(iSolutionFound), _timeElapsed(std::move(iTimeElapsed)) {}

}  // namespace kpuzzle4

#endif  // KPUZZLE4__ALGORITHM_IDA__HPP
