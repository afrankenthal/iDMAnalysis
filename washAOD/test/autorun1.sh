python macros/plotAcceptance.py histo_0p83cm.root
python macros/plotAcceptance.py histo_8p3cm.root
python macros/plotAcceptance.py histo_83cm.root

#date=`date +%Y%m%d`
#mkdir /publicweb/w/wsi/public/lpcdm/$date
#echo "Options +Indexes" > /publicweb/w/wsi/public/lpcdm/$date/.htaccess
mv *.pdf /publicweb/w/wsi/public/lpcdm
