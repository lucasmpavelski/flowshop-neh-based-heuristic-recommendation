#pragma once

#include <paradiseo/mo/mo>
#include <unordered_map>

#include "flowshop-solver/continuators/myTimeStat.hpp"
#include "flowshop-solver/heuristics.hpp"
#include "flowshop-solver/heuristics/neh.hpp"

#include "flowshop-solver/RunOptions.hpp"
#include "flowshop-solver/eoFSPFactory.hpp"


inline auto solveWith(
    std::string mh,
    const std::unordered_map<std::string, std::string>& problem_specs,
    const std::unordered_map<std::string, std::string>& params_values,
    const RunOptions runOptions = RunOptions()) -> Result {
  FSPProblem prob = FSPProblemFactory::get(problem_specs);
  MHParamsSpecs specs = MHParamsSpecsFactory::get(mh);
  MHParamsValues params(&specs);
  params.readValues(params_values);

  eoFSPFactory factory{params, prob};

  if (mh == "all") {
    mh = params.categoricalName("MH");
  }

  if (mh == "NEH")
    return solveWithNEH(prob, factory, runOptions);
  else
    throw std::runtime_error("Unknown MH: " + mh);
  return {};
}