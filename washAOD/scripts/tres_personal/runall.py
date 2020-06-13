import os, sys
import subprocess


test = True
if test:
	mchis=['6p0']#['60p0']
	dmchis=['2p0']#['20p0']
	lifelist=[1]
	masslist = ["Mchi-%s_dMchi-%s"%(mchi,dmchi) for mchi,dmchi in zip(mchis,dmchis)]

else:
	lifelist = [1,10,100,1000]
	#lifelist = [100]
	mchis=['52p5']
	dmchis=['5p0']
	#mchis=['5p25']
	#dmchis=['0p5']
	#mchis=['6p0']
	#dmchis=['2p0']
	#mchis=['60p0']
	#dmchis=['20p0']
	#mchis=['6p0','60p0','5p25','52p5']
	#dmchis=['2p0','20p0','0p5','5p0']
	masslist = ["Mchi-%s_dMchi-%s"%(mchi,dmchi) for mchi,dmchi in zip(mchis,dmchis)]

print masslist

odir = "/uscmst1b_scratch/lpc1/3DayLifetime/mireid/v4"
#makedir = subprocess.Popen("mkdir /uscmst1b_scratch/lpc1/3DayLifetime/mireid",shell=True,stdout=subprocess.PIPE)
makedir = subprocess.Popen("mkdir %s"%odir,shell=True,stdout=subprocess.PIPE)
makedir.wait()
#analyzer ='MetTrigSelfEffiForMuTrack_cfg.py'
analyzer ='iDMAnalyzer_cfg.py'
#analyzer='MuRecoEffi_cfg.py'
analyzer_name = {'MetTrigSelfEffiForMuTrack_cfg.py': 'MetTrigStudy_signal_gen_v8',
		'iDMAnalyzer_cfg.py': 'iDMAnalysis_seventhrunv2',
		'MuRecoEffi_cfg.py': 'MuRecoEffi'}
for mass in masslist:
	for life in lifelist:	
	#	odir = "/uscmst1b_scratch/lpc1/3DayLifetime/mireid/v3"
		ofile= "{2}_{0}_ctau-{1}.root".format(mass,life,analyzer_name[analyzer])
		if test:
			#cmdxx = "cmsRun ../python/{1} year=2018 inputFiles=/store/mc/RunIIAutumn18DRPremix/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v1/90000/E2ECFFD9-B0C9-E04A-99A4-31BB259AAD98.root outputFile=test_2018".format(ofile,analyzer)#,mass,life)
			#cmdxx = "cmsRun ../python/{1} year=2018 inputFiles=/store/mc/RunIIAutumn18DRPremix/ZZ_TuneCP5_13TeV-pythia8/AODSIM/102X_upgrade2018_realistic_v15-v2/80000/FD02DD57-1A67-A049-872F-6627567DA620.root outputFile=test_2018".format(ofile,analyzer)#,mass,life)
			#cmdxx = "cmsRun ../python/{1} year=2017 inputFiles=/store/mc/RunIIFall17DRPremix/WW_TuneCP5_13TeV-pythia8/AODSIM/PU2017_94X_mc2017_realistic_v11-v1/1110000/F216796B-8BD3-E811-816F-008CFA197AEC.root outputFile=test_2017".format(ofile,analyzer)#,mass,life)
			cmdxx = "cmsRun ../python/{1} year=2016 inputFiles=/store/mc/RunIISummer16DR80Premix/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/AODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/80000/BAAB4424-B6C9-E611-A731-901B0E5427B6.root outputFile=test_2016".format(ofile,analyzer)#,mass,life)
			#cmdxx = "cmsRun ../python/{1} year=2018 inputFiles=/store/data/Run2018A/SingleMuon/AOD/17Sep2018-v2/90000/F8382E90-9BC4-4E4A-8EB2-952904BAE395.root outputFile=test_{0}".format(ofile,analyzer)#,mass,life)
			#cmdxx = "cmsRun ../python/{1} year=2018 inputFiles_load=../data/iDM/2018/signal/test.list outputFile=test_{0}".format(ofile,analyzer)#,mass,life)
		
			print(cmdxx)
			process1 = subprocess.Popen(cmdxx,shell=True, stdout=subprocess.PIPE)
			for line1 in iter(process1.stdout.readline,b''):
				print(line1)
			process1.stdout.close()
			process1.wait()
		else:
			cmdxx = "cmsRun ../python/{4} year=2018 inputFiles_load=../data/iDM/2018/signal/{2}_ctau-{3}.list outputFile={0}/{1}".format(odir,ofile,mass,life,analyzer)
		
			print(cmdxx)
			process1 = subprocess.Popen(cmdxx,shell=True, stdout=subprocess.PIPE)
			for line1 in iter(process1.stdout.readline,b''):
				print(line1)
			process1.stdout.close()
			process1.wait()

			#transfer = "xrdcp {0}/{1} root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/signal/reco_effi/{2}_ctau-{3}/{1}".format(odir,ofile,mass,life)
			transfer = "xrdcp {0}/{1} root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/signal/{4}/{2}_ctau-{3}/{1}".format(odir,ofile,mass,life,analyzer_name[analyzer])
			print(transfer)
                	process2 = subprocess.Popen(transfer,shell=True, stdout=subprocess.PIPE)
                	for line2 in iter(process2.stdout.readline,b''):
                        	print(line2)
                	process2.stdout.close()
                	process2.wait()
