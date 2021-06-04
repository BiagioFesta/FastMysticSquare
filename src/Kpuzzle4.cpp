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
#include "Kpuzzle4.hpp"
#include <array>
#include <chrono>
#include <cxxopts.hpp>
#include <fstream>
#include <future>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include "AlgorithmIDA.hpp"
#include "DistanceManhattan.hpp"

namespace {

//! \brief prints an human readable representation of the state on stdout.
void printState(const kpuzzle4::State& iState) {
  using kpuzzle4::State;

  static constexpr char kSeparator = ',';

  std::cout << '[';
  for (int i = 0; i < State::kNumTiles; ++i) {
    if (i != 0) std::cout << kSeparator;
    std::cout << iState.getValueTileAt(i);
  }
  std::cout << ']';
}

/*! \brief Given a string in the format:
 *       1,2,3,4,...
 *  it returns the array of values.
 */
std::array<int, kpuzzle4::State::kNumTiles> parseValues(const std::string& iStr) {
  using kpuzzle4::State;

  std::array<int, kpuzzle4::State::kNumTiles> aValues;

  std::istringstream aSs(iStr);
  char aSeparator;
  for (int i = 0; i < State::kNumTiles; ++i) {
    if (i != 0) aSs >> aSeparator;
    aSs >> aValues[i];
  }

  return aValues;
}

/*! \brief Given a string it returns the state with that representation.
 *  \note it returns an optional null in case of parsing error.
 */
static std::optional<kpuzzle4::State> parseInitialState(const std::string& iStr) {
  using kpuzzle4::State;

  if (iStr == "RANDOM") {
    return State::generateValidRandState(
        static_cast<std::uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()));
  }

  State aInputState{::parseValues(iStr)};
  if (!aInputState.isValid()) {
    std::cerr << "The given state is not valid.\n";
    return std::nullopt;
  }

  if (!aInputState.isSolveable()) {
    std::cerr << "The given state is not solveable.\n";
    return std::nullopt;
  }

  return aInputState;
}

/*! \brief Given a string it returns the HeuristicType associated with it.
 *  \note it returns an optional null in case of parsing error.
 */
std::optional<kpuzzle4::Kpuzzle4::HeuristicType> parseHeuristicType(const std::string& iStr) {
  using kpuzzle4::Kpuzzle4;

  if (iStr == "MANHATTAN") {
    return Kpuzzle4::HeuristicType::MANHATTAN;
  } else if (iStr == "PATTERN") {
    return Kpuzzle4::HeuristicType::PATTERNS;
  }
  return std::nullopt;
}

//! \brief It prints the solution as sequence of moves.
void printSolutionMoves(const kpuzzle4::AlgorithmIDA& iAlgorithmIDA) {
  std::cout << '[';

  for (int i = 0; i < iAlgorithmIDA.getSolutionLength(); ++i) {
    if (i != 0) std::cout << ',';
    std::cout << iAlgorithmIDA.getSolutionPath()[i];
  }

  std::cout << ']';
}

//! \brief Ir prints all state as sequence of the solution.
void printSolutionStates(const kpuzzle4::AlgorithmIDA& iAlgorithmIDA, kpuzzle4::State* ioState) {
  std::cout << "--- Solution States ---\n";
  ::printState(*ioState);
  std::cout << '\n';

  for (int i = 0; i < iAlgorithmIDA.getSolutionLength(); ++i) {
    switch (iAlgorithmIDA.getSolutionPath()[i]) {
      case 'L':
        ioState->moveLeft(ioState);
        break;
      case 'R':
        ioState->moveRight(ioState);
        break;
      case 'U':
        ioState->moveUp(ioState);
        break;
      case 'D':
        ioState->moveDown(ioState);
        break;
    }
    ::printState(*ioState);
    std::cout << '\n';
  }

  std::cout << "-----------------------\n";
}

}  // anonymous namespace

namespace kpuzzle4 {

Kpuzzle4::OptionParsed Kpuzzle4::parseCommandLine(int argc, char* argv[]) {
  using cxxopts::Options;

  OptionParsed aOptionParsed;

  Options aOptions(kProgramName, "");
  aOptions.add_option("", "h", "help", "Display this help message.", ::cxxopts::value<bool>(), "");
  aOptions.add_option("",
                      "a",
                      "algorithm",
                      "Select the heuristic algorithm to use.",
                      ::cxxopts::value<std::string>(),
                      "{MANHATTAN|PATTERN}");
  aOptions.add_option("",
                      "s",
                      "state",
                      "Select the initial state of the problem.",
                      ::cxxopts::value<std::string>(),
                      "{RANDOM|0,1,2,3,...}");
  aOptions.add_option("", "i", "interactive", "Enables the interactive mode.", ::cxxopts::value<bool>(), "");

  try {
    auto aParseResult = aOptions.parse(argc, argv);

    if (aParseResult.count("help")) {
      std::cout << aOptions.help({"", "Solver"}) << "\n";
      std::exit(0);
    }

    if (aParseResult.count("algorithm") == 0) {
      std::cerr << "--algorithm option is mandatory.\n";
      std::exit(-1);
    } else if (auto aHeuristicType = parseHeuristicType(aParseResult["algorithm"].as<std::string>())) {
      aOptionParsed._heuristicType = *aHeuristicType;
    } else {
      std::cerr << "ALG_TYPE can be: 'MANHATTAN' or 'PATTERN'.\n";
      std::exit(-1);
    }

    if (aParseResult.count("state") == 0) {
      std::cerr << "--state option is mandatory.\n";
      std::exit(-1);
    } else if (auto aInitialState = parseInitialState(aParseResult["state"].as<std::string>())) {
      aOptionParsed._initialState = *aInitialState;
    } else {
      std::cerr << "STATE can be: {RANDOM|0,1,2,3,...}\n";
      std::exit(-1);
    }

    aOptionParsed._interactive = aParseResult.count("interactive");
  } catch (const cxxopts::OptionException& aError) {
    std::cerr << aError.what() << ".\n";
    std::exit(-1);
  }

  return aOptionParsed;
}

Kpuzzle4::HeuristicFnHandler_t Kpuzzle4::getHeuristicHandler(const HeuristicType iHeuristicType) const {
  switch (iHeuristicType) {
    case HeuristicType::MANHATTAN:
      return DistanceManhattan::computeDistanceWithFinal;
      break;
    case HeuristicType::PATTERNS:
      return std::bind(&PatternDB_t::getCost, _patternDB, std::placeholders::_1);
      break;
  }

  throw std::runtime_error("Heuristic function not recognized!");
}

bool Kpuzzle4::solveProblem(const State& iInitialState,
                            const HeuristicFnHandler_t& iHeuristicFnHandler,
                            const bool iInteractive,
                            AlgorithmIDA* oAlgorithmIDA) {
  if constexpr (sizeof(void*) < sizeof(std::uint64_t)) {
    std::cout << "[Warning]: No 64bit Architecture detected.\n";
  }

  const auto aStatsPrinter = [oAlgorithmIDA]() {
    std::cout << "\rNode Explored: " << oAlgorithmIDA->getExploredNodes()
              << " | Current MaxDepth: " << oAlgorithmIDA->getCurrentMaxDepth();
    std::cout.flush();
  };

  auto aSolverStatus = std::async(iInteractive ? std::launch::async : std::launch::deferred,
                                  &AlgorithmIDA::findSolution<const HeuristicFnHandler_t&>,
                                  oAlgorithmIDA,
                                  std::ref(iInitialState),
                                  std::ref(iHeuristicFnHandler));

  std::cout << "Initial State: ";
  ::printState(iInitialState);
  std::cout << '\n';

  if (iInteractive == false) {
    aSolverStatus.wait();
  } else {
    do {
      aStatsPrinter();
    } while (aSolverStatus.wait_for(kRefreshScreenPeriod) != std::future_status::ready);
  }

  aStatsPrinter();

  const AlgorithmIDA::SolverResult_t aResult = aSolverStatus.get();

  std::cout << "\nTime Elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(aResult._timeElapsed).count()
            << " [ms]\n";

  return aResult._solutionFound;
}

void Kpuzzle4::printSolutionDetails(const AlgorithmIDA& iAlgorithmIDA, State iInitialState) {
  std::cout << "No. Moves Optional Solution: " << iAlgorithmIDA.getSolutionLength() << '\n';
  std::cout << "Optional Solution Moves: ";
  ::printSolutionMoves(iAlgorithmIDA);
  std::cout << '\n';
  std::cout << '\n';

  ::printSolutionStates(iAlgorithmIDA, &iInitialState);
}

int Kpuzzle4::run(int argc, char* argv[]) {
  const auto aOptionParsed = parseCommandLine(argc, argv);

  if (aOptionParsed._heuristicType == HeuristicType::PATTERNS) {
    initializePatternDB();
  }

  const auto aHeuristicFunction = getHeuristicHandler(aOptionParsed._heuristicType);

  AlgorithmIDA aAlgorithmIDA;
  const auto aSolutionFound =
      solveProblem(aOptionParsed._initialState, aHeuristicFunction, aOptionParsed._interactive, &aAlgorithmIDA);

  if (!aSolutionFound) {
    std::cout << "Solution not found\n";
    return -1;
  }

  std::cout << "Found Solution: true\n";
  printSolutionDetails(aAlgorithmIDA, aOptionParsed._initialState);

  return 0;
}

void Kpuzzle4::initializePatternDB() {
  std::ifstream aFile(kFileNamePatternDB, std::ios_base::binary);
  if (aFile.fail()) {
    std::cout << "Generating Patterns Database...\n";
    _patternDB.generate();
    savePatternDBOnFile(kFileNamePatternDB);
    std::cout << "Done\n";
  } else {
    std::cout << "Load Patterns Database...\n";
    _patternDB.deserialize(&aFile);
    std::cout << "Done\n";
  }
}

void Kpuzzle4::savePatternDBOnFile(const char* iFileName) const {
  std::ofstream oFile(iFileName, std::ios_base::binary);
  if (oFile.fail()) {
    throw std::runtime_error("Cannot save the Pattern Database");
  }
  _patternDB.serialize(&oFile);
}

}  // namespace kpuzzle4

int main(int argc, char* argv[]) {
  using kpuzzle4::Kpuzzle4;

  return Kpuzzle4{}.run(argc, argv);
}
