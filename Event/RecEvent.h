#ifndef RECEVENT_H
#define RECEVENT_H

#include "TString.h"

class TFile;
class TTree;
class DAQEvent;
class WFAnalyzer;

class RecEvent
{
public:
    RecEvent();
    virtual ~RecEvent();

    void LoadData();
    void InitOutputTree();
    void Process();
    // void PrintInfo();

    TTree *fRecTree;

    double triggerTime;
    double dtLastTrigger;
    double baseline;
    int nPulses;
    double maxCharge;
    double riseTime;
    double totalCharge;

    DAQEvent *fDAQEvent;
    WFAnalyzer *fWFAnalyzer;
    TString fD2rFileName;
    TString fOutputName;
    TFile *fOutput;
};

#endif