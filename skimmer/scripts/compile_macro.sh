#!/bin/bash

debugflag=""
if [[ $2 == "debug" ]]; then
    debugflag="g"
fi

mkdir -p ./bin

if [[ $1 == "macroRun" ]]; then
    set -x
    #root -q -l TSelectors/mainAnalysisSelectorData.C+
    root -q -l TSelectors/mainAnalysisSelector.C+$debugflag
    set +x
fi

debugflag=""
if [[ $2 == "debug" ]]; then
    debugflag="-g"
fi

#g++ -std=c++17 $debugflag -O3 -m64 TSelectors/mainAnalysisSelector_C.so $1.o -L$ROOTSYS/lib `root-config --libs` -lCore -lCling -lProof -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -lASImage -rdynamic -lstdc++fs -o ./bin/$1

set -x

g++ -std=c++17 $debugflag -O3 -pipe -Wall -W -Woverloaded-virtual -fPIC -Iinclude -pthread -I $ROOTSYS/include -o $1.o -c $1.C

g++ -std=c++17 $debugflag -O3 -m64 RDFAnalysis.o TSelectors/mainAnalysisSelector_C.so $1.o `root-config --libs` -lstdc++fs -o ./bin/$1


#rm $1.o
