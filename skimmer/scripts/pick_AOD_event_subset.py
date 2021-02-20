import FWCore.ParameterSet.Config as cms

# Give the process a name
process = cms.Process("PickEvent")

# Tell the process which files to use as the source
process.source = cms.Source ("PoolSource",
        fileNames = cms.untracked.vstring ("file:9EC66C90-C134-DC4F-B2D6-9737362ABDB4.root")
)

# tell the process to only run over 100 events (-1 would mean run over
#  everything
process.maxEvents = cms.untracked.PSet(
            input = cms.untracked.int32 (100)

)

# Tell the process what filename to use to save the output
process.Out = cms.OutputModule("PoolOutputModule",
         fileName = cms.untracked.string ("data_subset_test.root")
)

# make sure everything is hooked up
process.end = cms.EndPath(process.Out)
