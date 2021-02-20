#!/bin/bash

# Runs the entire workflow to make CR plots and save them at EOS space at CERN

if [ $# -eq 0 ]; then
    echo "No arguments supplied!"
    exit
fi
if [ $# -eq 1 ]; then
    echo "Only 1 argument supplied!"
    exit
fi

# FIXME run from macros from now
MACRODIR=.
SCRIPTDIR=../scripts

OUTFILE=$MACRODIR/$1

REGION=$2

echo -e "\n\n\n"
echo "Making cutflows from Ntuples..."
echo -e "\n\n\n"

$MACRODIR/bin/macroRun -c "$MACRODIR/configs/cuts/CR_$REGION.json" -m "$MACRODIR/configs/macros/cutflow_CR_${REGION}.json" -d $MACRODIR/configs/samples/sixthrun/data_full.json -b $MACRODIR/configs/samples/sixthrun/backgrounds_full.json -o $OUTFILE

echo -e "\n\n\n"
echo "Creating plots from cutflows..."
echo -e "\n\n\n"

$MACRODIR/bin/macroRun -c "$MACRODIR/configs/cuts/CR_$REGION.json" -m $MACRODIR/configs/macros/MakePlots.json -i $OUTFILE

echo -e "\n\n\n"
echo "Saving canvases to plots directory..."
echo -e "\n\n\n"

$MACRODIR/bin/macroRun -c "$MACRODIR/configs/cuts/CR_$REGION.json" -m $MACRODIR/configs/macros/SaveCanvases.json -i $OUTFILE

echo -e "\n\n\n"
echo "Making HTML index pages..."
echo -e "\n\n\n"

FILEBASE=`basename $OUTFILE .root`
$SCRIPTDIR/make_html_listing.py $MACRODIR/plots/$FILEBASE/

echo -e "\n\n\n"
echo "Transferring plots to CERN's USER EOS space..."
echo -e "\n\n\n"

eos root://eosuser.cern.ch/ mkdir /eos/user/a/asterenb/www/iDM/plots/$FILEBASE/

#xrdcp --parallel 4 -r $MACRODIR/plots/$FILEBASE/ root://eosuser.cern.ch//eos/user/a/asterenb/www/iDM/plots/
xrdcp --parallel 4 -r $MACRODIR/plots/$FILEBASE root://eosuser.cern.ch//eos/user/a/asterenb/www/iDM/plots


echo "Done!"
