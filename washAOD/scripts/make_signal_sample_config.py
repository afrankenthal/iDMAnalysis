#!/usr/bin/env python

import sys
import subprocess
import json
from collections import OrderedDict

def main():

    with open('../data/signal_xsec_alphaD_alphaEM.json') as f:
        xsec_db = json.load(f)

    lumi = dict({'2018': 59.74, '2017': 41.53, '2016': 35.92})

    if len(sys.argv) < 3:
        print "Error! Need two arguments, year and run_number"
        exit()
    year = sys.argv[1]
    run_number = sys.argv[2]

    private_sample_path = '/store/group/lpcmetx/iDM/Ntuples/' + str(year) + '/signal/iDMAnalysis_' + str(run_number) + 'run'

    all_configs = dict()

    filelist = subprocess.check_output(['eos', 'root://cmseos.fnal.gov', 'ls', private_sample_path])

    for f in filelist.split('\n'):
        if '.root' not in f: continue
        if 'subset' in f: continue

        tokens = (private_sample_path + '/' + f).split('_')

        for token in tokens:
            if 'dMchi' in token:
                #dmchi = float(token.split('-')[1].replace('p', '.'))
                dmchi = token.split('-')[1]
            elif 'Mchi' in token:
                #mchi = float(token.split('-')[1].replace('p', '.'))
                mchi = token.split('-')[1]
            elif 'ctau' in token:
                ctau = int(token.split('-')[1].split('.')[0])

        central_sample_path = subprocess.check_output([
            '../scripts/print_crab_paths.sh',
            '../python/crab_' + run_number + 'run/crab_iDMAnalysis_sig_' + mchi + '_' +  dmchi + '_' + str(ctau) + '_' + str(year)
            ])

        mchi = float(mchi.replace('p', '.'))
        dmchi = float(dmchi.replace('p', '.'))

        m1 = mchi - dmchi/2
        delta = round(dmchi/m1, 1)
        ctau_cm = ctau/10
        if ctau_cm == 0: ctau_cm = 0.1

        cfg = dict()
        cfg["m1"] = m1
        cfg["delta_fraction"] = delta
        cfg["ctau_cm"] = ctau_cm

        cfg["mode"] = 'SIGNAL'
        cfg["group"] = 'sig_' + str(mchi).replace('.', 'p') + '_' + str(ctau)
        cfg["year"] = int(year)
        cfg["xsec"] = float("{:.8f}".format(xsec_db['sig_' + str(mchi).replace('.', 'p')]/float(ctau)))
        cfg["limit_num_files"] = -1
        if run_number in central_sample_path:
            cfg["dir"] = ['root://cmseos.fnal.gov/' + private_sample_path + '/' + f,
                    central_sample_path.replace('\n','')
                    ]
        else:
            cfg["dir"] = ['root://cmseos.fnal.gov/' + private_sample_path + '/' + f]
        cfg["sum_gen_wgt"] = 0.0
        cfg["lumi"] = lumi[year]

        all_configs[f.split('/')[-1].split('.')[0].split('iDMAnalysis_')[1]] = cfg
    
    with open('all_signal_samples_' + year + '.json', 'w') as f:
        json.dump(OrderedDict(sorted(all_configs.items())), f, indent=4)


if __name__ == '__main__':
    main()
