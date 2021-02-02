#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <paradiseo/eo/eo>
#include <paradiseo/mo/mo>

#include "flowshop-solver/heuristics/FSPOrderHeuristics.hpp"
#include "flowshop-solver/heuristics/InsertionStrategy.hpp"
#include "flowshop-solver/FSPProblemFactory.hpp"
#include "flowshop-solver/MHParamsSpecsFactory.hpp"
#include "flowshop-solver/heuristics/InsertionStrategy.hpp"

std::vector<double> enumerateAll(
    const std::unordered_map<std::string, std::string>& prob_params) {
  std::vector<double> res;

  using ProblemTp = FSPProblem;
  ProblemTp problem = FSPProblemFactory::get(prob_params);
  auto& fullEval = problem.eval();
  using EOT = ProblemTp::EOT;

  int n = problem.size();
  EOT sol(n);
  std::iota(sol.begin(), sol.end(), 0);
  long no_solutions = factorial(n);
  res.reserve(no_solutions);

  for (int i = 0; i < no_solutions; i++) {
    fullEval(sol);
    res.emplace_back(sol.fitness());
    std::next_permutation(sol.begin(), sol.end());
    sol.invalidate();
  }

  return res;
}

template <class Ngh, class EOT = typename Ngh::EOT>
std::vector<EOT> enumerateAllSolutions(Problem<Ngh>& problem) {
  const int n = problem.size();
  const long no_solutions = factorial(n);
  auto& fullEval = problem.eval();

  std::vector<EOT> solutions;
  solutions.reserve(no_solutions);

  EOT sol(n);
  std::iota(sol.begin(), sol.end(), 0);
  for (int i = 0; i < no_solutions; i++) {
    fullEval(sol);
    solutions.emplace_back(sol);
    std::next_permutation(sol.begin(), sol.end());
    sol.invalidate();
  }

  return solutions;
}

