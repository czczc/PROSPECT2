#ifndef WFANALYZER_H
#define WFANALYZER_H

class DAQEvent;

class WFAnalyzer {
public:
    WFAnalyzer(DAQEvent* event);
    virtual ~WFAnalyzer();
    
    void Process();
    void ProcessPMT();

    DAQEvent* fEvent;
    double fBaseline;
    int fNPulses;
    vector<double> fCharge;

};

#endif