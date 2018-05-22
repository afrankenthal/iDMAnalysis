import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.mcMatchLayer0.muonMatch_cfi import muonMatch

muonmatch = muonMatch.clone(
    maxDPtRel = cms.double(1.0)
)

samumatch = muonmatch.clone(
    src=cms.InputTag("saMuTrackCands")
)
samuUAVmatch = muonmatch.clone(
    src=cms.InputTag("saMuUAVTrackCands")
)
dsamumatch = muonmatch.clone(
    src=cms.InputTag("dsaMuTrackCands")
)
rsamumatch = muonmatch.clone(
    src=cms.InputTag("rsaMuTrackCands")
)
globalmumatch = muonmatch.clone(
    src=cms.InputTag("globalMuTrackCands")
)
dgmumatch = muonmatch.clone(
    src=cms.InputTag("dgMuTrackCands")
)
cosmicmumatch = muonmatch.clone(
    src=cms.InputTag("cosmicMuTrackCands")
)
cosmicmu1legmumatch = muonmatch.clone(
    src=cms.InputTag("cosmicMu1LegTrackCands")
)

mergedMCMatch = cms.EDProducer("GenParticleMatchMerger",
                               src=cms.VInputTag("muonmatch", "samumatch", "dsamumatch",
                                                 "rsamumatch", "globalmumatch",
                                                 "dgmumatch", "samuUAVmatch",
                                                 "cosmicmumatch", "cosmicmu1legmumatch")
                               )
mcMatchFromTrackCands = cms.Sequence(
    (muonmatch +
     samumatch +
     samuUAVmatch +
     dsamumatch +
     rsamumatch +
     globalmumatch +
     dgmumatch +
     cosmicmumatch +
     cosmicmu1legmumatch) * mergedMCMatch
)