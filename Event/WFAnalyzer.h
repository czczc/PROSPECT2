#ifndef WFANALYZER_H
#define WFANALYZER_H

class DAQEvent;

class WFAnalyzer {
public:
    WFAnalyzer(DAQEvent* event);
    virtual ~WFAnalyzer();
    
    void Process();
    void ProcessPMT();
    void Reset();
    
    DAQEvent* fEvent;

    double baseline;
    int nPulses;
    double maxCharge;
    double riseTime;
    double totalCharge;

    // int nPulses;
    // vector<double> charge;

};

#endif