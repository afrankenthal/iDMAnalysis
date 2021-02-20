#!/bin/env python3
import sys, os, subprocess
import json
from pathlib import Path

if __name__ == '__main__':

    if len(sys.argv) < 2:
        print("Error! Need one argument, the sample filename to process.")
        exit(2)

    sample_filename = Path(sys.argv[1])
    repo_path = subprocess.check_output(['git', 'rev-parse', '--show-toplevel']).decode('utf-8').strip('\n')
    sample_db_file = Path(repo_path + "/skimmer/data/sample_info_table_short.json").open('r')
    print(sample_db_file)
    sample_db = json.load(sample_db_file)

    print(f'Processing sample file {sample_filename}')

    with sample_filename.open('r+') as sample_file:
        data = json.load(sample_file)
        for sample, config in data.items():
            for db_sample, db_config in sample_db.items():
                if db_sample in config['dir'][0]:
                    config['group'] = db_config['group']
                    config['mode'] = db_config['mode']
                    config['xsec'] = db_config['xsec']
            print(config)

        sample_file.seek(0)
        sample_file.truncate()
        json.dump(data, sample_file, indent=4, sort_keys=False)
