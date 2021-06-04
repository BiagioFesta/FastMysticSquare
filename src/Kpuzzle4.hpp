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
#ifndef KPUZZLE4__KPUZZLE4__HPP
#define KPUZZLE4__KPUZZLE4__HPP
#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include "AlgorithmIDA.hpp"
#include "PatternDB.hpp"
#include "SearchNode.hpp"
#include "State.hpp"

namespace kpuzzle4 {

class Kpuzzle4 {
 public:
  enum class HeuristicType { MANHATTAN, PATTERNS };

  int run(int argc, char* argv[]);

 private:
  using HeuristicFnHandler_t = std::function<SearchNode::Cost_t(const State&)>;
  static constexpr std::array<State::Mask_t, 3> kMasksPattern = {0xFFFFF0000000000F,
                                                                 0x00000FFFFF00000F,
                                                                 0x0000000000FFFFFF};
  using PatternDB_t = PatternDB<std::get<0>(kMasksPattern), std::get<1>(kMasksPattern), std::get<2>(kMasksPattern)>;
  static_assert(PatternDB_t::isValidPartitions());

  PatternDB_t _patternDB;

  static constexpr const char* kProgramName = "kpuzzle4";
  static constexpr const char* kFileNamePatternDB = "patternDB.data";
  static constexpr auto kRefreshScreenPeriod = std::chrono::milliseconds(200);

  struct OptionParsed {
    HeuristicType _heuristicType;
    State _initialState;
    bool _interactive;
  };

  //! \return the proper function handler in accordance with the input.
  HeuristicFnHandler_t getHeuristicHandler(const HeuristicType iHeuristicType) const;

  void initializePatternDB();

  void savePatternDBOnFile(const char* iFileName) const;

  /*! \brief Solve the problem with AlgorithmIDA.
   *  \note This function will print information on the standard output.
   *  \return true if the optimal solution has been found.
   */
  static bool solveProblem(const State& iInitialState,
                           const HeuristicFnHandler_t& iHeuristicFHnandler,
                           const bool iInteractive,
                           AlgorithmIDA* oAlgorithmIDA);

  /*! \brief After the AlgorithmIDA has found a solution, this function prints
   * on the standard output the details of the solution itself.
   */
  static void printSolutionDetails(const AlgorithmIDA& iAlgorithmIDA, State iInitialState);

  /*! \brief It parses the command line and generate the options.
   *  This function will invoke `std::exit` in case of error.
   *  This function will prints help information.
   *  \return the options parsed.
   */
  static OptionParsed parseCommandLine(int argc, char* argv[]);
};

}  // namespace kpuzzle4

#endif  // KPUZZLE4__KPUZZLE4__HPP
