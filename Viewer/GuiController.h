#ifndef GUI_CONTROLLER_H
#define GUI_CONTROLLER_H

#include "TGFrame.h"
#include "RQ_OBJECT.h"
#include "TString.h"
#include "TTimeStamp.h"

class MainWindow;
class ViewWindow;
class InfoWindow;
class ControlWindow;
class DAQEvent;
class WFAnalyzer;
class TCanvas;
class TGraph;
class TLatex;

class GuiController
{
    RQ_OBJECT("EVDGuiController")
public:
    GuiController(const TGWindow *p, int w, int h);
    virtual ~GuiController();

    void InitEvent(const char* filename);
    void Reload();
    void InitConnections();
    void Modified();

    // slots
    void HandleFileMenu(int id);
    void MenuOpen();
    void Prev();
    void Next();
    void Jump();
    void FindNextCoincidence();
    void FindNextMuon();

    void DrawWF(int i);

    TGraph *gWF[3]; // previous, current, next waveform
    TGraph *gMu[2][3]; // previous, current, next waveform of the two muon paddles
    TTimeStamp triggerTS[3];  // previous, current, next waveform
    bool isMuon1[3];
    bool isMuon2[3];
    TLatex *text_dt[3];

    // member variables
    MainWindow    *mw;
    ViewWindow    *vw;
    InfoWindow    *iw;
    ControlWindow *cw;
    DAQEvent       *event;
    WFAnalyzer     *ana;

    TCanvas *can;
    TString baseDir;
    int currentEventEntry;


};

#endif
