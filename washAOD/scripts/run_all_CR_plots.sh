#!/bin/bash

# Runs the entire workflow to make CR plots and save them at EOS space at CERN

if [ $# -eq 0 ]; then
    echo "No arguments supplied!"
    exit
fi

# FIXME run from macros from now
MACRODIR=.
SCRIPTDIR=../scripts

OUTFILE=$MACRODIR/$1

echo -e "\n\n\n"
echo "Making cutflows from Ntuples..."
echo -e "\n\n\n"

$MACRODIR/bin/macroRun -m $MACRODIR/configs/macros/cutflow_CR_nJets.json -d $MACRODIR/configs/samples/fifthrun/data_full.json -b $MACRODIR/configs/samples/fifthrun/backgrounds_InclDibos_TTJets_full.json -o $OUTFILE

echo -e "\n\n\n"
echo "Creating plots from cutflows..."
echo -e "\n\n\n"

$MACRODIR/bin/macroRun -m $MACRODIR/configs/macros/MakePlots.json -i $OUTFILE

echo -e "\n\n\n"
echo "Saving canvases to plots directory..."
echo -e "\n\n\n"

$MACRODIR/bin/macroRun -m $MACRODIR/configs/macros/SaveCanvases.json -i $OUTFILE

echo -e "\n\n\n"
echo "Making HTML index pages..."
echo -e "\n\n\n"

FILEBASE=`basename $OUTFILE .root`
$SCRIPTDIR/make_html_listing.py $MACRODIR/plots/$FILEBASE/

echo -e "\n\n\n"
echo "Transferring plots to CERN's USER EOS space..."
echo -e "\n\n\n"

eos root://eosuser.cern.ch/ mkdir /eos/user/a/asterenb/www/iDM/plots/$FILEBASE/

xrdcp -r $MACRODIR/plots/$FILEBASE/ root://eosuser.cern.ch//eos/user/a/asterenb/www/iDM/plots/$FILEBASE/

echo "Done!"
