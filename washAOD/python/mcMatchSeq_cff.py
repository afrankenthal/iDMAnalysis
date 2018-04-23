import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.mcMatchLayer0.muonMatch_cfi import muonMatch

muonmatch = muonMatch.clone()

samumatch = muonMatch.clone(
    src=cms.InputTag("saMuTrackCands")
)
samuUAVmatch = muonMatch.clone(
    src=cms.InputTag("saMuUAVTrackCands")
)
dsamumatch = muonMatch.clone(
    src=cms.InputTag("dsaMuTrackCands")
)
rsamumatch = muonMatch.clone(
    src=cms.InputTag("rsaMuTrackCands")
)
globalmumatch = muonMatch.clone(
    src=cms.InputTag("globalMuTrackCands")
)
dgmumatch = muonMatch.clone(
    src=cms.InputTag("dgMuTrackCands")
)

mergedMCMatch = cms.EDProducer("GenParticleMatchMerger",
                               src=cms.VInputTag("muonmatch", "samumatch", "dsamumatch",
                                                 "rsamumatch", "globalmumatch",
                                                 "dgmumatch", "samuUAVmatch")
                               )
mcMatch = cms.Sequence(
    (muonmatch +
     samumatch +
     samuUAVmatch +
     dsamumatch +
     rsamumatch +
     globalmumatch +
     dgmumatch) * mergedMCMatch
)
