#pragma once

#include <paradiseo/eo/eo>
#include <paradiseo/mo/mo>

#include "flowshop-solver/global.hpp"

#include "flowshop-solver/eoFactory.hpp"

#include "flowshop-solver/MHParamsValues.hpp"

#include "flowshop-solver/problems/FSP.hpp"
#include "flowshop-solver/problems/FSPProblem.hpp"

#include "flowshop-solver/heuristics/NEH.hpp"
#include "flowshop-solver/heuristics/AppendingNEH.hpp"
#include "flowshop-solver/heuristics/FSPOrderHeuristics.hpp"
#include "flowshop-solver/heuristics/InsertionStrategy.hpp"

class eoFSPFactory : public eoFactory<FSPProblem::Ngh> {
  FSPProblem& _problem;

 public:
  eoFSPFactory(const MHParamsValues& params, FSPProblem& problem)
      : eoFactory<FSPProblem::Ngh>{params, problem}, _problem{problem} {};

  using EOT = FSP;
  using Ngh = FSPNeighbor;

 protected:

  auto buildInsertion(const std::string name) -> InsertionStrategy<Ngh>* {
    auto& neval = _problem.neighborEval();
    auto insertUPtr = buildInsertionStrategy<Ngh>(name, neval);
    if (insertUPtr.get() == nullptr) {
      insertUPtr = buildInsertionStrategyFSP(name, neval, _problem.data());
    }
    if (insertUPtr.get() == nullptr) {
      return nullptr;
    } else {
      InsertionStrategy<Ngh>* nehInsert = insertUPtr.release();
      storeFunctor(nehInsert);
      return nehInsert;
    }
  }

  auto domainInit() -> eoInit<EOT>* override {
    const std::string name = categoricalName(".Init");
    if (name == "neh") {
      const auto ratioStr = categoricalName(".Init.NEH.Ratio");
      const auto ratio = std::stod(ratioStr);

      eoInit<EOT>* firstOrder = nullptr;

      if (ratio > 0.0) {
        auto firstPriority = categoricalName(".Init.NEH.First.Priority");
        auto firstPriorityWeighted =
            categoricalName(".Init.NEH.First.PriorityWeighted") == "yes";
        auto firstPriorityOrder =
            categoricalName(".Init.NEH.First.PriorityOrder");

        firstOrder = buildPriority(_problem.data(), firstPriority,
                                   firstPriorityWeighted, firstPriorityOrder)
                         .release();
        storeFunctor(firstOrder);
        if (ratio == 1.0)
          return firstOrder;
      }

      if (ratio < 1.0) {
        auto nehPriority = categoricalName(".Init.NEH.Priority");
        auto nehPriorityWeighted = categoricalName(".Init.NEH.PriorityWeighted") == "yes";
        auto nehPriorityOrder = categoricalName(".Init.NEH.PriorityOrder");

        eoInit<EOT>* nehOrder = buildPriority(_problem.data(), nehPriority,
                                 nehPriorityWeighted, nehPriorityOrder)
                       .release();
        storeFunctor(nehOrder);

        auto nehInsertion = categoricalName(".Init.NEH.Insertion");
        auto nehInsert = buildInsertion(nehInsertion);

        if (ratio == 0.0) {
          return &pack<NEH<Ngh>>(*nehOrder, *nehInsert);
        } else {
          return &pack<AppendingNEH<Ngh>>(*firstOrder, *nehOrder, *nehInsert,
                                          ratio);
        }
      }

    }
    return nullptr;
  }
};