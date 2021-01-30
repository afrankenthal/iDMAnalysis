#ifndef NTUPLECONTAINER_HH
#define NTUPLECONTAINER_HH

#include <vector>
using std::vector;

#include <TTree.h>

class NtupleContainer {

public:
    NtupleContainer();
    virtual ~NtupleContainer();
    void SetRecoTree(TTree *tree);
    void SetGenTree(TTree *tree);
    void CreateTreeBranches();
    void ClearTreeBranches();

    // Trigger and event-level branches
    unsigned int fired_;
    unsigned long long eventNum_;
    unsigned long long runNum_;
    unsigned long long lumiSec_;

    uint32_t METFiltersFailBits_;

    // Gen branches
    
    // Pileup and gen weight
    int genpuobs_;
    int genputrue_;
    float genwgt_;
    int npv_;

    // Gen particles
    int nGen_;
    vector<int> genID_;
    // Only save hard-process gen particles
    //std::vector<bool> genHardProcess_; --> would always be 1 
    vector<int> genCharge_;
    vector<float> genPt_;
    vector<float> genEta_;
    vector<float> genPhi_;
    vector<float> genPz_;
    vector<float> genEn_;
    vector<float> genVxy_;
    vector<float> genVz_;
    vector<float> genMass_;
    
    // Gen jet
    vector<float> genJetPt_;
    vector<float> genJetEta_;
    vector<float> genJetPhi_;
    
    // Gen MET
    float genLeadMETPt_;
    float genLeadMETPhi_;

    // Reco branches

    // Reco dSA muon branches
    int recoNDSA_;
    int recoNGoodDSA_;
    vector<float> recoDSAPt_;
    vector<float> recoDSAPtError_;
    vector<float> recoDSAEta_;
    vector<float> recoDSAEtaError_;
    vector<float> recoDSAPhi_;
    vector<float> recoDSAPhiError_;
    vector<float> recoDSADxy_;
    vector<float> recoDSADxyError_;
    vector<float> recoDSADz_;
    vector<float> recoDSADzError_;
    vector<int> recoDSACharge_;
    vector<float> recoDSATrkChi2_;
    vector<float> recoDSATrkNumPlanes_;
    vector<float> recoDSATrkNumHits_;
    vector<float> recoDSATrkNumDTHits_;
    vector<float> recoDSATrkNumCSCHits_;
    vector<float> recoDSAInvBeta_;
    vector<float> recoDSAInvBetaErr_;
    vector<float> recoDSAFreeInvBeta_;
    vector<float> recoDSAFreeInvBetaErr_;
    vector<float> recoDSAtimeAtIpInOut_;
    vector<float> recoDSAtimeAtIpInOutErr_;
    vector<float> recoDSAtimeAtIpOutIn_;
    vector<float> recoDSAtimeAtIpOutInErr_;
    vector<float> recoDSAtimingNdof_;
    int recoDSAIdx0_;
    int recoDSAIdx1_;
    
    // Reco GM branches
    int recoNGM_;
    int recoNGoodGM_;
    vector<float> recoGMPt_;
    vector<float> recoGMPtError_;
    vector<float> recoGMEta_;
    vector<float> recoGMEtaError_;
    vector<float> recoGMPhi_;
    vector<float> recoGMPhiError_;
    vector<float> recoGMDxy_;
    vector<float> recoGMDxyError_;
    vector<float> recoGMDz_;
    vector<float> recoGMDzError_;
    vector<int> recoGMCharge_;
    vector<float> recoGMTrkChi2_;
    vector<float> recoGMTrkNumPlanes_;
    vector<float> recoGMTrkNumHits_;
    vector<float> recoGMTrkNumDTHits_;
    vector<float> recoGMTrkNumCSCHits_;
    vector<bool> recoGMIsPF_;
    vector<float> recoGMPFIso_;
    vector<float> recoGMTrkIso_;
    vector<float> recoGMTrkNumPixelHit_;
    vector<float> recoGMTrkNumTrkLayers_;

    // Selected muon branches
    int nSelectedMuons_;
    int recoNMatchedGBMvDSA_;
    vector<float> recoGMdSAdR_;
    vector<int> recoGMdSAmatch_;
    float recoGBMDSADr_;
    vector<float> selectedMuonsPt_;
    vector<float> selectedMuonsPtError_;
    vector<float> selectedMuonsEta_;
    vector<float> selectedMuonsEtaError_;
    vector<float> selectedMuonsPhi_;
    vector<float> selectedMuonsPhiError_;
    vector<float> selectedMuonsDxy_;
    vector<float> selectedMuonsDxyError_;
    vector<float> selectedMuonsDz_;
    vector<float> selectedMuonsDzError_;
    vector<int> selectedMuonsCharge_;
    
    // Reco electron branches
    int recoNElectron_;
    int recoNGoodElectron_;
    vector<float> recoElectronPt_;
    vector<float> recoElectronEta_;
    vector<float> recoElectronPhi_;
    vector<float> recoElectronVxy_;
    vector<float> recoElectronVz_;
    vector<int> recoElectronCharge_;
    vector<int> recoElectronIDResult_;
    
    // Reco photon branches
    int recoNPhoton_;
    int recoNGoodPhoton_;
    vector<float> recoPhotonPt_;
    vector<float> recoPhotonEta_;
    vector<float> recoPhotonPhi_;
    vector<int> recoPhotonIDResult_;

    // Vertex branches
    float pvx_;
    float pvy_;
    float pvz_;
    float recoVtxVxy_;
    float recoVtxVz_;
    float recoVtxSigmaVxy_;
    float recoVtxReducedChi2_;
    float recoVtxDr_;
    vector<float> dsadsa_recoVtxVxy_;
    vector<float> dsadsa_recoVtxVz_;
    vector<float> dsadsa_recoVtxSigmaVxy_;
    vector<float> dsadsa_recoVtxReducedChi2_;
    vector<float> dsadsa_recoVtxDr_;
    vector<float> gmgm_recoVtxVxy_;
    vector<float> gmgm_recoVtxVz_;
    vector<float> gmgm_recoVtxSigmaVxy_;
    vector<float> gmgm_recoVtxReducedChi2_;
    vector<float> gmgm_recoVtxDr_;
    vector<float> dsagm_recoVtxVxy_;
    vector<float> dsagm_recoVtxVz_;
    vector<float> dsagm_recoVtxSigmaVxy_;
    vector<float> dsagm_recoVtxReducedChi2_;
    vector<float> dsagm_recoVtxDr_;

    float recoMmumu_;

    // MET reco branches
    float recoPFMETPt_;
    float recoPFMETPhi_;
    float recoPFMETSmearingOnlyPt_;
    float recoPFMETSmearingOnlyPhi_;
    float recoPFMETCorrectedPt_;
    float recoPFMETCorrectedPhi_;
    float recoPFMETEEDeltaPx_;
    float recoPFMETEEDeltaPy_;
    float recoPFMETJESUpPt_;
    float recoPFMETJESUpPhi_;
    float recoPFMETJESDownPt_;
    float recoPFMETJESDownPhi_;
    float recoPFMETJERUpPt_;
    float recoPFMETJERUpPhi_;
    float recoPFMETJERDownPt_;
    float recoPFMETJERDownPhi_;
    float recoPFMETMuonEtFraction_;
    float recoCaloMETPt_;
    float recoCaloMETPhi_;
    float recoPFRecoilPt_;
    float recoPFRecoilPhi_;
    float recoDeltaPhiMETMu_;
    float recoDeltaPhiCorrectedMETMu_;

    // Jet reco branches
    int recoPFNJet_;
    int recoPFNPassIDJet_;
    int recoPFNHighPtJet_;
    vector<float> recoPFJetPt_;
    vector<float> recoPFJetEta_;
    vector<float> recoPFJetPhi_;
    vector<float> recoPFJetCorrectedPt_;
    vector<float> recoPFJetCorrectedEta_;
    vector<float> recoPFJetCorrectedPhi_;
    vector<float> recoPFJetCorrectedBTag_;
    vector<float> recoPFJetCorrectedCHEF_;
    vector<float> recoPFJetCorrectedNHEF_;
    vector<float> recoPFJetCorrectedCEEF_;
    vector<float> recoPFJetCorrectedNEEF_;
    vector<float> recoPFJetCorrectedNumDaughters_;
    vector<float> recoPFJetCorrectedChargedMultiplicity_;
    vector<float> recoPFJetCorrectedJESUpPt_;
    vector<float> recoPFJetCorrectedJESUpEta_;
    vector<float> recoPFJetCorrectedJESUpPhi_;
    vector<float> recoPFJetCorrectedJESDownPt_;
    vector<float> recoPFJetCorrectedJESDownEta_;
    vector<float> recoPFJetCorrectedJESDownPhi_;
    vector<float> recoPFJetCorrectedJERUpPt_;
    vector<float> recoPFJetCorrectedJERUpEta_;
    vector<float> recoPFJetCorrectedJERUpPhi_;
    vector<float> recoPFJetCorrectedJERDownPt_;
    vector<float> recoPFJetCorrectedJERDownEta_;
    vector<float> recoPFJetCorrectedJERDownPhi_;
    bool recoPFHEMFlag_;

    // MHT reco branch
    float MHTPt_;

    // DEPRECATED
    // uint32_t cuts_;
    // inline void setCutBit(int bit) { cuts_ |= (1 << bit); }
    // inline void clearCutBit(int bit) { cuts_ &= ~(1 << bit); }

private:
    // Reco and gen TTrees
    TTree * recoT;
    TTree * genT;
    bool isData_;

};


#endif // NTUPLECONTAINER_HH