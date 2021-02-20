# Produce DeepCSV b-tagging information for 2016 samples

from RecoBTag.Configuration.RecoBTag_cff import *
from RecoBTag.SoftLepton.SoftLeptonByMVAComputers_cff import *
from RecoBTag.TensorFlow.pfDeepDoubleX_cff import *

def addBTaggingAK4CHS(process,jets='ak4PFJetsCHS',cone=0.4,head='AK4',tail='CHS',useMiniAOD=False,dropSub=False):
    setattr(process, 'pfImpactParameterTagInfos', 
            pfImpactParameterTagInfos.clone(
            jets      = cms.InputTag(jets),
            maxDeltaR = cms.double(cone)
            ))
    setattr(process, 'pfInclusiveSecondaryVertexFinderTagInfos',
            pfInclusiveSecondaryVertexFinderTagInfos.clone(
            trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfos")
            ))
    setattr(process, 'pfSecondaryVertexTagInfos',
            pfSecondaryVertexTagInfos.clone(
            trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfos")
            ))
    setattr(process, 'pfInclusiveSecondaryVertexFinderCvsLTagInfos', 
            pfInclusiveSecondaryVertexFinderCvsLTagInfos.clone(
            trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfos")
            ))
    #Soft Lepton Info
    setattr(process, 'softPFMuonsTagInfos',     softPFMuonsTagInfos    .clone(jets=cms.InputTag(jets)))
    setattr(process, 'softPFElectronsTagInfos', softPFElectronsTagInfos.clone(jets=cms.InputTag(jets)))
    setattr(process, 'softPFMuonBJetTags',      softPFMuonBJetTags     .clone(tagInfos=cms.VInputTag('softPFMuonsTagInfos')))
    setattr(process, 'softPFElectronBJetTags',  softPFElectronBJetTags .clone(tagInfos=cms.VInputTag('softPFElectronsTagInfos')))
    #B-taggers
    setattr(process, 'pfDeepCSVTagInfos',
            pfDeepCSVTagInfos.clone(
            svTagInfos = cms.InputTag('pfInclusiveSecondaryVertexFinderTagInfos')
            )),
    setattr(process, 'pfDeepCSVJetTags',
            pfDeepCSVJetTags.clone(
            src = cms.InputTag('pfDeepCSVTagInfos'),
            ))
    setattr(process, 'pfDeepCMVATagInfos',
            pfDeepCMVATagInfos.clone(
            deepNNTagInfos = cms.InputTag('pfDeepCSVTagInfos'),
            ipInfoSrc = cms.InputTag("pfImpactParameterTagInfos"),
            muInfoSrc = cms.InputTag("softPFMuonsTagInfos"),
            elInfoSrc = cms.InputTag("softPFElectronsTagInfos"),
            ))
    setattr(process, 'pfDeepCMVAJetTags',  
            pfDeepCMVAJetTags.clone(
            src = cms.InputTag('pfDeepCMVATagInfos')
            ))
    process.btagging *= getattr(process,'pfImpactParameterTagInfos')
    process.btagging *= getattr(process,'pfSecondaryVertexTagInfos')
    process.btagging *= getattr(process,'pfInclusiveSecondaryVertexFinderTagInfos')
    process.btagging *= getattr(process,'pfInclusiveSecondaryVertexFinderCvsLTagInfos')
    process.btagging *= getattr(process,'softPFMuonsTagInfos')
    process.btagging *= getattr(process,'softPFElectronsTagInfos')
    process.btagging *= getattr(process,'softPFMuonBJetTags')
    process.btagging *= getattr(process,'softPFElectronBJetTags')
    process.btagging *= getattr(process,'pfDeepCSVTagInfos')
    process.btagging *= getattr(process,'pfDeepCSVJetTags')
    process.btagging *= getattr(process,'pfDeepCMVATagInfos')
    process.btagging *= getattr(process,'pfDeepCMVAJetTags')
