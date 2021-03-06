import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
#process.MessageLogger.cerr.threshold = 'ERROR'

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

# input
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/lustre/cmswork/pazzini/CMSSW_8_0_4/src/2455D4FC-A5F0-E511-88AC-0025905A48C0.root'
    )
)

#output
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("deleteme.root"),
    closeFileFast = cms.untracked.bool(True)
)

process.ntuple = cms.EDAnalyzer('Ntuple',
    electronSet = cms.PSet(
        electrons = cms.InputTag("slimmedElectrons"),
        vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
        electron1id = cms.int32(1), # 0: veto, 1: loose
        electron2id = cms.int32(1),
        electron1iso = cms.int32(1), # 0: veto, 1: standard
        electron2iso = cms.int32(1),
        electron1pt = cms.double(20.),
        electron2pt = cms.double(10.),
    ),
    muonSet = cms.PSet(
        muons = cms.InputTag("slimmedMuons"),
        vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
        muon1id = cms.int32(3), # 0: no selections, 1: loose, 2: soft, 3: tight
        muon2id = cms.int32(3),
        muon1iso = cms.int32(2), # 0: no selections, 1: loose, 2: tight
        muon2iso = cms.int32(2),
        muon1pt = cms.double(20.),
        muon2pt = cms.double(10.),
    ),
    photonSet = cms.PSet(
        photons = cms.InputTag("slimmedPhotons"),
        vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
        photon1id = cms.int32(1), # 0: veto, 1: loose, 2: medium, 3: tight
        photon2id = cms.int32(1),
        photon1iso = cms.int32(0),
        photon2iso = cms.int32(0),
        photon1pt = cms.double(20.),
        photon2pt = cms.double(10.),
    ),
    jetSet = cms.PSet(
        jets = cms.InputTag("slimmedJetsAK8"), #selectedPatJetsAK8PFCHSPrunedPacked
        jetid = cms.int32(1), # 0: no selection, 1: loose, 2: medium, 3: tight
        jet1pt = cms.double(20.),
        jet2pt = cms.double(20.),
        btag = cms.string("combinedSecondaryVertexBJetTags"),
        jet1btag = cms.int32(0), # 0: no selection, 1: loose, 2: medium, 3: tight
        jet2btag = cms.int32(0),
        met = cms.InputTag("slimmedMETs"),
    ),
    writeNElectrons = cms.int32(0),
    writeNMuons = cms.int32(0),
    writeNLeptons = cms.int32(2),
    writeNJets = cms.int32(2),
    verbose  = cms.bool(True),
)


#process.output = cms.OutputModule("PoolOutputModule",
#  splitLevel = cms.untracked.int32(0),
#  fileName = cms.untracked.string('deleteme.root'),
#  dataset = cms.untracked.PSet(
#    filterName = cms.untracked.string(''),
#    dataTier = cms.untracked.string('')
#  )
#)

process.p = cms.Path(process.ntuple)
