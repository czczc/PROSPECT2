#include "InfoWindow.h"
#include "DAQEvent.h"
#include "WFAnalyzer.h"

#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TLatex.h"

#include <iostream>
#include <vector>
using namespace std;


InfoWindow::InfoWindow(const TGWindow *p, int w,int h)
    :TRootEmbeddedCanvas("InfoWindowCanvas", p, w, h)
{
    can = GetCanvas();
}

InfoWindow::~InfoWindow()
{
}


//------------------------------------------------------
void InfoWindow::ClearCanvas()
{
    can->cd();    
    size_t size = listOfDrawables.size();
    for (size_t i=0; i!=size; i++) {
        delete listOfDrawables[i];
    }
    listOfDrawables.clear();
    can->Clear();
}

//------------------------------------------------------
void InfoWindow::DrawEventInfo(DAQEvent *event, WFAnalyzer *ana)
{
    ClearCanvas();
    vector<TString> lines;
    lines.push_back(Form("      Event: %i", event->eventNo));
    lines.push_back(Form("   baseline: %.0f", ana->baseline));
    lines.push_back(Form("    nPulses: %i", ana->nPulses));
    lines.push_back(Form("  maxCharge: %.0f", ana->maxCharge));
    lines.push_back(Form("   riseTime: %.0f", ana->riseTime));
    lines.push_back(Form("totalCharge: %.0f", ana->totalCharge));

    float startx = 0.05;
    float starty = 0.9;
    size_t size = lines.size();
    for (size_t i=0; i<size; i++) {
        TLatex* tex = new TLatex(startx, starty-i*0.1, lines[i].Data());
        listOfDrawables.push_back(tex);
        tex->SetNDC();
        tex->SetTextFont(102);
        tex->SetTextSize(0.08);
        tex->Draw();
    }

    UpdateCanvas();
}


//------------------------------------------------------
void InfoWindow::UpdateCanvas()
{
    can->Modified();
    can->Update();
}
