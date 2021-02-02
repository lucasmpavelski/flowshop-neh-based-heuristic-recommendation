#include <Rcpp.h>
using namespace Rcpp;

#include "flowshop-solver/MHParamsSpecsFactory.hpp"
#include "flowshop-solver/FSPProblemFactory.hpp"
#include "flowshop-solver/fla_methods.hpp"
#include "flowshop-solver/heuristics/all.hpp"
#include "flowshop-solver/heuristics.hpp"

// This is a simple function using Rcpp that creates an R list
// containing a character vector and a numeric vector.
//
// Learn more about how to use Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//
// and browse examples of code using Rcpp at:
//
//   http://gallery.rcpp.org/
//

// [[Rcpp::export]]
void initFactories(std::string data_folder)
{
  MHParamsSpecsFactory::init(data_folder + "/specs", true);
  FSPProblemFactory::init(data_folder);
}

auto rcharVec2map(Rcpp::CharacterVector charvec) -> std::unordered_map<std::string, std::string> {
  std::unordered_map<std::string, std::string> ret;
  for (const auto& name : Rcpp::as<std::vector<std::string>>(charvec.names())) {
    ret[name] = charvec[name];
  }
  return ret;
}


// [[Rcpp::export]]
List solveFSP(std::string mh, Rcpp::CharacterVector rproblem, long seed,
              Rcpp::CharacterVector rparams, bool verbose = false) 
try {
  using namespace Rcpp;
  
  RNG::seed(seed);
  std::unordered_map<std::string, std::string> params = rcharVec2map(rparams);
  std::unordered_map<std::string, std::string> problem = rcharVec2map(rproblem);
  
  if (verbose) {
    for (const auto& kv : params)
      Rcerr << kv.first << ": " << kv.second << '\n';
    for (const auto& kv : problem)
      Rcerr << kv.first << ": " << kv.second << '\n';
  }
  auto result = solveWith(mh, problem, params);
  return List::create(
    Named("fitness") = result.fitness,
    Named("time") = result.time,
    Named("no_evals") = result.no_evals);
} catch (std::exception &ex) {
  throw Rcpp::exception(ex.what());
}


#include "flowshop-solver/fla/SampleSolutionStatistics.hpp"

// [[Rcpp::export]]
List sampleSolutionStatisticsFLA(
  std::string dataFolder,
  Rcpp::CharacterVector rproblem,
                           long noSamples,
                           long seed)
{
  initFactories(dataFolder);
  const auto& probData = rcharVec2map(rproblem);
  auto problem = FSPProblemFactory::get(probData);
  RNG::seed(seed);
  SampleSolutionStatistics<FSPNeighbor> solutionStatistics(
    problem.size(),
    problem.eval(),
    problem.neighborEval()
  );
  const auto& res = solutionStatistics.sample(noSamples);
  using namespace Rcpp;
  return List::create(
      Named("up") = res.up,
      Named("down") = res.down,
      Named("side") = res.side,
      Named("slmin") = res.slmin,
      Named("lmin") = res.lmin,
      Named("iplat") = res.iplat,
      Named("ledge") = res.ledge,
      Named("slope") = res.slope,
      Named("lmax") = res.lmax,
      Named("slmax") = res.slmax
  );
}

#include "flowshop-solver/fla/SampleRandomWalk.hpp"

// [[Rcpp::export]]
std::vector<double> sampleRandomWalk(
  std::string dataFolder,
  Rcpp::CharacterVector rproblem,
  int noSamples,
  std::string samplingStrat,
  long seed)
{
  initFactories(dataFolder);
  const auto& probData = rcharVec2map(rproblem);
  auto problem = FSPProblemFactory::get(probData);
  RNG::seed(seed);
  SampleRandomWalk<FSPNeighbor> sampleRWFSP(
    problem.size(),
    problem.eval(),
    problem.neighborEval()
  );
  return sampleRWFSP.sample(noSamples, samplingStrat);
}

// [[Rcpp::export]]
std::vector<double> enumerateAllFitness(Rcpp::CharacterVector rproblem)
{
  auto prob_data = rcharVec2map(rproblem);
  return enumerateAll(prob_data);
}

template<class EOT>
std::vector<int> solToVec(const EOT& sol) {
  std::vector<int> vec(sol.size());
  std::copy(sol.begin(), sol.end(), vec.begin());
  return vec;
}

// [[Rcpp::export]]
List enumerateSolutions(Rcpp::List fspInstance, Rcpp::CharacterVector fspProblem) {
  std::vector<int> pts =  Rcpp::as<std::vector<int>>(fspInstance["pt"]);
  int no_jobs = fspInstance["no_jobs"];
  FSPData dt(pts, no_jobs);
  auto prob_data = rcharVec2map(fspProblem);
  FSPProblem problem(
      dt,
      prob_data["fsp_type"],
      prob_data["objective"],
      prob_data["budget"],
      prob_data["stopping_criterion"]
  );
  auto solutions = enumerateAllSolutions(problem);
  std::vector<std::vector<int>> sample_solutions;
  sample_solutions.reserve(solutions.size());
  IntegerVector sample_fitness;
  for (const auto& sol : solutions) {
    sample_solutions.push_back(solToVec(sol));
    sample_fitness.push_back(sol.fitness());
  }
  return List::create(
    Named("solutions") = sample_solutions,
    Named("fitness") = sample_fitness
  );
}
