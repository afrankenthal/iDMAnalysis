#define EDM_ML_DEBUG
// system include files
#include <memory>
#include <map>
#include <algorithm>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "TH1I.h"
#include "TH1F.h"
#include "TTree.h"

class MuAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>
{
public:
	explicit MuAnalyzer(const edm::ParameterSet&);
	~MuAnalyzer();

	static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    struct theMu
      {
        theMu() {};

        template<typename T>
        theMu(const T& m) {
          _pt = m.pt();
          _eta = m.eta();
          _phi = m.phi();
          _dxy = std::sqrt(m.vx()*m.vx() + m.vy()*m.vy());
          if (m.isStandAloneMuon()) {
            _saDxy = std::sqrt(m.standAloneMuon()->vx()*m.standAloneMuon()->vx()
                              +m.standAloneMuon()->vy()*m.standAloneMuon()->vy());
          } else {_saDxy = -1;}
        }

        double _pt, _eta, _phi, _dxy, _saDxy;
      };

private:
	virtual void beginJob() override;
	virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
	virtual void endJob() override;

  void branchTTree(TTree* t, theMu& m);

	edm::Service<TFileService> fs;
	const edm::EDGetTokenT<pat::MuonCollection> muons;

	TH1I *hNMuons, *hMuonType, *hMuIdAlgo;
  std::map<std::string, theMu> _basicMu;
  std::map<std::string, TTree*> _basicMuTree;
	
};

void
MuAnalyzer::branchTTree(TTree* t, theMu& m) {
  t->Branch("pt",  &m._pt,  "pt/D");
  t->Branch("eta", &m._eta, "eta/D");
  t->Branch("phi", &m._phi, "phi/D");
  t->Branch("dxy", &m._dxy, "dxy/D");
  t->Branch("saDxy", &m._saDxy, "saDxy/D");
}



MuAnalyzer::MuAnalyzer(const edm::ParameterSet& iC)
:
muons(consumes<pat::MuonCollection>(iC.getParameter<edm::InputTag>("_MuTag")))
{
	usesResource("TFileService");
}

MuAnalyzer::~MuAnalyzer()
{

}

void
MuAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
	Handle<pat::MuonCollection> muonsH;
	iEvent.getByToken(muons, muonsH);

	hNMuons->Fill(muonsH->size());
	for (const auto& mu : *muonsH) {

		// Which type of muons in the collection?
		unsigned int _muType(0);
    if(mu.isStandAloneMuon()) {
    	if(mu.isGlobalMuon()) {
    		if(mu.isTrackerMuon()) _muType=1; // STA + GLB + TM
				else _muType=2; // STA + GLB
    	}
      else {
				if(mu.isTrackerMuon()) _muType=3;  // STA + TM
				else _muType=5; // STA
    	}
    }
    else {
			if(mu.isTrackerMuon()) _muType=4; // TM
		}
		hMuonType->Fill(_muType);

    // if (_muType==1) {
    //   _basicMu["sa_glb_tkm"] = theMu(mu);
    //   _basicMuTree["sa_glb_tkm"]->Fill();
    // }
    // else if ()
    _basicMu["total"] = theMu(mu);
    _basicMuTree["total"]->Fill();
    switch(_muType) {
      case 1:
      {
        _basicMu["sa_glb_tkm"] = theMu(mu);
        _basicMuTree["sa_glb_tkm"]->Fill();
      }
      case 5:
      {
        _basicMu["saOnly"] = theMu(mu);
        _basicMuTree["saOnly"]->Fill();
      }
    }
	
		// Muon ID selection. As described in AN-2008/098
    if(muon::isGoodMuon(mu, muon::All))                                // dummy options - always true
      hMuIdAlgo->Fill(0);
    if(muon::isGoodMuon(mu, muon::AllStandAloneMuons))                 // checks isStandAloneMuon flag
      hMuIdAlgo->Fill(1);
    if(muon::isGoodMuon(mu, muon::AllTrackerMuons))                    // checks isTrackerMuon flag
      hMuIdAlgo->Fill(2);
    if(muon::isGoodMuon(mu, muon::TrackerMuonArbitrated))              // resolve ambiguity of sharing segments
      hMuIdAlgo->Fill(3);
    if(muon::isGoodMuon(mu, muon::AllArbitrated))                      // all muons with the tracker muon arbitrated
      hMuIdAlgo->Fill(4);
    if(muon::isGoodMuon(mu, muon::GlobalMuonPromptTight))              // global muons with tighter fit requirements
      hMuIdAlgo->Fill(5);
    if(muon::isGoodMuon(mu, muon::TMLastStationLoose))                 // penetration depth loose selector
      hMuIdAlgo->Fill(6);
    if(muon::isGoodMuon(mu, muon::TMLastStationTight))                 // penetration depth tight selector
      hMuIdAlgo->Fill(7);
    if(muon::isGoodMuon(mu, muon::TM2DCompatibilityLoose))             // likelihood based loose selector
      hMuIdAlgo->Fill(8);
    if(muon::isGoodMuon(mu, muon::TM2DCompatibilityTight))             // likelihood based tight selector
      hMuIdAlgo->Fill(9);
    if(muon::isGoodMuon(mu, muon::TMOneStationLoose))                  // require one well matched segment
      hMuIdAlgo->Fill(10);
    if(muon::isGoodMuon(mu, muon::TMOneStationTight))                  // require one well matched segment
      hMuIdAlgo->Fill(11);
    if(muon::isGoodMuon(mu, muon::TMLastStationOptimizedLowPtLoose))   // combination of TMLastStation and TMOneStation
      hMuIdAlgo->Fill(12);
    if(muon::isGoodMuon(mu, muon::TMLastStationOptimizedLowPtTight))   // combination of TMLastStation and TMOneStation
			hMuIdAlgo->Fill(13);


	}

}

void
MuAnalyzer::beginJob()
{
	hNMuons = fs->make<TH1I>("nMuons", "Number of muons per event;N_{#mu};A.U.", 10, 0, 10);
	hMuonType = fs->make<TH1I>("MuonType", "Type of muons;Type;A.U.", 5, 1, 6);
	hMuIdAlgo = fs->make<TH1I>("muonIdSelectors", "Result of muon id selectors;Id;A.U.", 13, 0, 13);

  _basicMuTree["total"] = fs->make<TTree>("Total", "");
  branchTTree(_basicMuTree["total"], _basicMu["total"]);

  _basicMuTree["saOnly"] = fs->make<TTree>("standAloneMuonsOnly", "");
  branchTTree(_basicMuTree["saOnly"], _basicMu["saOnly"]);

  _basicMuTree["sa_glb_tkm"] = fs->make<TTree>("standaloneGlobalTrackerMuon", "");
  branchTTree(_basicMuTree["sa_glb_tkm"], _basicMu["sa_glb_tkm"]);
}

void
MuAnalyzer::endJob()
{
	// hNMuons->Scale(1./hNMuons->Integral(), "width");
	// hMuonType->Scale(1./hMuonType->Integral(), "width");
	// hMuIdAlgo->Scale(1./hMuIdAlgo->Integral(), "width");
}

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
void
MuAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
	edm::ParameterSetDescription desc;
	desc.add<edm::InputTag>("_MuTag", edm::InputTag("slimmedMuons"));

	descriptions.add("muana", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(MuAnalyzer);