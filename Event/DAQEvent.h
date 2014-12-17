#ifndef MCEVENT_H
#define MCEVENT_H

#include <vector>
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
    std::vector<unsigned short> *ch0;  // pmt
    std::vector<unsigned short> *ch1;  // mu paddle b0tr1
    std::vector<unsigned short> *ch2;  // mu paddle b0tr4
    TTimeStamp ts; 

};

#endif