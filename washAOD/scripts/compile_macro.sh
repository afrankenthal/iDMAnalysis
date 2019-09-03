mkdir -p ./bin

if [[ $1 == "macroRun" ]]; then
    set -x
    root -q -l TSelectors/mainAnalysisSelectorData.C+
    root -q -l TSelectors/mainAnalysisSelectorMC.C+
fi

set -x

g++ -std=c++17 -O3 -pipe -Wall -W -Woverloaded-virtual -fPIC -Iinclude  -pthread -I $ROOTSYS/include -o $1.o -c $1.C

g++ -std=c++17 -O3 -m64 TSelectors/mainAnalysisSelectorData_C.so TSelectors/mainAnalysisSelectorMC_C.so $1.o -L$ROOTSYS/lib -lCore -lCling -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -lASImage -rdynamic  -o ./bin/$1

rm $1.o
