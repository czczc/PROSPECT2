#include "RecEvent.h"
#include "DAQEvent.h"
#include "WFAnalyzer.h"

#include "TTree.h"
#include "TString.h"
#include "TFile.h"

#include <iostream>

using namespace std;


RecEvent::RecEvent()
{
    LoadData();
    InitOutputTree();
}

RecEvent::~RecEvent()
{
}

//---------------------------------
void RecEvent::LoadData()
{
    fD2rFileName = "~/Projects/PROSPECT/PROSPECT2/PROSPECT2/data/d2r/muPaddle.root";
    fDAQEvent = new DAQEvent(fD2rFileName.Data());
    fWFAnalyzer = new WFAnalyzer(fDAQEvent);
}

//---------------------------------
void RecEvent::InitOutputTree()
{
    fOutputName = "rec.root";
    fOutput = new TFile(fOutputName.Data(), "recreate");

    fRecTree = new TTree("Rec", "reconstructed info");
    fRecTree->Branch("triggerTime", &triggerTime);
    fRecTree->Branch("dtLastTrigger", &dtLastTrigger);
    fRecTree->Branch("baseline", &baseline);
    fRecTree->Branch("nPulses", &nPulses);
    fRecTree->Branch("maxCharge", &maxCharge);
    fRecTree->Branch("riseTime", &riseTime);
    fRecTree->Branch("totalCharge", &totalCharge);

}

//---------------------------------
void RecEvent::Process()
{
    int nEvents = fDAQEvent->nEvents;
    double triggerTime_last = 0;

    for (int i=0; i<nEvents; i++) {
        fDAQEvent->GetEntry(i);
        triggerTime = fDAQEvent->triggerTime;
        dtLastTrigger = triggerTime - triggerTime_last;

        fWFAnalyzer->Process();
        baseline = fWFAnalyzer->baseline;
        nPulses = fWFAnalyzer->nPulses;
        maxCharge = fWFAnalyzer->maxCharge;
        riseTime = fWFAnalyzer->riseTime;
        totalCharge = fWFAnalyzer->totalCharge;

        fRecTree->Fill();

        triggerTime_last = triggerTime;
    }

    fRecTree->Write();
    fOutput->Close();
    cout << "rec tree saved to " << fOutputName << endl; 

}


