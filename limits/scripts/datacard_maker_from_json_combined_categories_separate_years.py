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
from collections import OrderedDict

def main():
    if len(sys.argv) < 2:
        print 'Error! No data filename specified (in json format). Exiting...'
        exit(0)

    lumi_syst = {'2016': 1.025, '2017': 1.023, '2018': 1.025}
    trig_MC_syst = {'2016': 1.01, '2017': 1.01, '2018': 1.01}
    trig_data_syst = {'2016': 1.005, '2017': 1.005, '2018': 1.005}
    JES_syst = {'2016': 1.02, '2017': 1.06, '2018': 1.02}
    JER_syst = {'2016': 1.01, '2017': 1.09, '2018': 1.025}
    #EGM_ID_syst = {'2016': 1.02, '2017': 1.03, '2018': 1.03}
    veto_ID_syst = {'2016': 1.005, '2017': 1.005, '2018': 1.005}
    GM_ID_syst = { # split by match category
            '0': {'2016': 1.000, '2017': 1.000, '2018': 1.000}, # just dummy = 1, no GMs
            '1': {'2016': 1.007, '2017': 1.003, '2018': 1.005},
            '2': {'2016': 1.012, '2017': 1.005, '2018': 1.010}
            }
    dSA_ID_prompt_syst = {'2016': 1.006, '2017': 1.007, '2018': 1.006}
    dSA_ID_displ_syst = {'2016': 1.001, '2017': 1.001, '2018': 1.001}
    dSA_reco_prompt_syst = {'2016': 1.004, '2017': 1.003, '2018': 1.003}

    # background ABCD closure syst
    closure_syst_1mm = {
            '0': {'2016': 1.25, '2017': 1.25, '2018': 1.25},
            '1': {'2016': 1.05, '2017': 1.05, '2018': 1.05},
            '2': {'2016': 1.80, '2017': 1.80, '2018': 1.80}
            }
    closure_syst_10mm = {
            '0': {'2016': 1.25, '2017': 1.25, '2018': 1.25},
            '1': {'2016': 1.05, '2017': 1.05, '2018': 1.05},
            '2': {'2016': 1.80, '2017': 1.80, '2018': 1.80}
            }
    closure_syst_100mm = {
            '0': {'2016': 1.20, '2017': 1.20, '2018': 1.20},
            '1': {'2016': 1.05, '2017': 1.05, '2018': 1.05},
            '2': {'2016': 1.20, '2017': 1.20, '2018': 1.20}
            }
    closure_syst_1000mm = {
            '0': {'2016': 1.10, '2017': 1.10, '2018': 1.10},
            '1': {'2016': 1.05, '2017': 1.05, '2018': 1.05},
            '2': {'2016': 1.20, '2017': 1.20, '2018': 1.20}
            }

    cat_translator = {u'29': '0', u'30': '1', u'31': '2'}

    with open(sys.argv[1]) as f:
        data = json.load(f)

    tot_yields = {}
    for name,yields in data.iteritems():
        
        sample = name.split('_sig_')[-1]

        cut = [token for token in name.split('_') if 'cut' in token][0]
        if '29' not in cut and '30' not in cut and '31' not in cut:
            continue

        if '29' in cut or '30' in cut:
            if 'vxy_zoom_' not in name:
                continue
        else:
            if 'vxy_zoomzoom_' not in name:
                continue

        print "name ", name

        if yields['A_sig'] == 0.0 and yields['B_sig'] == 0.0 and yields['C_sig'] == 0.0 and yields['D_sig'] == 0.0:
            continue

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

    c1s = {}
    c2s = {}
    for sample in tot_yields:
        if '161718' not in sample: continue
        basename_no_year = sample.split('_161718')[0]
        c1s[basename_no_year] = {}
        c2s[basename_no_year] = {}
        for cut_num,props in tot_yields[sample].items():
            c1s[basename_no_year][cut_num] = props['c1']
            c2s[basename_no_year][cut_num] = props['c2']

    #! [part1]
    # Define four categories labeled A, B, C and D, and
    # set the observed yields in a map.
    cats = OrderedDict()
    obs_rates = OrderedDict()
    sig_rates = OrderedDict()
    systs = OrderedDict()
    names = OrderedDict()
    for sample in tot_yields:
        if '161718' in sample: continue
        if '2016' in sample:
            year = '2016'
        elif '2017' in sample:
            year = '2017'
        elif '2018' in sample:
            year = '2018'

        basename_no_year = sample.split('_'+year)[0]

        if basename_no_year not in cats:
            cats[basename_no_year] = []
            obs_rates[basename_no_year] = {}
            sig_rates[basename_no_year] = {}
            systs[basename_no_year] = {}
            names[basename_no_year] = basename_no_year

        for cut in tot_yields[sample]:
            cat_temp = cat_translator[cut] + 'match' + '_' + year
            index = 0 if len(cats[basename_no_year]) == 0 else cats[basename_no_year][-1][0] + 1
            cats[basename_no_year].append((index, 'A_' + cat_temp))
            cats[basename_no_year].append((index, 'B_' + cat_temp))
            cats[basename_no_year].append((index, 'C_' + cat_temp))
            cats[basename_no_year].append((index, 'D_' + cat_temp))
            obs_rates[basename_no_year]['A_' + cat_temp] = tot_yields[sample][cut]['A_bkg']
            obs_rates[basename_no_year]['B_' + cat_temp] = tot_yields[sample][cut]['B_bkg']
            obs_rates[basename_no_year]['C_' + cat_temp] = tot_yields[sample][cut]['C_bkg']
            obs_rates[basename_no_year]['D_' + cat_temp] = tot_yields[sample][cut]['D_bkg']
            sig_rates[basename_no_year]['A_' + cat_temp] = tot_yields[sample][cut]['A_sig']
            sig_rates[basename_no_year]['B_' + cat_temp] = tot_yields[sample][cut]['B_sig']
            sig_rates[basename_no_year]['C_' + cat_temp] = tot_yields[sample][cut]['C_sig']
            sig_rates[basename_no_year]['D_' + cat_temp] = tot_yields[sample][cut]['D_sig']
            systs[basename_no_year]["bkgA_norm_" + cat_temp] = tot_yields[sample][cut]['A_bkg']
            #systs[basename_no_year]["c1_" + cat_temp] = tot_yields[sample][cut]['c1']
            #systs[basename_no_year]["c2_" + cat_temp] = tot_yields[sample][cut]['c2']
            systs[basename_no_year]["c1_" + cat_temp] = c1s[basename_no_year][cut]
            systs[basename_no_year]["c2_" + cat_temp] = c2s[basename_no_year][cut]
            closure_syst = {}
            if '_1_' in sample:
                closure_syst = closure_syst_1mm
            elif '_10_' in sample:
                closure_syst = closure_syst_10mm
            elif '_100_' in sample:
                closure_syst = closure_syst_100mm
            elif '_1000_' in sample:
                closure_syst = closure_syst_1000mm
            systs[basename_no_year]["closure_" + cat_temp] = closure_syst[cat_translator[cut]][year]

            systs[basename_no_year]["lumi_" + year] = lumi_syst[year]
            systs[basename_no_year]["trig_MC_" + year] = trig_MC_syst[year]
            systs[basename_no_year]["trig_data_" + year] = trig_data_syst[year]
            systs[basename_no_year]["JES_" + year] = JES_syst[year]
            systs[basename_no_year]["JER_" + year] = JER_syst[year]
            #systs[basename_no_year]["EGM_ID_" + year] = EGM_ID_syst[year]
            systs[basename_no_year]["veto_ID_" + year] = veto_ID_syst[year]
            systs[basename_no_year]["GM_ID_" + cat_temp] = GM_ID_syst[cat_translator[cut]][year]
            systs[basename_no_year]["dSA_ID_prompt_" + year] = dSA_ID_prompt_syst[year]
            systs[basename_no_year]["dSA_ID_displ_" + year] = dSA_ID_displ_syst[year]
            systs[basename_no_year]["dSA_reco_prompt_" + year] = dSA_reco_prompt_syst[year]

    #! [part1]

    for name, cat, obs_rate, sig_rate, syst in zip(names.values(), cats.values(), obs_rates.values(), sig_rates.values(), systs.values()):

        cb = ch.CombineHarvester()
        cb.SetVerbosity(0)

        #! [part2]
        print "cat ", cat
        cb.AddObservations(["*"], [""], ["13TeV"], [""],          cat)
        cb.AddProcesses(   ["*"], [""], ["13TeV"], [""], ["sig"], cat, True)
        cb.AddProcesses(   ["*"], [""], ["13TeV"], [""], ["bkg"], cat, False)

        cb.cp().ForEachObs(lambda x: x.set_rate(obs_rate[x.bin()]))
        cb.cp().backgrounds().ForEachProc(lambda x: x.set_rate(1))
        cb.cp().signals().ForEachProc(lambda x: x.set_rate(sig_rate[x.bin()]))
        #! [part2]

        #! [part3]
        # Create a unqiue floating parameter in each bin
        #print 'name ', sample
        #print 'tot_yields name keys', tot_yields[sample].keys()
        for y in ['2016', '2017', '2018']:
            for m in ['0', '1', '2']:
                if 'A_'+m+'match_'+y not in sig_rate:
                    continue
                cb.cp().backgrounds().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'bkgA_norm_'+m+'match_'+y, "rateParam", ch.SystMap()(syst['bkgA_norm_'+m+'match_'+y]))
                cb.cp().backgrounds().bin(['B_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'c1_'+m+'match', "rateParam", ch.SystMap()(syst['c1_'+m+'match_'+y]))
                cb.cp().backgrounds().bin(['C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'c2_'+m+'match', "rateParam", ch.SystMap()(syst['c2_'+m+'match_'+y]))
                cb.cp().backgrounds().bin(['C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'c2_'+m+'match', "rateParam", ch.SystMap()(syst['c2_'+m+'match_'+y]))
                # background systs
                cb.cp().backgrounds().bin(['C_'+m+'match_'+y]).AddSyst(cb, 'closure_'+m+'match', 'lnN', ch.SystMap()(syst['closure_'+m+'match_'+y]))

                # signal systs
                cb.cp().signals().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'lumi_'+y, 'lnN', ch.SystMap()(syst['lumi_'+y]))
                cb.cp().signals().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'trig_MC', 'lnN', ch.SystMap()(syst['trig_MC_'+y]))
                cb.cp().signals().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'trig_data', 'lnN', ch.SystMap()(syst['trig_data_'+y]))
                cb.cp().signals().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'JES', 'lnN', ch.SystMap()(syst['JES_'+y]))
                cb.cp().signals().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'JER_'+y, 'lnN', ch.SystMap()(syst['JER_'+y]))
                #cb.cp().signals().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'EGM_ID', 'lnN', ch.SystMap()(syst['EGM_ID_'+y]))
                cb.cp().signals().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'veto_ID', 'lnN', ch.SystMap()(syst['veto_ID_'+y]))
                cb.cp().signals().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'GM_ID_'+m+'match', 'lnN', ch.SystMap()(syst['GM_ID_'+m+'match_'+y]))
                cb.cp().signals().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'dSA_ID_prompt', 'lnN', ch.SystMap()(syst['dSA_ID_prompt_'+y]))
                cb.cp().signals().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'dSA_ID_displ', 'lnN', ch.SystMap()(syst['dSA_ID_displ_'+y]))
                cb.cp().signals().bin(['A_'+m+'match_'+y, 'B_'+m+'match_'+y, 'C_'+m+'match_'+y, 'D_'+m+'match_'+y]).AddSyst(cb, 'dSA_reco_prompt', 'lnN', ch.SystMap()(syst['dSA_reco_prompt_'+y]))
        #! [part3]

        #! [part4]
        #cb.PrintAll();
        #for cb_name, cb in cbs.iteritems():
        print ">> Writing datacard for hist: ", name 
        if not os.path.exists(name):
            os.mkdir("sig_" + name)
        cb.WriteDatacard("sig_" + str(name) + "/datacard.txt")
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
