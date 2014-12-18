#include "WFAnalyzer.h"
#include "DAQEvent.h"

#include "TMath.h"

#include <vector>
#include <iostream>
using namespace std;

WFAnalyzer::WFAnalyzer(DAQEvent *evt):
fEvent(evt),
baseline(0)
{
}

// ------------------------------------
WFAnalyzer::~WFAnalyzer()
{
    
}

// ------------------------------------
void WFAnalyzer::Process()
{
    ProcessPMT();
}

// ------------------------------------
void WFAnalyzer::ProcessPMT()
{
    // int trace[NSAMPLES];
    // for (int i=0; i< NSAMPLES; i++) {
    //     trace[i] = (*(fEvent->ch0))[i]; 
    // }

    unsigned short *trace = &(*(fEvent->ch0))[0];

    // calculate baseline, the larger value of average of 20 bins from (0, 20), (220, 240)
    const int NBINS_BASELINE = 20;
    double bl = TMath::Mean(NBINS_BASELINE, trace);
    double bl_tolerance = 4*TMath::RMS(NBINS_BASELINE, trace);
    int nBins_used = NBINS_BASELINE;
    for (int i=0; i<NBINS_BASELINE; i++) {
        // remove points out of 4 sigma
        if (TMath::Abs(trace[i]-bl) > bl_tolerance) {
            bl = (bl*nBins_used - trace[i])/(nBins_used-1);
            nBins_used--;
            // cout << "removed one bin" << endl;
        }
    }
    double bl2 = TMath::Mean(NBINS_BASELINE, trace+200+NBINS_BASELINE);
    double bl2_tolerance = 4*TMath::RMS(NBINS_BASELINE, trace+200+NBINS_BASELINE);
    nBins_used = NBINS_BASELINE;
    for (int i=200+NBINS_BASELINE; i<NBINS_BASELINE+200+NBINS_BASELINE; i++) {
        // remove points out of 4 sigma
        if (TMath::Abs(trace[i]-bl2) > bl2_tolerance) {
            bl2 = (bl2*nBins_used - trace[i])/(nBins_used-1);
            nBins_used--;
            // cout << "removed one bin" << endl;
        }
    }
    baseline = bl > bl2 ? bl: bl2;
    // cout << bl << ", " << bl2 << ", " << baseline << endl;

    // remove baseline, invert to positive trace
    const int NSAMPLES = 252;
    double cleanTrace[NSAMPLES];
    for (int i=0; i< NSAMPLES; i++) {
        cleanTrace[i] = baseline - trace[i];
        // cout << cleanTrace[i] << " ";
    }
    // cout << endl;

    // calculate pulse with pulse finding of continuous area
    vector<double> pulses;
    vector<double> tdcs;
    double charge = 0;
    double tdc = 0;
    bool foundPulse = false;
    const int THRESHOLD = 10; // threshold for register a pulse and for tdc start 
    for (unsigned i=0; i<NSAMPLES-1; i++) {
        if (cleanTrace[i]>0 && cleanTrace[i+1]>0) {
            foundPulse = true;
            charge += cleanTrace[i];
            if(i>0 && tdc<0.1 && cleanTrace[i-1]<THRESHOLD && cleanTrace[i]>THRESHOLD) tdc = i;
        }
        else {
            if(foundPulse && tdc>0.1) {
                pulses.push_back(charge);
                tdcs.push_back(tdc);
            }
            charge = 0;
            tdc = 0;
            foundPulse = false;
        }
    }
    // cout << pulses.size() << " pulses." << endl;
    nPulses = pulses.size();
    // for (int i=0; i< nPulses; i++) {
    //     cout << pulses[i] << " at " << tdcs[i] << endl;
    // }
    maxCharge = 0;
    riseTime = 0;
    totalCharge = 0;
    for (int i=0; i<nPulses; i++) {
        totalCharge += pulses[i];
        if (pulses[i] > maxCharge) {
            maxCharge = pulses[i];
            riseTime = tdcs[i] * 4;  // 4 ns per sample
        }
    }

}
