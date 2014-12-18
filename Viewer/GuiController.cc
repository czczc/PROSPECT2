#include "GuiController.h"
#include "MainWindow.h"
#include "ViewWindow.h"
#include "InfoWindow.h"
#include "ControlWindow.h"
#include "DAQEvent.h"
#include "WFAnalyzer.h"

#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TApplication.h"
#include "TGMenu.h"
#include "TGFileDialog.h"

#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGraph.h"
#include "TTimeStamp.h"
#include "TAxis.h"
#include "TLatex.h"

#include <exception>
#include <iostream>
#include <map>
using namespace std;

GuiController::GuiController(const TGWindow *p, int w, int h)
{

    baseDir = baseDir + gSystem->DirName(__FILE__) + "/..";
    event = 0;
    ana =  0;

    mw = new MainWindow(p, w, h);
    vw = mw->fViewWindow;
    cw = mw->fControlWindow;
    iw = mw->fControlWindow->fInfoWindow;
    can = vw->can;

    for (int i=0; i<3; i++) {
        gWF[i] = new TGraph(DAQEvent::MAX_SAMPLE);
        gWF[i]->SetLineWidth(2);
        for (int j=0; j<2; j++) {
            gMu[j][i] = new TGraph(DAQEvent::MAX_SAMPLE);
            gMu[j][i]->SetLineWidth(2);
        }
        gMu[0][i]->SetLineColor(kGreen+1);
        gMu[1][i]->SetLineColor(kMagenta+1);

        text_dt[i] = new TLatex();
        text_dt[i]->SetNDC();
        text_dt[i]->SetTextSize(0.08);

        isMuon1[i] = false;
        isMuon2[i] = false;

        can->GetPad(i+1)->SetGridx();
        can->GetPad(i+1)->SetGridy();
    }
    gWF[0]->SetLineColor(kBlue-2); 
    text_dt[0]->SetTextColor(kBlue-2);
    gWF[2]->SetLineColor(kRed-2); 
    text_dt[2]->SetTextColor(kRed-2);

    MenuOpen();
    InitConnections();
}

GuiController::~GuiController()
{

}

void GuiController::InitConnections()
{
    mw->fFileMenu->Connect("Activated(int)", "GuiController", this, "HandleFileMenu(int)");

    cw->fPrevButton->Connect("Clicked()", "GuiController", this, "Prev()");
    cw->fNextButton->Connect("Clicked()", "GuiController", this, "Next()");
    cw->fEventEntry->Connect("ReturnPressed()", "GuiController", this, "Jump()");
    cw->fNextCIButton->Connect("Clicked()", "GuiController", this, "FindNextCoincidence()");
    cw->fNextMuonButton->Connect("Clicked()", "GuiController", this, "FindNextMuon()");

}


//-------------------------------------------------
void GuiController::Modified()
{   

    can->GetPad(1)->Modified();
    can->GetPad(2)->Modified();
    can->GetPad(3)->Modified();
    can->Update();
}


//-------------------------------------------------
void GuiController::Prev()
{
    if (currentEventEntry==0) {
        currentEventEntry = event->nEvents-1;
    }
    else {
        currentEventEntry--;
    }
    Reload();
}

//-------------------------------------------------
void GuiController::Next()
{
    if (currentEventEntry==event->nEvents-1) {
        currentEventEntry = 0;
    }
    else {
        currentEventEntry++;
    }
    Reload();
}

//-------------------------------------------------
void GuiController::Jump()
{
    currentEventEntry = int(cw->fEventEntry->GetNumber());
    if (currentEventEntry>=event->nEvents-1) {
        currentEventEntry = event->nEvents-1;
    }
    else if (currentEventEntry < 0) {
        currentEventEntry=0;
    }
    Reload();
}

//-------------------------------------------------
void GuiController::FindNextCoincidence()
{
    Next();
    while ( (triggerTS[1].AsDouble()-triggerTS[0].AsDouble())*1e6 > 200 ) {
        Next();
    }
}

//-------------------------------------------------
void GuiController::FindNextMuon()
{
    Next();
    while ( !isMuon1[1] && !isMuon2[1] ) {
        Next();
    }
}

//-------------------------------------------------
void GuiController::DrawWF(int i)
{   
    // i here is the 0, 1, 2, corresponding to previous, current, next WF
    can->cd(i+1);

    vector<unsigned short>& ch = *(event->ch0);

    for (int n=0; n<DAQEvent::MAX_SAMPLE; n++) {
        gWF[i]->SetPoint(n, n*4, ch[n]);
    }
    triggerTS[i] = event->ts;

    gWF[i]->SetTitle(triggerTS[i].AsString());
    gWF[i]->Draw("ALP");
    gWF[i]->GetXaxis()->SetTitle("ns");
    gWF[i]->GetXaxis()->SetRangeUser(0, 1050);

    TString line_dt = Form("%+d #mus", int((triggerTS[i].AsDouble()-triggerTS[1].AsDouble())*1e6));
    text_dt[i]->SetText(0.7, 0.2, line_dt.Data());
    if (i!=1) { 
        text_dt[i]->Draw();
    }

    // mu paddles
    vector<unsigned short>& ch1 = *(event->ch1);
    int size = ch1.size();
    isMuon1[i] = false;
    if (size>0) {
        for (int n=0; n<size; n++) {
            gMu[0][i]->SetPoint(n, n*4, ch1[n]);
        }
        gMu[0][i]->Draw("same");
        isMuon1[i] = true;
    }
    vector<unsigned short>& ch2 = *(event->ch2);
    size = ch2.size();
    isMuon2[i] = false;
    if (size>0) {
        for (int n=0; n<size; n++) {
            gMu[1][i]->SetPoint(n, n*4, ch2[n]);
        }
        gMu[1][i]->Draw("same");
        isMuon2[i] = true;
    }

}


//-------------------------------------------------
void GuiController::Reload()
{    
    cw->fEventEntry->SetNumber(currentEventEntry);

    event->GetEntry(currentEventEntry);
    ana->Process();
    iw->DrawEventInfo(event, ana);

    DrawWF(1);
    // event->PrintInfo(1);
    event->PrintInfo();

    if (currentEventEntry >= 1) {
        event->GetEntry(currentEventEntry-1);
        DrawWF(0);
    }
    if (currentEventEntry <= event->nEvents-2) {
        event->GetEntry(currentEventEntry+1);
        DrawWF(2);
    }


    Modified();
}



//---------------------------------------------------
void GuiController::InitEvent(const char* filename)
{
    if (event) delete event;
    event = new DAQEvent(filename);
    currentEventEntry = 0;

    if (ana) delete ana;
    ana = new WFAnalyzer(event);

    Reload();
}

//---------------------------------------------------
void GuiController::MenuOpen()
{
    const char *filetypes[] = {"ROOT files", "*.root", 0, 0};
    TString dir(baseDir + "/data/d2r");
    TGFileInfo fi;
    fi.fFileTypes = filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(gClient->GetRoot(), mw, kFDOpen, &fi);
    cout << "opening file: " << fi.fFilename << endl;

    InitEvent(fi.fFilename);
}

//---------------------------------------------------
void GuiController::HandleFileMenu(int id)
{
    switch (id) {
        case M_FILE_OPEN:
            MenuOpen();
            break;

        case M_FILE_EXIT:
            gApplication->Terminate(0);
            break;
    }
}

