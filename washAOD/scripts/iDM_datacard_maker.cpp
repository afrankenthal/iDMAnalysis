#include <string>
#include <iostream>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"

using namespace std;

int main() {
  //! [part1]
  // Define four categories labelled A, B, C and D, and
  // set the observed yields in a map.
  ch::Categories cats = {
      {0, "A"},
      {1, "B"},
      {2, "C"},
      {3, "D"}
    };
  std::map<std::string, float> obs_rates = {
    {"A", 688.},
    {"B", 60.},
    {"C", 5.},
    {"D", 1.}
  };
  std::map<std::string, float> sig_rates = {
    {"A", 10.61},
    {"B", 0.},
    {"C", 6.56},
    {"D", 0.}
  };
  //! [part1]

  //! [part2]
  ch::CombineHarvester cb;
  cb.SetVerbosity(3);

  cb.AddObservations({"*"}, {""}, {"13TeV"}, {""},          cats);
  cb.AddProcesses(   {"*"}, {""}, {"13TeV"}, {""}, {"sig"}, cats, true);
  cb.AddProcesses(   {"*"}, {""}, {"13TeV"}, {""}, {"bkg"}, cats, false);

  cb.ForEachObs([&](ch::Observation *x) {
    x->set_rate(obs_rates[x->bin()]);
  });
  cb.cp().backgrounds().ForEachProc([](ch::Process *x) {
    x->set_rate(1);
  });
  cb.cp().signals().ForEachProc([&](ch::Process *x) {
    x->set_rate(sig_rates[x->bin()]);
  });
  //! [part2]

  //! [part3]
  using ch::syst::SystMap;
  using ch::syst::SystMapFunc;
  using ch::syst::bin;

  // Add a traditional lnN systematic
  //cb.cp().bin({"D"}).AddSyst(cb, "DummySys", "lnN", SystMap<>::init(1.0001));
  
  // Create a unqiue floating parameter in each bin
  cb.cp().backgrounds().bin({"A", "B", "C", "D"}).AddSyst(cb, "bkgA_norm", "rateParam", SystMap<>::init(688));
  cb.cp().backgrounds().bin({"B", "D"}).AddSyst(cb, "c1", "rateParam", SystMap<>::init(0.087));
  cb.cp().backgrounds().bin({"C", "D"}).AddSyst(cb, "c2", "rateParam", SystMap<>::init(0.0072));

  //! [part3]

  //! [part4]
  cb.PrintAll();
  cb.WriteDatacard("iDM_datacard.txt");
  //! [part4]
}





