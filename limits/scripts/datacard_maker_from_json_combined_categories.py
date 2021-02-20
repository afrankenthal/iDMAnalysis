#!/usr/bin/env python

#include <string>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "utils/json.hpp"

import CombineHarvester.CombineTools.ch as ch
import json
import sys
import os

def main():
    if len(sys.argv) < 2:
        print 'Error! No data filename specified (in json format). Exiting...'

    cat_translator = {u'28': '0', u'29': '1', u'30': '2'}

    with open(sys.argv[1]) as f:
        data = json.load(f)

    tot_yields = {}
    for name,yields in data.iteritems():
        if 'vtx_sign' not in name: continue

        print "name ", name
        
        sample = name.split('_sig_')[-1]

        cut = [token for token in name.split('_') if 'cut' in token][0]
        print 'cut ', cut
        if '28' not in cut and '29' not in cut and '30' not in cut: continue

        if sample not in tot_yields:
            tot_yields[sample] = {}

        cut_num = cut.split('cut')[-1]

        if cut_num not in tot_yields[sample]:
            tot_yields[sample][cut_num] = {}

        tot_yields[sample][cut_num]['A_sig'] = yields["A_sig"]
        tot_yields[sample][cut_num]['B_sig'] = yields["B_sig"]
        tot_yields[sample][cut_num]['C_sig'] = yields["C_sig"]
        tot_yields[sample][cut_num]['D_sig'] = yields["D_sig"]
        tot_yields[sample][cut_num]['A_bkg'] = max(yields["A_bkg"], 0.1)
        tot_yields[sample][cut_num]['B_bkg'] = max(yields["B_bkg"], 0.1)
        tot_yields[sample][cut_num]['C_bkg'] = max(yields["C_bkg"], 0.1)
        tot_yields[sample][cut_num]['D_bkg'] = max(yields["D_bkg"], 0.1)
        tot_yields[sample][cut_num]['c1'] = tot_yields[sample][cut_num]['B_bkg'] / tot_yields[sample][cut_num]['A_bkg']
        tot_yields[sample][cut_num]['c2'] = tot_yields[sample][cut_num]['C_bkg'] / tot_yields[sample][cut_num]['A_bkg']

    #! [part1]
    # Define four categories labeled A, B, C and D, and
    # set the observed yields in a map.
    for sample in tot_yields:
        cats = []
        obs_rates = {}
        sig_rates = {}
        for cut in tot_yields[sample]:
            cat = cat_translator[cut] + 'match'
            cats.append('A_' + cat)
            cats.append('B_' + cat)
            cats.append('C_' + cat)
            cats.append('D_' + cat)
            obs_rates['A_' + cat] = tot_yields[sample][cut]['A_bkg']
            obs_rates['B_' + cat] = tot_yields[sample][cut]['B_bkg']
            obs_rates['C_' + cat] = tot_yields[sample][cut]['C_bkg']
            obs_rates['D_' + cat] = tot_yields[sample][cut]['D_bkg']
            sig_rates['A_' + cat] = tot_yields[sample][cut]['A_sig']
            sig_rates['B_' + cat] = tot_yields[sample][cut]['B_sig']
            sig_rates['C_' + cat] = tot_yields[sample][cut]['C_sig']
            sig_rates['D_' + cat] = tot_yields[sample][cut]['D_sig']
        cats_with_number = []
        for num in range(0, len(cats)):
            cats_with_number.append((num, cats[num]))
        #! [part1]

        #! [part2]
        cb = ch.CombineHarvester()
        cb.SetVerbosity(0)

        cb.AddObservations(["*"], [""], ["13TeV"], [""],          cats_with_number)
        cb.AddProcesses(   ["*"], [""], ["13TeV"], [""], ["sig"], cats_with_number, True)
        cb.AddProcesses(   ["*"], [""], ["13TeV"], [""], ["bkg"], cats_with_number, False)

        cb.cp().ForEachObs(lambda x: x.set_rate(obs_rates[x.bin()]))
        #cb.cp().ForEachObs([&](ch::Observation *x) {
        #        x->set_rate(obs_rates[x->bin()]);
        #        });
        cb.cp().backgrounds().ForEachProc(lambda x: x.set_rate(1))
        #cb.cp().backgrounds().ForEachProc([](ch::Process *x) {
        #        x->set_rate(1);
        #        });
        cb.cp().signals().ForEachProc(lambda x: x.set_rate(sig_rates[x.bin()]))
        #cb.cp().signals().ForEachProc([&](ch::Process *x) {
        #        x->set_rate(sig_rates[x->bin()]);
        #        });
        #! [part2]

        #! [part3]
        # Create a unqiue floating parameter in each bin
        print 'name ', sample
        print 'tot_yields name keys', tot_yields[sample].keys()
        cb.cp().backgrounds().bin(["A_0match", "B_0match", "C_0match", "D_0match"]).AddSyst(cb, "bkgA_norm_0match", "rateParam", ch.SystMap()(tot_yields[sample][u'28']['A_bkg']))
        cb.cp().backgrounds().bin(["B_0match", "D_0match"]).AddSyst(cb, "c1_0match", "rateParam", ch.SystMap()(tot_yields[sample][u'28']['c1']))
        cb.cp().backgrounds().bin(["C_0match", "D_0match"]).AddSyst(cb, "c2_0match", "rateParam", ch.SystMap()(tot_yields[sample][u'28']['c2']))
        cb.cp().backgrounds().bin(["A_1match", "B_1match", "C_1match", "D_1match"]).AddSyst(cb, "bkgA_norm_1match", "rateParam", ch.SystMap()(tot_yields[sample][u'29']['A_bkg']))
        cb.cp().backgrounds().bin(["B_1match", "D_1match"]).AddSyst(cb, "c1_1match", "rateParam", ch.SystMap()(tot_yields[sample][u'29']['c1']))
        cb.cp().backgrounds().bin(["C_1match", "D_1match"]).AddSyst(cb, "c2_1match", "rateParam", ch.SystMap()(tot_yields[sample][u'29']['c2']))
        cb.cp().backgrounds().bin(["A_2match", "B_2match", "C_2match", "D_2match"]).AddSyst(cb, "bkgA_norm_2match", "rateParam", ch.SystMap()(tot_yields[sample][u'30']['A_bkg']))
        cb.cp().backgrounds().bin(["B_2match", "D_2match"]).AddSyst(cb, "c1_2match", "rateParam", ch.SystMap()(tot_yields[sample][u'30']['c1']))
        cb.cp().backgrounds().bin(["C_2match", "D_2match"]).AddSyst(cb, "c2_2match", "rateParam", ch.SystMap()(tot_yields[sample][u'30']['c2']))
        #! [part3]

        #! [part4]
        #cb.PrintAll();
        print ">> Writing datacard for hist: ", sample 
        if not os.path.exists(sample):
            os.mkdir("sig_" + sample)
        cb.WriteDatacard("sig_" + str(sample) + "/datacard.txt")
        #writer = ch.CardWriter('$TAG/datacard.txt', '$TAG/datacard.root')
        #writer.WriteCards(str(sample), cb)
        #! [part4]


if __name__ == '__main__':
    main()

#    for (auto & [name, yields] : data.items()) {
#
#        float A_sig = yields["A_sig"].get<float>();
#        float B_sig = yields["B_sig"].get<float>();
#        float C_sig = yields["C_sig"].get<float>();
#        float D_sig = yields["D_sig"].get<float>();
#
#        float A_bkg = yields["A_bkg"].get<float>();
#        float B_bkg = yields["B_bkg"].get<float>();
#        float C_bkg = yields["C_bkg"].get<float>();
#        float D_bkg = yields["D_bkg"].get<float>();
#
#        float c1 = B_bkg / A_bkg;
#        float c2 = C_bkg / A_bkg;
#
#        //! [part1]
#        // Define four categories labelled A, B, C and D, and
#        // set the observed yields in a map.
#        ch::Categories cats = {
#            {0, "A"},
#            {1, "B"},
#            {2, "C"},
#            {3, "D"}
#        };
#        std::map<std::string, float> obs_rates = {
#            {"A", A_bkg},
#            {"B", B_bkg},
#            {"C", C_bkg},
#            {"D", D_bkg}
#        };
#        std::map<std::string, float> sig_rates = {
#            {"A", A_sig},
#            {"B", B_sig},
#            {"C", C_sig},
#            {"D", D_sig}
#        };
#        //! [part1]
#
#        //! [part2]
#        ch::CombineHarvester cb;
#        cb.SetVerbosity(0);
#
#        cb.AddObservations({"*"}, {""}, {"13TeV"}, {""},          cats);
#        cb.AddProcesses(   {"*"}, {""}, {"13TeV"}, {""}, {"sig"}, cats, true);
#        cb.AddProcesses(   {"*"}, {""}, {"13TeV"}, {""}, {"bkg"}, cats, false);
#
#        cb.cp().ForEachObs([&](ch::Observation *x) {
#                x->set_rate(obs_rates[x->bin()]);
#                });
#        cb.cp().backgrounds().ForEachProc([](ch::Process *x) {
#                x->set_rate(1);
#                });
#        cb.cp().signals().ForEachProc([&](ch::Process *x) {
#                x->set_rate(sig_rates[x->bin()]);
#                });
#        //! [part2]
#
#        //! [part3]
#        using ch::syst::SystMap;
#        using ch::syst::SystMapFunc;
#        using ch::syst::bin;
#
#        // Add a traditional lnN systematic
#        //cb.cp().bin({"D"}).AddSyst(cb, "DummySys", "lnN", SystMap<>::init(1.0001));
#
#        // Create a unqiue floating parameter in each bin
#        cb.cp().backgrounds().bin({"A", "B", "C", "D"}).AddSyst(cb, "bkgA_norm", "rateParam", SystMap<>::init(A_bkg));
#        cb.cp().backgrounds().bin({"B", "D"}).AddSyst(cb, "c1", "rateParam", SystMap<>::init(c1));
#        cb.cp().backgrounds().bin({"C", "D"}).AddSyst(cb, "c2", "rateParam", SystMap<>::init(c2));
#
#        //! [part3]
#
#        //! [part4]
#        //cb.PrintAll();
#
#        cout << ">> Writing datacard for hist: " << name << "\n";
#        mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#        cb.WriteDatacard(name + "/datacard.txt");
#        //! [part4]
#    }
#}
