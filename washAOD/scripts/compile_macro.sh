set -x
g++ -std=c++17 -O3 -pipe -Wall -W -Woverloaded-virtual -fPIC -Iinclude  -pthread \
-I $ROOTSYS/include -o $1.o -c $1.C

g++ -std=c++17 -O3 -m64 $1.o -L$ROOTSYS/lib -lCore -lCling -lRIO -lNet -lHist -lGraf \
-lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore \
-lThread -pthread -lm -ldl -rdynamic  -o $1

rm $1.o
