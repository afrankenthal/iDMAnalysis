import os, sys
import subprocess
print("cleaning files if they exist")


#mchis=['48p0','52p0','63p0','72p0','84p0','96p0','105p0','120p0']
#dmchis=['16p0','5p0','6p0','24p0','8p0','32p0','10p0','40p0']
#mchis=['1p2','10p5','12p0','21p0','24p0','31p5','36p0','42p0']
#dmchis=['0p4','1p0','4p0','2p0','8p0','3p0','12p0','4p0']
#mchis=['1p2','10p5','12p0','21p0','24p0','31p5','36p0','42p0','48p0','52p5','63p0','72p0','84p0','96p0','105p0','120p0']
#dmchis=['0p4','1p0','4p0','2p0','8p0','3p0','12p0','4p0','16p0','5p0','6p0','24p0','8p0','32p0','10p0','40p0']
#mchis=['72p0','3p15','3p6','60']
#dmchis=['24p0','0p3', '1p2','20']
mchis=['5p25']#['60p0']
dmchis=['0p5']#['20p0']
#mchis=['5p25', '6p0','52p5','60p0']
#dmchis=['0p5','2p0','5p0','20p0']


#os.system("rm Mchi-*.txt")
#masslist = ["Mchi-5p25_dMchi-0p5","Mchi-52p5_dMchi-5p0","Mchi-6p0_dMchi-2p0"]#,"Mchi-60_dMchi-20"]
masslist = ["Mchi-%s_dMchi-%s"%(mchi,dmchi) for mchi,dmchi in zip(mchis,dmchis)]
print masslist
#lifelist = [1]
lifelist = [1]#,10,100,1000]
#lifelist = [10]
for mass in masslist:
	for life in lifelist:	
		#cmdxx = "cmsRun python/TrackQuality_cfg.py year=2018 inputFiles_load=data/iDM/2018/signal/pileup/{0}_ctau-{1}.list outputFile=~/nobackup/signal_region/2018/pileup/pileup_trackquality_{0}_ctau-{1}.root".format(mass,life)
		#cmdxx = "cmsRun python/SigRegEff_cfg.py year=2018 inputFiles_load=data/iDM/2018/pileup/test.list outputFile=~/nobackup/signal_region/2018/pileup/test_nogen_{0}_ctau-{1}.root".format(mass,life)
		cmdxx = "cmsRun python/TrackQuality_cfg.py year=2018 inputFiles_load=data/iDM/2018/test/test.list outputFile=~/nobackup/signal_region/2018/pileup/test_skim_TrackQuality_{0}_ctau-{1}.root".format(mass,life)
		
		
		print(cmdxx)
		process1 = subprocess.Popen(cmdxx,shell=True, stdout=subprocess.PIPE)
		for line1 in iter(process1.stdout.readline,b''):
			print(line1)
		process1.stdout.close()
		process1.wait()
		#print(cmdx)
		#process = subprocess.Popen(cmdx,shell=True, stdout=subprocess.PIPE)
		#for line in iter(process.stdout.readline,b''):
		#	print(line)
		#process.stdout.close()
		#process.wait()
