#!/bin/env python3
import sys
import json
from pathlib import Path

if __name__ == '__main__':

    sample_filename = Path(sys.argv[1])
    sample_db_file = Path("../data/sample_info_table.json").open('r')
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
