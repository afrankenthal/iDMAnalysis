import FWCore.ParameterSet.Config as cms

trigeffiana = cms.EDAnalyzer('TrigEffiAnalyzer',
    _trigResults = cms.InputTag("TriggerResults","","HLT"),
    _trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    _genParticles = cms.InputTag('genParticles'),
    _saMuons = cms.InputTag("standAloneMuons"),
    _rsaMuons = cms.InputTag("refittedStandAloneMuons"),
    _dsaMuons = cms.InputTag("displacedStandAloneMuons"),
    _globalMuons = cms.InputTag("globalMuons"),
    _dgMuons = cms.InputTag("displacedGlobalMuons"),
    _muons = cms.InputTag("muons"),
    _trigPaths = cms.untracked.vstring(
        # 'DST_DoubleMu3_noVtx_CaloScouting_v4',
        # 'HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx_v4',
        'HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx_v10',
        # 'HLT_TrkMu17_DoubleTrkMu8NoFiltersNoVtx_v11'
    ),
    _trigFilters = cms.untracked.vstring(
        # 'hltDoubleMu3L3FilteredNoVtx',
        # 'hltL3pfL1sDoubleMu155ORTripleMu444L1f0L2pf0TwoMuL3PreFiltered5NoVtx',
        'hltL3pfL1sDoubleMu155ORTripleMu444L1f0L2pf0TwoMuL3PreFiltered6NoVtx',
        # 'hltL3pfL1sDoubleMu155ORTripleMu444L1f0L2pf0TwoMuL3PreFiltered8NoVtx'
    )
)
