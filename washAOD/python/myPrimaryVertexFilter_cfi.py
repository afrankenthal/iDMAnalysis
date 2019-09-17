import FWCore.ParameterSet.Config as cms

myPrimaryVertexFilter = cms.EDFilter("myGoodVertexFilter",
        vertexCollection = cms.InputTag('offlinePrimaryVertices'),
        minimumNDOF = cms.uint32(4) ,
        maxAbsZ = cms.double(24), 
        maxd0 = cms.double(2),
        taggingMode = cms.bool(True)
        )
