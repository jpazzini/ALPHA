#ifndef JETANALYZER_H
#define JETANALYZER_H

#include <iostream>
#include <fstream>
#include <cmath>
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDConsumerBase.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "RecoParticleFlow/PFProducer/interface/Utils.h"
#include "DataFormats/BTauReco/interface/SoftLeptonTagInfo.h"
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "TFile.h"
#include "TH2.h"


class JetAnalyzer {
    public:
        JetAnalyzer(edm::ParameterSet&, edm::ConsumesCollector&&);
        ~JetAnalyzer();
        virtual std::vector<pat::Jet> FillJetVector(const edm::Event&);
        virtual pat::MET FillMetVector(const edm::Event&);
        virtual float GetScaleUncertainty(pat::Jet&);
        virtual float GetResolutionRatio(float);
        virtual float GetResolutionErrorUp(float);
        virtual float GetResolutionErrorDown(float);
        virtual bool isLooseJet(pat::Jet&);
        virtual bool isMediumJet(pat::Jet&);
        virtual bool isTightJet(pat::Jet&);
      
    private:
    
        edm::EDGetTokenT<std::vector<pat::Jet> > JetToken;
        edm::EDGetTokenT<std::vector<pat::MET> > MetToken;
        int JetId;
        float Jet1Pt, Jet2Pt;
        std::string BTag;
        int Jet1BTag, Jet2BTag;
        bool isJESFile;
        
        TFile* JESFile;
        TH2F* hist;
};

#endif
