#include "JetAnalyzer.h"

JetAnalyzer::JetAnalyzer(edm::ParameterSet& PSet, edm::ConsumesCollector&& CColl):
    JetToken(CColl.consumes<std::vector<pat::Jet> >(PSet.getParameter<edm::InputTag>("jets"))),
    MetToken(CColl.consumes<std::vector<pat::MET> >(PSet.getParameter<edm::InputTag>("met"))),
    JetId(PSet.getParameter<int>("jetid")),
    Jet1Pt(PSet.getParameter<double>("jet1pt")),
    Jet2Pt(PSet.getParameter<double>("jet2pt")),
    BTag(PSet.getParameter<std::string>("btag")),
    Jet1BTag(PSet.getParameter<int>("jet1btag")),
    Jet2BTag(PSet.getParameter<int>("jet2btag"))
{
  
    isJESFile=false;
    
    JESFile=new TFile("data/JESUncertainty.root", "READ");
    JESFile->cd();
    if(!JESFile->IsZombie()) {
      hist=(TH2F*)JESFile->Get("test/AK5PFchs");
      isJESFile=true;
    }
    
    std::cout << " - JetAnalyzer initialized:" << std::endl;
    std::cout << "Id  :\t" << JetId << std::endl;
    std::cout << "pT  :\t" << Jet1Pt << "\t" << Jet2Pt << std::endl;
    std::cout << "bTag:\t" << Jet1BTag << "\t" << Jet2BTag << std::endl;
}

JetAnalyzer::~JetAnalyzer() {
    JESFile->Close();
}





std::vector<pat::Jet> JetAnalyzer::FillJetVector(const edm::Event& iEvent) {
    bool isMC(!iEvent.isRealData());
    int BTagTh(Jet1BTag);
    float PtTh(Jet1Pt);
    std::vector<pat::Jet> Vect;
    // Declare and open collection
    edm::Handle<std::vector<pat::Jet> > PFJetsCollection;
    iEvent.getByToken(JetToken, PFJetsCollection);
    // Loop on Jet collection
    for(std::vector<pat::Jet>::const_iterator it=PFJetsCollection->begin(); it!=PFJetsCollection->end(); ++it) {
        if(Vect.size()>0) {
            PtTh=Jet2Pt;
            BTagTh=Jet2BTag;
        }
        pat::Jet jet=*it;
        int idx=it-PFJetsCollection->begin();
        jet.addUserInt("Index", idx);
        // Jet Energy Smearing
        if(isMC) {
            const reco::GenJet* genJet=jet.genJet();
            if(genJet) {
                float smearFactor=GetResolutionRatio(jet.eta());
                reco::Candidate::LorentzVector smearedP4;
                smearedP4=jet.p4()-genJet->p4();
                smearedP4*=smearFactor; // +- 3*smearFactorErr;
                smearedP4+=genJet->p4();
                jet.setP4(smearedP4);
            }
        }
        // Pt and eta cut
        if(jet.pt()<PtTh || fabs(jet.eta())>2.4) continue;
        // Quality cut
        //if(JetId==1 && !isLooseJet(jet)) continue;
        // b-tagging
        if(BTagTh==1 && jet.bDiscriminator(BTag)<BTagTh) continue;
        // Fill jet scale uncertainty
        jet.addUserFloat("JESUncertainty", jet.pt()*GetScaleUncertainty(jet));
        Vect.push_back(jet); // Fill vector
    }
    return Vect;
}


pat::MET JetAnalyzer::FillMetVector(const edm::Event& iEvent) {
    
    edm::Handle<std::vector<pat::MET> > MetCollection;
    iEvent.getByToken(MetToken, MetCollection);
    pat::MET MEt = MetCollection->front();
    
    return MEt;
}


float JetAnalyzer::GetScaleUncertainty(pat::Jet& jet) {
    if(!isJESFile) return 1.;
    float pt(jet.pt()), eta(fabs(jet.eta()));
    return hist->GetBinContent(hist->FindBin(pt, eta));
}

// https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
float JetAnalyzer::GetResolutionRatio(float eta) {
    eta=fabs(eta);
    if(eta>=0.0 && eta<0.5) return 1.052; // +-0.012+0.062-0.061
    if(eta>=0.5 && eta<1.1) return 1.057; // +-0.012+0.056-0.055
    if(eta>=1.1 && eta<1.7) return 1.096; // +-0.017+0.063-0.062
    if(eta>=1.7 && eta<2.3) return 1.134; // +-0.035+0.087-0.085
    if(eta>=2.3 && eta<5.0) return 1.288; // +-0.127+0.155-0.153
    return -1.;
}
float JetAnalyzer::GetResolutionErrorUp(float eta) {
    eta=fabs(eta);
    if(eta>=0.0 && eta<0.5) return 1.115;
    if(eta>=0.5 && eta<1.1) return 1.114;
    if(eta>=1.1 && eta<1.7) return 1.161;
    if(eta>=1.7 && eta<2.3) return 1.228;
    if(eta>=2.3 && eta<5.0) return 1.488;
    return -1.;
}
float JetAnalyzer::GetResolutionErrorDown(float eta) {
    eta=fabs(eta);
    if(eta>=0.0 && eta<0.5) return 0.990;
    if(eta>=0.5 && eta<1.1) return 1.001;
    if(eta>=1.1 && eta<1.7) return 1.032;
    if(eta>=1.7 && eta<2.3) return 1.042;
    if(eta>=2.3 && eta<5.0) return 1.089;
    return -1.;
}

// PFJet Quality ID: see https://twiki.cern.ch/twiki/bin/view/CMS/JetID
bool JetAnalyzer::isLooseJet(pat::Jet& jet) {
    if(jet.neutralHadronEnergyFraction()>0.99) return false;
    if(jet.neutralEmEnergyFraction()>0.99) return false;
    if(jet.numberOfDaughters()<=1) return false;
    if(fabs(jet.eta())<2.4) {
      if(jet.chargedHadronEnergyFraction()<=0.) return false;
      if(jet.chargedEmEnergyFraction()>0.99) return false;
      if(jet.chargedMultiplicity()<=0) return false;
    }
    return true;
}
bool JetAnalyzer::isMediumJet(pat::Jet& jet) {
    if(jet.neutralHadronEnergyFraction()>0.95) return false;
    if(jet.neutralEmEnergyFraction()>0.95) return false;
    if(jet.numberOfDaughters()<=1) return false;
    if(fabs(jet.eta())<2.4) {
      if(jet.chargedHadronEnergyFraction()<=0.) return false;
      if(jet.chargedEmEnergyFraction()>0.99) return false;
      if(jet.chargedMultiplicity()<=0) return false;
    }
    return true;
}
bool JetAnalyzer::isTightJet(pat::Jet& jet) {
    if(jet.neutralHadronEnergyFraction()>0.90) return false;
    if(jet.neutralEmEnergyFraction()>0.90) return false;
    if(jet.numberOfDaughters()<=1) return false;
    if(fabs(jet.eta())<2.4) {
      if(jet.chargedHadronEnergyFraction()<=0.) return false;
      if(jet.chargedEmEnergyFraction()>0.99) return false;
      if(jet.chargedMultiplicity()<=0) return false;
    }
    return true;
}




