#include "DAQEvent.h"

#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"

using namespace std;

DAQEvent::DAQEvent(const char* dataFileName)
{
    ch0 = new vector<unsigned short>;
    ch1 = new vector<unsigned short>;
    ch2 = new vector<unsigned short>;

    currentEventEntry = -1; // don't load event at initialization

    rootFile = new TFile(dataFileName);
    if (rootFile->IsZombie()) {
        cout << "Data file " << dataFileName << " Does NOT exist! exiting ..." << endl;
        exit(1);
    }
    eventTree = (TTree*)rootFile->Get("Event");
    if (!eventTree) {
        cout << "TTee /Event does NOT exist! exiting ..." << endl;
        exit(1);
    }

    nEvents = eventTree->GetEntries();
    cout << "total events: " << nEvents << endl;

    InitBranchAddress();
}

//----------------------------------------------------------------
DAQEvent::~DAQEvent()
{
    rootFile->Close();
    delete rootFile;
}

//----------------------------------------------------------------
void DAQEvent::InitBranchAddress()
{
    eventTree->SetBranchAddress("eventNo" , &eventNo);
    eventTree->SetBranchAddress("triggerTime" , &triggerTime);
    eventTree->SetBranchAddress("ch0" , &ch0);
    eventTree->SetBranchAddress("ch1" , &ch1);
    eventTree->SetBranchAddress("ch2" , &ch2);
}

//----------------------------------------------------------------
void DAQEvent::Reset()
{
}

//----------------------------------------------------------------
void DAQEvent::GetEntry(int entry)
{
    Reset();
    eventTree->GetEntry(entry);

    ts.SetSec(int(triggerTime));
    ts.SetNanoSec( int((triggerTime-ts.GetSec())*1e9) );

    currentEventEntry = entry;
}

//----------------------------------------------------------------
void DAQEvent::PrintInfo(int level)
{
    // cout << "event: (" << eventNo << "/" << nEvents << ")" << endl;
    cout << "event " << eventNo  
         << " @ " << ts 
         << endl;
    // print waveform
    if (level>0) {
        int size = (*ch0).size();
        for (int i=0; i<size; i++) {
            cout << (*ch0)[i] << " ";
        }
        cout << endl;
    }
    cout << "-----------" << endl;
}


