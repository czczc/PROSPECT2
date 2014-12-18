#include "WFAnalyzer.h"
#include "DAQEvent.h"

#include "TMath.h"

#include <vector>
#include <iostream>
using namespace std;

WFAnalyzer::WFAnalyzer(DAQEvent *evt):
fEvent(evt),
fBaseline(0),
fNPulses(1)
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
    // const int NSAMPLES = 252;
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
    fBaseline = bl > bl2 ? bl: bl2;
    // cout << bl << ", " << bl2 << ", " << fBaseline << endl;
}
