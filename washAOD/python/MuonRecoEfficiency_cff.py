import FWCore.ParameterSet.Config as cms

from Firefighter.washAOD.MuonRecoEfficiency_cfi import muonrecoeffi

globalMuon_recoEffi = muonrecoeffi.clone(
        _muTrack = cms.InputTag('globalMuTrackCands')
        )
displacedGlobalMuon_recoEffi = muonrecoeffi.clone(
        _muTrack = cms.InputTag("dgMuTrackCands")
        )
standAloneMuon_recoEffi = muonrecoeffi.clone(
        _muTrack = cms.InputTag("saMuTrackCands")
        )
displacedStandAloneMuon_recoEffi = muonrecoeffi.clone(
        _muTrack = cms.InputTag("dsaMuTrackCands")
        )
refittedStandAloneMuon_recoEffi = muonrecoeffi.clone(
        _muTrack = cms.InputTag("rsaMuTrackCands")
        )
standAloneMuonUpdatedAtVertex_recoEffi = muonrecoeffi.clone(
        _muTrack = cms.InputTag("saMuUAVTrackCands")
        )
cosmicMuon_recoEffi = muonrecoeffi.clone(
        _muTrack = cms.InputTag("cosmicMuTrackCands")
        )
cosmicMuon1Leg_recoEffi = muonrecoeffi.clone(
        _muTrack = cms.InputTag("cosmicMu1LegTrackCands")
        )

makeMuTrackEfficiencyFromMatched = cms.Sequence(
        globalMuon_recoEffi
       +displacedGlobalMuon_recoEffi
       +standAloneMuon_recoEffi
       +displacedStandAloneMuon_recoEffi
       +refittedStandAloneMuon_recoEffi
       +standAloneMuonUpdatedAtVertex_recoEffi
       +cosmicMuon_recoEffi
       +cosmicMuon1Leg_recoEffi
       )
