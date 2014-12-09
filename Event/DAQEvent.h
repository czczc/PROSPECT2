#ifndef MCEVENT_H
#define MCEVENT_H

#include "TTimeStamp.h"

class TTree;
class TFile;

class DAQEvent
{
public:
    DAQEvent(const char* dataFileName="");
    virtual ~DAQEvent();

    TTree* EventTree()   { return eventTree; }
    TTree* HeaderTree() { return header; }
    void InitBranchAddress();
    void GetEntry(int entry);
    void Reset();
    void PrintInfo(int level=0);

    TFile *rootFile;
    TTree *header;
    TTree *eventTree;

    enum LIMITS {
        MAX_SAMPLE = 252,
    };

    int nEvents;
    int currentEventEntry;

    // eventTree leafs
    int eventNo;
    double triggerTime;
    unsigned short ch0[MAX_SAMPLE];
    TTimeStamp ts; 

};

#endif