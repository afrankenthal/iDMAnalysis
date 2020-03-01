from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff import *
from RecoMuon.MuonIdentification.muons1stStep_cfi import *

muonsFromdSA = muons1stStep.clone()
muonsFromdSA.inputCollectionLabels = ["displacedStandAloneMuons"]
muonsFromdSA.inputCollectionTypes = ["outer tracks"]
muonsFromdSA.TrackExtractorPSet.inputTrackCollection = "displacedStandAloneMuons"
muonsFromdSA.TimingFillerParameters.DTTimingParameters.PruneCut = 9999
muonsFromdSA.TimingFillerParameters.CSCTimingParameters.PruneCut = 9999
muonsFromdSA.TrackAssociatorParameters.EERecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEE")
muonsFromdSA.TrackAssociatorParameters.EBRecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEB")
muonsFromdSA.TrackAssociatorParameters.HBHERecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "hbhereco")
muonsFromdSA.TrackAssociatorParameters.HORecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "horeco")
muonsFromdSA.TrackAssociatorParameters.useCalo = cms.bool(False)

muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.EERecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEE")
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.EBRecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEB")
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.HBHERecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "hbhereco")
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.HORecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "horeco")
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useCalo = cms.bool(False)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useEcal = cms.bool(True)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useGEM = cms.bool(False)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useHO = cms.bool(True)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useHcal = cms.bool(True)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useME0 = cms.bool(False)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.useMuon = cms.bool(True)
muonsFromdSA.CaloExtractorPSet.TrackAssociatorParameters.usePreshower = cms.bool(False)

muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.EERecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEE")
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.EBRecHitCollectionLabel = cms.InputTag("reducedEcalRecHitsEB")
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.HBHERecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "hbhereco")
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.HORecHitCollectionLabel = cms.InputTag("reducedHcalRecHits", "horeco")
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useCalo = cms.bool(False)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useEcal = cms.bool(True)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useGEM = cms.bool(False)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useHO = cms.bool(True)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useHcal = cms.bool(True)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useME0 = cms.bool(False)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.useMuon = cms.bool(True)
muonsFromdSA.JetExtractorPSet.TrackAssociatorParameters.usePreshower = cms.bool(False)

muonsFromdSA.fillIsolation = False
muonsFromdSA.fillGlobalTrackQuality = False
muonsFromdSA.fillGlobalTrackRefits = False
muonsFromdSA.writeIsoDeposits = False
muonsFromdSA.arbitrateTrackerMuons = False
muonsFromdSA.fillTrackerKink = False
muonsFromdSA.runArbitrationCleaner = False
