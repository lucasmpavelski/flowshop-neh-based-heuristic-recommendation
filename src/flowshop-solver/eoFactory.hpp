#pragma once

#include <type_traits>

#include <paradiseo/eo/eo>
#include <paradiseo/mo/mo>

#include "flowshop-solver/MHParamsValues.hpp"
#include "flowshop-solver/problems/Problem.hpp"
#include "flowshop-solver/heuristics/falseContinuator.hpp"


template <class Ngh>
class myOrderNeighborhood : public moOrderNeighborhood<Ngh>, public eoFunctorBase {
  using moOrderNeighborhood<Ngh>::moOrderNeighborhood;
};

template <class Ngh>
class myRndWithoutReplNeighborhood : public moRndWithoutReplNeighborhood<Ngh>, public eoFunctorBase {
  using moRndWithoutReplNeighborhood<Ngh>::moRndWithoutReplNeighborhood;
};

template <class Ngh, class EOT = typename Ngh::EOT>
class eoFactory : public eoFunctorStore {
  const MHParamsValues& _params;
  Problem<Ngh>& _problem;

 protected:
  virtual auto domainInit() -> eoInit<EOT>* { return nullptr; }
  virtual auto domainSolComparator() -> moSolComparator<Ngh>* {
    return nullptr;
  }
  virtual auto domainSolNeighborComparator() -> moSolNeighborComparator<Ngh>* {
    return nullptr;
  }
  virtual auto domainNeighborComparator() -> moNeighborComparator<Ngh>* {
    return nullptr;
  }

 public:
  template <class T>
  using ptr = std::unique_ptr<T>;

  eoFactory(const MHParamsValues& params, Problem<Ngh>& problem)
      : _params{params}, _problem{problem} {};

  [[nodiscard]] auto params() const -> const MHParamsValues& { return _params; }

  auto problem() const -> const Problem<Ngh>& { return _problem; }

  [[nodiscard]] auto categoricalName(const std::string& name) const
      -> std::string {
    return _params.categoricalName(_params.mhName() + name);
  }

  [[nodiscard]] auto categorical(const std::string& name) const -> int {
    return _params.categorical(_params.mhName() + name);
  }

  [[nodiscard]] auto integer(const std::string& name) const -> int {
    return _params.integer(_params.mhName() + name);
  }

  [[nodiscard]] auto real(const std::string& name) const -> double {
    return _params.real(_params.mhName() + name);
  }

  void params(MHParamsValues& _params) {
    this->_params = _params;
  }

  auto buildInit() -> eoInit<EOT>* {
    const std::string init = categoricalName(".Init");
    if (init == "random")
      return &pack<eoInitPermutation<EOT>>(_problem.size(0));
    return domainInit();
  }

  auto buildSolComparator() -> moSolComparator<EOT>* {
    const std::string name = categoricalName(".Comp.Strat");
    if (name == "strict")
      return &pack<moSolComparator<EOT>>();
    else if (name == "equal")
      return &pack<moEqualSolComparator<EOT>>();
    else
      return domainSolComparator();
  }

  auto buildSolNeighborComparator() -> moSolNeighborComparator<Ngh>* {
    const std::string name = categoricalName(".Comp.Strat");
    if (name == "strict")
      return &pack<moSolNeighborComparator<Ngh>>();
    else if (name == "equal")
      return &pack<moEqualSolNeighborComparator<Ngh>>();
    else
      return domainSolNeighborComparator();
  }

  auto buildNeighborComparator() -> moNeighborComparator<Ngh>* {
    const std::string name = categoricalName(".Comp.Strat");
    if (name == "strict")
      return &pack<moNeighborComparator<Ngh>>();
    else if (name == "equal")
      return &pack<moEqualNeighborComparator<Ngh>>();
    else
      return domainNeighborComparator();
  }
};