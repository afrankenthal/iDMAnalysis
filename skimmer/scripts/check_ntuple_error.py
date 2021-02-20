#!/usr/bin/env python

from ROOT import TFile,TTree
import sys

def main():
    if len(sys.argv) < 2 or '.root' not in sys.argv[1]:
        print("need one argument, ROOT file")
        exit()

    print(sys.argv[1])

    f = TFile.Open(sys.argv[1])

    t = f.Get("ntuples_gbm/recoT")

    branches = t.GetListOfBranches()
    if not t.GetTree():
        print("Error on file", sys.argv[1])
        f.Close()
        exit()

    for i in range(0,t.GetEntries()):
        error = t.GetEntry(i)
        if error < 0:
            print("Error on file", sys.argv[1])
            f.Close()
            exit()



if __name__ == '__main__':
    main()
