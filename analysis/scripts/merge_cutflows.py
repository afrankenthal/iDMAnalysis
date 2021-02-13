#!/usr/bin/env python

from XRootD import client
from XRootD.client.flags import OpenFlags
from io import StringIO
import pandas as pd
import argparse
import sys

def main():
    
    parser = argparse.ArgumentParser(description='Merge csv cutflow tables from parallel condor runs.')
    parser.add_argument('files', metavar='File', nargs='*', help='File name')
    parser.add_argument('-l', '--list', metavar='list_file', dest='list', nargs='?', help='File with list of csv filenames')
    args = parser.parse_args()

    if args.list is not None:
        with open(args.list) as f:
            csv_list = f.read().splitlines()
    elif len(args.files) > 1:
        csv_list = args.files
    else:
        print("Error! Need at least two files to merge or a list of files. Exiting...")
        exit()

    total_df = pd.DataFrame()
    for csv_name in csv_list:
        with client.File() as f:
            print("Opening file ", csv_name)
            f.open(csv_name, OpenFlags.READ)
            try:
                status, data = f.read()
            except:
                print("Unexpected error when reading file with XRootD:", sys.exc_info()[0])
                print("Skipping file...")
                continue
            data_as_file = StringIO(data.decode('utf-8').replace(',', ''))
            df = pd.read_csv(data_as_file, delimiter=' ')
            df.set_index(['Cut#', 'Description'], inplace=True)
            total_df = df.copy() if total_df.empty else total_df.add(df, fill_value=0)

    # Rearrange columns to bring 'Data' up front and signal columns to the back
    columns = list(total_df.columns.values)
    if 'Data' in columns:
        columns.remove('Data')
        columns.insert(0, 'Data')
    for col in columns:
        if 'sig' in col:
            columns.remove(col)
            columns.insert(-1, col)
    total_df = total_df[columns]
    print("Total df")
    print(total_df)

    total_df.to_csv('merged_cutflow.csv', sep=' ', float_format='%g')

if __name__ == "__main__":
    main()